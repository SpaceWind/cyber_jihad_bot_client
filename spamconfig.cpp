#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QStringList>
#include "spamconfig.h"

SpamConfig::SpamConfig()
{

}

SpamConfig::~SpamConfig()
{

}

void SpamConfig::load()
{
    if (!fileExists())
        create();
    QFile config("config.dat");
    config.open(QFile::ReadOnly);
    QString data(qUncompress(config.readAll()));
    QStringList items = data.split("\n");
    foreach(const QString& it,items)
    {
        QStringList tokens = it.split("=");
        if (tokens[0] == "login")
        {
            if (tokens.count()>1)
                login = tokens[1];
        }
        else if (tokens[0] == "pass")
        {
            if (tokens.count()>1)
                pass = tokens[1];
        }
        else if (tokens[0] == "autologin")
            autologin = (tokens[1] == "true");
        else if (tokens[0] == "saveBannedToServer")
            saveBannedToServer = (tokens[1] == "true");
        else if (tokens[0] == "helloMessageCount")
            helloMessageCount = tokens[1].toInt();
    }
    config.close();
}

void SpamConfig::save()
{
    QString configString = "login=" + login + "\npass=" + pass + "\nautologin=";
    configString += autologin ? "true\n" : "false\n";
    configString += "saveBannedToServer=";
    configString += saveBannedToServer ? "true\n" : "false\n";
    configString += "helloMessageCount=" + QString::number(helloMessageCount);
    QByteArray data = qCompress(QByteArray::fromStdString(configString.toStdString()));

    QFile f("config.dat");
    f.open(QFile::WriteOnly);
    f.write(data);
    f.close();
}

void SpamConfig::create()
{
    QFile defaultConfig("config.dat");
    defaultConfig.open(QFile::WriteOnly);
    QString config;
    config = "login\npass\nautologin=true\nsaveBannedToServer=false\nhelloMessageCount=10";
    QByteArray data = qCompress(QByteArray::fromStdString(config.toStdString()));
    defaultConfig.write(data);
    defaultConfig.close();
}

bool SpamConfig::fileExists()
{
    QFileInfo checkFile("config.dat");
    return checkFile.exists() && checkFile.isFile();
}

