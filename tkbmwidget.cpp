#include "tkbmwidget.h"
#include "ui_tkbmwidget.h"
#include "iostream"
#include <QDesktopWidget>
#include <QAxObject>
#include <QFile>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <string.h>
#include <QRadioButton>
#include <static_string.h>
#include <QMutex>
#include <QDateTime>
#include <QIntValidator>

struct recv_data can_data;
static VCI_CAN_OBJ recv_buff[128];
extern QMutex ext_mutex;
QList<VCI_CAN_OBJ> vec_buff;

TkbmWidget::TkbmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TkbmWidget)
{
    ui->setupUi(this);
    QDesktopWidget *thisDesk = QApplication::desktop();
    QRect rect = thisDesk->availableGeometry();
    can_bsp = NULL;
    can_data.len = 0;
    this->move((rect.width()-this->width())/2,(rect.height()-this->height())/2-20);
    this->ui_init();

    this->chg_stage_data_init();
    this->data_struct_init();
    this->set_eep_config();
    login_dialog.show();

    timer_100 = new QTimer(this);
    monitor_dialog = new MonitorDialog(this);
    connect(timer_100,&QTimer::timeout,this,&TkbmWidget::update_msg_timeout);
    timer_100->start(100);
    timer_5s = new QTimer(this);
    connect(timer_5s,&QTimer::timeout,this,&TkbmWidget::update_data_timeout);
    timer_5s->start(10000);

    connect(&login_dialog,&LoginDialog::sig_send_can_param,this,&TkbmWidget::sig_get_can_param);
    connect(&login_dialog,&LoginDialog::sig_send_window_close,this,&TkbmWidget::close);
    connect(this,&TkbmWidget::sig_process_exit,&login_dialog,&QDialog::close);
    connect(this,&TkbmWidget::sig_process_exit,monitor_dialog,&QDialog::close);
    connect(monitor_dialog,&MonitorDialog::sig_send_board_id,this,&TkbmWidget::slot_get_board_id);
}

void TkbmWidget::chg_stage_data_init()
{
    QTableWidgetItem *item;
    for(int i=0;i<CHG_STAGE_ARRAY_SIZE;i++){
        msg_chg_stage[i] = ' ';
    }
    msg_chg_stage[0] = QString("握手识别阶段1");
    msg_chg_stage[1] = QString("握手识别阶段2");
    msg_chg_stage[0x10] = QString("参数准备阶段1");
    msg_chg_stage[0x11] = QString("参数准备阶段2");
    msg_chg_stage[0x20] = QString("充电阶段1");
    msg_chg_stage[0x21] = QString("充电阶段2");
    msg_chg_stage[0x30] = QString("充电结束");
    item = ui->tb_ctl_info->item(MSG_CNT_FORCE_CTL,0);
    item->setText(msg_cnt_force_ctrol[0]);
    item = ui->tb_ctl_info->item(MSG_VCUCAN_ALM_CTL,0);
    item->setText(msg_vcu_can_alarm[0]);
    cnt_ctrl_clk = 0;
    vcu_alarm = 0;
    memset(&bms_sub_info,0,sizeof(struct per_battery_info_discription));

    struct sub_each_board each_bd;
    bms_sub_info = new struct per_battery_info_discription;
    bms_sub_info->cur_id = 0;
    memset(&each_bd,0,sizeof (struct sub_each_board));
    bms_sub_info->each_board.push_back(each_bd);
}

