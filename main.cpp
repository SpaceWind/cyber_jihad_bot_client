#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginWindow w;
    MainWindow mw;
    if (w.exec())
    {
        mw.show();
        mw.apikey = w.getApikey();
        mw.login = w.getLogin();
        mw.admin = w.isAdmin();
        mw.setup();
        return a.exec();
    }
    else
        return 0;
}
