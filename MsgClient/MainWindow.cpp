#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    if(!Login()) {
        qDebug() << "Login window is was closed";
        exit(0);
    }

    ui->setupUi(this);
    this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* Вызов диалога авторизации (Логина) */
bool MainWindow::Login()
{
    LoginDialog loginDialog;
    if (!loginDialog.exec()) return 0;
    else return 1;
}

/* Если нажата кнопка Logout */
void MainWindow::on_m_btnLogout_clicked()
{
    // Скрываем основное окно и отвязываем сигнал
    this->hide();
    disconnect(ui->m_btnLogout, SIGNAL(clicked()), this, SLOT(on_m_btnLogout_clicked()));

    // Выводим окно Логина
    if(!Login()) return;

    // переинициалицация интерфейса, подключение слота, отображаем главное окно
    ui->setupUi(this);
    connect(ui->m_btnLogout, SIGNAL(clicked()), this, SLOT(on_m_btnLogout_clicked()));
    this->show();

}
