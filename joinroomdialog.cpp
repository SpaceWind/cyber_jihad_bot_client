#include "joinroomdialog.h"
#include "ui_joinroomdialog.h"

JoinRoomDialog::JoinRoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinRoomDialog)
{
    ui->setupUi(this);
}

JoinRoomDialog::~JoinRoomDialog()
{
    delete ui;
}
