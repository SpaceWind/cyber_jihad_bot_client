#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QStringList>
#include <QHash>

class jsonParser
{
public:
    jsonParser(QByteArray data);
    ~jsonParser();

    QStringList get(QString selector, QString defaultValue = "");
    int getInt(QString selector);
    bool getBool(QString selector);
    QString first(QString selector);
private:
    QStringList getSelectorTokens(QString selector);
    QString valueToString(QJsonValue value, QString defaultValue);
    enum parseState {OBJECT_STATE, VALUE_STATE, ARRAY_STATE, VALUES_ARRAY, OBJECTS_ARRAY};

    QJsonDocument jDoc;
    parseState state;

};


#endif // JSONPARSER_H
