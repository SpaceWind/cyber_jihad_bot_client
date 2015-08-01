#include "tinylogger.h"

TinyLogger::TinyLogger(QString filename, QObject *parent) : QObject(parent)
{
    file_ = new QFile(filename);
    if (!file_->open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
        qDebug() << file_->errorString();
}

void TinyLogger::log(TinyLogger::logType type, QString text)
{
    QTextStream ts(file_);
    switch (type)
    {
    case DEBUG:
        ts << "DEBUG: ";
        break;
    case INFO:
        ts << "INFO: ";
        break;
    case WARNING:
        ts << "WARNING: ";
        break;
    case ERROR:
        ts << "ERROR: ";
        break;
    }
    ts << text << endl;
}

TinyLogger::~TinyLogger()
{
    delete file_;
}

