#ifndef TINYLOGGER_H
#define TINYLOGGER_H

#include <QObject>

class TinyLogger : public QObject
{
    Q_OBJECT
public:
    explicit TinyLogger(QObject *parent = 0);
    ~TinyLogger();

signals:

public slots:
};

#endif // TINYLOGGER_H
