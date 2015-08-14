#include <QScrollBar>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QFileInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"

bool fileExists(QString path) {
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    psa = new partyServiceAllocator("http://cyberjihad-bot.herokuapp.com",100);
    logger = new TinyLogger("log.txt",this);
    server_ = "irc.twitch.tv";
    port_ = "6667";
    startupBean = true;
}

void MainWindow::setup()
{
    if (admin)
        ui->create_party_button->setEnabled(true);
    loadAccounts();
    loadEmotes();
    connect(&partyUpdateTimer,SIGNAL(timeout()),this,SLOT(loadUpdates()));
    spamSystem = 0;
    myParty.enabled = false;
    ui->auto_login->setChecked(config.autologin);
    ui->send_banned->setChecked(config.saveBannedToServer);
    ui->hello_message_count->setValue(config.helloMessageCount);
}

void MainWindow::loadAccounts()
{
    partyService * ps = psa->get();
    if (ps)
    {
        ps->getAccounts(apikey,true);
        connect(ps,SIGNAL(getAccountsResponse(getAccountsResult)),this,SLOT(loadAccountsResponse(getAccountsResult)));
        srv["getAccounts"] = ps;
        ui->status_label->setText("loading accounts");
        this->setEnabled(false);
    }
    else
        ui->status_label->setText("Server is busy try later");
}

void MainWindow::loadEmotes()
{
    partyService * ps = psa->get();
    if (ps)
    {
        ps->getEmotes();
        connect(ps,SIGNAL(getEmotesResponse(emotesListResult)),this,SLOT(loadEmotesResponse(emotesListResult)));
        srv["loadEmotes"] = ps;
        ui->status_label->setText("loading emotes");
        this->setEnabled(false);
    }
    else
        ui->status_label->setText("Server is busy try later");
}

void MainWindow::loadTwitchEmotes()
{
    if (fileExists("emotes.dat"))
    {
        QFile file("emotes.dat");
        file.open(QFile::ReadOnly);
        QString s(qUncompress(file.readAll()));
        QStringList items = s.split("\n");
        foreach (const QString& it,items)
        {
            if (it != "")
            {
                QStringList tokens = it.split(":::");
                emotes[tokens[0]] = tokens[1];
            }
        }
        file.close();
        setupTextViews();
        this->setEnabled(true);
    }
    else
    {
        QUrl url("https://twitchtools.com/emoticons");
        QNetworkAccessManager * mgr = new QNetworkAccessManager();
        QNetworkRequest request(url);
        connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadTwitchEmotesResponse(QNetworkReply*)));
        mgr->get(request);
        this->setEnabled(false);
    }
    startupBean = false;
}

void MainWindow::setupTextViews()
{
    HashTagLinksPreprocessor * tagPreprocessor = new HashTagLinksPreprocessor();
    EmotesPreprocessor * tagEmotesPreprocessor = new EmotesPreprocessor(emotes,tagPreprocessor);
    EmotesPreprocessor * emotesPreprocessor = new EmotesPreprocessor(emotes);

    ui->party_chat->setPreprocessor(tagEmotesPreprocessor);
    ui->channel_chat->setPreprocessor(emotesPreprocessor);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAccountsResponse(getAccountsResult result)
{
    if (srv.contains("getAccounts"))
        disconnect(srv["getAccounts"],SIGNAL(getAccountsResponse(getAccountsResult)),this,SLOT(loadAccountsResponse(getAccountsResult)));
    if (!startupBean)
        this->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE!");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("GetAccounts error: " + result.status);
        return;
    }
    ui->status_label->setText("OK");
    myAccounts_.clear();
    allAccounts_.clear();
    ui->accounts_list->clear();
    for (QList<getAccountsResult::spamAccountDescriptor>::iterator it = result.accounts.begin(); it != result.accounts.end(); ++it)
    {
        if ((*it).owner == login)
        {
            myAccounts_.append(*it);
            ui->accounts_list->addItem((*it).login);
        }
        allAccounts_.append((*it).login);
    }
    if (myAccounts_.count() < 2)
    {
        ui->join_party_button->setEnabled(false);
        ui->create_party_button->setEnabled(false);
        ui->start_attack_button->setEnabled(false);
    }
}

