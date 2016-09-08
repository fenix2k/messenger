#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "netclient.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private:
    Ui::LoginDialog *_ui;          // объект интерфейса
    NetClient *_pClient;           // объект для работы с Tcp сокетом

    QString     _serverAddr;       // IP адрес сервера
    QString     _serverPort;       // порт сервера
    QString     _userLogin;        // логин пользователя
    QString     _userPassword;     // пароль пользователя


private slots:
    void on_m_btnLogin_clicked();
    void onConnectionResult(quint8 type);
//    void onDisconnect();

};

#endif // LOGINDIALOG_H
