#ifndef MESSAGEPREPROCESSORS_H
#define MESSAGEPREPROCESSORS_H

#include <QString>
#include <QHash>


class ChatPreprocessor
{
public:
    ChatPreprocessor();
    ChatPreprocessor(ChatPreprocessor * parent);
    ~ChatPreprocessor();
    QString preprocess(QString s);
protected:
    virtual QString preprocessInternal(QString s) = 0;
private:
    ChatPreprocessor *parent_;
};

class HashTagLinksPreprocessor : public ChatPreprocessor
{
public:
    HashTagLinksPreprocessor();
    HashTagLinksPreprocessor(ChatPreprocessor * parent);
    ~HashTagLinksPreprocessor();
protected:
    virtual QString preprocessInternal(QString s);
};

class EmotesPreprocessor : public ChatPreprocessor
{
public:
    EmotesPreprocessor(QHash<QString,QString> emotes);
    EmotesPreprocessor(QHash<QString, QString> emotes, ChatPreprocessor * parent);
protected:
    virtual QString preprocessInternal(QString s);
private:
    QHash<QString,QString> emotes_;
};

#endif // MESSAGEPREPROCESSORS_H
