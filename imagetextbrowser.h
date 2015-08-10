#ifndef IMAGETEXTBROWSER_H
#define IMAGETEXTBROWSER_H

#include <QObject>
#include <QTextBrowser>
#include <QWidget>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "messagepreprocessors.h"



class ImageTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    ImageTextBrowser(QWidget * parent);
    void addString(QString s);
    void setPreprocessor(ChatPreprocessor * preprocessor) { preprocessor_ = preprocessor; }
    void clear();
    ~ImageTextBrowser();
protected:
public slots:
    void requestFinished(QNetworkReply*result);
private:
    void downloadResources(QString s);
    QStringList imageResources;
    ChatPreprocessor * preprocessor_;
};

#endif // IMAGETEXTBROWSER_H
