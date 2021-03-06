#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QHash>
#include <QList>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QRegExp>
#include "partyservice.h"
#include "loginpassinputdialog.h"
#include "spamsystem.h"
#include "tinylogger.h"
#include "messagepreprocessors.h"
#include "spamconfig.h"
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

struct chatHistory
{
    QString toString() {return sender + " ::: " + message + ":::" + QString::number(score) + date;}
    static chatHistory fromString(QString s);
    QString sender;
    QString message;
    QString date;
    int score;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setup();
    void loadAccounts();
    void loadEmotes();
    void loadTwitchEmotes();
    void setupTextViews();
    ~MainWindow();



    QString apikey, login;
    bool admin;
    SpamConfig config;


public slots:


    void loadAccountsResponse(getAccountsResult result);
    void loadEmotesResponse(emotesListResult result);
    void addAccountResponse(nonQueryResult result);
    void removeAccountResponse(nonQueryResult result);
    void createPartyResponse(nonQueryResult result);
    void joinPartyResponse(joinPartyResult result);
    void leavePartyResponse(nonQueryResult result);
    void updatePartyResponse(partyUpdatesResult result);
    void savePartyParamsResponse(nonQueryResult result);
    void loadTwitchEmotesResponse(QNetworkReply* reply);
    void getInviteResponse(invitesResult result);


    void loadUpdates();
    void startAttack();
    void leavePartyOnExit();
    void applicationExit();

    void saveToHistory(QString sender, QString message);
private slots:
    void connectingState(int max);
    void readingState(int max);
    void spammingState(int max);
    void wordAdded(QString word);
    void messageSent(int current);
    void socketConnected(int current);
    void messageRead(int current);

    void markBanned(QString nick);
    void sendStatus(QString status, bool ready, QString info);
    void closeEvent(QCloseEvent *event);

    void displayTookServices();


    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_create_party_button_clicked();
    void on_join_party_button_clicked();
    void on_leave_party_button_clicked();
    void on_ready_checkbox_clicked(bool checked);
    void on_party_status_returnPressed();
    void on_save_params_button_clicked();
    void on_lineEdit_3_returnPressed();
    void on_accounts_list_currentItemChanged(QListWidgetItem *current);
    void on_start_attack_button_clicked();
    void on_stop_attack_button_clicked();

    void on_party_chat_anchorClicked(const QUrl &arg1);

    void on_auto_login_clicked(bool checked);

    void on_send_banned_clicked(bool checked);

    void on_hello_message_count_editingFinished();

    void on_Steal_coockies_fake_clicked();

    void on_get_invite_button_clicked();

private:
    Ui::MainWindow *ui;
    QStringList allAccounts_;
    QStringList users_;
    QList<chatHistory> history_;
    QList<getAccountsResult::spamAccountDescriptor> myAccounts_;
    QList<getBannedResult::spamAccountBan> banned_;
    QStringList words_;
    QString spamMode_;

    partyServiceAllocator * psa;
    QHash<QString,partyService*> srv;
    QHash<QString,QString> emotes;
    QStringList imgResources;

    partyDescriptor myParty;

    QTimer partyUpdateTimer;
    SpamSystem * spamSystem;
    QString attackStatus;
    TinyLogger * logger;
    QString imgMessage;

    QString server_, port_;
    bool startupBean;

};

#endif // MAINWINDOW_H