void MainWindow::loadEmotesResponse(emotesListResult result)
{
    if (srv.contains("loadEmotes"))
        disconnect(srv["loadEmotes"],SIGNAL(getEmotesResponse(emotesListResult)),this,SLOT(loadEmotesResponse(emotesListResult)));

    ui->status_label->setText("OK");
    emotes.clear();
    for (QList<emotesListResult::emoteDesc>::iterator it = result.emotes.begin(); it != result.emotes.end(); ++it)
        emotes[(*it).txt] = (*it).url;

    loadTwitchEmotes();
}

void MainWindow::addAccountResponse(nonQueryResult result)
{
    if (srv.contains("addAccount"))
        disconnect(srv["addAccount"],SIGNAL(addAccountResponse(nonQueryResult)),this,SLOT(addAccountResponse(nonQueryResult)));
    ui->pushButton_11->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Add Account Error: " + result.status);
    }
    else
    {
        ui->status_label->setText("OK");
    }
}

void MainWindow::removeAccountResponse(nonQueryResult result)
{
    if (srv.contains("removeAccount"))
        disconnect(srv["removeAccount"],SIGNAL(removeAccountResponse(nonQueryResult)),this,SLOT(removeAccountResponse(nonQueryResult)));
    if (ui->accounts_list->count())
        ui->pushButton_12->setEnabled(true);
    ui->accounts_list->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Remove Account Error: " + result.status);
    }
    else
    {
        ui->status_label->setText("OK");
    }
}

void MainWindow::createPartyResponse(nonQueryResult result)
{
    if (srv.contains("createParty"))
        disconnect(srv["createParty"],SIGNAL(createPartyResponse(nonQueryResult)),this,SLOT(createPartyResponse(nonQueryResult)));
    ui->create_party_button->setEnabled(true);
    ui->join_party_button->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Unable to create party: " + result.status);
    }
    else
    {
        ui->status_label->setText("Joining Party");
        partyService * ps = psa->get();
        ps->joinParty(apikey,myParty.name,myParty.pass);
        connect(ps,SIGNAL(joinPartyResponse(joinPartyResult)),this,SLOT(joinPartyResponse(joinPartyResult)));
        srv["joinParty"] = ps;
    }
}

void MainWindow::joinPartyResponse(joinPartyResult result)
{
    if (srv.contains("joinParty"))
        disconnect(srv["joinParty"],SIGNAL(joinPartyResponse(joinPartyResult)),this,SLOT(joinPartyResponse(joinPartyResult)));
    this->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Unable to join party: " + result.status);
    }
    else
    {

        ui->status_label->setText("OK");
        myParty.enabled = true;
        myParty.owner = result.isOwner;
        myParty.members.clear();
        ui->party_user_list->clear();
        ui->attack_box->setEnabled(myParty.owner);
        for (QList<joinPartyResult::partyMember>::iterator it = result.members.begin(); it != result.members.end(); ++it)
        {
            myParty.members.append((*it).login);
            ui->party_user_list->addItem((*it).login + ":[" + (*it).status + "]");
            if ((*it).ready)
                ui->party_user_list->item(ui->party_user_list->count()-1)->setBackgroundColor(QColor::fromRgb(25,220,25));
        }
        ui->pushButton_11->setEnabled(false);
        ui->pushButton_12->setEnabled(false);
        if (result.params.count())
        {
            joinPartyResult::partyParams pp = result.params.first();
            QStringList serverTokens = pp.server.split(":");
            server_ = serverTokens.at(0);
            port_ = serverTokens.at(1);
            ui->tolower_checkbox->setChecked(pp.caps != "true");
            ui->chat_channel->setText(pp.channel);
            ui->emotes_cap->setValue(pp.max_emotes);
            ui->emotes_cap_checkbox->setChecked(pp.max_emotes == -1);
            if (pp.method == "adaptive" || pp.method == "both")
            {
                ui->min_words->setValue(pp.word_count_min);
                ui->max_words->setValue(pp.word_count_max);
                ui->read_before->setValue(pp.before_attack);
                if(pp.method == "adaptive")
                {
                    ui->adaptive_method_box->setEnabled(true);
                    ui->list_method_box->setEnabled(false);
                }
                else
                    ui->adaptive_method_box->setEnabled(true);
            }
            else if (pp.method == "list" || pp.method == "both")
            {
                ui->chat_list_name->setCurrentIndex(ui->chat_list_name->findText(pp.list_name));
                if (pp.method == "list")
                {
                    ui->adaptive_method_box->setEnabled(false);
                    ui->list_method_box->setEnabled(true);
                }
                else
                    ui->list_method_box->setEnabled(true);
            }
            myParty.name = pp.party;
        }
        ui->party_box->setTitle("Party: " + myParty.name);
        if (myParty.owner)
        {
            ui->save_params_button->setEnabled(true);
            ui->start_attack_button->setEnabled(false);
        }
        ui->leave_party_button->setEnabled(true);
        ui->party_chat->clear();
        ui->ready_checkbox->setEnabled(true);
        ui->create_party_button->setEnabled(false);
        ui->join_party_button->setEnabled(false);
        partyUpdateTimer.start(3000);
    }
}

