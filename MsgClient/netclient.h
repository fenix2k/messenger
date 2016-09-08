#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include <QTimer>

class NetClient : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *_socket;
    QTimer *_timer;
    quint32 _blockSize;
    quint8  _connectionStatus;
    QString _serverAddress;
    QString _serverPort;

    QString _username;
    quint8 _command;
    QString _data;

    QMap<QString, QString> doParse(QString data);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketDisplayError(QAbstractSocket::SocketError socketError);
    void onTimeout();

signals:
    void sigConnectionResult(quint8 type);
    void recvCommand(quint8 type);
    void recvCommand(quint8 type, QString var1);
    void recvCommand(quint8 type, QString var1, QString var2);
    void sigEvent(quint8 type);
    void recvError(QString error);

public:
    static const QString constNameUnknown;
    const QString allUsers = "AllUsers";

    static const quint8 comAuthRequest = 1;
    static const quint8 comUsersOnline = 2;
    static const quint8 comUserJoin = 3;
    static const quint8 comUserLeft = 4;
    static const quint8 comMessageToAll = 5;
    static const quint8 comMessageToUsers = 6;
    static const quint8 comPublicServerMessage = 7;
    static const quint8 comPrivateServerMessage = 8;
    static const quint8 comAuthSuccess = 9;
    static const quint8 comErrNameInvalid = 201;
    static const quint8 comErrNameUsed = 202;

    static const quint8 socketConnected = 250;
    static const quint8 socketDisconnected = 251;
    static const quint8 socketConnError = 252;

public:
    NetClient();
    ~NetClient();

    void setUsername(QString username) {_username = username;}
    QString getName() const {return _username;}

    void setServerAddress(QString serverAddr) {_serverAddress = serverAddr;}
    QString getServerAddr() const {return _serverAddress;}

    void setServerPort(QString serverPort) {_serverPort = serverPort;}
    QString getServerPort() const {return _serverPort;}

    void doConnect();
    void doDisconnect();
    void doSendToUser(QString user, QString msg);

};

#endif // NETCLIENT_H
