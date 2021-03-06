#ifndef TWOSTRINGINPUTDIALOG_H
#define TWOSTRINGINPUTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class twoStringInputDialog;
}

class loginPassInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginPassInputDialog(QWidget *parent = 0);
    void setTitle(QString title, QString label1, QString label2);
    QString getValue1();
    QString getValue2();
    ~loginPassInputDialog();

private slots:
    void on_OkButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::twoStringInputDialog *ui;
};

#endif // TWOSTRINGINPUTDIALOG_H