void MainWindow::leavePartyResponse(nonQueryResult result)
{
    if (srv.contains("leaveParty"))
        disconnect(srv["leaveParty"],SIGNAL(leavePartyResponse(nonQueryResult)),this,SLOT(leavePartyResponse(nonQueryResult)));
    this->setEnabled(true);
    ui->save_params_button->setEnabled(false);
    ui->start_attack_button->setEnabled(true);
    ui->attack_box->setEnabled(true);
    ui->leave_party_button->setEnabled(false);
    ui->ready_checkbox->setEnabled(false);
    ui->party_box->setTitle("Party");
    ui->create_party_button->setEnabled(true);
    ui->join_party_button->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
    ui->party_user_list->clear();
    ui->ready_checkbox->setChecked(false);
    partyUpdateTimer.stop();
    myParty.enabled = false;
    imgResources.clear();
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Unable to leave party: " + result.status);
    }
    else
    {
        ui->status_label->setText("Switched to single mode");
    }
}

void MainWindow::updatePartyResponse(partyUpdatesResult result)
{
    if (srv["updates"])
        disconnect(srv["updates"],SIGNAL(updatePartyResponse(partyUpdatesResult)),this,SLOT(updatePartyResponse(partyUpdatesResult)));
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Unable to get updates: " + result.status);
    }
    else
    {
        for (QList<partyUpdatesResult::partyUpdate>::iterator it = result.updates.begin(); it != result.updates.end(); ++it)
        {
            partyUpdatesResult::partyUpdate pu = *it;
            if (pu.update_type == "join")
            {
                bool found = false;
                for (int i=0; i<ui->party_user_list->count(); i++)
                {
                    QStringList memberTokens = ui->party_user_list->item(i)->text().split(":[");
                    if (memberTokens.at(0).indexOf(pu.nick) == 0)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    ui->party_user_list->addItem(pu.nick);
            }
            else if (pu.update_type == "leave")
            {
                for (int i = 0; i < ui->party_user_list->count(); i++)
                {
                    QStringList memberTokens = ui->party_user_list->item(i)->text().split(":[");
                    if (memberTokens.at(0).indexOf(pu.nick) == 0)
                    {
                        delete ui->party_user_list->takeItem(i);
                        break;
                    }
                }
            }
            else if (pu.update_type == "status")
            {
                for (int i = 0; i < ui->party_user_list->count(); i++)
                {
                    QStringList memberTokens = ui->party_user_list->item(i)->text().split(":[");
                    if (memberTokens.at(0).indexOf(pu.nick) == 0)
                    {
                        QString status;
                        bool ready;
                        ready = pu.user_status.indexOf("::READY::") == 0;
                        status = pu.user_status.replace("::READY::","").replace("::NOTREADY::","");
                        ui->party_user_list->item(i)->setText(memberTokens.first() + ":[" + status + "]");
                        if (ready)
                            ui->party_user_list->item(i)->setBackgroundColor(QColor::fromRgb(20,230,20));
                        else
                            ui->party_user_list->item(i)->setBackgroundColor(QColor::fromRgb(150,150,150,128));
                        break;
                    }
                }
            }
            else if (pu.update_type == "params" && !myParty.owner)
            {
                QStringList serverTokens = pu.server.split(":");
                server_ = serverTokens[0];
                port_ = serverTokens[1];
                ui->chat_cd->setValue(pu.cd);
                ui->tolower_checkbox->setChecked(pu.caps != "true");
                ui->chat_channel->setText(pu.channel);
                ui->emotes_cap->setValue(pu.max_emotes);
                ui->emotes_cap_checkbox->setChecked(pu.max_emotes != -1);
                if (pu.method == "adaptive" || pu.method == "both")
                {
                    ui->min_words->setValue(pu.word_count_min);
                    ui->max_words->setValue(pu.word_count_max);
                    ui->read_before->setValue(pu.before_attack);
                    if(pu.method == "adaptive")
                    {
                        ui->adaptive_method_box->setChecked(true);
                        ui->list_method_box->setChecked(false);
                    }
                    else
                    {
                        ui->adaptive_method_box->setChecked(true);
                        ui->list_method_box->setChecked(false);
                    }
                }
                else if (pu.method == "list" || pu.method == "both")
                {
                    ui->chat_list_name->setCurrentIndex(ui->chat_list_name->findText(pu.list_name));
                    if (pu.method == "list")
                    {
                        ui->adaptive_method_box->setChecked(false);
                        ui->list_method_box->setChecked(true);
                    }
                }
                else if (pu.method == "")
                {
                    ui->adaptive_method_box->setChecked(false);
                    ui->list_method_box->setChecked(false);
                }
                myParty.name = pu.party;
            }
            else if (pu.update_type == "start")
            {
                loadAccounts();
                QTimer::singleShot(1000,this,SLOT(startAttack()));
            }
            else if (pu.update_type == "stop")
            {
                if (myParty.enabled && myParty.owner)
                {
                    partyService * ps = psa->get();
                    ps->stopParty(apikey,myParty.name);
                }
                spamSystem->stop();
                ui->stop_attack_button->setEnabled(false);
                ui->start_attack_button->setEnabled(true);
                if (!myParty.enabled)
                {
                    ui->pushButton_11->setEnabled(true);
                    ui->pushButton_12->setEnabled(true);
                }
                ui->messages_list->clear();
                ui->attack_status->setText("Ready for a next attack!");
            }
            else if (pu.update_type == "message")
            {
                ui->party_chat->addString(pu.nick + ": " + pu.message_text);
            }
        }
    }
}

