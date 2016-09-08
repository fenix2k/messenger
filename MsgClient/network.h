#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>

class Network : public QObject
{
    Q_OBJECT

public:
    Network();
    ~Network();

    int setServer(const QString serverIP, const QString serverPort);
    int setCredential(const QString userLogin, const QString userPasswd);

    int connectToServer();
    int disconnectFromServer();

    int authOnServer();

private:
    QTcpSocket* m_pTcpSocket;       // объект для работы с Tcp сокетом
    quint16     m_nNextBlockSize;   // размер следующего блока данных

    QString     m_serverIP;       // IP адрес сервера
    int         m_serverPort;       // порт сервера
    QString     m_userLogin;        // логин пользователя
    QString     m_userPassword;     // пароль пользователя

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
    void slotDisconnected();
};

#endif // NETWORK_H
