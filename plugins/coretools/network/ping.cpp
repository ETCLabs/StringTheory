#include "ping.h"

#include <QDebug>

#include <WinSock2.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

Ping::Ping(QObject *parent) :
    QObject(parent)
{
}

void Ping::ping(QString host, int timeout)
{
    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    DWORD dwError = 0;
    char SendData[] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;

    ipaddr = inet_addr(host.toLatin1());
    if (ipaddr == INADDR_NONE) {
        qDebug() << "Bad IP Address";
        return;
    }

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        qDebug("Unable to open handle");
        qDebug() << "IcmpCreatefile returned error:" << GetLastError();
        return;
    }
    // Allocate space for at a single reply
    ReplySize = sizeof (ICMP_ECHO_REPLY) + sizeof (SendData) + 8;
    ReplyBuffer = (VOID *) malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        qDebug("Unable to allocate memory for reply buffer");
        return;
    }

    dwRetVal = IcmpSendEcho2(hIcmpFile, NULL, NULL, NULL,
                             ipaddr, SendData, sizeof (SendData), NULL,
                             ReplyBuffer, ReplySize, timeout);
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY) ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        QString replyHost = QString("%1.%2.%3.%4")
                .arg(ReplyAddr.S_un.S_un_b.s_b1)
                .arg(ReplyAddr.S_un.S_un_b.s_b2)
                .arg(ReplyAddr.S_un.S_un_b.s_b3)
                .arg(ReplyAddr.S_un.S_un_b.s_b4);
        qDebug() << "Sent icmp message to" << host
                 << "Reply from" << replyHost;
        switch (pEchoReply->Status) {
        case IP_SUCCESS:
            emit pingSuccess(replyHost, pEchoReply->RoundTripTime);
            return;
        case IP_DEST_HOST_UNREACHABLE:
            emit pingFailure(replyHost, tr("Destination host was unreachable"));
            return;
        case IP_DEST_NET_UNREACHABLE:
            emit pingFailure(replyHost, tr("Destination Network was unreachable"));
            return;
        case IP_REQ_TIMED_OUT:
            emit pingFailure(replyHost, tr("Request timed out"));
            return;
        default:
            emit pingFailure(replyHost, tr("Failed: Error Code %1").arg(pEchoReply->Status));
            return;
        }

    } else {
        dwError = GetLastError();
        switch (dwError) {
        case IP_BUF_TOO_SMALL:
            emit pingFailure(host, tr("ReplyBufferSize too small"));
            break;
        case IP_REQ_TIMED_OUT:
            emit pingFailure(host, tr("Request timed out"));
            break;
        default:
            emit pingFailure(host, tr("Failed: Error Code %1").arg(dwError));
            break;
        }
        return;
    }
    return;
}