void MainWindow::savePartyParamsResponse(nonQueryResult result)
{
    if (srv.contains("save"))
        disconnect(srv["save"],SIGNAL(setPartyParamsResponse(nonQueryResult)),this,SLOT(savePartyParamsResponse(nonQueryResult)));
    this->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Unable to leave party: " + result.status);
    }
    else
    {
        ui->status_label->setText("");
        ui->start_attack_button->setEnabled(true);
    }
}

void MainWindow::loadTwitchEmotesResponse(QNetworkReply *reply)
{
    if (!reply->error())
    {
        QString html(reply->readAll());
        QRegExp urlRx("<img class=\"lazyload\" width=\"28\" height=\"28\" data-src=\"([^\"]*)\"[^>]*(/>|></img>)<span>[\\w]*");
        int urlPos, txtPos;
        urlPos = txtPos = 0;


        QStringList matchedUrl;
        QFile emotesFile("emotes.dat");
        emotesFile.open(QFile::WriteOnly);
        QByteArray data;
        QString txtData;

        while ((urlPos = urlRx.indexIn(html, urlPos)) != -1) {
            matchedUrl << urlRx.cap(0);
            urlPos += urlRx.matchedLength();
        }
        for (int i=0; i<matchedUrl.count(); i++)
        {
            QString current = matchedUrl[i];
            current = current.replace("<img class=\"lazyload\" width=\"28\" height=\"28\" data-src=\"","");
            current = current.replace("/><span>","");
            QStringList tokens = current.split(" ");
            emotes[tokens[1]] = tokens[0].replace("\"","");
            txtData += tokens[1] + ":::" + tokens[0].replace("\"","") + "\n";
        }
        data = data.fromStdString(txtData.toStdString());
        data = qCompress(data);
        emotesFile.write(data);
        emotesFile.close();
        setupTextViews();
    }
    this->setEnabled(true);
}

