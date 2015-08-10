#include <QRegExp>
#include <QStringList>
#include "messagepreprocessors.h"


ChatPreprocessor::ChatPreprocessor()
{
    parent_ = 0;
}

ChatPreprocessor::ChatPreprocessor(ChatPreprocessor *parent)
{
    parent_ = parent;
}

ChatPreprocessor::~ChatPreprocessor()
{

}

QString ChatPreprocessor::preprocess(QString s)
{
    if (parent_)
        return preprocessInternal(parent_->preprocess(s));
    else
        return preprocessInternal(s);
}


HashTagLinksPreprocessor::HashTagLinksPreprocessor()
    : ChatPreprocessor()
{
}

HashTagLinksPreprocessor::HashTagLinksPreprocessor(ChatPreprocessor *parent)
    : ChatPreprocessor(parent)
{
}

QString HashTagLinksPreprocessor::preprocessInternal(QString s)
{
    QRegExp regex("#[\\w@]*\\b");
    int pos = 0;
    QStringList channelLinks;
    while(regex.indexIn(s,pos) != -1)
    {
        if (!channelLinks.contains(regex.capturedTexts()[0]))
        {
            channelLinks.append(regex.capturedTexts()[0]);
            QString lastItem = channelLinks.last();
            lastItem.replace("#","");
            if (lastItem.indexOf("@") == 0)
            {
                QString pureLink = lastItem;
                pureLink.replace("@","");
                s.replace(channelLinks.last(),"<a href=\""+lastItem+"\">"+lastItem + "</a> (<a href=\""
                                         + pureLink +"\">switch</a>)");
            }
            else
                s.replace(channelLinks.last(),"<a href=\""+lastItem+"\">"+lastItem + "</a>");
        }
        pos += regex.matchedLength();
    }
    return s;
}

EmotesPreprocessor::EmotesPreprocessor(QHash<QString, QString> emotes)
    : ChatPreprocessor()
{
    emotes_ = emotes;
}

EmotesPreprocessor::EmotesPreprocessor(QHash<QString, QString> emotes, ChatPreprocessor *parent)
    : ChatPreprocessor (parent)
{
    emotes_ = emotes;
}

QString EmotesPreprocessor::preprocessInternal(QString s)
{
    QStringList messageWords = s.split(" ");
    for (QStringList::iterator it = messageWords.begin(); it != messageWords.end(); ++it)
        if (emotes_.contains((*it)))
            (*it) = "<img src=\"" + emotes_[(*it)] + "\"/>";
    s = "";
    for (QStringList::iterator it = messageWords.begin(); it != messageWords.end(); ++it)
        s += (*it) + " ";
    return s.simplified();

}