void TkbmWidget::comm_timeout()
{
    int i,j;
    QString msg;

    for(i=0;i<can_data.len;i++){
        if(can_data.data[i].ID == SUB_MAIN_MSG2_ID){                //处理主板数据2
            switch (can_data.data[i].Data[0]) {
            case 0:
                msg = main_info_msg(can_data.data[i].Data[1],1);
                msg_summary[MSG_SUMMARY_LIST_CNST].s_val = msg;
                (can_data.data[i].Data[1] > 1) ? (msg_summary[MSG_SUMMARY_LIST_CNST].f_color = Qt::red) : (msg_summary[MSG_SUMMARY_LIST_CNST].f_color = Qt::black);
                msg = main_info_msg(can_data.data[i].Data[2],2);
                msg_summary[MSG_SUMMARY_LIST_CHGST].s_val = msg;
                msg_summary[MSG_SUMMARY_LIST_CHGST].f_color = Qt::black;
                msg_summary[MSG_SUMMARY_LIST_CCON].s_val = QString("%1").arg(can_data.data[i].Data[3]);
                msg_summary[MSG_SUMMARY_LIST_TVOL].s_val = QString("%1").arg(can_data.data[i].Data[4]+can_data.data[i].Data[5]*256);
                msg_summary[MSG_SUMMARY_LIST_TCUR].s_val = QString("%1").arg((can_data.data[i].Data[6]+can_data.data[i].Data[7]*256)*0.1-400);
                break;
            case 1:
                msg_summary[MSG_SUMMARY_LIST_SOC].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_ACCAI].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_OFCAT].s_val = QString("%1").arg((can_data.data[i].Data[5]+can_data.data[i].Data[6]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_HERTICK].s_val = QString("%1").arg(can_data.data[i].Data[7]);
                break;
            case 2:
                msg_summary[MSG_SUMMARY_LIST_MTPV].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.001);
                msg_summary[MSG_SUMMARY_LIST_MVPS].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*1);
                msg_summary[MSG_SUMMARY_LIST_MTTP].s_val = QString("%1").arg(can_data.data[i].Data[5]-40);
                break;
            case 3:
                msg_summary[MSG_SUMMARY_LIST_MLPV].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.001);
                msg_summary[MSG_SUMMARY_LIST_MLPS].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*1);
                msg_summary[MSG_SUMMARY_LIST_MLTP].s_val = QString("%1").arg(can_data.data[i].Data[5]-40);
                break;
            case 4:
                msg = anasy_alart_msg(can_data.data[i].Data+1);
                if(msg.isNull() || msg.isEmpty()){
                    msg_alarm.f_color = Qt::black;
                    msg_alarm.s_val = ' ';
                }else{
                    msg_alarm.s_val = msg;
                    msg_alarm.f_color = Qt::red;
                }
                break;
            case 5:
                msg_chg_summary[MSG_CHG_SUMMARY_CONNTEMP1].s_val = QString("%1").arg(can_data.data[i].Data[1]-40);
                msg_chg_summary[MSG_CHG_SUMMARY_CONNTEMP2].s_val = QString("%1").arg(can_data.data[i].Data[2]-40);
                msg_chg_summary[MSG_CHG_SUMMARY_CC2_STATE].s_val = msg_cc2_state[can_data.data[i].Data[3]&0x01];
                msg_chg_summary[MSG_CHG_SUMMARY_CC2_FRQ].s_val = QString("%1").arg((can_data.data[i].Data[4]+can_data.data[i].Data[5]*256));
                msg_chg_summary[MSG_CHG_SUMMARY_CHG_VER].s_val = msg_global_ver[can_data.data[i].Data[6]&0x01];
                break;
            case 6:
                msg_chg_summary[MSG_CHG_SUMMARY_VOL_NEED].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.1);
                msg_chg_summary[MSG_CHG_SUMMARY_CUR_NEED].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*0.1-400);
                msg_chg_summary[MSG_CHG_SUMMARY_MODE_NEED].s_val = msg_need_mode[can_data.data[i].Data[5]&0x03];
                break;
            case 7:
                msg_chg_summary[MSG_CHG_SUMMARY_CHG_STAGE].s_val = msg_chg_stage[can_data.data[i].Data[1]];
                msg_chg_summary[MSG_CHG_SUMMARY_IIME_NEED].s_val = QString("%1").arg(can_data.data[i].Data[2]+can_data.data[i].Data[3]*256);
                msg_chg_summary[MSG_CHG_SUMMARY_OUT_POW].s_val = QString("%1").arg((can_data.data[i].Data[4]+can_data.data[i].Data[5]*256)*0.1);
                msg_chg_summary[MSG_CHG_SUMMARY_CHG_TIM].s_val = QString("%1").arg(can_data.data[i].Data[6]+can_data.data->Data[7]);
                break;
            case 8:
                msg_chg_summary[MSG_CHG_SUMMARY_VOL_MAX].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.1);
                msg_chg_summary[MSG_CHG_SUMMARY_VOL_MIN].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*0.1);
                msg_chg_summary[MSG_CHG_SUMMARY_OUT_VOL].s_val = QString("%1").arg((can_data.data[i].Data[5]+can_data.data[i].Data[6]*256)*0.1);
                break;
            case 9:
                msg_chg_summary[MSG_CHG_SUMMARY_CUR_MAX].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.1-400);
                msg_chg_summary[MSG_CHG_SUMMARY_CUR_MIN].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*0.1-400);
                msg_chg_summary[MSG_CHG_SUMMARY_OUT_CUR].s_val = QString("%1").arg((can_data.data[i].Data[5]+can_data.data[i].Data[6]*256)*0.1-400);
                break;
            case 10:
                msg = QString("");
                for(j=0;j<8;j++){
                    if((can_data.data[i].Data[1] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_bms_stop_chg[j] + "  ";
                }
                if(j==8)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_BMS_STOP].s_val = msg;

                msg = QString("");
                for(j=0;j<4;j++){
                    if((can_data.data[i].Data[1] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_chg_stop_err[j] + "  ";
                }
                if(j==4)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_CHG_STOP].s_val = msg;

                msg = QString("");
                for(j=0;j<8;j++){
                    if((can_data.data[i].Data[1] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_chg_stop_fat[j] + "  ";
                }
                if(j==8)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_CHG_ERR].s_val = msg;

                msg = QString("");
                for(j=0;j<7;j++){
                    if((can_data.data[i].Data[1] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_bms_rcv_err[j] + "  ";
                }
                if(j==7)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_BMS_COM].s_val = msg;

                msg = QString("");
                for(j=0;j<8;j++){
                    if((can_data.data[i].Data[1] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_chg_rcv_err[j] + "  ";
                }
                if(j==8)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_CHG_RCV].s_val = msg;
                break;
            case 11:
                msg_ac_chg_state[MSG_AC_CHG_CP_PWM].s_val = QString("%1").arg(can_data.data[i].Data[1]);
                msg_ac_chg_state[MSG_AC_CHG_CC_STATE].s_val = msg_cc2_state[can_data.data[i].Data[2]&0x01];
                msg_ac_chg_state[MSG_AC_CHG_CC_FRQ].s_val = QString("%1").arg(can_data.data[i].Data[3]+can_data.data[i].Data[4]*256);
                break;
            case 12:
                msg_ac_chg_state[MSG_AC_CHG_ENABLE].s_val = msg_state_enable[can_data.data[i].Data[1]&0x01];
                msg_ac_chg_state[MSG_AC_CHG_VOL_NEED].s_val = QString("%1").arg((can_data.data[i].Data[2]+can_data.data[i].Data[3]*256)*0.1);
                msg_ac_chg_state[MSG_AC_CHG_CUR_NEED].s_val = QString("%1").arg((can_data.data[i].Data[4]+can_data.data[i].Data[5]*256)*0.1);
                msg_ac_chg_state[MSG_AC_CHG_STATE].s_val = msg_ac_chg_state_a[can_data.data[i].Data[6]&0x0f];
                break;
            case 13:
                msg_ac_chg_state[MSG_AC_CHG_OUT_STE].s_val = msg_ac_chger_out_state[can_data.data[i].Data[1]&0x07];
                msg_ac_chg_state[MSG_AC_CHG_OUT_VOL].s_val = QString("%1").arg((can_data.data[i].Data[2]+can_data.data[i].Data[3]*256)*0.1);
                msg_ac_chg_state[MSG_AC_CHG_OUT_CUR].s_val = QString("%1").arg((can_data.data[i].Data[4]+can_data.data[i].Data[5]*256)*0.1);
                break;
            case 14:
                msg_main_ctl_info[MSG_OVER_CHG_ACH_FLAG].s_val = msg_over_chg_acc_flag[can_data.data[i].Data[1]&0x01];
                msg_main_ctl_info[MSG_INTO_OFF_TIME].s_val = QString("%1").arg(can_data.data[i].Data[2]);
                msg_main_ctl_info[MSG_INTO_30_AC_TIME].s_val = QString("%1").arg(can_data.data[i].Data[3]);
                msg_main_ctl_info[MSG_INTO_20_AC_TIME].s_val = QString("%1").arg(can_data.data[i].Data[4]);
                msg_main_ctl_info[MSG_INTO_10_AC_TIME].s_val = QString("%1").arg(can_data.data[i].Data[5]);
                msg_main_ctl_info[MSG_INTO_1_AC_TIME].s_val = QString("%1").arg(can_data.data[i].Data[6]);
                msg_main_ctl_info[MSG_STORE_TIME].s_val = QString("%1").arg(can_data.data[i].Data[7]);
                break;
            case 15:
                msg_main_ctl_info[MSG_SOC_EXC_BEF].s_val = QString("%1").arg(can_data.data[i].Data[1]);
                msg_main_ctl_info[MSG_SOC_EXC_AFT].s_val = QString("%1").arg(can_data.data[i].Data[2]);
                break;
            case 16:break;
            case 17:break;
            case 18:
                msg = QString("%1.").arg(can_data.data[i].Data[1],2,16,QLatin1Char('0'))            //年
                        + QString("%1.").arg(can_data.data[i].Data[2],2,16,QLatin1Char('0'))        //月
                        + QString("%1 ").arg(can_data.data[i].Data[3],2,16,QLatin1Char('0'))       //日
                        + QString("%1:").arg(can_data.data[i].Data[4],2,16,QLatin1Char('0'))        //时
                        + QString("%1:").arg(can_data.data[i].Data[5],2,16,QLatin1Char('0'))        //分
                        + QString("%1").arg(can_data.data[i].Data[6],2,16,QLatin1Char('0'));        //秒
                msg_main_ctl_info[MSG_BMS_TIME].s_val = msg;
                msg_main_ctl_info[MSG_LOCAL_TIME].s_val = QDateTime::currentDateTime().toString("yy.MM.dd hh:mm:ss");
                break;
            case 19:
                msg_main_ctl_info[MSG_SOFT_VER].s_val = QString("%1H").arg(can_data.data[i].Data[1],2,16,QLatin1Char('0'));
                msg_main_ctl_info[MSG_HARD_VER].s_val = QString("%1H").arg(can_data.data[i].Data[2],2,16,QLatin1Char('0'));
                msg = QString("%1.").arg(can_data.data[i].Data[3],2,16,QLatin1Char('0'))            //年
                        + QString("%1.").arg(can_data.data[i].Data[4],2,16,QLatin1Char('0'))        //月
                        + QString("%1").arg(can_data.data[i].Data[5],2,16,QLatin1Char('0'));       //日
                msg_main_ctl_info[MSG_SOFT_MOD_TIME].s_val = msg;
                msg_main_ctl_info[MSG_SOFT_MOD_NUM].s_val = QString("%1H").arg(can_data.data[i].Data[6],2,16,QLatin1Char('0'));
                break;
            default:
                break;
            }
        }
        if(can_data.data[i].ID == JURYUAN_MSG_ID){                  //处理绝缘板数据
            msg_summary[MSG_SUMMARY_LIST_RESIS].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*1);
            msg_summary[MSG_SUMMARY_LIST_NORSV].s_val = QString("%1").arg((can_data.data[i].Data[5]+can_data.data[i].Data[6]*256)*1);
            msg_summary[MSG_SUMMARY_LIST_RSVTIC].s_val = QString("%1").arg(can_data.data[i].Data[7]);
        }
        if(CONG_BOARD_STATE_ID == (can_data.data[i].ID & 0xfffffff0)){  //处理从板状态信息
            QVector<struct sub_each_board>::iterator iter;
            for(iter = bms_sub_info->each_board.begin();iter < bms_sub_info->each_board.end();iter++){
                if(iter->bid == can_data.data[i].Data[0]){          //如果存在该id,则按id修改内容
                    sub_state_msg_ana(iter,can_data.data[i].Data);
                    j = -1;
                }
            }
            if(j != -1){                    //如果不存在该id,则创建一个数据,查找合适的位置放入
                struct sub_each_board ebd;
                sub_state_msg_ana(&ebd,can_data.data[i].Data);
                int k=0;
                for(iter = bms_sub_info->each_board.begin();iter < bms_sub_info->each_board.end();iter++){
                    if(ebd.bid < iter->bid){
                        bms_sub_info->each_board.insert(k,ebd);
                        j = -1;
                        break;
                    }
                    k++;
                }
                if(j != -1)
                    bms_sub_info->each_board.push_back(ebd);
            }
        }
    }
    can_data.len = 0;

}

