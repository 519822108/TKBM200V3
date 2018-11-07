#include "monitordialog.h"
#include "ui_monitordialog.h"
#include <QDesktopWidget>
#include <data_organize.h>
#include "string.h"
#include <iostream>

extern struct recv_data can_data;
VCI_CAN_OBJ last_msg;

MonitorDialog::MonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("CAN 监视");
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    this->move(rect.width()-this->width()-14,rect.height()-this->height()-40);
    memset(&last_msg,0,sizeof(VCI_CAN_OBJ));
    msg_id_list.len = 0;
    filter_id = 0;

    QTimer *timer100 = new QTimer(this);
    connect(timer100,&QTimer::timeout,this,&MonitorDialog::timer100_timeout);
    timer100->start();
}

void MonitorDialog::timer100_timeout()
{
    int i,j;
    QString temp_str;
    for(i=0;i<can_data.len;i++){
        if(last_msg.TimeStamp >= can_data.data[i].TimeStamp)    continue;       //如果不是最新的报文则跳过
        memcpy(&last_msg,&can_data.data[i],sizeof(VCI_CAN_OBJ));
        for(j=0;j<msg_id_list.len;j++){
            if(last_msg.ID == can_data.data->ID) break;
        }
        if(j==msg_id_list.len){
            msg_id_list.id[j] = last_msg.ID;
            msg_id_list.len++;
            temp_str = QString("%1").arg(last_msg.ID,8,16,QChar('0'));
            ui->lw_fid->addItem(temp_str);
        }
        if(last_msg.ID == filter_id || filter_id==0){
            temp_str = QString("%1: ").arg(last_msg.ID,8,16,QChar('0'));
            for(int k=0;k<last_msg.DataLen;k++){
                temp_str += QString("%1 ").arg(last_msg.Data[k],2,16,QChar('0'));
            }
            ui->lw_msg->addItem(temp_str);
        }
        if(ui->lw_msg->count() > 400){
            ui->lw_msg->clear();
            ui->lw_msg->clearSelection();
        }
        ui->lw_msg->scrollToBottom();
    }
}

MonitorDialog::~MonitorDialog()
{
    delete ui;
}

void MonitorDialog::on_lw_fid_itemDoubleClicked(QListWidgetItem *item)
{
    bool ok;
    QString temp = item->text();
    if(temp.isEmpty() || temp.isNull()){
        filter_id = 0;
    }else{
        filter_id = temp.toInt(&ok,16);
    }
}
