#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_pb_ok_clicked();

signals:
    void sig_send_can_param(int type,int num,int rate,int port);
    bool sig_send_window_close();

protected:
    void closeEvent(QCloseEvent *obj);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
