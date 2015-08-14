#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginWindow w;
    w.setFixedSize(w.geometry().width(),w.geometry().height());
    MainWindow mw;
    if (w.exec())
    {
        mw.show();
        mw.apikey = w.getApikey();
        mw.login = w.getLogin();
        mw.admin = w.isAdmin();
        mw.config = w.getConfig();
        mw.setup();
        return a.exec();
    }
    else
        return 0;
}