void TkbmWidget::sub_state_msg_ana(struct sub_each_board *bd,unsigned char Data[8])
{
    int j;
    bd->bid = Data[0];
    switch (Data[1]) {
    case 0xD0:
        bd->sw = Data[2];
        bd->hw = Data[4];
        break;
    case 0xD1:
        for(j=0;j<6;j++){
            bd->sn[j] = Data[j+2];
        }
        break;
    case 0xD2:
        bd->module_num = Data[5];
        bd->htick = Data[6];
        bd->state = Data[7];
        break;
    case 0xD3: break;
    case 0xD4:
        bd->disc[Data[2]] = Data[3] + Data[4]*256;
        break;
    default: break;
    }
}

void TkbmWidget::update_data_timeout()
{
    data_struct_init();
}

QString TkbmWidget::anasy_alart_msg(unsigned char data[5])
{
    QString temp;
    int i,j;
    int val;
    for(i=0;i<5;i++){
        for(j=0;j<4;j++){
            val = data[i]>>(2*j) & 0x03;
            if(val){
                temp += msg_alart_list[i][j] + msg_alart_level[val] + '\n';
            }
        }
    }
    return temp;
}

QString TkbmWidget::main_info_msg(unsigned char data,char pos)
{
    QString msg;
    switch (pos) {
    case 1:
        msg = msg_connector_state[data&0x0f];break;
    case 2:
        msg = msg_chg_state[data&0x03];break;
    default:
        msg = QString("无此消息\0");break;
    }
    return msg;
}

