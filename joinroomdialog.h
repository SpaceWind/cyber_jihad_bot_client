#ifndef JOINROOMDIALOG_H
#define JOINROOMDIALOG_H

#include <QDialog>

namespace Ui {
class JoinRoomDialog;
}

class JoinRoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinRoomDialog(QWidget *parent = 0);
    ~JoinRoomDialog();

private:
    Ui::JoinRoomDialog *ui;
};

#endif // JOINROOMDIALOG_H
