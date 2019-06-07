#include "serialcomms.h"
#include "ui_serialcommsform.h"

#include <QDebug>

#include <QSerialPort>

const int SERIAL_MAX_BUFFER_SIZE = 1024;

QString serialErrorToString(QSerialPort::SerialPortError error)
{
    switch (error)
    {
    case QSerialPort::NoError :
        return QObject::tr("No error occurred.");
    case QSerialPort::DeviceNotFoundError :
        return QObject::tr("An error occurred while attempting to open an non-existing device.");
    case QSerialPort::PermissionError :
        return QObject::tr("An error occurred while attempting to open an already opened device by another process or a user not having enough permission and credentials to open.");
    case QSerialPort::OpenError :
        return QObject::tr("An error occurred while attempting to open an already opened device in this object.");
    case QSerialPort::ParityError :
        return QObject::tr("Parity error detected by the hardware while reading data.");
    case QSerialPort::FramingError :
        return QObject::tr("Framing error detected by the hardware while reading data.");
    case QSerialPort::BreakConditionError :
        return QObject::tr("Break condition detected by the hardware on the input line.");
    case QSerialPort::WriteError :
        return QObject::tr("An I/O error occurred while writing the data.");
    case QSerialPort::ReadError :
        return QObject::tr("An I/O error occurred while reading the data.");
    case QSerialPort::ResourceError :
        return QObject::tr("An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system.");
    case QSerialPort::UnsupportedOperationError :
        return QObject::tr("The requested device operation is not supported or prohibited by the running operating system.");
    }
    return QObject::tr("An unidentified error occurred.");
}

/*!
  Returns a string representing the port settings.
  Eg "9600/8-N-1" for 9600 baud, 8 bits, no parity, 1 stop bit.
*/
QString portSettingsString(const QSerialPort &serialPort)
{
    QString result;
    // Baud rate
    result += QString::number(serialPort.baudRate());

    // Append Data Bits
    result += "/" + QString::number((int)serialPort.dataBits());

    // Append parity
    switch (serialPort.parity())
    {
    case QSerialPort::EvenParity :
        result += "-E-";
        break;
    case QSerialPort::SpaceParity :
        result += "-S-";
        break;
    case QSerialPort::NoParity :
        result += "-N-";
        break;
    case QSerialPort::OddParity :
        result += "-O-";
        break;
    case QSerialPort::MarkParity :
        result += "-M-";
        break;
    default:
        result += "-?-";
    }

    // Append stop bits
    switch (serialPort.stopBits())
    {
    case QSerialPort::OneStop:    result += "1";
        break;
    case QSerialPort::OneAndHalfStop:  result += "1.5";
        break;
    case QSerialPort::TwoStop:    result += "2";
        break;
    default:
        result += "?";
    }

    // Append Flow Control
    switch (serialPort.flowControl())
    {
    case QSerialPort::NoFlowControl : break;
    case QSerialPort::HardwareControl :
        result +=" (RTS/CTS)";
        break;
    case QSerialPort::SoftwareControl :
        result +=" (XON/XOFF)";
        break;
    default:
        result +=" (Unknown Flow)";
    }

    // All done
    return result;
}