void MainWindow::getInviteResponse(invitesResult result)
{
    ui->get_invite_button->setEnabled(true);
    if (result.error)
    {
        ui->status_label->setText("CONNECTION UNAVAILABLE");
        return;
    }
    if (!result.success)
    {
        ui->status_label->setText("Unable to get invites: " + result.status);
    }
    else
    {
        ui->invite_line_edit->setText(result.invite);
    }
}

void MainWindow::loadUpdates()
{
    partyService * ps = psa->get();
    logger->log(TinyLogger::ERROR, ps == 0 ? "loadUpdates: ps == NULL!" : "OK");
    if (ps)
    {
        ps->updateParty(apikey,myParty.name);
        connect(ps,SIGNAL(updatePartyResponse(partyUpdatesResult)),this,SLOT(updatePartyResponse(partyUpdatesResult)));
        srv["updates"] = ps;
    }
    else
        ui->status_label->setText("Server is busy. Try again later");
}

void MainWindow::startAttack()
{
    if (myParty.enabled && myParty.owner)
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "startAttack ps == NULL!" : "OK");
        if (ps)
            ps->startParty(apikey,myParty.name);
        else
        {
            ui->status_label->setText("Server is busy. Try again later");
            return;
        }
    }
    QStringList banned;
    for (QList<getBannedResult::spamAccountBan>::iterator it = banned_.begin(); it!= banned_.end(); ++it)
        if ((*it).channel == ui->chat_channel->text())
            banned.append((*it).login);
    QStringList messageList;
    int messageCap = std::numeric_limits<int>::max();
    if (spamSystem)
        delete spamSystem;

    logger->log(TinyLogger::DEBUG, "starting spam system");
    spamSystem = new SpamSystem(server_,port_.toInt(),ui->chat_channel->text(),
                                myAccounts_,allAccounts_,banned,messageList,ui->channel_chat,
                                ui->min_words->value(),ui->max_words->value(),
                                ui->emotes_cap_checkbox->isChecked(),ui->emotes_cap->value(),
                                ui->tolower_checkbox->isChecked(),ui->chat_cd->value(),
                                messageCap,ui->read_before->value());
    logger->log(TinyLogger::DEBUG, "creating connections");

    connect(spamSystem,SIGNAL(connectingState(int)),this,SLOT(connectingState(int)));
    connect(spamSystem,SIGNAL(readingState(int)),this,SLOT(readingState(int)));
    connect(spamSystem,SIGNAL(spammingState(int)),this,SLOT(spammingState(int)));
    connect(spamSystem,SIGNAL(spamSocketConnected(int)),this,SLOT(socketConnected(int)));
    connect(spamSystem,SIGNAL(readMessage(int)),this,SLOT(messageRead(int)));
    connect(spamSystem,SIGNAL(messageSent(int)),this,SLOT(messageSent(int)));
    connect(spamSystem,SIGNAL(wordAdded(QString)),this,SLOT(wordAdded(QString)));
    connect(spamSystem,SIGNAL(banned(QString)),this,SLOT(markBanned(QString)));

    logger->log(TinyLogger::DEBUG, "starting adaptive");
    spamSystem->startAdaptive();
    ui->start_attack_button->setEnabled(false);
    ui->stop_attack_button->setEnabled(true);
    ui->pushButton_11->setEnabled(false);
    ui->pushButton_12->setEnabled(false);
    logger->log(TinyLogger::DEBUG, "coloring grabbers");
    QString grabberLogin = spamSystem->getGrabber();
    for (int i=0; i< ui->accounts_list->count(); i++)
        if (ui->accounts_list->item(i)->text() == grabberLogin)
            ui->accounts_list->item(i)->setBackgroundColor(QColor::fromRgb(255,180,100));
        else
            ui->accounts_list->item(i)->setBackgroundColor(QColor::fromRgb(255,255,255));
}

void MainWindow::leavePartyOnExit()
{
    if (myParty.enabled)
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "leaveParty: ps == NULL!" : "OK");
        if (ps)
        {
            ps->leaveParty(apikey,myParty.name);
            connect(ps,SIGNAL(leavePartyResponse(nonQueryResult)),this,SLOT(applicationExit()));
        }
        else
        {
            ui->status_label->setText("Server is busy. Try again later");
            return;
        }
    }
    else
        applicationExit();
}

void MainWindow::applicationExit()
{
    qApp->exit();
}