void TkbmWidget::update_msg_timeout()
{
    int item_count;
    int col_count;
    int i,j;
    item_count = ui->tb_breif->rowCount();
    for(i=0;i<item_count;i++){                          //更新概述表
        QTableWidgetItem *item = ui->tb_breif->item(i,0);
        item->setText(msg_summary[i].s_val);
        item->setForeground(QBrush(msg_summary[i].f_color));
        item->setBackground(QBrush(msg_summary[i].b_color));
    }
    ui->teb_msg->setText(msg_alarm.s_val);
    ui->teb_msg->setTextColor(msg_alarm.f_color);

    item_count = ui->tb_chg_info->rowCount();
    col_count = ui->tb_chg_info->columnCount();
    for(i=0;i<col_count/2;i++){                     //更新充电信息概述表
        for(j=0;j<item_count;j++){
            QTableWidgetItem *item = ui->tb_chg_info->item(j,i*2+1);
            item->setText(msg_chg_summary[j+i*item_count].s_val);
            item->setForeground(QBrush(msg_chg_summary[j+i*item_count].f_color));
            item->setBackground(QBrush(msg_chg_summary[j+i*item_count].b_color));
        }
    }
    item_count = ui->tb_chg_err->rowCount();
    for(i=0;i<item_count;i++){                   //更新充电故障表
        QTableWidgetItem *item = ui->tb_chg_err->item(i,0);
        item->setText(msg_chg_err_disc[i].s_val);
        item->setForeground(QBrush(msg_chg_err_disc[i].f_color));
        item->setBackground(QBrush(msg_chg_err_disc[i].b_color));
    }
    item_count = ui->tb_need->rowCount();
    col_count = ui->tb_need->columnCount();
    for(i=0;i<col_count/2;i++){                 //更新交流充电状态表
        for(j=0;j<item_count;j++){
            QTableWidgetItem *item = ui->tb_need->item(j,i*2+1);
            item->setText(msg_ac_chg_state[j+i*item_count].s_val);
            item->setForeground(QBrush(msg_ac_chg_state[j+i*item_count].f_color));
            item->setBackground(QBrush(msg_ac_chg_state[j+i*item_count].b_color));
        }
    }
    item_count = ui->tb_ctl_info->rowCount();
    for(i=0;i<item_count;i++){                  //更新控制信息表
        if(i == MSG_ACT_CONTAIN || i == MSG_VCUCAN_ALM_CTL || i==MSG_OFF_CONTAIN || i==MSG_CNT_FORCE_CTL)     continue;
        QTableWidgetItem *item = ui->tb_ctl_info->item(i,0);
        item->setText(msg_main_ctl_info[i].s_val);
        item->setForeground(msg_main_ctl_info[i].f_color);
        item->setBackground(msg_main_ctl_info[i].b_color);
    }
}

