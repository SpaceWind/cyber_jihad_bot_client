#include <QFile>
#include <QTextStream>
#include "spamsystem.h"

IrcPrivMessageParser IrcPrivMessageParser::parse(QString message)
{
    QStringList parts = message.split(" ");
    QString nick = parts.at(0).mid(1,parts.at(0).indexOf("!")-1);
    QString msg;
    for (int i=3; i<parts.count(); i++)
    {
        msg += parts.at(i);
        if (i!= parts.count()-1)
            msg += " ";
    }
    IrcPrivMessageParser result;
    result.nickname = nick;
    result.message = msg.mid(1,msg.length()-1);
    return result;
}

IrcPrivMessageParser::IrcPrivMessageParser()
{
}

//--------------------------------------------------------------------------------------------

void AbstractIrcSocket::login()
{
    QString passStr = "PASS "+pass_+"\r\n";
    QString nickStr = "NICK "+nickname_+"\r\n";
    QString sysName = " " + nickname_.toLower()+"."+server_;
    QString userStr = "USER "+nickname_ + sysName + sysName+ " :NOTHERBOT\r\n";
    QString joinStr = "JOIN #" + channel_ + "\r\n";
    socket->write(passStr.toUtf8());
    socket->write(nickStr.toUtf8());
    socket->write(userStr.toUtf8());
    socket->write(joinStr.toUtf8());
}

AbstractIrcSocket::AbstractIrcSocket(QObject *parent) :
    QObject(parent)
{
    words_ = 0;
    socket = 0;
}

AbstractIrcSocket::AbstractIrcSocket(QString host, int port, QString channel, QString nickname, QString pass, QStringList *words)
{
    server_ = host;
    port_ = port;
    channel_ = channel;
    nickname_ = nickname;
    pass_ = pass;
    words_ = words;
    socket = new QTcpSocket(this);
    //connecting "connected" signal from internal socket to "connected" slot
    connect(socket,SIGNAL(connected()),SLOT(connected()));
    //connecting "readyRead" signal from internal socket to "parseMessage" slot
    //"readyRead" emitted every time socket got some data from server
    connect(socket,SIGNAL(readyRead()),SLOT(parseMessage()));
}

void AbstractIrcSocket::connectToHost() const
{
    if (!socket)
        return;
    socket->connectToHost(server_,port_);
}

void AbstractIrcSocket::disconnect()
{
    socket->disconnect(this);
}

void AbstractIrcSocket::connected()
{
    login();
    emit connectSignal();
}

//-----------------------------------------------------------------------------------

GrabMessageIrcSocket::GrabMessageIrcSocket(QObject *parent) :
    AbstractIrcSocket(parent)
{
    textEdit_ = 0;
    spambots_ = 0;
}

GrabMessageIrcSocket::GrabMessageIrcSocket(QString host, int port, QString channel, QString nickname, QString pass,
                                           QStringList *words, QStringList *spambots, QTextEdit *out) :
                      AbstractIrcSocket(host,port,channel,nickname,pass,words)
{
    textEdit_ = out;
    spambots_ = spambots;
}


void GrabMessageIrcSocket::parseMessage()
{
    //reading every line from socket
    while(socket->canReadLine())
    {
        QString readLine = socket->readLine();
        //Answering to "PING origin" command
        if (readLine.indexOf("PING ")!=-1)
        {
            readLine[1] = QChar('O');
            socket->write(readLine.toUtf8());
        }
        //Processing message in chat
        if (readLine.indexOf("PRIVMSG #"+channel_)!=-1)
        {
            //parsing message
            IrcPrivMessageParser message = IrcPrivMessageParser::parse(readLine);

            //if message sender's nickname is not in spambots list
            //then parse his message as words and add it in words list
            if (!spambots_->contains(message.nickname))
            {
                if (textEdit_)
                    textEdit_->append(message.nickname + ": " + message.message);
                QStringList wordsInMessage = message.message.split(" ");
                QString wordToAdd;
                foreach (const QString& str, wordsInMessage)
                {
                    wordToAdd += (" " + str);
                    //30% chance that "word" will be a sentence of two words
                    //4% chance that "word" will be a sentence of three words
                    //1% of four words
                    //etc
                    int randomInt = rand()%100;
                    if (randomInt < 80)
                    {
                        if (!words_->contains(wordToAdd,Qt::CaseInsensitive))
                        {
                            words_->append(wordToAdd.replace("\r\n",""));
                            emit wordAdded(wordToAdd.replace("\r\n",""));
                        }
                        wordToAdd = "";
                    }
                }
                if (wordToAdd != "" && !words_->contains(wordToAdd,Qt::CaseInsensitive))
                {
                    words_->append(wordToAdd.replace("\r\n",""));
                    emit wordAdded(wordToAdd.replace("\r\n",""));
                }

                emit messageRead();

            }
        }
    }
}