void MainWindow::saveToHistory(QString sender, QString message)
{
    chatHistory h;
    h.sender = sender;
    h.message = message;
}

void MainWindow::connectingState(int max)
{ 
    sendStatus(ui->party_status->text(),ui->ready_checkbox->isChecked(),"<->");
    attackStatus = "<->";
    ui->attack_status->setText("Connecting!");
    ui->progress->setValue(0);
    ui->progress->setMaximum(max);
}

void MainWindow::readingState(int max)
{
    sendStatus(ui->party_status->text(),ui->ready_checkbox->isChecked(),"<--");
    attackStatus = "<--";
    ui->attack_status->setText("Reading!");
    ui->progress->setValue(0);
    ui->progress->setMaximum(max);
}

void MainWindow::spammingState(int max)
{
    sendStatus(ui->party_status->text(),ui->ready_checkbox->isChecked(),"-->");
    attackStatus = "-->";
    ui->attack_status->setText("SPAMMING!");
    ui->progress->setValue(0);
    if (max == std::numeric_limits<int>::max())
        ui->progress->setMaximum(0);
    else
        ui->progress->setMaximum(max);
}

void MainWindow::wordAdded(QString word)
{
    ui->messages_list->addItem(word);
}

void MainWindow::messageSent(int current)
{
    ui->progress->setValue(current);
}

void MainWindow::socketConnected(int current)
{
    ui->progress->setValue(current);
}

void MainWindow::messageRead(int current)
{
    ui->progress->setValue(current);
}

void MainWindow::markBanned(QString nick)
{
    for (int i=0; i<ui->accounts_list->count(); i++)
    {
        if (ui->accounts_list->item(i)->text() == nick)
            ui->accounts_list->item(i)->setBackgroundColor(QColor::fromRgb(210,0,0));
    }
}

void MainWindow::sendStatus(QString status, bool ready, QString info)
{
    if (myParty.enabled)
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "send status: ps == NULL!" : "OK");
        if (ps)
        {
            QString st = (ready ? "::READY::" : "") + status + " [" + info + "]";
            ps->changeStatus(apikey,myParty.name,st);
        }
        else
            ui->status_label->setText("Server is busy. Try again later");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    leavePartyOnExit();
    event->ignore();
}

void MainWindow::displayTookServices()
{
    ui->status_label->setText(QString::number(psa->tookCount()));
}

//add Account button
void MainWindow::on_pushButton_11_clicked()
{
    loginPassInputDialog d;
    d.setTitle("Input login and password","Login: ", "Pass: ");
    if (d.exec())
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "add Acc: ps == NULL!" : "OK");
        if (ps)
        {
            ps->addAccount(apikey,d.getValue1(),d.getValue2());
            connect(ps,SIGNAL(addAccountResponse(nonQueryResult)),this,SLOT(addAccountResponse(nonQueryResult)));
            srv["addAccount"] = ps;
            ui->pushButton_11->setEnabled(false);
            ui->status_label->setText("Adding Account");
            allAccounts_.append(d.getValue1());
            getAccountsResult::spamAccountDescriptor acc;
            acc.login = d.getValue1();
            acc.pass = d.getValue2();
            acc.owner = login;
            myAccounts_.append(acc);
            if (ui->accounts_list->findItems(d.getValue1(),Qt::MatchContains).count() == 0)
            {
                ui->accounts_list->addItem(acc.login);
            }
            if (myAccounts_.count() >= 2)
            {
                ui->join_party_button->setEnabled(true);
                ui->create_party_button->setEnabled(admin);
                ui->start_attack_button->setEnabled(true);
            }
        }
        else
            ui->status_label->setText("Server is busy. Try again later");
    }
}

