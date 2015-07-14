#include "twostringinputdialog.h"
#include "ui_twostringinputdialog.h"

twoStringInputDialog::twoStringInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::twoStringInputDialog)
{
    ui->setupUi(this);
}

void twoStringInputDialog::setTitle(QString title, QString label1, QString label2)
{
    this->setWindowTitle(title);
    ui->label->setText(label1);
    ui->label_2->setText(label2);
}

QString twoStringInputDialog::getValue1()
{
    return ui->lineEdit->text();
}

QString twoStringInputDialog::getValue2()
{
    return ui->lineEdit_2->text();
}

twoStringInputDialog::~twoStringInputDialog()
{
    delete ui;
}

void twoStringInputDialog::on_OkButton_clicked()
{
    accept();
}

void twoStringInputDialog::on_CancelButton_clicked()
{
    reject();
}
