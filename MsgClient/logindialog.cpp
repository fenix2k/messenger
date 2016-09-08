#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::LoginDialog)
{
    _ui->setupUi(this);
    //connect(_ui->m_btnLogin, SIGNAL(clicked()), this, SLOT(on_m_btnLogin_clicked()));

    _pClient = new NetClient();
    connect(_pClient, SIGNAL(sigEvent(quint8)), this, SLOT(onConnectionResult(quint8)));
}

LoginDialog::~LoginDialog()
{
    delete _ui;
}

/* Если нажата кнопка Login */
void LoginDialog::on_m_btnLogin_clicked()
{
    /* доработать проверку введённых значений */
    _serverAddr = _ui->m_txtServer->text();
    _serverPort = _ui->m_txtPort->text();
    _userLogin  = _ui->m_txtLogin->text();
    _userPassword = _ui->m_txtPasswd->text();

    _ui->m_btnLogin->setDisabled(true);
    _ui->m_lblMsg->setText("");

    _pClient->setServerAddress(_serverAddr);
    _pClient->setServerPort(_serverPort);
    _pClient->setUsername(_userLogin);

    _ui->m_lblMsg->setText("Connecting to server...");
    _pClient->doConnect();
}

void LoginDialog::onConnectionResult(quint8 type)
{
    switch (type)
    {
        case NetClient::socketDisconnected:
        {
            _ui->m_btnLogin->setDisabled(false);
            _ui->m_lblMsg->setText("Disconnected from server");
        }
        break;
        case NetClient::socketConnError:
        {
            _ui->m_btnLogin->setDisabled(false);
            _ui->m_lblMsg->setText("Can't connect to server");
        }
        break;
        case NetClient::comAuthSuccess:
        {
            disconnect(_pClient, SIGNAL(sigEvent(quint8)), this, SLOT(onConnectionResult(quint8)));

            this->accept();
        }
        break;
    }
}
