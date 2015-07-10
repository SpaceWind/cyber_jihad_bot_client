#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    psa = new partyServiceAllocator("https://cyberjihad-bot.herokuapp.com",5);
}

void MainWindow::setup()
{
    if (admin)
        ui->create_party_button->setEnabled(true);
    loadAccounts();
    connect(&partyUpdateTimer,SIGNAL(timeout()),this,SLOT(loadUpdates()));
}

void MainWindow::loadAccounts()
{
    partyService * ps = psa->get();
    ps->getAccounts(apikey,true);
    connect(ps,SIGNAL(getAccountsResponse(getAccountsResult)),this,SLOT(loadAccountsResponse(getAccountsResult)));
    srv["getAccounts"] = ps;
    ui->status_label->setText("loading accounts");
    this->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAccountsResponse(getAccountsResult result)
{
    if (srv.contains("getAccounts"))
        disconnect(srv["getAccounts"],SIGNAL(getAccountsResponse(getAccountsResult)),this,SLOT(loadAccountsResponse(getAccountsResult)));
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
    for (QList<getAccountsResult::spamAccountDescriptor>::iterator it = result.accounts.begin(); it != result.accounts.end(); ++it)
    {
        if ((*it).owner == login)
        {
            myAccounts_.append(*it);
            ui->accounts_list->addItem((*it).login);
        }
        allAccounts_.append((*it).login);
    }
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
            ui->chat_server->setText(serverTokens.at(0));
            ui->server_port->setText(serverTokens.at(1));
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
        partyUpdateTimer.start(2000);
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
                            ui->party_user_list->item(i)->setBackgroundColor(QColor::fromRgb(20,220,20));
                        else
                            ui->party_user_list->item(i)->setBackgroundColor(QColor::fromRgb(128,128,128,128));
                        break;
                    }
                }
            }
            else if (pu.update_type == "params" && !myParty.owner)
            {
                QStringList serverTokens = pu.server.split(":");
                ui->chat_server->setText(serverTokens.at(0));
                ui->server_port->setText(serverTokens.at(1));
                ui->tolower_checkbox->setChecked(pu.caps != "true");
                ui->chat_channel->setText(pu.channel);
                ui->emotes_cap->setValue(pu.max_emotes);
                ui->emotes_cap_checkbox->setChecked(pu.max_emotes == -1);
                if (pu.method == "adaptive" || pu.method == "both")
                {
                    ui->min_words->setValue(pu.word_count_min);
                    ui->max_words->setValue(pu.word_count_max);
                    ui->read_before->setValue(pu.before_attack);
                    if(pu.method == "adaptive")
                    {
                        ui->adaptive_method_box->setEnabled(true);
                        ui->list_method_box->setEnabled(false);
                    }
                    else
                        ui->adaptive_method_box->setEnabled(true);
                }
                else if (pu.method == "list" || pu.method == "both")
                {
                    ui->chat_list_name->setCurrentIndex(ui->chat_list_name->findText(pu.list_name));
                    if (pu.method == "list")
                    {
                        ui->adaptive_method_box->setEnabled(false);
                        ui->list_method_box->setEnabled(true);
                    }
                    else
                        ui->list_method_box->setEnabled(true);
                }
                myParty.name = pu.party;
            }
            else if (pu.update_type == "start")
            {
                ui->start_attack_button->click();
            }
            else if (pu.update_type == "stop")
            {
                ui->stop_attack_button->click();
            }
            else if (pu.update_type == "message")
            {
                QString html = ui->party_chat->toHtml();
                html += "<p>"+ pu.nick + ": " + pu.message_text + "</p>";
                ui->party_chat->setHtml(html);
            }
        }
    }
}

void MainWindow::loadUpdates()
{
    partyService * ps = psa->get();
    ps->updateParty(apikey,myParty.name);
    connect(ps,SIGNAL(updatePartyResponse(partyUpdatesResult)),this,SLOT(updatePartyResponse(partyUpdatesResult)));
    srv["updates"] = ps;
}

//add Account button
void MainWindow::on_pushButton_11_clicked()
{
    twoStringInputDialog d;
    d.setTitle("Input login and password","Login: ", "Pass: ");
    if (d.exec())
    {
        partyService * ps = psa->get();
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
    }
    partyService * ps = psa->get();
    ps->removeAccount(apikey,selectedAccount);
    connect(ps,SIGNAL(removeAccountResponse(nonQueryResult)),this,SLOT(removeAccountResponse(nonQueryResult)));
    srv["removeAccount"] = ps;
    ui->accounts_list->setEnabled(false);
    ui->pushButton_12->setEnabled(false);
}

void MainWindow::on_create_party_button_clicked()
{
    twoStringInputDialog d;
    d.setTitle("Create new group", "Name: ", "Pass: ");
    if (d.exec())
    {
        partyService * ps = psa->get();
        ps->createParty(apikey,d.getValue2(),d.getValue1());
        connect(ps,SIGNAL(createPartyResponse(nonQueryResult)),this,SLOT(createPartyResponse(nonQueryResult)));
        srv["createParty"] = ps;
        ui->create_party_button->setEnabled(false);
        ui->join_party_button->setEnabled(false);
        ui->status_label->setText("Creating Party");
        myParty.name = d.getValue1();
        myParty.pass = d.getValue2();
    }
}

void MainWindow::on_join_party_button_clicked()
{
    twoStringInputDialog d;
    d.setTitle("Join the party", "Party name: ", "Pass: ");
    if (d.exec())
    {
        partyService * ps = psa->get();
        ps->joinParty(apikey, d.getValue1(), d.getValue2());
        connect(ps,SIGNAL(joinPartyResponse(joinPartyResult)),this, SLOT(joinPartyResponse(joinPartyResult)));
        srv["joinParty"] = ps;
        this->setEnabled(false);
        myParty.name = d.getValue1();
        myParty.pass = d.getValue2();
        myParty.enabled = false;
    }
}

void MainWindow::on_leave_party_button_clicked()
{
    if (myParty.enabled)
    {
        partyService * ps = psa->get();
        ps->leaveParty(apikey,myParty.name);
        connect(ps,SIGNAL(leavePartyResponse(nonQueryResult)),this,SLOT(leavePartyResponse(nonQueryResult)));
        srv["leaveParty"] = ps;
        this->setEnabled(false);
        myParty.enabled = false;
    }
}

void MainWindow::on_ready_checkbox_clicked(bool checked)
{
    partyService * ps = psa->get();
    QString status = (checked ? "::READY::" : "") + ui->party_status->text();
    ps->changeStatus(apikey,myParty.name,status);
}

void MainWindow::on_party_status_returnPressed()
{
    partyService * ps = psa->get();
    QString status = (ui->ready_checkbox->isChecked() ? "::READY::" : "") + ui->party_status->text();
    ps->changeStatus(apikey,myParty.name,status);
}
