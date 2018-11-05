#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDesktopWidget>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("登陆");
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move((rect.width()-this->width())/2,(rect.height()-this->height())/2-20);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
