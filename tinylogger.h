#ifndef TINYLOGGER_H
#define TINYLOGGER_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

class TinyLogger : public QObject
{
    Q_OBJECT
public:
    enum logType{DEBUG, INFO, WARNING, ERROR};
    explicit TinyLogger(QString filename, QObject *parent = 0);
    void log(logType type, QString text);
    ~TinyLogger();

signals:

public slots:

private:
    QFile * file_;
};

#endif // TINYLOGGER_H