//----------------------------------------------------------------------------------------


SpamMessageSocket::SpamMessageSocket(QObject *parent) :
    AbstractIrcSocket(parent)
{
    socket = 0;
    words_ = 0;
}

SpamMessageSocket::SpamMessageSocket(QString host, int port, QString channel, QString nickname, QString pass,
                                     QStringList *words, int minWords, int maxWords,
                                     bool emotesCap, int emotesCount, bool toLower) :
                                     AbstractIrcSocket(host, port,channel,nickname,pass,words)

{
    minWords_ = minWords;
    maxWords_ = maxWords;
    emotesCap_ = emotesCap;
    emotesCapValue_ = emotesCount;
    toLower_ = toLower;
    setupTwitchEmotes();
}

void SpamMessageSocket::sendMessage()
{
    //message prefix
    QString out = "PRIVMSG #" + channel_ + " :";
    //calculating words count: random between minWords_ and maxWords_
    int wordsCount = (qrand()%(maxWords_-minWords_+1))+minWords_;
    //picking up $wordsCount words from words_ list
    //and writing it into socket
    for (int i=0; i<wordsCount; i++)
    {
        out += words_->at(qrand()%words_->count());
        if (i != wordsCount-1)
            out+= " ";
    }
    out+="\r\n";
    out = preProcessMessage(out);
    socket->write(out.toUtf8());
    emit messageSent(nickname_, out);
}

void SpamMessageSocket::parseMessage()
{
    while(socket->canReadLine())
    {
        QString readLine = socket->readLine();
        //answering a PING command
        if (readLine.indexOf("PING ")!=-1)
        {
            readLine[1] = QChar('O');
            socket->write(readLine.toUtf8());
        }
    }
}

QString SpamMessageSocket::preProcessMessage(QString str)
{
    int countEmotes = 0;
    QStringList words = str.split(" ");
    for (QStringList::iterator i = words.begin(); i != words.end(); ++i)
    {
        bool isEmote = false;
        if (twitchEmotes.contains(*i))
        {
            countEmotes++;
            isEmote = true;
        }

        if (isEmote && emotesCap_ && (countEmotes > emotesCapValue_))
            (*i) = (std::rand()%5 == 1) ? "Я КiБЕРДЖИХАД" : "";
    }
    QString result = words.join(" ");
    if (toLower_)
        result = result.toLower();
    return result.replace("http://","").replace("www","").replace("."," ").replace("/"," !!! ");
}

void SpamMessageSocket::setupTwitchEmotes()
{
    twitchEmotes << ":)" << ":(" << ":o" << ":z" << "B)" << ":/" << ";)" << ";p" << ":p" << "R)" << "o_O" << ":D" << ">(" << "<3";

    QFile textFile("emotes.txt");
    if(!textFile.open(QIODevice::ReadOnly))
    {
        qDebug("Error");
        return;
    }

    QTextStream textStream(&textFile);
    while (true)
    {
        QString line = textStream.readLine();
        if (line.isNull())
            break;
        twitchEmotes.append(line);
    }
    textFile.close();
}

//-----------------------------------------------------------------------------------------------
SpamParams::SpamParams()
{
    _minWords = 0;
    _maxWords = 0;
    _emotesCap = false;
    _emotesCount = 0;
    _toLower = false;
    _cd = 0;
    _maxMessages = 0;
    _messageCount = 0;
}

SpamParams::SpamParams(QString host, int port, int minWords, int maxWords, bool emotesCap,
                     bool tolower, int emotesCount, int cd, int maxMessages)
{
    _host = host;
    _port = port;
    _minWords = minWords;
    _maxWords = maxWords;
    _emotesCap = emotesCap;
    _emotesCount = emotesCount;
    _toLower = tolower;
    _cd = cd;
    _maxMessages = maxMessages;
    _messageCount = 0;
    out = 0;
}

