#ifndef ABSTRACTIRCSOCKET_H
#define ABSTRACTIRCSOCKET_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>
#include <QApplication>
#include <QTextEdit>
#include <QtGlobal>
#include <QList>
#include <QTimer>
#include <QTime>
#include <limits>
#include "partyservice.h"

class IrcPrivMessageParser
{
public:
    QString nickname;
    QString message;

    //Parses the messega to two strings: sender's nickname and his message exactly.
    static IrcPrivMessageParser parse(QString message);
    IrcPrivMessageParser();
    ~IrcPrivMessageParser(){;}
};

class AbstractIrcSocket : public QObject
{
    Q_OBJECT
protected:
    QTcpSocket *socket;
    QString nickname_, pass_;
    QString channel_;
    QString server_;
    int port_;
    QStringList * words_;

    //method for logining on server
    virtual void login();
public:
    explicit AbstractIrcSocket(QObject *parent = 0);
    ///
    /// \brief AbstractIrcSocket
    /// \param host Irc server to connect
    /// \param channel channel on IRC server
    /// \param nickname your nickname
    /// \param pass your pass (or oAuth code for twitch)
    /// \param words pointer to list with words to spam
    ///
    explicit AbstractIrcSocket(QString host, int port, QString channel, QString nickname, QString pass,
                               QStringList* words);
    virtual void connectToHost() const;
    virtual void disconnect();
    virtual QString getNick(){return nickname_;}

signals:
    //this signal sends when socket got connected
    void connectSignal();

public slots:
    //this slot is for processing incoming messages from server
    virtual void parseMessage()=0;
    //this slot is activated when socket is connected
    void connected();
};

class GrabMessageIrcSocket : public AbstractIrcSocket
{
    Q_OBJECT
    //textEdit widget where listening bot will type incoming chat messages
    //can be NULL_PTR
    QTextEdit * textEdit_;
    //list of spambots nicknames
    QStringList * spambots_;

public:
    explicit GrabMessageIrcSocket(QObject *parent = 0);
    explicit GrabMessageIrcSocket(QString host, int port, QString channel, QString nickname, QString pass,
                                  QStringList* words, QStringList* spambots, QTextEdit* out = 0);

signals:
    void wordAdded(QString word);
    void messageRead();
    void spamBotResponsed(QString nickname);


public slots:
    void parseMessage();

};


class SpamMessageSocket : public AbstractIrcSocket
{
    Q_OBJECT
    //min and max count of words in spam message
    int minWords_, maxWords_;
public:
    explicit SpamMessageSocket(QObject *parent = 0);
    explicit SpamMessageSocket(QString host, int port, QString channel, QString nickname, QString pass,
                               QStringList* words, QHash<QString,QString>* emotes, int minWords, int maxWords,
                               bool emotesCap=false, int emotesCount=0, bool toLower = false);
    void sendMessage();

signals:
    void messageSent(QString nickname, QString message);
public slots:
    void parseMessage();
protected:
    QString preProcessMessage(QString str);
    void setupTwitchEmotes();

    bool emotesCap_;
    int emotesCapValue_;
    bool toLower_;
    QHash<QString,QString>* emotes_;
};

struct SpamParams
{
    SpamParams();
    SpamParams(QString host, int port, int minWords, int maxWords,
              bool emotesCap, bool tolower, int emotesCount, int cd, int maxMessages = std::numeric_limits<int>::max());
    ~SpamParams(){;}
    bool newMessage();
    bool isOverCap(){return _messageCount > _maxMessages;}
    QString _host;
    int _port;
    int _minWords, _maxWords;
    bool _emotesCap;
    bool _toLower;
    int _emotesCount;
    int _cd;
    int _maxMessages;
    int _messageCount;
    int _beforeAttack;
    QString _method;
    QString _channel;
    QTextEdit * out;
};

class SpamSystem : public QObject
{
    Q_OBJECT
public:
    SpamSystem(QString host, int port, QString channel, QList<getAccountsResult::spamAccountDescriptor>& myAccounts,
               QStringList& allAccounts, QStringList& bannedAccounts, QStringList& messageList, QTextEdit * out, QHash<QString, QString> *emotes,
               int minWords, int maxWords, bool emotesCap, int emotesCount, bool toLower, int cd, int maxMessages, int beforeAttack);
    ~SpamSystem();
    enum spamState {IDLE, CONNECTING, READING, GREETINGS, SPAMMING};
    void startAdaptive();
    void startList();
    void startBoth();
    void stop();
    QString getGrabber(){return grabberLogin;}
signals:
    void connectingState(int total);
    void readingState(int total);
    void greetingsState(int total);
    void spammingState(int max);
    void messageSent(int count);
    void spamSocketConnected(int count);
    void readMessage(int current);
    void wordAdded(QString word);
    void stopped();
    void banned(QString nick);
public slots:
    void socketConnected();
    void connectNextSocket();
    void grabberReadMessage();
    void grabberWordAdd(QString word);
    void sendMessage();
    void spamBotResponsed(QString nickname);
    void checkBanned();

private:
    QList<getAccountsResult::spamAccountDescriptor> myAccounts_;
    QList<SpamMessageSocket*> spammers;
    GrabMessageIrcSocket* grabber;
    QStringList bannedAccounts_;
    QStringList allAccounts_;
    QStringList messageList_;
    QStringList words_;

    int currentConnectNumber;

    QTimer connectTimer;
    QTimer sendMessageTimer;
    QTimer checkBannedTimer;

    SpamParams params;
    spamState state;
    QString grabberLogin;
    QHash<QString,QTime> spamCommits;
    QHash<QString,QString>* emotes_;
    int socketConnectedCount;
    int grabberMessageReadCount;
};



#endif // ABSTRACTIRCSOCKET_H
