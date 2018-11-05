#include "monitordialog.h"
#include "ui_monitordialog.h"
#include <QDesktopWidget>

MonitorDialog::MonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("CAN 监视");
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move(rect.width()-this->width()-14,rect.height()-this->height()-40);
}

MonitorDialog::~MonitorDialog()
{
    delete ui;
}