void TkbmWidget::slot_get_board_id(int bid)
{
    bms_sub_info->cur_id = bid;
    ui->le_id_in->setText(QString("%1").arg(bid));
    emit ui->le_id_in->editingFinished();
}

void TkbmWidget::sig_get_can_param(int dev,int num,int rate,int port)
{
    can_bsp = new CtlCan(dev,num,port);
    if(can_bsp->open((CtlCan::baudRate)rate)){
        can_bsp->state = CtlCan::off;
        QMessageBox::information(this,"ERROR",QString("打开设备失败,请检查连接\0"));
        this->close();
    }else{
        can_bsp->state = CtlCan::on;
        RecvCan *tRecv = new RecvCan;
        tRecv->start();
        connect(this,&TkbmWidget::sig_set_cthread_state,tRecv,&RecvCan::slot_get_state);
        emit sig_set_cthread_state(can_bsp);

        timer10 = new QTimer(this);
        connect(timer10,&QTimer::timeout,this,&TkbmWidget::comm_timeout);
        timer10->start(50);
    }
}

void TkbmWidget::data_struct_init()
{
    for(int i=0;i<MSG_SUMMARY_LIST_LENGTH;i++){
        msg_summary[i].f_color = Qt::black;
        msg_summary[i].b_color = Qt::white;
        msg_summary[i].i_val = 0;
        msg_summary[i].s_val = ' ';
    }
    msg_summary[0].s_val = QString(THIS_SOFT_VERSION);
    msg_alarm.s_val = QString(" ");
    msg_alarm.b_color = Qt::white;
    msg_alarm.f_color = Qt::black;
    msg_alarm.i_val = 0;
    for(int i=0;i<MSG_CHG_SUMMARY_LENGTH;i++){
        msg_chg_summary[i].f_color = Qt::black;
        msg_chg_summary[i].b_color = Qt::white;
        msg_chg_summary[i].i_val = 0;
        msg_chg_summary[i].s_val = ' ';
    }
    for(int i=0;i<MSG_CHG_ERROR_CURSOR_LEN;i++){
        msg_chg_err_disc[i].f_color = Qt::black;
        msg_chg_err_disc[i].b_color = Qt::white;
        msg_chg_err_disc[i].i_val = 0;
        msg_chg_err_disc[i].s_val = ' ';
    }
    for(int i=0;i<MSG_AC_CHG_STATE_LEN;i++){
        msg_ac_chg_state[i].f_color = Qt::black;
        msg_ac_chg_state[i].b_color = Qt::white;
        msg_ac_chg_state[i].i_val = 0;
        msg_ac_chg_state[i].s_val = ' ';
    }
    for(int i=0;i<MSG_MAIN_BD_CTRL;i++){
        if(i == MSG_ACT_CONTAIN || i == MSG_VCUCAN_ALM_CTL || i==MSG_OFF_CONTAIN || i==MSG_CNT_FORCE_CTL)     continue;
        msg_main_ctl_info[i].f_color = Qt::black;
        msg_main_ctl_info[i].b_color = Qt::white;
        msg_main_ctl_info[i].i_val = 0;
        msg_main_ctl_info[i].s_val = ' ';
    }

}