SerialComms::SerialComms(QObject *parent) :
    ui(new Ui::SerialCommsForm)
{
    Q_UNUSED(parent);
    m_configWidget = new QWidget;
    ui->setupUi(m_configWidget);

    // Init serial
    m_serialPort = new QSerialPort(this);

    // Connect serial port signals
    connect(m_serialPort, SIGNAL(aboutToClose()),
            this, SLOT(portClosed()));
    connect(m_serialPort, SIGNAL(readyRead()),
            this, SLOT(dataReceived()));

    // Init timer
    m_timer.setSingleShot(true);
    m_timer.setInterval(100);
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(timerExpired()));

    // Populate Serial port selection
    updateActiveSerial();

    // Fill serial dropdowns
    ui->cmbBaud->addItem("1200", QSerialPort::Baud1200);
    ui->cmbBaud->addItem("2400", QSerialPort::Baud2400);
    ui->cmbBaud->addItem("4800", QSerialPort::Baud4800);
    ui->cmbBaud->addItem("9600", QSerialPort::Baud9600);
    ui->cmbBaud->addItem("14400", 14400);
    ui->cmbBaud->addItem("19200", QSerialPort::Baud19200);
    ui->cmbBaud->addItem("38400", QSerialPort::Baud38400);
    ui->cmbBaud->addItem("57600", QSerialPort::Baud57600);
    ui->cmbBaud->addItem("115200", QSerialPort::Baud115200);
    ui->cmbBaud->setCurrentIndex(3);    // Default 9600

    ui->cmbParity->addItem("Even", QSerialPort::EvenParity);
    ui->cmbParity->addItem("None", QSerialPort::NoParity);
    ui->cmbParity->addItem("Odd", QSerialPort::OddParity);
    ui->cmbParity->addItem("Mark", QSerialPort::MarkParity);
    ui->cmbParity->addItem("Space", QSerialPort::SpaceParity);
    ui->cmbParity->setCurrentIndex(1);

    ui->cmbDataBits->addItem("5", QSerialPort::Data5);
    ui->cmbDataBits->addItem("6", QSerialPort::Data6);
    ui->cmbDataBits->addItem("7", QSerialPort::Data7);
    ui->cmbDataBits->addItem("8", QSerialPort::Data8);
    ui->cmbDataBits->setCurrentIndex(3);    // Default 8 bit

    ui->cmbStopBits->addItem("1", QSerialPort::OneStop);
    ui->cmbStopBits->addItem("2", QSerialPort::TwoStop);

    //connect UI signals
    connect(ui->cmbSerialList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(commPortChanged(int)));
    connect(ui->actionCommSettingsChange, SIGNAL(triggered()),
            this, SLOT(commSettingsChanged()));
}

QWidget * SerialComms::configWidget()
{
    return qobject_cast<QWidget *>(m_configWidget);
}

void SerialComms::open()
{
    qDebug("Serial Opened");
    m_buffer.clear();
    commPortChanged(ui->cmbSerialList->currentIndex());
}

void SerialComms::close()
{
    m_serialPort->close();
    // And emit final buffer contents
    if (m_buffer.size() > 0)
    {
        emit receivedData(QDateTime::currentDateTime(),
                          m_serialPort->portName(),
                          QString(),
                          m_buffer);
    }
}

void SerialComms::transmit(QByteArray &data)
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->write(data);
        emit sentData(QDateTime::currentDateTime(), QString(), m_serialPort->portName(), data);
    }
    else
    {
        emit note(QDateTime::currentDateTime(), tr("Port %1 is not open")
                  .arg(m_serialPort->portName()), QString(), false);
    }
}

void SerialComms::updateActiveSerial()
{
    portList = QSerialPortInfo::availablePorts();
    QSerialPortInfo portInfo;

    ui->cmbSerialList->clear();
    qDebug() << "List of ports:";
    for(int i=0; i<portList.count(); ++i)
    {
        portInfo = portList.at(i);
//        if (portInfo.portName.contains("COM"))
        {
            ui->cmbSerialList->addItem(QString("%1 - %2 - %3")
                                       .arg(portInfo.portName(),
                                            portInfo.description(),
                                            portInfo.manufacturer()), i);
        }
        qDebug() << "port name:" << portInfo.portName();
        qDebug() << "description:" << portInfo.description();
        qDebug() << "system location:" << portInfo.systemLocation();
        qDebug() << "manufacturer:" << portInfo.manufacturer();
        qDebug() << "vendor ID:" << QString::number(portInfo.vendorIdentifier(), 16);
        qDebug() << "product ID:" << QString::number(portInfo.productIdentifier(), 16);
        qDebug() << "===================================";
    }
}

