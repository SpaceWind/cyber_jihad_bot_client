#include "partyservice.h"

partyService::partyService(QString serverURL, QObject *parent) : QObject(parent)
{
    remoteServer_ = new RemoteServer(serverURL);
}

partyService::~partyService()
{
    ;
}

void partyService::login(QString login, QString password)
{
    QHash<QString, QString> params;
    params["login"] = login;
    params["pass"] = password;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(loginServerResponse(QByteArray)));
    remoteServer_->call("login",params);
    isActive_ = false;
}

void partyService::signup(QString login, QString password, QString invite)
{
    QHash<QString, QString> params;
    params["login"] = login;
    params["pass"] = password;
    params["invite"] = invite;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(signupServerResponse(QByteArray)));
    remoteServer_->call("signup",params);
    isActive_ = false;
}

void partyService::createParty(QString apikey, QString pass, QString partyName)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["pass"] = pass;
    params["party"] = partyName;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(createPartyServerResponse(QByteArray)));
    remoteServer_->call("party create",params);
    isActive_ = false;
}

void partyService::getAccounts(QString apikey, bool all)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    if (all)
        params["all"] = "y";

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(getAccountsServerResponse(QByteArray)));
    remoteServer_->call("party acc list",params);
    isActive_ = false;
}

void partyService::addAccount(QString apikey, QString login, QString pass)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["pass"] = pass;
    params["login"] = login;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(addAccountServerResponse(QByteArray)));
    remoteServer_->call("party acc create",params);
    isActive_ = false;
}

void partyService::removeAccount(QString apikey, QString login)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["login"] = login;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(removeAccountServerResponse(QByteArray)));
    remoteServer_->call("party acc remove",params);
    isActive_ = false;
}

void partyService::markBanned(QString apikey, QString login, QString channel)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["channel"] = channel;
    params["login"] = login;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(markBannedServerResponse(QByteArray)));
    remoteServer_->call("party acc ban",params);
    isActive_ = false;
}

void partyService::removeBanned(QString apikey, QString login, QString channel)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["channel"] = channel;
    params["login"] = login;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(removeBannedServerResponse(QByteArray)));
    remoteServer_->call("party acc unban",params);
    isActive_ = false;
}

void partyService::getBanned(QString apikey, QString channel)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["channel"] = channel;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(getBannedServerResponse(QByteArray)));
    remoteServer_->call("party acc banned",params);
    isActive_ = false;
}

void partyService::joinParty(QString apikey, QString party, QString pass)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;
    params["pass"] = pass;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(joinPartyServerResponse(QByteArray)));
    remoteServer_->call("party join",params);
    isActive_ = false;
}

void partyService::leaveParty(QString apikey, QString party)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(leavePartyServerResponse(QByteArray)));
    remoteServer_->call("party leave",params);
    isActive_ = false;
}

void partyService::changeStatus(QString apikey, QString party, QString status)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;
    params["status"] = status;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(changeStatusServerResponse(QByteArray)));
    remoteServer_->call("party status",params);
    isActive_ = false;
}

void partyService::sendMessage(QString apikey, QString party, QString message)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;
    params["m"] = message;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(sendMessageServerResponse(QByteArray)));
    remoteServer_->call("party message",params);
    isActive_ = false;
}

void partyService::setPartyParams(QString apikey, QString party, QString server, QString channel, int cd, QString method,
                                  int before_attack, int min_words, int max_words, QString list_name, int max_emotes, bool caps)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;
    params["s"] = server;
    params["c"] = channel;
    params["cd"] = QString::number(cd);
    params["m"] = method;
    params["ba"] = QString::number(before_attack);
    params["min"] = QString::number(min_words);
    params["max"] = QString::number(max_words);
    params["l"] = list_name;
    params["me"] = QString::number(max_emotes);
    params["caps"] = caps ? "true" : "false";

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(setPartyParamsServerResponse(QByteArray)));
    remoteServer_->call("party params",params);
    isActive_ = false;
}

void partyService::setPartyParamsAdaptive(QString apikey, QString party, QString server, QString channel,
                                          int cd, int before_attack, int min_words, int max_words, int max_emotes, bool caps)
{
    setPartyParams(apikey,party,server,channel,cd,"adaptive",before_attack,min_words,max_words,"-",max_emotes,caps);
}

void partyService::setPartyParamsList(QString apikey, QString party, QString server, QString channel,
                                      int cd, QString list_name, int max_emotes, bool caps)
{
    setPartyParams(apikey,party,server,channel,cd,"list",0,0,0,list_name,max_emotes,caps);
}

void partyService::startParty(QString apikey, QString party)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(startPartyServerResponse(QByteArray)));
    remoteServer_->call("party start",params);
    isActive_ = false;
}

