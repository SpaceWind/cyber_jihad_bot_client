#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QString>
#include <QHash>
#include "partyservice.h"
#include "spamconfig.h"

namespace Ui {
class loginWindow;
}

class loginWindow : public QDialog
{
    Q_OBJECT
    const QString server;
public:
    explicit loginWindow(QWidget *parent = 0);
    QString getApikey(){return apikey_;}
    QString getLogin(){return login_;}
    SpamConfig getConfig() {return config;}
    bool isAdmin(){return admin_;}
    ~loginWindow();

private slots:
    void displayConfig();
    void on_register_button_clicked();
    void signupResponse(nonQueryResult result);
    void loginResponse(loginResult result);

    void on_login_button_clicked();

private:
    Ui::loginWindow *ui;
    partyServiceAllocator * psa;
    QHash<QString,partyService*> srv;

    QString apikey_;
    QString login_;
    bool admin_;
    SpamConfig config;
};

#endif // LOGINWINDOW_H
