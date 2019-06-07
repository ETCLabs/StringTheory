#ifndef TCPCOMMS_H
#define TCPCOMMS_H

#include "genericcomms.h"

#include <QIcon>
#include <QTimer>
#include <QHostAddress>

class Ping;
class QTcpServer;
class QTcpSocket;

const int TCP_MAX_BUFFER_SIZE = 1024;

namespace Ui {
    class TcpCommsForm;
}

class TcpComms : public GenericComms
{
    Q_OBJECT
public:
    explicit TcpComms(QObject *parent = 0);
    ~TcpComms();

    QString commType() {return tr("Ethernet TCP");}
    float commVersion() {return 1.1f;}
    QWidget * configWidget();
    
signals:
    void ping(QString host, int timeout);
    
public slots:
    void open();
    void close();

    void transmit(QByteArray &data);    

private:
    Ui::TcpCommsForm *ui;
    QWidget *m_configWidget;
    Ping *m_pinger;
    QThread *m_pingThread;

    // Icons
    QIcon m_waitingIcon;
    QIcon m_connectedIcon;
    QIcon m_disconnectedIcon;

    // General state
    bool m_isOpen;
    // Deliberately disconnecting?
    bool m_userDisconnected;

    QByteArray m_rxBuffer;
    QTimer m_timer;

    // To listen for incoming connections
    QTcpServer *m_tcpServer;
    // Holds the current connection
    QTcpSocket *m_tcpSocket;

    // User-entered connection settings
    quint16 m_userLocalPort;
    quint16 m_userRemotePort;
    QHostAddress m_userLocalHostAddr;
    QHostAddress m_userRemoteIp;

    QString connectionLabel() const;
    void disconnectLocalSocket();
    void stopServer();

    void setConnectMode();
    void setTcpListening();
    bool tryTcpRemote();

    void setupSocketSignals();
    void displaySocketSettings(QAbstractSocket *socket = 0);
    int getNicListIndexOf(const QHostAddress &nicIp) const;

private slots:
    void inboundConnection();
    void tcpSocketStateChanged(QAbstractSocket::SocketState newState);
    void tcpSocketDeleteDisconnected();

    void tcpDataReceived();
    void timerExpired();

    void btnConnectClicked();

    void localNicIndexChanged(int index);
    void localPortChanged();

    void remoteIpEdited(const QString &text);
    void remoteIpReturnPressed();
    void remotePortChanged();

    void doPing();
    void onPingSuccess(QString host, int time);
    void onPingFailure(QString host, QString message);
};

#endif // TCPCOMMS_H