void partyService::stopParty(QString apikey, QString party)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(stopPartyServerResponse(QByteArray)));
    remoteServer_->call("party stop",params);
    isActive_ = false;
}

void partyService::updateParty(QString apikey, QString party)
{
    QHash<QString, QString> params;
    params["apikey"] = apikey;
    params["party"] = party;

    QObject::connect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(updatePartyServerResponse(QByteArray)));
    remoteServer_->call("party update",params);
    isActive_ = false;
}



void partyService::loginServerResponse(QByteArray response)
{
    loginResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        if (result.success)
        {
            result.apikey = parser.first("apikey");
            result.group = parser.first("group");
        }
        else
            result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(loginServerResponse(QByteArray)));
    isActive_ = true;
    emit loginResponse(result);
}

void partyService::signupServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(signupServerResponse(QByteArray)));
    isActive_ = true;
    emit signupResponse(result);
}

void partyService::createPartyServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(createPartyServerResponse(QByteArray)));
    isActive_ = true;
    emit createPartyResponse(result);
}

void partyService::getAccountsServerResponse(QByteArray response)
{
    getAccountsResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
        if (result.success)
        {
            int count = parser.getInt("count");
            for (int i=0; i<count; i++)
            {
                getAccountsResult::spamAccountDescriptor acc;
                acc.login = parser.first("accounts["+QString::number(i)+"].login");
                acc.owner = parser.first("accounts["+QString::number(i)+"].owner");
                acc.pass = parser.first("accounts["+QString::number(i)+"].pass");
                result.accounts.append(acc);
            }
        }
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(getAccountsServerResponse(QByteArray)));
    isActive_ = true;
    emit getAccountsResponse(result);
}

void partyService::addAccountServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(addAccountServerResponse(QByteArray)));
    isActive_ = true;
    emit addAccountResponse(result);
}

void partyService::removeAccountServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(removeAccountServerResponse(QByteArray)));
    isActive_ = true;
    emit removeAccountResponse(result);
}

void partyService::markBannedServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(markBannedServerResponse(QByteArray)));
    isActive_ = true;
    emit markBannedResponse(result);
}

void partyService::removeBannedServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(removeBannedServerResponse(QByteArray)));
    isActive_ = true;
    emit removeBannedResponse(result);
}

void partyService::getBannedServerResponse(QByteArray response)
{
    getBannedResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
        if (result.success)
        {
            int count = parser.getInt("count");
            for (int i=0; i<count; i++)
            {
                getBannedResult::spamAccountBan acc;
                acc.login = parser.first("accounts["+QString::number(i)+"].login");
                acc.channel = parser.first("accounts["+QString::number(i)+"].channel");
                result.accounts.append(acc);
            }
        }
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(getBannedServerResponse(QByteArray)));
    isActive_ = true;
    emit getBannedResponse(result);
}

void partyService::joinPartyServerResponse(QByteArray response)
{
    joinPartyResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
        if (result.success)
        {
            int count = parser.getInt("count");
            int partyParamsCount = parser.getInt("pp_count");
            for (int i=0; i<count; i++)
            {
                joinPartyResult::partyMember acc;
                acc.login = parser.first("members["+QString::number(i)+"].member");
                acc.party = parser.first("members["+QString::number(i)+"].party");
                acc.ready = parser.first("members["+QString::number(i)+"].status").indexOf("::READY::") == 0;
                acc.status = parser.first("members["+QString::number(i)+"].status").replace("::READY::","").replace("::NOTREADY::","");
                result.members.append(acc);
            }
            for (int i=0; i<partyParamsCount; i++)
            {
                joinPartyResult::partyParams pp;
                pp.before_attack = parser.getInt("party_params[" + QString::number(i) + "].before_attack");
                pp.caps = parser.first("party_params[" + QString::number(i) + "].caps");
                pp.cd = parser.getInt("party_params[" + QString::number(i) + "].cd");
                pp.channel = parser.first("party_params[" + QString::number(i) + "].channel");
                pp.list_name = parser.first("party_params[" + QString::number(i) + "].list_name");
                pp.max_emotes = parser.getInt("party_params[" + QString::number(i) + "].max_emotes");
                pp.method = parser.first("party_params[" + QString::number(i) + "].method");
                pp.party = parser.first("party_params[" + QString::number(i) + "].party");
                pp.server = parser.first("party_params[" + QString::number(i) + "].server");
                pp.word_count_max = parser.getInt("party_params[" + QString::number(i) + "].word_count_max");
                pp.word_count_min = parser.getInt("party_params[" + QString::number(i) + "].word_count_min");
                result.params.append(pp);
            }
            result.isOwner = parser.getBool("owner");
        }
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(joinPartyServerResponse(QByteArray)));
    isActive_ = true;
    emit joinPartyResponse(result);
}

