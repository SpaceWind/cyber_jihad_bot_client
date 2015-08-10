#include <QRegExp>
#include <QByteArray>
#include <QImage>
#include <QPixmap>
#include <QScrollBar>
#include "imagetextbrowser.h"


ImageTextBrowser::ImageTextBrowser(QWidget *parent)
 : QTextBrowser(parent)
{
    preprocessor_ = 0;
}

void ImageTextBrowser::addString(QString s)
{
    if (preprocessor_)
        s = preprocessor_->preprocess(s);
    downloadResources(s);
    QString html = toHtml();
    if (s.indexOf("<img") == 0)
        s = "<br>" + s;
    html += s + "\n";
    setHtml(html);
    QScrollBar * vBar = this->verticalScrollBar();
    vBar->setValue(vBar->maximum());
}

void ImageTextBrowser::clear()
{
    QTextBrowser::clear();
    imageResources.clear();
}

ImageTextBrowser::~ImageTextBrowser()
{

}

void ImageTextBrowser::requestFinished(QNetworkReply * result)
{
    if (!result->error())
    {
        QByteArray res = result->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(res);
        QImage img = pixmap.toImage();

        QTextDocument *doc = this->document();
        doc->addResource(QTextDocument::ImageResource,result->url(),QVariant(img));
        setHtml(toHtml());
    }
}

void ImageTextBrowser::downloadResources(QString s)
{
    QRegExp rx("<img\\s+[^>]*src=\"([^\"]*)\"[^>]*(/>|></img>)");
    QStringList matched;
    int pos = 0;

    while ((pos = rx.indexIn(s, pos)) != -1) {
        matched << rx.cap(0);
        pos += rx.matchedLength();
    }
    for (QStringList::iterator it = matched.begin(); it != matched.end(); ++it)
    {
        if (!imageResources.contains(*it))
        {
            QNetworkAccessManager * mgr = new QNetworkAccessManager();
            QString urlString = (*it);
            urlString = urlString.replace("<img src=","").replace("/>","").replace("\"","").replace("></img>","");
            QUrl url(urlString);
            QNetworkRequest request(url);
            connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
            mgr->get(request);
            imageResources.append(*it);
        }
    }
}