bool SpamParams::newMessage()
{
    _messageCount++;
    return !isOverCap();
}


SpamSystem::SpamSystem(QString host, int port, QString channel, QList<getAccountsResult::spamAccountDescriptor> &myAccounts,
                       QStringList &allAccounts, QStringList &bannedAccounts, QStringList &messageList, QTextEdit *out,
                       int minWords, int maxWords, bool emotesCap, int emotesCount,
                       bool toLower, int cd, int maxMessages, int beforeAttack)
{
    SpamParams p(host,port,minWords,maxWords,emotesCap,toLower,emotesCount,cd,maxMessages);
    params = p;
    params._channel = channel;
    params.out = out;
    params._beforeAttack = beforeAttack;
    myAccounts_ = myAccounts;
    allAccounts_ = allAccounts;
    bannedAccounts_ = bannedAccounts;
    messageList_ = messageList;
    state = IDLE;
    grabber = 0;
}

SpamSystem::~SpamSystem()
{

}

void SpamSystem::startAdaptive()
{
    state = CONNECTING;
    int grabberIndex = std::rand()%myAccounts_.count();
    if (grabber)
        delete grabber;
    grabber = new GrabMessageIrcSocket(params._host,params._port,params._channel,
                                       myAccounts_.at(grabberIndex).login,myAccounts_.at(grabberIndex).pass,
                                       &words_,&allAccounts_,params.out);
    myAccounts_.removeAt(grabberIndex);

    emit connectingState(myAccounts_.count());
    params._method = "adaptive";
    currentConnectNumber = 0;
    connect(&connectTimer,SIGNAL(timeout()),this,SLOT(connectNextSocket()));
    connectTimer.start(300);
    while (!spammers.isEmpty())
    {
        SpamMessageSocket* s = spammers.first();
        delete s;
        spammers.removeFirst();
    }
}

void SpamSystem::stop()
{
    connectTimer.stop();
    sendMessageTimer.stop();
}
void SpamSystem::socketConnected()
{
    static int socketConnectedCount = 0;
    socketConnectedCount++;
    emit spamSocketConnected(socketConnectedCount);
    if (socketConnectedCount == myAccounts_.count())
    {
        socketConnectedCount = 0;
        if (params._method == "adaptive")
        {
            state = READING;
            emit readingState(params._beforeAttack);
            grabber->connectToHost();
            connect(grabber,SIGNAL(messageRead()),this,SLOT(grabberReadMessage()));
            connect(grabber,SIGNAL(wordAdded(QString)),this,SLOT(grabberWordAdd(QString)));
        }
    }
}
void SpamSystem::connectNextSocket()
{
    SpamMessageSocket * s = new SpamMessageSocket(params._host,params._port,params._channel,
                                                  myAccounts_.at(currentConnectNumber).login,myAccounts_.at(currentConnectNumber).pass,
                                                  &words_,params._minWords,params._maxWords,params._emotesCap,params._emotesCount,params._toLower);
    connect(s,SIGNAL(connectSignal()),this,SLOT(socketConnected()));
    s->connectToHost();
    spammers.append(s);
    currentConnectNumber++;
    if (currentConnectNumber == myAccounts_.count())
    {
        connectTimer.stop();
    }
}

void SpamSystem::grabberReadMessage()
{
    static int grabberMessageReadCount = 0;
    grabberMessageReadCount++;
    emit readMessage(grabberMessageReadCount);
    if (grabberMessageReadCount == params._beforeAttack)
    {
        disconnect(grabber,SIGNAL(messageRead()),this,SLOT(grabberReadMessage()));
        state = SPAMMING;
        emit spammingState(params._maxMessages);
        connect(&sendMessageTimer,SIGNAL(timeout()),this,SLOT(sendMessage()));
        sendMessageTimer.start(params._cd);
    }
}

void SpamSystem::grabberWordAdd(QString word)
{
    emit wordAdded(word);
}

void SpamSystem::sendMessage()
{
    if (params.newMessage())
    {
        spammers.at(params._messageCount%spammers.count())->sendMessage();
        emit messageSent(params._messageCount);
    }
    else
    {
        emit stopped();
        disconnect(&sendMessageTimer,SIGNAL(timeout()),this,SLOT(sendMessage()));
        state = IDLE;
    }
}
