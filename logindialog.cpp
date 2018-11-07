#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDesktopWidget>
#include "ControlCAN.h"
#include "ctlcan.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowTitle("登陆");
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move((rect.width()-this->width())/2,(rect.height()-this->height())/2-20);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::closeEvent(QCloseEvent *event)
{
    emit sig_send_window_close();
    QDialog::closeEvent(event);
}

void LoginDialog::on_pb_ok_clicked()
{
    int dev_type = ui->cb_type->currentIndex();
    int can_num = ui->cb_num->currentIndex();
    int can_rate = ui->cb_rate->currentIndex();
    int can_port = ui->cb_rate->currentIndex();
    switch(dev_type){
    case 0:
        dev_type = VCI_USBCAN_E_U;break;
    case 1:
        dev_type = VCI_USBCAN_2E_U;break;
    default:
        dev_type = VCI_USBCAN_E_U;break;
    }
    switch (can_rate) {
    case 0:
        can_rate = CtlCan::Kbps250;break;
    case 1:
        can_rate = CtlCan::Kbps125;break;
    case 2:
        can_rate = CtlCan::Kbps500;break;
    }
    emit sig_send_can_param(dev_type,can_num,can_rate,can_port);
    this->hide();
}