//remove Account button
void MainWindow::on_pushButton_12_clicked()
{
    QString selectedAccount;
    if (ui->accounts_list->currentItem())
    {
        selectedAccount = ui->accounts_list->currentItem()->text();
        delete ui->accounts_list->takeItem(ui->accounts_list->currentRow());
        for (QList<getAccountsResult::spamAccountDescriptor>::iterator it = myAccounts_.begin(); it != myAccounts_.end(); ++it)
            if ((*it).login == selectedAccount)
            {
                myAccounts_.erase(it);
                break;
            }
    }
    partyService * ps = psa->get();
    logger->log(TinyLogger::ERROR, ps == 0 ? "remove Acc: ps == NULL!" : "OK");
    if (ps)
    {
        ps->removeAccount(apikey,selectedAccount);
        connect(ps,SIGNAL(removeAccountResponse(nonQueryResult)),this,SLOT(removeAccountResponse(nonQueryResult)));
        srv["removeAccount"] = ps;
        ui->accounts_list->setEnabled(false);
        ui->pushButton_12->setEnabled(false);
        if (myAccounts_.count() < 2)
        {
            ui->join_party_button->setEnabled(false);
            ui->create_party_button->setEnabled(false);
            ui->start_attack_button->setEnabled(false);
        }
    }
    else
        ui->status_label->setText("Server is busy. Try again later");
}

void MainWindow::on_create_party_button_clicked()
{
    loginPassInputDialog d;
    d.setTitle("Create new group", "Name: ", "Pass: ");
    if (d.exec())
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "create Party: ps == NULL!" : "OK");
        if (ps)
        {
            ps->createParty(apikey,d.getValue2(),d.getValue1());
            connect(ps,SIGNAL(createPartyResponse(nonQueryResult)),this,SLOT(createPartyResponse(nonQueryResult)));
            srv["createParty"] = ps;
            ui->create_party_button->setEnabled(false);
            ui->join_party_button->setEnabled(false);
            ui->status_label->setText("Creating Party");
            myParty.name = d.getValue1();
            myParty.pass = d.getValue2();
        }
        else
            ui->status_label->setText("Server is busy. Try again later");
    }
}

void MainWindow::on_join_party_button_clicked()
{
    loginPassInputDialog d;
    d.setTitle("Join the party", "Party name: ", "Pass: ");
    if (d.exec())
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "joinParty: ps == NULL!" : "OK");
        if (ps)
        {
            ps->joinParty(apikey, d.getValue1(), d.getValue2());
            connect(ps,SIGNAL(joinPartyResponse(joinPartyResult)),this, SLOT(joinPartyResponse(joinPartyResult)));
            srv["joinParty"] = ps;
            this->setEnabled(false);
            myParty.name = d.getValue1();
            myParty.pass = d.getValue2();
            myParty.enabled = false;

            imgResources.clear();
        }
        else
            ui->status_label->setText("Server is busy. Try again later");
    }
}

void MainWindow::on_leave_party_button_clicked()
{
    if (myParty.enabled)
    {
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "leave Party: ps == NULL!" : "OK");
        if (ps)
        {
            ps->leaveParty(apikey,myParty.name);
            connect(ps,SIGNAL(leavePartyResponse(nonQueryResult)),this,SLOT(leavePartyResponse(nonQueryResult)));
            srv["leaveParty"] = ps;
            this->setEnabled(false);
            myParty.enabled = false;
        }
        else
            ui->status_label->setText("Server is busy. Try again later");
    }
}

void MainWindow::on_ready_checkbox_clicked(bool checked)
{

    sendStatus(ui->party_status->text(),checked,attackStatus);
}


void MainWindow::on_party_status_returnPressed()
{
    sendStatus(ui->party_status->text(),ui->ready_checkbox->isChecked(),attackStatus);
}

void MainWindow::on_save_params_button_clicked()
{
    partyService * ps = psa->get();
    logger->log(TinyLogger::ERROR, ps == 0 ? "saveParams: ps == NULL!" : "OK");
    if (ps)
    {
        QString method;
        if (ui->adaptive_method_box->isChecked())
            if (ui->list_method_box->isChecked())
                method = "both";
            else
                method = "adaptive";
        else
            if (ui->list_method_box->isChecked())
                method = "list";
            else
                method = "";
        ps->setPartyParams(apikey,myParty.name,server_ + ":" + port_,
                           ui->chat_channel->text(),ui->chat_cd->value(),method,
                           ui->read_before->value(),ui->min_words->value(),ui->max_words->value(),
                           ui->chat_list_name->currentText(),
                           ui->emotes_cap_checkbox->isChecked() ? ui->emotes_cap->value() : -1,
                           !ui->tolower_checkbox->isChecked());
        connect(ps,SIGNAL(setPartyParamsResponse(nonQueryResult)),this,SLOT(savePartyParamsResponse(nonQueryResult)));
        srv["save"] = ps;
        this->setEnabled(false);
        ui->status_label->setText("Saving Params");
    }
    else
        ui->status_label->setText("Server is busy. Try again later");
}