void TkbmWidget::ui_init(void)
{
    ui->tb_breif->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_breif->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m3->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m4->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m4->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_temp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_temp->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_disconnect->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_disconnect->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_version->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_version->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_run_state->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_run_state->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_info->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_err->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_chg_err->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_need->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_need->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_ctl_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_ctl_info->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_hi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_hi->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_lo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_each_lo->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_event->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_event->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_brief->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_brief->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_file->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->tb_eep_file->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tb_eep_file->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tb_eep_file->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_state->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_state->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_content->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_content->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->le_id_in->setValidator(new QIntValidator(0,100,this));
    ui->le_id_in->setText(QString("0"));
}

void TkbmWidget::set_eep_config(void)
{
    this->read_excel_data();
    this->set_eeprom_table();
}

void TkbmWidget::read_excel_data()
{
    QString file_path = QApplication::applicationDirPath() + FILE_PATH_CONFIG;
    QFile config_file(file_path);
    eep_config.len = 0;
    if(config_file.exists() == false){
        QMessageBox::information(this,"error",QString("缺少配置文件:\n[%1]\n程序关闭").arg(file_path));
        this->~TkbmWidget();
    }

    QAxObject excel("Excel.Application");
    excel.setProperty("Visible",false);
    QAxObject *pWorkbooks = excel.querySubObject("WorkBooks");
    pWorkbooks->dynamicCall("Open (Const QString&)",config_file.fileName());
    QAxObject *pWorkBook = excel.querySubObject("ActiveWorkBook");
    QAxObject *pSheet = pWorkBook->querySubObject("Sheets(int)",1);
    QAxObject *pCell;
    QVariant cell_val;
    QString val;
    do{
        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_NUM);     //获取参数编号
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        if(val.isEmpty() == true || val.isNull() == true){
            break;
        }
        eep_config.param[eep_config.len].s_num = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_NAME);     //获取参数名
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].s_name = val;

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_LEN);     //获取数据长度
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].o_byte_len = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_FORMAT);     //获取显示格式
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].o_dis_format = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_RATE);     //获取比例
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].o_rate = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_OFF);     //获取偏移
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].o_off = val.toInt();

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config.len,CONFIG_COL_PARAM_NOTE);     //获取参数注释
        cell_val = pCell->property("Value");
        val = cell_val.toString();
        eep_config.param[eep_config.len].s_note = val;

        eep_config.len++;
    }while(true);
    pWorkBook->dynamicCall("Close(Boolen)",false);
    excel.dynamicCall("Quit(void)");
}

