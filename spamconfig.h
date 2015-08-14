#ifndef SPAMCONFIG_H
#define SPAMCONFIG_H

#include <QObject>
#include <QString>

class SpamConfig
{
public:
    SpamConfig();
    ~SpamConfig();

    void load();
    void save();

    QString login;
    QString pass;
    bool autologin;
    bool saveBannedToServer;
    int helloMessageCount;
private:
    void create();
    bool fileExists();


};

#endif // SPAMCONFIG_H
