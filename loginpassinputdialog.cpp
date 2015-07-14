#include "loginpassinputdialog.h"
#include "ui_loginpassinputdialog.h"

loginPassInputDialog::loginPassInputDialog(QWidget *parent) :
    QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::twoStringInputDialog)
{
    ui->setupUi(this);
}

void loginPassInputDialog::setTitle(QString title, QString label1, QString label2)
{
    this->setWindowTitle(title);
    ui->label->setText(label1);
    ui->label_2->setText(label2);
}

QString loginPassInputDialog::getValue1()
{
    return ui->lineEdit->text();
}

QString loginPassInputDialog::getValue2()
{
    return ui->lineEdit_2->text();
}

loginPassInputDialog::~loginPassInputDialog()
{
    delete ui;
}

void loginPassInputDialog::on_OkButton_clicked()
{
    accept();
}

void loginPassInputDialog::on_CancelButton_clicked()
{
    reject();
}
