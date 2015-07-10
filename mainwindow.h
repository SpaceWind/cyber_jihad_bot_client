#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QHash>
#include <QList>
#include "partyservice.h"
#include "twostringinputdialog.h"
namespace Ui {
class MainWindow;
}

struct partyDescriptor
{
    bool enabled;
    bool owner;
    QString name;
    QString pass;
    QList<QString> members;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setup();
    void loadAccounts();
    ~MainWindow();



    QString apikey, login;
    bool admin;


public slots:
    void loadAccountsResponse(getAccountsResult result);
    void addAccountResponse(nonQueryResult result);
    void removeAccountResponse(nonQueryResult result);
    void createPartyResponse(nonQueryResult result);
    void joinPartyResponse(joinPartyResult result);
    void leavePartyResponse(nonQueryResult result);
    void updatePartyResponse(partyUpdatesResult result);


    void loadUpdates();

private slots:
    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_create_party_button_clicked();

    void on_join_party_button_clicked();

    void on_leave_party_button_clicked();

    void on_ready_checkbox_clicked(bool checked);

    void on_party_status_returnPressed();

private:
    Ui::MainWindow *ui;
    QList<QString> allAccounts_;
    QList<getAccountsResult::spamAccountDescriptor> myAccounts_;
    QList<getBannedResult::spamAccountBan> banned_;
    QList<QString> words_;
    QString spamMode_;

    partyServiceAllocator * psa;
    QHash<QString,partyService*> srv;

    partyDescriptor myParty;

    QTimer partyUpdateTimer;

};

#endif // MAINWINDOW_H
