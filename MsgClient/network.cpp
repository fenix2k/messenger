#include "network.h"
#include <QDataStream>
#include <QTime>

/* Конструктор класса */
Network::Network()
{

}

/* Деструктор класса */
Network::~Network()
{

}

/* Задание адреся и порта сервера */
int Network::setServer(const QString serverIP, const QString serverPort)
{
    /* сделать проверку правильности переданных значений */
    m_serverIP = serverIP;
    m_serverPort = serverPort.toInt();

    return 1;
}

/* Задание параметров аутентификации */
int Network::setCredential(const QString userLogin, const QString userPasswd)
{
    /* сделать проверку правильности переданных значений */
    m_userLogin = userLogin;
    m_userPassword = userPasswd;

    return 1;
}

/* Установление TCP сессии с сервером */
int Network::connectToServer()
{
    /* нужна провека подключения??? */
    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(m_serverIP, m_serverPort);
    connect(m_pTcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));

    return 1;
}

/* Отключения от сервера */
int Network::disconnectFromServer()
{
    if(m_pTcpSocket->state() == QAbstractSocket::UnconnectedState)
        m_pTcpSocket->connectToHost(m_serverIP, m_serverPort);

    return 1;
}

/* Аутентификация на сервере */
int Network::authOnServer()
{
    if(m_userLogin == "fenix" && m_userPassword == "123")
        return 1;
    else return 0;
}

/* Отправка данных на сервер */
void Network::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out << quint16(0) << QTime::currentTime() << "msg";

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    qDebug() << "Msg was send";
}

/* Получение данных с сервера */
void Network::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    while(true) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        QTime time;
        QString str;
        in >> time >> str;

        //m_ptxtInfo->append(time.toString() + " " + str);
        m_nNextBlockSize = 0;
    }
}

/* Слот для обработки события ошибки соединения */
void Network::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                         "The host was not found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                         "The remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                         "The connection was refused." :
                         QString(m_pTcpSocket->errorString())
                        );
    /* Запись в лог файл */
    qDebug() << strError;
}

/* Слот для обработки события успешного подключения */
void Network::slotConnected()
{
    /* Запись в лог файл */
    qDebug() << "Connencted to server";
}

/* Слот для обработки события отключеня от сервера */
void Network::slotDisconnected()
{
    /* Запись в лог файл */
    qDebug() << "Disconnected from server";
}
