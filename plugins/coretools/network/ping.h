#ifndef PING_H
#define PING_H

#include <QHostAddress>

class Ping : public QObject
{
    Q_OBJECT
public:
    explicit Ping(QObject *parent = 0);
    
signals:
    void pingSuccess(QString host, int time);
    void pingFailure(QString host, QString message);
    
public slots:
    void ping(QString host, int timeout);

};

#endif // PING_H