void TkbmWidget::set_eeprom_table()
{
    int i;
    int row_num;
    for(i=0;i<eep_config.len;i++){
        row_num = ui->tb_eep_file->rowCount();
        ui->tb_eep_file->insertRow(row_num);
        ui->tb_eep_file->setVerticalHeaderItem(i,new QTableWidgetItem(QString("%1").arg(i)));
        ui->tb_eep_file->setItem(i,0,new QTableWidgetItem(eep_config.param[i].s_name));
    }
}

TkbmWidget::~TkbmWidget()
{
    if(can_bsp != NULL)
        delete can_bsp;
    delete bms_sub_info;
    delete ui;
}

void TkbmWidget::on_tb_eep_file_cellClicked(int row, int column)
{
    ui->tbs_note->setText(eep_config.param[row].s_note);
}

void TkbmWidget::closeEvent(QCloseEvent *event)
{
    emit sig_set_cthread_state(NULL);
    emit sig_process_exit();
    monitor_dialog->hide();
    if(can_bsp != NULL)
        can_bsp->close();
    QWidget::closeEvent(event);
}

void TkbmWidget::on_rb_exp_dis_clicked()
{
    if(ui->rb_exp_dis->isChecked())
    {
        monitor_dialog->show();
    }else{
        monitor_dialog->hide();
    }
}

RecvCan::RecvCan(QObject *parent)
{
    this->state = true;
    can_bps = NULL;
}

RecvCan::~RecvCan()
{

}

void RecvCan::slot_get_state(CtlCan *pCan)
{
    if(pCan == NULL){
        this->state = false;
    }else{
        can_bps = pCan;
    }
}

void RecvCan::run()
{
    int i=0;
    int read_len;
    while(this->state){
       if(can_bps){
            if(can_data.len > CAN_RECV_BUFF_LEN_MAX){
                can_data.len = 0;
            }
            read_len = can_bps->get_recive_num();
            if(read_len == 0)   continue;
            if(can_data.len + read_len > CAN_RECV_BUFF_LEN_MAX){
                can_bps->clear_buff();
                read_len = 0;
                can_data.len = 0;
            }

            read_len = can_bps->can_rec_read(recv_buff,read_len);
            memcpy(&can_data.data[can_data.len],recv_buff,sizeof (VCI_CAN_OBJ)*read_len);
            can_data.len += read_len;

            ext_mutex.lock();
            for(i=0;i<read_len;i++)
                vec_buff.push_back(recv_buff[i]);
            ext_mutex.unlock();
        }
    }
}



void TkbmWidget::on_pb_sync_clicked()
{
    unsigned char data[8];
    bool state;
    QDateTime time = QDateTime::currentDateTime();
    QString year = time.toString("yy");
    QString mon = time.toString("MM");
    QString day = time.toString("dd");
    QString hour = time.toString("hh");
    QString min = time.toString("mm");
    QString sec = time.toString("ss");

    data[0] = 0xF2;
    data[1] = (unsigned char)year.toInt(&state,16);
    data[2] = (unsigned char)mon.toInt(&state,16);
    data[3] = (unsigned char)day.toInt(&state,16);
    data[4] = (unsigned char)hour.toInt(&state,16);
    data[5] = (unsigned char)min.toInt(&state,16);
    data[6] = (unsigned char)sec.toInt(&state,16);
    data[7] = 0xFF;
    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);
}

