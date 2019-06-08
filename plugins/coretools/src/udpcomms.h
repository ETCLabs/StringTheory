#ifndef UDPCOMMS_H
#define UDPCOMMS_H

#include "genericcomms.h"

#include <QUdpSocket>

class Ping;

enum CommunicationMode
{
    Mode_Unicast,
    Mode_Multicast,
    Mode_None
};

namespace Ui {
    class UdpCommsForm;
}

class UdpComms : public GenericComms
{
    Q_OBJECT
public:
    explicit UdpComms(QObject *parent = 0);
    ~UdpComms();

    QString commType() {return tr("Ethernet UDP");}
    float commVersion() {return 1.5f;}
    QWidget * configWidget();
    
signals:
    void ping(QString host, int timeout);
    
public slots:
    void open();
    void close();

    void transmit(const QByteArray &data);

private:
    Ui::UdpCommsForm *ui;
    QWidget *m_configWidget;
    Ping *m_pinger;
    QThread *m_pingThread;

    CommunicationMode m_commMode;

    QUdpSocket *m_udpSocket;
    QHostAddress prevAddress;

    void rebind(QHostAddress newAddress);

private slots:
    void nicChanged();
    void destinationChanged();
    void rxPortChanged();

    void disconnected();
    void udpDataReceived();

    void onPingSuccess(QString host, int time);
    void onPingFailure(QString host, QString message);

    void edTxIpEdited(const QString &arg1);
    void doPing();
};

#endif // UDPCOMMS_H
