#ifndef PARTYSERVICE_H
#define PARTYSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QSslConfiguration>
#include <QDateTime>
#include <QVector>
#include <QList>

#include <map>

#include "jsonparser.h"


//----------------------------
//structs
//----------------------------

struct nonQueryResult
{
    bool success;
    bool error;
    QString status;
};
struct loginResult : public nonQueryResult
{
    QString apikey;
    QString group;
};
struct userExistResult : public nonQueryResult
{
    bool exists;
};
struct getAccountsResult : public nonQueryResult
{
    struct spamAccountDescriptor
    {
        QString login;
        QString pass;
        QString owner;
    };
    QList<spamAccountDescriptor> accounts;
};

struct getBannedResult : public nonQueryResult
{
    struct spamAccountBan
    {
        QString login;
        QString channel;
    };
    QList<spamAccountBan> accounts;
};

struct joinPartyResult : public nonQueryResult
{
    struct partyMember
    {
        QString login;
        QString party;
        QString status;
        bool ready;
    };
    struct partyParams
    {
        QString party;
        QString server;
        QString channel;
        int cd;
        QString method;
        int before_attack;
        int word_count_min;
        int word_count_max;
        QString list_name;
        int max_emotes;
        QString caps;
    };
    QList<partyMember> members;
    QList<partyParams> params;
    bool isOwner;
};

struct partyUpdatesResult : public nonQueryResult
{
    struct partyUpdate
    {
        QString party;
        QString rec;
        QString update_type;
        QString nick;
        QString message_text;
        QString user_status;
        QString server;
        QString channel;
        int cd;
        QString method;
        int before_attack;
        int word_count_min;
        int word_count_max;
        QString list_name;
        int max_emotes;
        QString caps;
    };
    QList<partyUpdate> updates;
};

struct emotesListResult : public nonQueryResult
{
    struct emoteDesc
    {
        QString txt;
        QString url;
    };
    QList<emoteDesc> emotes;
};

struct invitesResult : public nonQueryResult
{
    QString invite;
};


//----------------------------
class partyService;
class RemoteServer;
class partyServiceAllocator
{
public:
    explicit partyServiceAllocator(QString serverURL, int count);
    partyService *get();
    int tookCount();
    ~partyServiceAllocator();
private:
    QVector<partyService*> items;
};

class partyService : public QObject
{
    Q_OBJECT
public:
    explicit partyService(QString serverURL, QObject *parent = 0);
    bool isActive() {return isActive_;}
    ~partyService();

    void login(QString login, QString password);
    void signup(QString login, QString password, QString invite);
    void createParty(QString apikey, QString pass, QString partyName);
    void getAccounts(QString apikey, bool all);
    void addAccount(QString apikey, QString login, QString pass);
    void removeAccount(QString apikey, QString login);
    void markBanned(QString apikey, QString login, QString channel);
    void removeBanned(QString apikey, QString login, QString channel);
    void getBanned(QString apikey, QString channel);
    void joinParty(QString apikey, QString party, QString pass);
    void leaveParty(QString apikey, QString party);
    void changeStatus(QString apikey, QString party, QString status);
    void sendMessage(QString apikey, QString party, QString message);
    void setPartyParams(QString apikey, QString party,
                        QString server, QString channel, int cd, QString method,
                        int before_attack, int min_words, int max_words,
                        QString list_name, int max_emotes, bool caps);
    void setPartyParamsAdaptive(QString apikey, QString party,
                                QString server, QString channel, int cd,
                                int before_attack, int min_words, int max_words,
                                int max_emotes, bool caps);
    void setPartyParamsList(QString apikey, QString party,
                            QString server, QString channel, int cd,
                            QString list_name, int max_emotes, bool caps);
    void startParty(QString apikey, QString party);
    void stopParty(QString apikey, QString party);
    void updateParty(QString apikey, QString party);
    void getEmotes();
    void makeAdmin(QString apikey, QString login);
    void getInvite(QString apikey);


    partyService *take();

signals:
    void loginResponse(loginResult result);
    void signupResponse(nonQueryResult result);
    void createPartyResponse(nonQueryResult result);
    void getAccountsResponse(getAccountsResult result);
    void addAccountResponse(nonQueryResult result);
    void removeAccountResponse(nonQueryResult result);
    void markBannedResponse(nonQueryResult result);
    void removeBannedResponse(nonQueryResult result);
    void getBannedResponse(getBannedResult result);
    void joinPartyResponse(joinPartyResult result);
    void leavePartyResponse(nonQueryResult result);
    void changeStatusResponse(nonQueryResult result);
    void sendMessageResponse(nonQueryResult result);
    void setPartyParamsResponse(nonQueryResult result);
    void startPartyResponse(nonQueryResult result);
    void stopPartyResponse(nonQueryResult result);
    void updatePartyResponse(partyUpdatesResult result);
    void getEmotesResponse(emotesListResult result);
    void makeAdminResponse(nonQueryResult result);
    void getInviteResponse(invitesResult result);
private slots:
    void loginServerResponse(QByteArray response);
    void signupServerResponse(QByteArray response);
    void createPartyServerResponse(QByteArray response);
    void getAccountsServerResponse(QByteArray response);
    void addAccountServerResponse(QByteArray response);
    void removeAccountServerResponse(QByteArray response);
    void markBannedServerResponse(QByteArray response);
    void removeBannedServerResponse(QByteArray response);
    void getBannedServerResponse(QByteArray response);
    void joinPartyServerResponse(QByteArray response);
    void leavePartyServerResponse(QByteArray response);
    void changeStatusServerResponse(QByteArray response);
    void sendMessageServerResponse(QByteArray response);
    void setPartyParamsServerResponse(QByteArray response);
    void startPartyServerResponse(QByteArray response);
    void stopPartyServerResponse(QByteArray response);
    void updatePartyServerResponse(QByteArray response);
    void getEmotesServerResponse(QByteArray response);
    void makeAdminServerResponse(QByteArray response);
    void getInviteServerResponse(QByteArray response);
private:
    RemoteServer * remoteServer_;
    bool isActive_;
};


class RemoteServer : public QObject
{
    Q_OBJECT
public:
    explicit RemoteServer(QString serverURL, QObject *parent = 0);

    void call(QString method, QHash<QString, QString> params);
    ~RemoteServer();

signals:
   void callFinished(QByteArray result);
public slots:
   void replyFinished (QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;
    QString server;
};

struct connectDescriptor
{
    connectDescriptor(){p=0;client=""; method="";}
    ~connectDescriptor(){;}
    bool operator==(const connectDescriptor& d)
    {
        return d.client == client && d.method == method;
    }

    partyService *p;
    QString client;
    QString method;
};

#endif // PARTYSERVICE_H