void TkbmWidget::on_pb_modify_clicked()
{
    QTableWidgetItem *item = ui->tb_ctl_info->item(MSG_ACT_CONTAIN,0);
    QString text = item->text();
    bool ok;
    unsigned char data[8];
    int ac_contain,off_contain,write_soc;
    ac_contain = text.toInt(&ok,10);
    if(ok == false){
        QMessageBox::information(this,"错误","[无效的参数] 实际容量");
        return;
    }
    item = ui->tb_ctl_info->item(MSG_OFF_CONTAIN,0);
    text = item->text();
    off_contain = text.toInt(&ok,10);
    if(ok == false){
        QMessageBox::information(this,"错误","[无效的参数] 补偿容量");
        return;
    }
    ac_contain = ac_contain * 10;
    off_contain = off_contain * 10;
    write_soc = 0x30;
    data[0] = 0xF3;
    data[1] = write_soc & 0xFF;
    data[2] = (write_soc >> 8) & 0xff;
    data[3] = ac_contain & 0xFF;
    data[4] = (ac_contain >> 8) & 0xFF;
    data[5] = off_contain & 0xff;
    data[6] = (off_contain >> 8) & 0xFF;
    data[7] = 0xFF;
    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);
}

void TkbmWidget::on_tb_ctl_info_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem *item;
    switch (row) {
    case MSG_CNT_FORCE_CTL:
        cnt_ctrl_clk=(cnt_ctrl_clk>=2)?(0):(++cnt_ctrl_clk);
        item = ui->tb_ctl_info->item(row,0);
        item->setText(msg_cnt_force_ctrol[cnt_ctrl_clk]);
        break;
    case MSG_VCUCAN_ALM_CTL:
        vcu_alarm = (vcu_alarm>=1)?(0):(++vcu_alarm);
        item = ui->tb_ctl_info->item(row,0);
        item->setText(msg_vcu_can_alarm[vcu_alarm]);
        break;
    default:
        break;
    }
}

void TkbmWidget::on_pb_ctl_tch_clicked()
{
    QTableWidgetItem *item = ui->tb_ctl_info->item(MSG_CNT_FORCE_CTL,0);
    QString txt = item->text();
    txt = txt.left(2);
    bool ok;
    int ctrl = txt.toInt(&ok,16);
    if(ok == false)
        return;
    unsigned char data[8];
    data[0] = 0xF4;
    data[1] = ctrl & 0xFF;
    data[2]= 0x00;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFF;

    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);
}

void TkbmWidget::on_pb_ctl_alarm_clicked()
{
    QTableWidgetItem *item = ui->tb_ctl_info->item(MSG_VCUCAN_ALM_CTL,0);
    QString txt = item->text();
    txt = txt.left(2);
    bool ok;
    int ctrl = txt.toInt(&ok,16);
    if(ok == false)
        return;
    unsigned char data[8];
    data[0] = 0xF4;
    data[1] = 0x00;
    data[2]= ctrl & 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFF;

    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);
}

void TkbmWidget::on_le_id_in_editingFinished()
{
    bool ok;
    int j=0;
    QVector<struct sub_each_board>::iterator iter;
    int bid = ui->le_id_in->text().toInt(&ok);
    if(ok == false){
        QMessageBox::information(this,"ERROR","无效的从板 ID");
        return;
    }
    for(iter=bms_sub_info->each_board.begin();iter<bms_sub_info->each_board.end();iter++){
        if(bid == iter->bid){
            bms_sub_info->cur_id = bid;
            j = -1;
        }
    }
    if(j != -1){
        iter = bms_sub_info->each_board.begin();
        bms_sub_info->cur_id =iter->bid;
        ui->le_id_in->setText(QString("%1").arg(bms_sub_info->cur_id));
    }
}

void TkbmWidget::on_pb_after_clicked()
{
    bool ok;
    QVector<struct sub_each_board>::iterator iter;
    int bid = ui->le_id_in->text().toInt(&ok);
    if(ok == false){
        QMessageBox::information(this,"ERROR","无效的从板 ID");
        return;
    }
    for(iter = bms_sub_info->each_board.begin();iter < bms_sub_info->each_board.end();iter++){
        if(bid < iter->bid){
            bid = iter->bid;
            break;
        }
    }
    ui->le_id_in->setText(QString("%1").arg(bid));
    emit ui->le_id_in->editingFinished();
}

void TkbmWidget::on_pb_before_clicked()
{
    bool ok;
    QVector<struct sub_each_board>::iterator iter;
    int bid = ui->le_id_in->text().toInt(&ok);
    if(ok == false){
        QMessageBox::information(this,"ERROR","无效的从板 ID");
        return;
    }
    for(iter = bms_sub_info->each_board.begin();iter < bms_sub_info->each_board.end();iter++){
        if(bid <= iter->bid){
            bid = (--iter)->bid;
            break;
        }
    }
    ui->le_id_in->setText(QString("%1").arg(bid));
    emit ui->le_id_in->editingFinished();
}