void SerialComms::portClosed()
{
    emit note(QDateTime::currentDateTime(),
              QString("Com Port Closed"), QString(), false);
}

void SerialComms::dataReceived()
{
    if (m_serialPort->bytesAvailable())
    {
        // Stop the clock
        m_timer.stop();

        m_buffer.append(m_serialPort->readAll());
        // Split up
        char tmpChar = (char)ui->spinBoxSplitChar->value();
        QList<QByteArray> dataSplitList = m_buffer.split(tmpChar);

        // Emit all the split bytes except the last
        for (int i=0; i< dataSplitList.count()-1; ++i)
        {
            // Split takes it away, so add dividing char back on
            QByteArray tmpData = dataSplitList.at(i);
            tmpData.append(tmpChar);

            emit receivedData(QDateTime::currentDateTime(),
                              m_serialPort->portName(),
                              QString(),
                              tmpData);
        }
        m_buffer = dataSplitList.last();
        // Ensure we never get *too* much data
        if (m_buffer.size() > SERIAL_MAX_BUFFER_SIZE)
        {
            timerExpired();
        }

        // If still have data, start the clock
        if (m_buffer.size() > 0)
        {
            m_timer.start();
        }
    }
}

void SerialComms::timerExpired()
{
    // Emit all the data we have right now
    emit receivedData(QDateTime::currentDateTime(),
                      m_serialPort->portName(),
                      QString(),
                      m_buffer);
    m_buffer.clear();
}

void SerialComms::commPortChanged(int index)
{
    // Can't change port if open
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
    // Now change port
    m_serialPort->setPort(portList.at(ui->cmbSerialList->itemData(index).toInt()));

    // And assign the settings
    commSettingsChanged();
}

void SerialComms::commSettingsChanged()
{
    QString errorString = tr("Unknown error");
    // Ensure port is open - can't do anything otherwise
    if (!m_serialPort->isOpen())
    {
        // Try to open
        if (!m_serialPort->open(QIODevice::ReadWrite))
        {   // Didn't open
            if (m_serialPort->error() != QSerialPort::NoError)
            {
                errorString = serialErrorToString(m_serialPort->error());
            }
            qDebug() << "Serial Port Error Code" << m_serialPort->error();
            emit note(QDateTime::currentDateTime(),
                      QString("%1 did not open")
                      .arg(m_serialPort->portName()),errorString, false);
            m_serialPort->clearError();
            return;
        }

    }
    // Setup Serial Port
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);   // No Flow Control
    m_serialPort->setBaudRate((QSerialPort::BaudRate)ui->cmbBaud->itemData(ui->cmbBaud->currentIndex()).toInt());
    m_serialPort->setDataBits((QSerialPort::DataBits)ui->cmbDataBits->itemData(ui->cmbDataBits->currentIndex()).toInt());
    m_serialPort->setParity((QSerialPort::Parity)ui->cmbParity->itemData(ui->cmbParity->currentIndex()).toInt());
    m_serialPort->setStopBits((QSerialPort::StopBits)ui->cmbStopBits->itemData(ui->cmbStopBits->currentIndex()).toInt());

    // Let everyone know how we did!
    if (m_serialPort->isOpen())
    {
        emit note(QDateTime::currentDateTime(),
                  QString("%1 opened")
                  .arg(m_serialPort->portName()),
                  portSettingsString(*m_serialPort), true);
    }
    else
    {
        if (m_serialPort->error() != QSerialPort::NoError)
        {
            errorString = serialErrorToString(m_serialPort->error());
        }
        qDebug() << "Serial Port Error" << m_serialPort->error();
        emit note(QDateTime::currentDateTime(),
                  QString("%1 did not open")
                  .arg(m_serialPort->portName()),
                  QString("Tried %2: %3")
                  .arg(portSettingsString(*m_serialPort),errorString), false);
    }
}