void partyService::leavePartyServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(leavePartyServerResponse(QByteArray)));
    isActive_ = true;
    emit leavePartyResponse(result);
}

void partyService::changeStatusServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(changeStatusServerResponse(QByteArray)));
    isActive_ = true;
    emit changeStatusResponse(result);
}

void partyService::sendMessageServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(sendMessageServerResponse(QByteArray)));
    isActive_ = true;
    emit sendMessageResponse(result);
}

void partyService::setPartyParamsServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(setPartyParamsServerResponse(QByteArray)));
    isActive_ = true;
    emit setPartyParamsResponse(result);
}

void partyService::startPartyServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(startPartyServerResponse(QByteArray)));
    isActive_ = true;
    emit startPartyResponse(result);
}

void partyService::stopPartyServerResponse(QByteArray response)
{
    nonQueryResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(stopPartyServerResponse(QByteArray)));
    isActive_ = true;
    emit stopPartyResponse(result);
}

void partyService::updatePartyServerResponse(QByteArray response)
{
    partyUpdatesResult result;
    if (response.indexOf("!!:HTTP") == 0)
        result.error = true;
    else
    {
        result.error = false;
        jsonParser parser(response);
        result.success = parser.getBool("success");
        result.status = parser.first("status");
        if (result.success)
        {
            int count = parser.getInt("count");
            for (int i=0; i<count; i++)
            {
                partyUpdatesResult::partyUpdate pu;
                pu.before_attack = parser.getInt("updates[" + QString::number(i) + "].before_attack");
                pu.caps = parser.first("updates[" + QString::number(i) + "].caps");
                pu.cd = parser.getInt("updates[" + QString::number(i) + "].cd");
                pu.channel = parser.first("updates[" + QString::number(i) + "].channel");
                pu.list_name = parser.first("updates[" + QString::number(i) + "].list_name");
                pu.max_emotes = parser.getInt("updates[" + QString::number(i) + "].max_emotes");
                pu.method = parser.first("updates[" + QString::number(i) + "].method");
                pu.party = parser.first("updates[" + QString::number(i) + "].party");
                pu.server = parser.first("updates[" + QString::number(i) + "].server");
                pu.word_count_max = parser.getInt("updates[" + QString::number(i) + "].word_count_max");
                pu.word_count_min = parser.getInt("updates[" + QString::number(i) + "].word_count_min");
                pu.message_text = parser.first("updates[" + QString::number(i) + "].message_text");
                pu.nick = parser.first("updates[" + QString::number(i) + "].nick");
                pu.rec = parser.first("updates[" + QString::number(i) + "].rec");
                pu.update_type = parser.first("updates[" + QString::number(i) + "].update_type");
                pu.user_status = parser.first("updates[" + QString::number(i) + "].user_status");
                result.updates.append(pu);
            }
        }
    }
    QObject::disconnect(remoteServer_,SIGNAL(callFinished(QByteArray)),this,SLOT(updatePartyServerResponse(QByteArray)));
    isActive_ = true;
    emit updatePartyResponse(result);
}

//GameServer
RemoteServer::RemoteServer(QString serverURL, QObject *parent) : QObject(parent)
{
    server = serverURL;
    manager = 0;
}

void RemoteServer::call(QString method, QHash<QString, QString> params)
{
    if (manager)
        delete manager;
    manager = new QNetworkAccessManager(this);
    QUrl url(server);
    QUrlQuery query;

    for(QHash<QString, QString>::iterator i = params.begin(); i != params.end(); ++i)
        query.addQueryItem(i.key(),i.value());

    method = method.replace(".","/").replace("::","/").replace(":","/").replace(" ", "/");
    url.setPath("/" + method);
    url.setQuery(query);

    QNetworkRequest request(url);
 //   request.setSslConfiguration(QSslConfiguration::defaultConfiguration());



    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    manager->get(request);
}

RemoteServer::~RemoteServer()
{
    delete manager;
}

void RemoteServer::replyFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        emit callFinished(QString("!!:HTTP ERROR: " + reply->errorString()).toUtf8());
    }
    else
    {
        emit callFinished(reply->readAll());
    }
}


partyServiceAllocator::partyServiceAllocator(QString serverURL, int count)
{
    for (int i=0; i<count; ++i)
    {
        partyService * p = new partyService(serverURL);
        items.push_back(p);
    }
}

partyService *partyServiceAllocator::get()
{
    for (int i=0; i<items.count(); ++i)
        if (items[i]->isActive())
            return items[i];
    return 0;
}

partyServiceAllocator::~partyServiceAllocator()
{
    for (int i=0; i<items.count(); ++i)
        delete items[i];
    items.clear();
}

