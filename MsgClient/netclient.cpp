#include "netclient.h"

NetClient::NetClient()
{
    //размер принимаемого блока 0
    _blockSize = 0;

    //создаем сокет
    _socket = new QTcpSocket(this);

    //подключаем сигналы
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onSocketDisplayError(QAbstractSocket::SocketError)));

    // таймер ожидания ответа авторизации от сервера
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

NetClient::~NetClient()
{

}

//-----------------------------------------------------------//
/* Подключение к серверу */
void NetClient::doConnect()
{
    if(_serverAddress != "" && _serverPort != "" && _username != "") {
        _socket->connectToHost(_serverAddress, _serverPort.toInt());
        qDebug() << "Connecting to server...";
    }
    else qDebug() << "Connection failed: addr, port or username is empty";
}

/* Отключение от сервера */
void NetClient::doDisconnect()
{
    _socket->disconnectFromHost();
    qDebug() << "Disconnected from server";
}

/* Отправка сообщение пользователю */
void NetClient::doSendToUser(QString user, QString msg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    if (user == NetClient::allUsers)
        out << (quint8)NetClient::comMessageToAll;
    else
    {
        out << (quint8)NetClient::comMessageToUsers;
        out << user;
    }

    out << msg;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    _socket->write(block);
    qDebug() << "Message was send to " << user;
}


//-----------------------------------------------------------//
/* Если ошибка подключения к серверу (сигнал error) */
void NetClient::onSocketDisplayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            emit recvError("Error: The host was not found");
            qDebug() << "Error: The host was not found";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            emit recvError("Error: The connection was refused by the peer.");
            qDebug() << "Error: The connection was refused by the peer.";
            break;
        default:
            emit recvError("Error: The following error occurred: "+_socket->errorString());
            qDebug() << "Error: The following error occurred: "+_socket->errorString();
    }
    emit sigEvent(NetClient::socketConnError);
}

/* Если получен новый пакет (сигнал readyread) считываем его из буфера */
void NetClient::onSocketReadyRead()
{
    qDebug() << "Read data from socket...";
    QDataStream in(_socket);
    //если считываем новый блок первые 2 байта это его размер
    if (_blockSize == 0) {
        //если пришло меньше 2 байт ждем пока будет 2 байта
        if (_socket->bytesAvailable() < (int)sizeof(quint32))
            return;
        //считываем размер (2 байта)
        in >> _blockSize;
        qDebug() << "_blockSize now " << _blockSize;
    }
    //ждем пока блок прийдет полностью
    if (_socket->bytesAvailable() < _blockSize)
        return;
    else
        //можно принимать новый блок
        _blockSize = 0;

    //3 байт - команда серверу
    QString data;
    in >> data;

    QMap<QString, QString> mapData;
    mapData = doParse(data);

    if(!mapData["command"].length()) return;

    qDebug() << "Received command " << mapData["command"];

    switch (mapData["command"].toInt())
    {
        case NetClient::comAuthSuccess:
        {
            _timer->stop();
            emit sigEvent(NetClient::comAuthSuccess);
            qDebug() << "Authentication success!";
        }
        break;
        case NetClient::comUsersOnline:
        {
            emit recvCommand(NetClient::comUsersOnline, mapData["users"]);
        }
        break;
        case NetClient::comPublicServerMessage:
        {
            emit recvCommand(NetClient::comPublicServerMessage, mapData["message"]);
        }
        break;
        case NetClient::comMessageToAll:
        {
            emit recvCommand(NetClient::comMessageToAll, mapData["user"], mapData["message"]);
        }
        break;
        case NetClient::comMessageToUsers:
        {
            emit recvCommand(NetClient::comMessageToUsers, mapData["user"], mapData["message"]);
        }
        break;
        case NetClient::comPrivateServerMessage:
        {
            emit recvCommand(NetClient::comPrivateServerMessage, mapData["message"]);
        }
        break;
        case NetClient::comUserJoin:
        {
            emit recvCommand(NetClient::comUserJoin, mapData["username"]);
        }
        break;
        case NetClient::comUserLeft:
        {
            emit recvCommand(NetClient::comUserLeft, mapData["username"]);
        }
        break;
        case NetClient::comErrNameInvalid:
        {
            emit recvCommand(NetClient::comErrNameInvalid);
            _socket->disconnectFromHost();
        }
        break;
        case NetClient::comErrNameUsed:
        {
            emit recvCommand(NetClient::comErrNameUsed);
            _socket->disconnectFromHost();
        }
        break;
    }
}

/* Если если получен сигнал connected, то авторизуемся на сервере */
void NetClient::onSocketConnected()
{
    _blockSize = 0;

    emit sigEvent(NetClient::socketConnected);
    qDebug() << "Connected to server";
    qDebug() << "Trying to authenticate...";

    //try auth
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << (quint8)NetClient::comAuthRequest;
    out << _username;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    _socket->write(block);

    // таймер ожидания ответа авторизации от сервера
    _timer->start(3000);
}

/* Если получен сигнал disconnected */
void NetClient::onSocketDisconnected()
{
    qDebug() << "emit msg: Disconnected from server";
    emit sigEvent(socketDisconnected);
}

/* Если получен сигнал timeout (не получен ответ авторизации) */
void NetClient::onTimeout() {
    _timer->stop();
    //_connectionStatus = NetClient::socketConnError;
    qDebug() << "Authentication timeout";
    doDisconnect();
}


//-----------------------------------------------------------//
/* Парсер управляющего сообщения от клиентов */
QMap<QString, QString> NetClient::doParse(QString data)
{
    quint8 delim;
    QMap<QString, QString> map;
    QStringList list = data.split(";",QString::SkipEmptyParts);

    QStringList::iterator it = list.begin();
    for(it = list.begin();it != list.end(); ++it)
    {
        delim = (*it).trimmed().indexOf("=", 0);
        if(delim == 0) break;
        map[(*it).mid(0, delim).trimmed()] = (*it).mid(delim + 1).trimmed();
    }
    return map;
}
