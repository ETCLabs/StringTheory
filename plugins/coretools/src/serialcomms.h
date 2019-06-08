#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include "genericcomms.h"

#include <QByteArray>
#include <QTimer>

#include <QSerialPortInfo>
class QSerialPort;

namespace Ui {
    class SerialCommsForm;
}

class SerialComms : public GenericComms
{
    Q_OBJECT
public:
    explicit SerialComms(QObject *parent = 0);

    QString commType() {return tr("RS232 Serial");}
    float commVersion() {return 2.1f;}
    QWidget * configWidget();

signals:

public slots:
    void open();
    void close();

    void transmit(const QByteArray &data);

private:
    Ui::SerialCommsForm *ui;
    QWidget *m_configWidget;

    //Serial comms
    QSerialPort *m_serialPort;

    void updateActiveSerial();
    QList<QSerialPortInfo> portList;

    QByteArray m_buffer;
    QTimer m_timer;

private slots:
    void commPortChanged(int index);
    void commSettingsChanged();

    void portClosed();
    void dataReceived();

    void timerExpired();
};

#endif // SERIALCOMMS_H
