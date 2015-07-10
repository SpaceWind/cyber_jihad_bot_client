#ifndef TWOSTRINGINPUTDIALOG_H
#define TWOSTRINGINPUTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class twoStringInputDialog;
}

class twoStringInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit twoStringInputDialog(QWidget *parent = 0);
    void setTitle(QString title, QString label1, QString label2);
    QString getValue1();
    QString getValue2();
    ~twoStringInputDialog();

private:
    Ui::twoStringInputDialog *ui;
};

#endif // TWOSTRINGINPUTDIALOG_H