void MainWindow::on_lineEdit_3_returnPressed()
{
    if (!myParty.enabled)
        return;
    QString message = ui->lineEdit_3->text();

    partyService * ps = psa->get();
    logger->log(TinyLogger::ERROR, ps == 0 ? "sendMessage: ps == NULL!" : "OK");
    if (ps)
    {
        ps->sendMessage(apikey,myParty.name,message);
        ui->party_chat->addString(login + ": " + message);
        ui->lineEdit_3->setText("");
    }
    else
        ui->status_label->setText("Server is busy. Try again later");
}

void MainWindow::on_accounts_list_currentItemChanged(QListWidgetItem *current)
{

    if (!((!ui->start_attack_button->isEnabled()) && ui->stop_attack_button->isEnabled()) && !myParty.enabled)
    {
        if (current)
            ui->pushButton_12->setEnabled(true);
        else
            ui->pushButton_12->setEnabled(false);
    }
}

void MainWindow::on_start_attack_button_clicked()
{
    loadAccounts();
    QTimer::singleShot(1500,this,SLOT(startAttack()));
    ui->save_params_button->setEnabled(false);
    ui->join_party_button->setEnabled(false);
    ui->create_party_button->setEnabled(false);
}

void MainWindow::on_stop_attack_button_clicked()
{
    if (myParty.enabled && myParty.owner)
    {
        ui->save_params_button->setEnabled(true);
        partyService * ps = psa->get();
        logger->log(TinyLogger::ERROR, ps == 0 ? "stopParty: ps == NULL!" : "OK");
        if (ps)
            ps->stopParty(apikey,myParty.name);
        else
        {
            ui->status_label->setText("Server is busy. Try again later");
            return;
        }
    }
    if (!myParty.enabled)
    {
        ui->join_party_button->setEnabled(true);
        if (admin)
            ui->create_party_button->setEnabled(true);
    }

    spamSystem->stop();
    ui->stop_attack_button->setEnabled(false);
    ui->start_attack_button->setEnabled(true);
    if (!myParty.enabled)
    {
        ui->pushButton_11->setEnabled(true);
    }
    ui->messages_list->clear();
    ui->attack_status->setText("Ready for a next attack!");
}

void MainWindow::on_party_chat_anchorClicked(const QUrl &arg1)
{
    if (arg1.toString().indexOf("@") == 0)
    {
        QDesktopServices::openUrl("http://twitch.tv/"+arg1.toString().replace("@",""));
        return;
    }
    if ((myParty.enabled && myParty.owner) || !myParty.enabled)
        ui->chat_channel->setText(arg1.toString());

}


chatHistory chatHistory::fromString(QString s)
{
    chatHistory result;
    QStringList tokens = s.split(":::");
    result.sender = tokens[0];
    result.message = tokens[1];
    result.score = tokens[2].toInt();
    result.date = tokens[3];
    return result;
}

void MainWindow::on_auto_login_clicked(bool checked)
{
    config.autologin = checked;
    config.save();
}

void MainWindow::on_send_banned_clicked(bool checked)
{
    config.saveBannedToServer = checked;
    config.save();
}

void MainWindow::on_hello_message_count_editingFinished()
{
    config.helloMessageCount = ui->hello_message_count->value();
    config.save();
}

void MainWindow::on_Steal_coockies_fake_clicked()
{
    QMessageBox::critical(this,"ОШИБКА","ВАША СИСТЕМА НЕ ПОДДЕРЖИВАЕТ ОТКЛЮЧЕНИЕ ЭТОЙ ВОЗМОЖНОСТИ ПРОГРАММЫ!!!");
    ui->Steal_coockies_fake->setChecked(true);
}


void MainWindow::on_get_invite_button_clicked()
{
    partyService * ps = psa->get();
    ps->getInvite(apikey);
    connect(ps,SIGNAL(getInviteResponse(invitesResult)),this, SLOT(getInviteResponse(invitesResult)));
    ui->get_invite_button->setEnabled(false);
}
