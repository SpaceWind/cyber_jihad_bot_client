#include <QTimer>
#include "loginwindow.h"
#include "ui_loginwindow.h"

loginWindow::loginWindow(QWidget *parent) :
    QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::loginWindow)
{

    psa = new partyServiceAllocator("http://cyberjihad-bot.herokuapp.com",2);
    config.load();
    if (config.autologin)
    {
        this->setWindowOpacity(0);
        this->setEnabled(false);
    }
    QTimer::singleShot(500,this, SLOT(displayConfig()));
    ui->setupUi(this);
}

loginWindow::~loginWindow()
{
    delete ui;
}

void loginWindow::displayConfig()
{

    ui->login_login->setText(config.login);
    ui->login_pass->setText(config.pass);
    ui->login_button->setFocus();
    if (config.autologin)
        on_login_button_clicked();
}

void loginWindow::on_register_button_clicked()
{
    partyService *ps = psa->get();
    ps->signup(ui->reg_login->text(),ui->reg_pass->text(),ui->reg_invite->text());
    connect(ps,SIGNAL(signupResponse(nonQueryResult)),this, SLOT(signupResponse(nonQueryResult)));
    srv["signup"] = ps;
    ui->register_button->setEnabled(false);
    ui->status_label->setText("...");
}

void loginWindow::signupResponse(nonQueryResult result)
{
    if (srv.contains("signup"))
        disconnect(srv["signup"],SIGNAL(signupResponse(nonQueryResult)),this, SLOT(signupResponse(nonQueryResult)));
    ui->register_button->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE!");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Error: " + result.status);
        return;
    }
    ui->status_label->setText("Success!!");
    ui->tabWidget->setCurrentIndex(0);
    ui->login_login->setFocus();
    ui->login_login->setText(ui->reg_login->text());
    ui->login_pass->setText(ui->reg_pass->text());
    ui->reg_login->setText("");
    ui->reg_pass->setText("");
    ui->reg_invite->setText("");
}

void loginWindow::loginResponse(loginResult result)
{
    setEnabled(true);
    if (srv.contains("login"))
        disconnect(srv["login"],SIGNAL(loginResponse(loginResult)),this,SLOT(loginResponse(loginResult)));
    ui->login_button->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        this->setWindowOpacity(1);
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Error: " + result.status);
        this->setWindowOpacity(1);
    }
    else
    {
        ui->status_label->setText("apikey= " + result.apikey + " as " + result.group);
        apikey_ = result.apikey;
        admin_ = result.group == "admins";
        login_ = ui->login_login->text();
        config.login = ui->login_login->text();
        config.pass = ui->login_pass->text();
        config.save();
        this->accept();
    }
}


void loginWindow::on_login_button_clicked()
{
    partyService *p = psa->get();
    p->login(ui->login_login->text(),ui->login_pass->text());
    connect(p,SIGNAL(loginResponse(loginResult)),this,SLOT(loginResponse(loginResult)));
    srv["login"] = p;
    ui->login_button->setEnabled(false);
    ui->status_label->setText("...");
}
