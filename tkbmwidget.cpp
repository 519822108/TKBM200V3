#include "tkbmwidget.h"
#include "ui_tkbmwidget.h"
#include "iostream"
#include <string.h>
#include <QDesktopWidget>
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
#include <string.h>
#include <QFileDialog>
#include <QDir>
#include <QColor>
#include <QFileInfo>
#include <QVector>
#include <qt_windows.h>
#include <QChar>
#include <QDomDocument>
#include <QDomElement>
#include <QDomAttr>
#include <QDomNodeList>
#include <QDebug>
#include <QTime>

//# pragma execution_character_set("utf-8")

struct recv_data can_data;
static VCI_CAN_OBJ recv_buff[128];
extern QMutex ext_mutex;
static QMutex mutex;
static QDateTime mechine_first_launch_time;
QList<VCI_CAN_OBJ> vec_buff;
static QString excel_or_wps = EXCEL_USED_NAME;
static int epprom_set_ac_pos;                              //校验字节的位置

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

    this->txt_xml_anasys();
    this->chg_stage_data_init();
    login_dialog.show();

    this->data_struct_init();
    this->set_eep_config();

    timer_100 = new QTimer(this);
    connect(timer_100,&QTimer::timeout,this,&TkbmWidget::update_msg_timeout);   //事件连接,定时器超时刷新显示
    timer_100->start(DISPLAY_UPDATE_TIME);
    timer_5s = new QTimer(this);
    connect(timer_5s,&QTimer::timeout,this,&TkbmWidget::update_data_timeout);   //定时器超时保存电压数据到EXCEL，和刷新数据缓存
    timer_5s->start(STORE_FILE_SAVE_TI);

    bat_store = new BatteryStore;
    connect(this,&TkbmWidget::sig_get_store_obj,bat_store,&BatteryStore::slot_get_store_obj);

    connect(&login_dialog,&LoginDialog::sig_send_can_param,this,&TkbmWidget::sig_get_can_param);
    connect(&login_dialog,&LoginDialog::sig_send_window_close,this,&TkbmWidget::close);
    connect(this,&TkbmWidget::sig_process_exit,&login_dialog,&QDialog::close);
    connect(this,&TkbmWidget::sig_process_exit,&monitor_dialog,&QDialog::close);
    connect(&monitor_dialog,&MonitorDialog::sig_send_board_id,this,&TkbmWidget::slot_get_board_id);
    connect(this,&TkbmWidget::sig_sub_unit_outline,&monitor_dialog,&MonitorDialog::slots_unit_outline);
}
/**    @breif: 分析配置文件ctxt.xml,填充各种数组，初始化各种可配置的参数
*/
void TkbmWidget::txt_xml_anasys()
{
    QString file_path = QApplication::applicationDirPath() + FILE_PATH_ARRTXT;
    QFile file(file_path);
    QString msg;
    int line,col;
    int i,j;

    if(file.exists() == true){
        file.open(QIODevice::ReadOnly|QFile::Text);
    }else{
        QMessageBox::information(this,"ERROR",QString("缺少配置文件\n[%1]\n无法启动").arg(file.fileName()));
        this->~TkbmWidget();
    }
    QDomDocument doc;
    if(doc.setContent(&file,false,&msg,&line,&col) == false){
        QMessageBox::information(this,"ERROR",QString("打开配置文件失败\n[%1:Line%2Col%3]").arg(msg).arg(line).arg(col));
        file.close();
    }

    QDomElement root = doc.documentElement();
    QDomNodeList dl;
    QDomNodeList dt;
    //填充报警等级
    STRING_ARRAY_INIT(msg_alart_level);
    dl = doc.elementsByTagName("msg_alart_level").item(0).childNodes();
    for (i=0;i<dl.length();i++) {
        msg_alart_level[i] = dl.item(i).toElement().text();
    }
    //填充报警列表
    dl = doc.elementsByTagName("msg_alart_list").item(0).childNodes();
    line = dl.size();
    msg_alart_list_line = line;
    for(i=0;i<line;i++){
        STRING_ARRAY_INIT(msg_alart_list[i]);
        dt = dl.item(i).childNodes();
        for(j=0;j<dt.size();j++)
            msg_alart_list[i][j] = dt.item(j).toElement().text();
    }
    //填充充电状态
    STRING_ARRAY_INIT(msg_chg_state);
    dl = doc.elementsByTagName("msg_chg_state").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_chg_state[i] = dl.item(i).toElement().text();
    }
    //填充连接器状态
    STRING_ARRAY_INIT(msg_connector_state);
    dl = doc.elementsByTagName("msg_connector_state").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_connector_state[i] = dl.item(i).toElement().text();
    }
    //填充CC2连接状态
    STRING_ARRAY_INIT(msg_cc2_state);
    dl = doc.elementsByTagName("msg_cc2_state").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_cc2_state[i] = dl.item(i).toElement().text();
    }
    //填充充电需求
    STRING_ARRAY_INIT(msg_need_mode);
    dl = doc.elementsByTagName("msg_need_mode").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_need_mode[i] = dl.item(i).toElement().text();
    }
    //填充BMS停止充电原因
    STRING_ARRAY_INIT(msg_bms_stop_chg);
    dl = doc.elementsByTagName("msg_bms_stop_chg").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_bms_stop_chg[i] = dl.item(i).toElement().text();
    }
    //填充充电机故障
    STRING_ARRAY_INIT(msg_chg_stop_err);
    dl = doc.elementsByTagName("msg_chg_stop_err").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_chg_stop_err[i] = dl.item(i).toElement().text();
    }
    //填充充电停止原因
    STRING_ARRAY_INIT(msg_chg_stop_fat);
    dl = doc.elementsByTagName("msg_chg_stop_fat").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_chg_stop_fat[i] = dl.item(i).toElement().text();
    }
    //填充状态
    STRING_ARRAY_INIT(msg_state_enable);
    dl = doc.elementsByTagName("msg_state_enable").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_state_enable[i] = dl.item(i).toElement().text();
    }
    //填充交流充电状态
    STRING_ARRAY_INIT(msg_ac_chg_state_a);
    dl = doc.elementsByTagName("msg_ac_chg_state_a").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_ac_chg_state_a[i] = dl.item(i).toElement().text();
    }
    //填充交流充电机输出状态
    STRING_ARRAY_INIT(msg_ac_chger_out_state);
    dl = doc.elementsByTagName("msg_ac_chger_out_state").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_ac_chger_out_state[i] = dl.item(i).toElement().text();
    }
    //填充
    STRING_ARRAY_INIT(msg_over_chg_acc_flag);
    dl = doc.elementsByTagName("msg_over_chg_acc_flag").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_over_chg_acc_flag[i] = dl.item(i).toElement().text();
    }
    //填充解除器强制控制
    STRING_ARRAY_INIT(msg_cnt_force_ctrol);
    dl = doc.elementsByTagName("msg_cnt_force_ctrol").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_cnt_force_ctrol[i] = dl.item(i).toElement().text();
    }
    //填充整车控制器报警控制
    STRING_ARRAY_INIT(msg_vcu_can_alarm);
    dl = doc.elementsByTagName("msg_vcu_can_alarm").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_vcu_can_alarm[i] = dl.item(i).toElement().text();
    }
    //填充BMS运行状态
    STRING_ARRAY_INIT(msg_bms_run_state);
    dl = doc.elementsByTagName("msg_bms_run_state").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_bms_run_state[i] = dl.item(i).toElement().text();
    }
    //填充充电阶段
    STRING_ARRAY_INIT(msg_chg_stage);
    dl = doc.elementsByTagName("msg_chg_stage").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_chg_stage[i] = dl.item(i).toElement().text();
    }
    //填充最高最低
    STRING_ARRAY_INIT(msg_unit_limit_param);
    dl = doc.elementsByTagName("msg_unit_limit_param").item(0).childNodes();
    for(i=0;i<dl.length();i++){
        msg_unit_limit_param[i] = dl.item(i).toElement().text();
    }
#if (CONFIG_USE_FILE_CTXTXML)
    THIS_SOFT_VERSION = doc.elementsByTagName("THIS_SOFT_VERSION").item(0).toElement().text();
    EEP_MOD_PASSWD = doc.elementsByTagName("EEP_MOD_PASSWD").item(0).toElement().text();
    VOLTAG_SAVE_TIME = doc.elementsByTagName("VOLTAG_SAVE_TIME").item(0).toElement().text().toInt();
    VOLTAG_IS_SAVE = doc.elementsByTagName("VOLTAG_IS_SAVE").item(0).toElement().text().toInt();
    IS_DATA_ARRY_CLEAR = doc.elementsByTagName("IS_DATA_ARRY_CLEAR").item(0).toElement().text().toInt();
    BMS_UNIT_OUTLINE_CNT = doc.elementsByTagName("BMS_UNIT_OUTLINE_CNT").item(0).toElement().text().toInt();
    msg = doc.elementsByTagName("SUB_MAIN_MSG1_ID").item(0).toElement().text();
    SUB_MAIN_MSG1_ID = stringid_to_intid(msg);
    msg = doc.elementsByTagName("SUB_MAIN_MSG2_ID").item(0).toElement().text();
    SUB_MAIN_MSG2_ID = stringid_to_intid(msg);
    msg = doc.elementsByTagName("JURYUAN_MSG_ID").item(0).toElement().text();
    JURYUAN_MSG_ID = stringid_to_intid(msg);
    msg = doc.elementsByTagName("CONG_BOARD_STATE_ID").item(0).toElement().text();
    CONG_BOARD_STATE_ID = stringid_to_intid(msg);
    msg = doc.elementsByTagName("CONG_BOARD_TEMP_ID").item(0).toElement().text();
    CONG_BOARD_TEMP_ID = stringid_to_intid(msg);
    msg = doc.elementsByTagName("CONG_BOARD_VOL_ID").item(0).toElement().text();
    CONG_BOARD_VOL_ID = stringid_to_intid(msg);
    msg = doc.elementsByTagName("UPME_SET_MAIN_PARAM_ID").item(0).toElement().text();
    UPME_SET_MAIN_PARAM_ID = stringid_to_intid(msg);
    param_ac_byte = doc.elementsByTagName("param_ac_byte").item(0).toElement().text();
    voltag_store_file_len_max = doc.elementsByTagName("voltag_store_file_len_max").item(0).toElement().text().toLong();
    VOLTAG_SAVE_MODE = doc.elementsByTagName("VOLTAG_SAVE_MODE").item(0).toElement().text().toInt();
    BMS_BAT_COUNT_IN_EXCEL = doc.elementsByTagName("BMS_BAT_COUNT_IN_EXCEL").item(0).toElement().text().toInt();
    EXCEL_FILENAME_FORMAT = doc.elementsByTagName("EXCEL_FILENAME_FORMAT").item(0).toElement().text().toInt();
    DISPLAY_UPDATE_TIME = doc.elementsByTagName("DISPLAY_UPDATE_TIME").item(0).toElement().text().toInt();
    COMM_PROCESS_TIME = doc.elementsByTagName("COMM_PROCESS_TIME").item(0).toElement().text().toInt();
    STORE_FILE_SAVE_TI = doc.elementsByTagName("STORE_FILE_SAVE_TI").item(0).toElement().text().toInt();
    IS_MUST_NEED_CFG = doc.elementsByTagName("IS_MUST_NEED_CFG").item(0).toElement().text().toInt();
#endif
    file.close();
}
/**    @breif: 转化字符形式的CAN报文帧ID为整型
 *      @param:
 *          qmsg:   字符型式的帧ID
 *      @return:    整型的帧ID
*/
unsigned int TkbmWidget::stringid_to_intid(QString qmsg)
{
    bool ok;
    QString msg = qmsg;
    //去处字符串前面的 16进制头,如果不带0x头的字符串将会转换失败
    if(QString::compare("0x",msg.left(2)) == 0 || QString::compare("0X",msg.left(2)) == 0){
        msg = msg.mid(2);
    }
    if(ok==false)
        return 0;
    return (unsigned int)msg.toLong(&ok,16);
}
/**    @breif: 启动时初始化的数据
*/
void TkbmWidget::chg_stage_data_init()
{
    QTableWidgetItem *item;
    item = ui->tb_ctl_info->item(MSG_CNT_FORCE_CTL,0);
    item->setText(msg_cnt_force_ctrol[0]);
    item = ui->tb_ctl_info->item(MSG_VCUCAN_ALM_CTL,0);
    item->setText(msg_vcu_can_alarm[0]);
    cnt_ctrl_clk = 0;
    vcu_alarm = 0;
    main_is_test_mode = false;
    eeprom_is_send_data = false;
    eeprom_recv_data_flag = false;
    eeprom_read_btn_state = false;
    eeprom_set_launch_setting = false;
    mechine_first_launch_time = QDateTime::currentDateTime();
    memset(&eeprom_info,0,sizeof(struct eeprom_data_info_discription));

    item = ui->tb_brief->item(0,0);
    item->setText(QString("%1").arg(VOLTAG_SAVE_TIME*50));

    bms_sub_info = new struct per_battery_info_discription;
    bms_sub_info->cur_id = 0;
    bms_sub_info->unit_num = 0;
    bms_sub_info->online_count = 50;
}
/**    @breif: 定时器超时处理函数，定时处理通信数据
*/
void TkbmWidget::comm_timeout()
{
    int i,j,k;
    QString msg;
    int rowCount;

    mutex.lock();
    for(i=0;i<can_data.len;i++){                        //报文分析,一次处理接收缓存中的全部报文
        if(can_data.data[i].ID == SUB_MAIN_MSG2_ID){                //处理主板数据2
            bms_sub_info->online_count = BMS_UNIT_OUTLINE_CNT;         //主板在线状态
            switch (can_data.data[i].Data[0]) {
            case 0:
                msg = main_info_msg(can_data.data[i].Data[1],1);
                msg_summary[MSG_SUMMARY_LIST_CNST].s_val = msg;
                (can_data.data[i].Data[1] > 1) ? (msg_summary[MSG_SUMMARY_LIST_CNST].f_color = Qt::red) : (msg_summary[MSG_SUMMARY_LIST_CNST].f_color = Qt::black);
                msg = main_info_msg(can_data.data[i].Data[2],2);
                msg_summary[MSG_SUMMARY_LIST_CHGST].s_val = msg;
                msg_summary[MSG_SUMMARY_LIST_CHGST].f_color = Qt::black;
                msg_summary[MSG_SUMMARY_LIST_CCON].s_val = QString("%1").arg(can_data.data[i].Data[3]);
                msg_summary[MSG_SUMMARY_LIST_TVOL].s_val = QString("%1").arg((can_data.data[i].Data[4]+can_data.data[i].Data[5]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_TCUR].s_val = QString("%1").arg((can_data.data[i].Data[6]+can_data.data[i].Data[7]*256)*0.1-400);
                break;
            case 1:
                msg_summary[MSG_SUMMARY_LIST_SOC].s_val = QString("%1").arg((can_data.data[i].Data[1]+can_data.data[i].Data[2]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_ACCAI].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_OFCAT].s_val = QString("%1").arg((can_data.data[i].Data[5]+can_data.data[i].Data[6]*256)*0.1);
                msg_summary[MSG_SUMMARY_LIST_HERTICK].s_val = QString("%1").arg(can_data.data[i].Data[7]);
                break;
            case 2:
                unit_limit_mast.max = can_data.data[i].Data[1]+can_data.data[i].Data[2]*256;
                msg_summary[MSG_SUMMARY_LIST_MTPV].s_val = QString("%1").arg(unit_limit_mast.max*0.001);
                msg_summary[MSG_SUMMARY_LIST_MVPS].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*1);
                msg_summary[MSG_SUMMARY_LIST_MTTP].s_val = QString("%1").arg(can_data.data[i].Data[5]-40);
                break;
            case 3:
                unit_limit_mast.min = can_data.data[i].Data[1]+can_data.data[i].Data[2]*256;
                msg_summary[MSG_SUMMARY_LIST_MLPV].s_val = QString("%1").arg(unit_limit_mast.min*0.001);
                msg_summary[MSG_SUMMARY_LIST_MLPS].s_val = QString("%1").arg((can_data.data[i].Data[3]+can_data.data[i].Data[4]*256)*1);
                msg_summary[MSG_SUMMARY_LIST_MLTP].s_val = QString("%1").arg(can_data.data[i].Data[5]-40);
                msg_summary[MSG_SUMMARY_LIST_DICVOL].s_val = QString("%1").arg(unit_limit_mast.max-unit_limit_mast.min);
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
                msg_chg_summary[MSG_CHG_SUMMARY_CHG_STAGE].s_val = msg_chg_stage[can_data.data[i].Data[1]&0x07];
                msg_chg_summary[MSG_CHG_SUMMARY_IIME_NEED].s_val = QString("%1").arg(can_data.data[i].Data[2]+can_data.data[i].Data[3]*256);
                msg_chg_summary[MSG_CHG_SUMMARY_OUT_POW].s_val = QString("%1").arg((can_data.data[i].Data[4]+can_data.data[i].Data[5]*256)*0.1);
                msg_chg_summary[MSG_CHG_SUMMARY_CHG_TIM].s_val = QString("%1").arg(can_data.data[i].Data[6]+can_data.data[i].Data[7]*256);
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
                    if((can_data.data[i].Data[2] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_chg_stop_err[j] + "  ";
                }
                if(j==4)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_CHG_STOP].s_val = msg;

                msg = QString("");
                for(j=0;j<8;j++){
                    if((can_data.data[i].Data[3] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_chg_stop_fat[j] + "  ";
                }
                if(j==8)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_CHG_ERR].s_val = msg;

                msg = QString("");
                for(j=0;j<6;j++){
                    if((can_data.data[i].Data[4] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_bms_rcv_err[j] + "  ";
                }
                if(j==6)    msg = ' ';
                msg_chg_err_disc[MSG_CHG_ERROR_BMS_COM].s_val = msg;

                msg = QString("");
                for(j=0;j<7;j++){
                    if((can_data.data[i].Data[5] >> j) & 0x01)
                        msg += QString("%1:").arg(j) + msg_chg_rcv_err[j] + "  ";
                }
                if(j==7)    msg = ' ';
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
            case 17:                //从板配置信息,计算从板第一通道在系统中的位置
                bms_sub_info->unit_num = can_data.data[i].Data[7];
                k=0;
                for(auto iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
                    if(can_data.data[i].Data[1] == iter->bid){
                        iter->modu_num = can_data.data[i].Data[2];
                        for(j=0;j<iter->modu_num;j++){
                            iter->per_chinnal[j] = (can_data.data[i].Data[j+3] > EACH_MODULE_CHINNEL) ? (EACH_MODULE_CHINNEL) :(can_data.data[i].Data[j+3]);
                            k += can_data.data[i].Data[j+3];
                        }
                        if(iter == bms_sub_info->each_board.begin()){
                            iter->chinnel_start = 0;
                        }
                 //此处计算从板位置要经过多次接收主板状态报文才能得到正确的结果
                        (iter+1)->chinnel_start = iter->chinnel_start + k;
                        break;
                    }
                }
                break;
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
            for(auto iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
                if(iter->bid == can_data.data[i].Data[0]){          //如果存在该id,则按id修改内容
                    iter->online_count = BMS_UNIT_OUTLINE_CNT;                    
                    sub_state_msg_ana(iter,can_data.data[i].Data);
                    if(iter != bms_sub_info->each_board.begin()){
                        int temp = 0;
                        for(j=0;j<(iter-1)->modu_num;j++)
                            temp += (iter-1)->per_chinnal[j];
                        iter->chinnel_start = (iter-1)->chinnel_start + temp;
                    }
                    j = -1;
                }
            }
            if(j != -1){                    //如果不存在该id,则创建一个数据,查找合适的位置放入
                struct sub_each_board ebd;
                //新建从板时设置默认值
                memset(&ebd,0,sizeof(struct sub_each_board));
                ebd.modu_num = SUB_MODULE_NUM_MAX;
                ebd.per_chinnal[0] = ebd.per_chinnal[1] = ebd.per_chinnal[2] = ebd.per_chinnal[3] = EACH_MODULE_CHINNEL;
                sub_state_msg_ana(&ebd,can_data.data[i].Data);
                k=0;
                //按ID顺序查找，插入Vector
                for(iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
                    if(ebd.bid < iter->bid){
                        bms_sub_info->each_board.insert(k,ebd);
                        j = -1;
                        break;
                    }
                    k++;
                }
                //如果没有找到比当前ID更大的值，则把当前ID放到最后
                if(j != -1)
                    bms_sub_info->each_board.push_back(ebd);
                //插入新从板后重新计算起始端口在系统中的位置
                for(iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
                    if(iter != bms_sub_info->each_board.begin()){
                        int temp = 0;
                        for(j=0;j<(iter-1)->modu_num;j++)
                            temp += (iter-1)->per_chinnal[j];
                        iter->chinnel_start = (iter-1)->chinnel_start + temp;
                    }
                }
                if(bms_sub_info->online_count == 0)
                    bms_sub_info->unit_num++;           //插入新从板后增加计数
            }            
        }    
        if(CONG_BOARD_TEMP_ID == (can_data.data[i].ID & 0xfffffff0)){   //处理从板温度信息
            for(auto iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
                if(iter->bid == can_data.data[i].Data[0]){
                    for(j=0;j<CONG_BOARD_EACH_TEMP;j++){
                        k = can_data.data[i].Data[1]*CONG_BOARD_EACH_TEMP+j;
                        if(k<SUB_TEMPRATURE_NUM_MAX)
                            iter->temp[k] = can_data.data[i].Data[j+2]-40;
                    }
                    break;
                }
            }
        }
        if(CONG_BOARD_VOL_ID == (can_data.data[i].ID & 0xfffffff0)){    //从板电压信息
            for(auto iter = bms_sub_info->each_board.begin();iter!=bms_sub_info->each_board.end();iter++){
                if(iter->bid == can_data.data[i].Data[0]){              
                    for(j=0;j<CONG_BOARD_EACH_VOL;j++){
                        k = can_data.data[i].Data[1]*CONG_BOARD_EACH_VOL+j;
                        if(k<TOTAL_MODULE_LENGTH)           //电池数溢出控制
                            iter->each_volt[k] = can_data.data[i].Data[2+j*2] + can_data.data[i].Data[3+j*2]*256;
                    }
                    break;
                }
            }
        }
        if(SUB_MAIN_MSG1_ID == can_data.data[i].ID){                //处理主板数据1
            if(can_data.data[i].Data[0] <= 0x2D){
                for(j=0;j<EEPROM_SEND_DATA_LEN;j++){
                    eeprom_info.eeprom[can_data.data[i].Data[0]*EEPROM_SEND_DATA_LEN+j] = can_data.data[i].Data[1+j];
                    eeprom_info.recv_mark[can_data.data[i].Data[0]] = 1;
                }
            }
            for(j=0;j<EEPROM_DATA_RECV_MARK;j++){
                if(eeprom_info.recv_mark[j] == 0)
                    break;
            }
            if(j == EEPROM_DATA_RECV_MARK){
                eeprom_launch_send(false);
                eeprom_info.recv_finish_flag = true;
            }
            if(can_data.data[i].Data[0] == 0xD0){
                eeprom_info.send_mark[can_data.data[i].Data[1]] = 1;
                if(can_data.data[i].Data[2] == 1){
                    QTableWidgetItem *item = ui->tb_state->item(0,2);
                    item->setText("修改成功\0");
                    eeprom_recv_data_flag = false;
                    QTimer *timer_temp = new QTimer(this);
                    connect(timer_temp,&QTimer::timeout,this,[=](){
                        eeprom_launch_send(true);
                        timer_temp->stop();
                    });
                    timer_temp->start(5000);
                }
            }
        }
    }
    can_data.len = 0;       //处理完成后设置缓存长度为0，并没有清理缓存
    mutex.unlock();
    if(voltag_store_cnt-- <0){                          //数据保存周期到达,数据写入到缓存
        voltag_store_cnt = VOLTAG_SAVE_TIME;
        //保存全部电压
        save_data_mutex.lock();
        for(auto iter = bms_sub_info->each_board.begin();iter!=bms_sub_info->each_board.end();iter++){
            iter->data_time = QDateTime::currentDateTime();
            store_voltag_data.push_back(*iter);
        }
        //保存概述信息
        rowCount = ui->tb_breif->rowCount();
        struct breif_info_for_bms lable;
        lable.time = QDateTime::currentDateTime();
        for(i=0;i<rowCount;i++){
            QTableWidgetItem *item;
            item = ui->tb_breif->verticalHeaderItem(i);
            lable.tital.push_back(item->text());
            item = ui->tb_breif->item(i,0);
            lable.value.push_back(item->text());
        }
        //保存单体最高
        for(i=0;i<CONG_BOARD_LIMIT_VOL_LEN;i++){
            lable.tital.push_back(QString("%2 %1").arg(i+1).arg(msg_unit_limit_param[0]));
            lable.tital.push_back(QString("%2 %1").arg(i+1).arg(msg_unit_limit_param[1]));
            if(i<limit_vol_info.max.size()){
                lable.value.push_back(QString("%1").arg(limit_vol_info.max.at(i).voltag));
                lable.value.push_back(QString("%1").arg(limit_vol_info.max.at(i).pos));
            }else{
                lable.value.push_back(QString(""));
                lable.value.push_back(QString(""));
            }
        }
        //保存单体最低
        for(i=0;i<CONG_BOARD_LIMIT_VOL_LEN;i++){
            lable.tital.push_back(QString("%2 %1").arg(i+1).arg(msg_unit_limit_param[2]));
            lable.tital.push_back(QString("%2 %1").arg(i+1).arg(msg_unit_limit_param[3]));
            if(i<limit_vol_info.min.size()){
                lable.value.push_back(QString("%1").arg(limit_vol_info.min.at(i).voltag));
                lable.value.push_back(QString("%1").arg(limit_vol_info.min.at(i).pos));
            }else{
                lable.value.push_back(QString(""));
                lable.value.push_back(QString(""));
            }
        }
        breif_info.push_back(lable);
        save_data_mutex.unlock();
    }

    bms_sub_info->online_count = (bms_sub_info->online_count <= 0)?(0):(bms_sub_info->online_count-1);       //主板在线计数

    for(auto iter=bms_sub_info->each_board.begin();iter!=bms_sub_info->each_board.end();iter++){
        if(iter->bid == bms_sub_info->cur_id){         //标记通信超时的从板，通过从板状态报文
            if(iter->online_count != 0)
                iter->online_count--;
            if(iter->online_count == 0){
                memset(iter->each_volt,0,ARRAYSIZE(iter->each_volt)*sizeof(int));
            }
        }
    }
    if(eeprom_recv_data_flag == true){          //EEPROM配置报文发送状态显示
        for(i=0;i<EEPROM_DATA_SEND_MARK;i++){
            if(eeprom_info.send_mark[i] != 1){
                can_bsp->send_msg(1,&eeprom_send_array[i]);
                QTableWidgetItem *item = ui->tb_state->item(0,2);
                item->setText(QString("正在发送[%1/%2]").arg(i).arg(EEPROM_DATA_SEND_MARK));
                break;
            }
        }
//        if(i==EEPROM_DATA_SEND_MARK){
//            eeprom_recv_data_flag = false;
//            eeprom_launch_send(false);
//            QTableWidgetItem *item = ui->tb_state->item(0,2);
//            item->setText("发送完成\0");
//        }
    }
    per_bat_vol_limit_cacul();
}
/**    @breif: 计算电压极限值，最高最低
*/
void TkbmWidget::per_bat_vol_limit_cacul()
{
    unsigned int i;
    struct array_vol_pos vol_pos;
    unsigned int board_size;
    std::vector<struct array_vol_pos> temp;
    //收集全部电压和位置信息
    for(auto iter=bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
        if((main_is_test_mode == false) && (iter->bid > bms_sub_info->unit_num-1))
            break;
        if(iter->modu_num > 0){
            for(i=0;i<iter->per_chinnal[0];i++){
                vol_pos.voltag = iter->each_volt[i];
                vol_pos.pos = iter->chinnel_start+i+1;
                temp.push_back(vol_pos);
            }
        }
        if(iter->modu_num > 1){
            for(i=0;i<iter->per_chinnal[1];i++){
                vol_pos.voltag = iter->each_volt[EACH_MODULE_CHINNEL+i];
                vol_pos.pos = iter->chinnel_start+iter->per_chinnal[0]+i+1;
                temp.push_back(vol_pos);
            }
        }
        if(iter->modu_num > 2){
            for(i=0;i<iter->per_chinnal[2];i++){
                vol_pos.voltag = iter->each_volt[EACH_MODULE_CHINNEL*2+i];
                vol_pos.pos = iter->chinnel_start+iter->per_chinnal[0]+iter->per_chinnal[1]+i+1;
                temp.push_back(vol_pos);
            }
        }
        if(iter->modu_num > 3){
            for(i=0;i<iter->per_chinnal[3];i++){
                vol_pos.voltag = iter->each_volt[EACH_MODULE_CHINNEL*3+i];
                vol_pos.pos = iter->chinnel_start+iter->per_chinnal[0]+iter->per_chinnal[1]+iter->per_chinnal[2]+i+1;
                temp.push_back(vol_pos);
            }
        }
    }
    board_size = temp.size();
    //如果全部电池数量小于待显示的电池数量，则仅显示有的电池，否则显示配置的显示数量
    board_size = (board_size < CONG_BOARD_LIMIT_VOL_LEN)?(board_size):(CONG_BOARD_LIMIT_VOL_LEN);
    //按值大小升序排序
    std::sort(temp.begin(),temp.end(),[](struct array_vol_pos a,struct array_vol_pos b){return a.voltag < b.voltag;});
    limit_vol_info.max.clear();
    limit_vol_info.min.clear();
    //直接向上取小值，向下取大值
    for(i=0;i<board_size;i++){
        vol_pos = QVector<struct array_vol_pos>::fromStdVector(temp).at(i);
        limit_vol_info.min.push_back(vol_pos);
        vol_pos = QVector<struct array_vol_pos>::fromStdVector(temp).at(temp.size()-i-1);
        limit_vol_info.max.push_back(vol_pos);
    }
}
/**    @breif: 部分从板状态报文分析
 *      @param:
 *          bd: 待设置的从板
 *          Data:   CAN报文
*/
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
        if(bms_sub_info->online_count == 0)
            bd->modu_num = Data[5];
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
/**    @breif: EXCEL写入时间达到，发送信号到EXCEL写入子线程。同时刷新残留的数据缓存
*/
static int run_time=0;
void TkbmWidget::update_data_timeout()
{
    //发送信号给子线程保存数据到磁盘
    if((store_voltag_data.size() > 0) && (VOLTAG_IS_SAVE == 1)){    
        save_data_mutex.lock();
        emit sig_get_store_obj(&store_voltag_data,&breif_info);
        bat_store->start();
        QThread::sleep(1);
        store_voltag_data.clear();
        breif_info.clear();
        save_data_mutex.unlock();
    }
    if(IS_DATA_ARRY_CLEAR != 1) return;
    data_struct_init();         //刷新残留的数据缓存
}
/**    @breif: 分析报警信息报文
 *      @param:
 *          data:   报文数据
 *      @return:    直接得到了可以显示的报文内容
*/
QString TkbmWidget::anasy_alart_msg(unsigned char data[5])
{
    QString temp;
    int i,j;
    int val;
    for(i=0;i<msg_alart_list_line;i++){
        for(j=0;j<4;j++){
            val = data[i]>>(2*j) & 0x03;
            if(val){
                temp += msg_alart_list[i][j] + '\t' + msg_alart_level[val] + '\n';
            }
        }
    }
    return temp;
}
/**    @breif: 部分主板报文分析
 *      @param:
 *          data:   报文数据
 *          pos:    位置标记
 *      @return:    得到的报文内容
*/
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
/**    @breif: 定时器超时,刷新信息到屏幕
*/
void TkbmWidget::update_msg_timeout()
{
    int item_count;
    int col_count;
    int i,j;
    char temp_tmp;
    QString msg;
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
        if(i == MSG_ACT_CONTAIN || i == MSG_VCUCAN_ALM_CTL || i==MSG_OFF_CONTAIN || i==MSG_CNT_FORCE_CTL || i==MSG_SOC_SET)     continue;
        QTableWidgetItem *item = ui->tb_ctl_info->item(i,0);
        item->setText(msg_main_ctl_info[i].s_val);
        item->setForeground(msg_main_ctl_info[i].f_color);
        item->setBackground(msg_main_ctl_info[i].b_color);
    }
    //--------------------------------------------
    if(bms_sub_info->each_board.size() != 0){        //更新单体信息
        QVector<struct sub_each_board>::iterator iter;
        for(iter=bms_sub_info->each_board.begin();iter!=bms_sub_info->each_board.end();iter++){
            if(iter->bid == bms_sub_info->cur_id){                
                msg = "";
                for(i=0;i<8;i++){
                    if((iter->state >> i) & 0x01)
                        msg += msg_bms_run_state[i] + '\n';
                }
                QTableWidgetItem *item = ui->tb_run_state->item(0,1);
                if(iter->online_count == 0)
                    msg += QString("从板离线中\0");
                item->setText(msg);
                item = ui->tb_run_state->item(0,2);
                item->setText(QString("%1").arg(iter->htick));
                msg_bms_run_state_dsc[MODULE_NUM].s_val = QString("%1").arg(iter->modu_num);
                msg_bms_run_state_dsc[SOFT_VERSION].s_val = QString("%1").arg(iter->sw,2,16,QLatin1Char('0'));
                msg_bms_run_state_dsc[HARD_VERSION].s_val = QString("%1").arg(iter->hw,2,16,QLatin1Char('0'));
                msg_bms_run_state_dsc[SERIAL_NUMBER].s_val = QString("%1-%2-%3-%4-%5-%6").arg(iter->sn[0]).arg(iter->sn[1])\
                                                        .arg(iter->sn[2]).arg(iter->sn[3]).arg(iter->sn[4]).arg(iter->sn[5]);
                for(i=0;i<UNIT_INFO_NUM_MAX;i++){               //更新运行状态
                    item = ui->tb_version->item(i,0);
                    item->setText(msg_bms_run_state_dsc[i].s_val);
                    item->setBackground(QBrush(msg_bms_run_state_dsc[i].b_color));
                    item->setForeground(QBrush(msg_bms_run_state_dsc[i].f_color));
                }
                for(i=0;i<ui->tb_disconnect->columnCount();i++){        //晴空断线检测显示
                    item = ui->tb_disconnect->item(0,i);
                    item->setText("");
                }
                for(i=0;i<iter->modu_num;i++){                  //更新断线状态
                    msg = "";
                    for(j=0;j<iter->per_chinnal[i];j++){
                        if((iter->disc[i] >> j) & 0x01)
                            msg += QString("%1").arg(j) + ' ';
                    }
                    item = ui->tb_disconnect->item(0,i);
                    item->setText(msg);
                }
                for(i=0;i<SUB_TEMP_DISPLAY_ROW;i++){            //更新温度信息
                    for(j=0;j<SUB_TEMP_DISPLAY_COL;j++){
                        item = ui->tb_temp->item(i,2*j+1);
                        temp_tmp = iter->temp[i*SUB_TEMP_DISPLAY_COL+j];
                        msg = (temp_tmp != (char)0xFF) ?(QString("%1").arg(temp_tmp,0,10,QLatin1Char('0'))) : (QString("NONE"));
                        item->setText(msg);
                    }
                }
                for(i=0;i<ui->tb_vol_m1->rowCount();i++){       //清空电压显示
                    item = ui->tb_vol_m1->item(i,0);
                    item->setText("");
                    item = ui->tb_vol_m1->item(i,1);
                    item->setText("");
                    item = ui->tb_vol_m2->item(i,0);
                    item->setText("");
                    item = ui->tb_vol_m2->item(i,1);
                    item->setText("");
                    item = ui->tb_vol_m3->item(i,0);
                    item->setText("");
                    item = ui->tb_vol_m3->item(i,1);
                    item->setText("");
                    item = ui->tb_vol_m4->item(i,0);
                    item->setText("");
                    item = ui->tb_vol_m4->item(i,1);
                    item->setText("");
                }                
                if(iter->modu_num > 0){                        //更新模块A电压
                    for(i=0;i<iter->per_chinnal[0];i++){
                        item = ui->tb_vol_m1->item(i,1);
                        item->setText(QString("%1").arg(iter->each_volt[i]));
                        item = ui->tb_vol_m1->item(i,0);
                        item->setText(QString("%1").arg(iter->chinnel_start+i+1));
                    }
                }
                if(iter->modu_num > 1){                     //更新模块B电压
                    for(i=0;i<iter->per_chinnal[1];i++){
                        item = ui->tb_vol_m2->item(i,1);
                        item->setText(QString("%1").arg(iter->each_volt[EACH_MODULE_CHINNEL+i]));
                        item = ui->tb_vol_m2->item(i,0);
                        item->setText(QString("%1").arg(iter->chinnel_start+iter->per_chinnal[0]+i+1));
                    }
                }
                if(iter->modu_num > 2){                     //更新模块C电压
                    for(i=0;i<iter->per_chinnal[2];i++){
                        item = ui->tb_vol_m3->item(i,1);
                        item->setText(QString("%1").arg(iter->each_volt[EACH_MODULE_CHINNEL*2+i]));
                        item = ui->tb_vol_m3->item(i,0);
                        item->setText(QString("%1").arg(iter->chinnel_start+iter->per_chinnal[0]+iter->per_chinnal[1]+i+1));
                    }
                }
                if(iter->modu_num > 3){                     //更新模块D电压
                    for(i=0;i<iter->per_chinnal[3];i++){
                        item = ui->tb_vol_m4->item(i,1);
                        item->setText(QString("%1").arg(iter->each_volt[EACH_MODULE_CHINNEL*3+i]));
                        item = ui->tb_vol_m4->item(i,0);
                        item->setText(QString("%1").arg(iter->chinnel_start+iter->per_chinnal[0]+iter->per_chinnal[1]+iter->per_chinnal[2]+i+1));
                    }
                }
                break;
            }
        }
    }
    if(eeprom_is_send_data == true){                 //显示EEPROM数据
        item_count = ui->tb_eep_content->rowCount();
        col_count = ui->tb_eep_content->columnCount();
        for(i=0;i<item_count;i++){
            for(j=0;j<col_count;j++){                
                QTableWidgetItem *item = ui->tb_eep_content->item(i,j);
                item->setText(QString("%1").arg(eeprom_info.eeprom[i*col_count + j],2,16,QLatin1Char('0')).toUpper());
            }
        }
    }
    if(eeprom_info.recv_finish_flag == true){        //分析EEPROM数据        
        tb_eeprom_file_setting(eeprom_info.eeprom,EEP_TB_RECV);
    }
    if(limit_vol_info.max.isEmpty() == false){          //更新单体最高最低
        for(i=0;i<limit_vol_info.max.size();i++){
            QTableWidgetItem *item = ui->tb_each_hi->item(i,0);
            msg = QString("%1").arg(limit_vol_info.max.at(i).voltag);
            item->setText(msg);
            item = ui->tb_each_hi->item(i,1);
            msg = QString("%1").arg(limit_vol_info.max.at(i).pos);
            item->setText(msg);

            item = ui->tb_each_lo->item(i,0);
            msg = QString("%1").arg(limit_vol_info.min.at(i).voltag);
            item->setText(msg);

            item = ui->tb_each_lo->item(i,1);
            msg = QString("%1").arg(limit_vol_info.min.at(i).pos);
            item->setText(msg);
        }
    }

}
/**    @breif: 把EEPROM数据按规则分析，并显示到屏幕
 *      @param:
 *          data:   EEPROM数据
 *          col:    显示区上待显示的列
*/
void TkbmWidget::tb_eeprom_file_setting(unsigned char *data,int col)
{
    int item_count,i,j;
    int iparam;
    double dparam;
    QString msg,msg1,msg2;
    QTableWidgetItem *item,*item1,*item2;
    item_count = ui->tb_eep_file->rowCount();
    eep_config.pos = 0;
    eeprom_set_launch_setting = false;
    for(i=0;i<item_count;i++){      //按显示的 表格控件 行 分析此行数据占有的字节数
        iparam = 0;
        for(j=0;j<eep_config.param[i].o_byte_len;j++)       //按指定长度获取数据
            iparam += data[eep_config.pos + j] << (8*j);
        eep_config.pos += eep_config.param[i].o_byte_len;   //取出数据后更新位置索引
        dparam = iparam * eep_config.param[i].o_rate + eep_config.param[i].o_off;   //按照规则计数数据， val = kx + b
        item = ui->tb_eep_file->item(i,col);
        if(eep_config.param[i].o_dis_format == 10){         //按十进制或其它进制转换数据
            msg = QString("D: %1").arg(dparam,0,'g',10);
        }else{
            msg = QString("H: %1").arg((int)dparam,2,eep_config.param[i].o_dis_format,QLatin1Char('0')).toUpper();
        }
        item1 = ui->tb_eep_file->item(i,EEP_TB_RECV);
        msg1 = item1->text().toUpper();
        item2 = ui->tb_eep_file->item(i,EEP_TB_FILE);
        msg2 = item2->text().toUpper();
        if(msg1.isEmpty() || msg1.isNull() || msg2.isEmpty() || msg2.isNull()){
        }else{          //若有目标值和预设值不同则着色显示
            if(QString::compare(msg1,msg2,Qt::CaseInsensitive) != 0)
                item->setBackgroundColor(QColor(255,150,45));
        }
        item->setText(msg);
    }
    eeprom_set_launch_setting = true;
}
/**    @breif: 获取监视器程序发送来的从板ID选择
 *      @param:
 *          bid:    监视器程序发送的 从板 ID
*/
void TkbmWidget::slot_get_board_id(int bid)
{
    bms_sub_info->cur_id = bid;
    ui->le_id_in->setText(QString("%1").arg(bid));
    emit ui->le_id_in->editingFinished();       //修改ID后发送信号通知从板ID已改变
}
/**    @breif: 获取启动程序发送CAN配置信息
 *      @param:
 *          dev；    CAN设备名
 *          num:    CAN设备号
 *          rate:   波特率
 *          prot:   CAN段口号
*/
void TkbmWidget::sig_get_can_param(int dev,int num,int rate,int port)
{
    can_bsp = new CtlCan(dev,num,port);
    if(can_bsp->open((CtlCan::baudRate)rate)){
        can_bsp->state = CtlCan::off;
        QMessageBox::information(this,"ERROR",QString("打开设备失败,请检查连接\0"));
        this->close();
    }else{
        can_bsp->state = CtlCan::on;
        RecvCan *tRecv = new RecvCan;           //创建子线程，接收CAN数据
        tRecv->start();
        connect(this,&TkbmWidget::sig_set_cthread_state,tRecv,&RecvCan::slot_get_state);
        emit sig_set_cthread_state(can_bsp);    //发送信号，CAN接收缓存

        timer10 = new QTimer(this);
        connect(timer10,&QTimer::timeout,this,&TkbmWidget::comm_timeout);
        timer10->start(COMM_PROCESS_TIME);      //启动定时器，定时处理CAN数据
    }
}
/**    @breif: 重新初始化数据，清空数据缓存
*/
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
        if(i == MSG_ACT_CONTAIN || i == MSG_VCUCAN_ALM_CTL || i==MSG_OFF_CONTAIN || i==MSG_CNT_FORCE_CTL || i==MSG_SOC_SET)     continue;
        msg_main_ctl_info[i].f_color = Qt::black;
        msg_main_ctl_info[i].b_color = Qt::white;
        msg_main_ctl_info[i].i_val = 0;
        msg_main_ctl_info[i].s_val = ' ';
    }
    for(int i=0;i<UNIT_INFO_NUM_MAX;i++){
        msg_bms_run_state_dsc[i].f_color = Qt::black;
        msg_bms_run_state_dsc[i].b_color = Qt::white;
        msg_bms_run_state_dsc[i].i_val = 0;
        msg_bms_run_state_dsc[i].s_val = ' ';
    }
}

void TkbmWidget::ui_init(void)
{
    ui->tb_breif->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_breif->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->horizontalHeader()->setVisible(true);
    ui->tb_vol_m1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m1->verticalHeader()->setVisible(true);
    ui->tb_vol_m2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m2->horizontalHeader()->setVisible(true);
    ui->tb_vol_m2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m2->verticalHeader()->setVisible(true);
    ui->tb_vol_m3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m3->horizontalHeader()->setVisible(true);
    ui->tb_vol_m3->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m3->verticalHeader()->setVisible(true);
    ui->tb_vol_m4->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m4->horizontalHeader()->setVisible(true);
    ui->tb_vol_m4->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_vol_m4->verticalHeader()->setVisible(true);
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
    for(int i=0;i<ui->tb_eep_content->rowCount();i++)
        for(int j=0;j<ui->tb_eep_content->columnCount();j++)
            ui->tb_eep_content->setItem(i,j,new QTableWidgetItem);
    ui->tb_eep_content->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_eep_content->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->le_id_in->setValidator(new QIntValidator(0,100,this));
    ui->le_id_in->setText(QString("0"));
}

void TkbmWidget::set_eep_config(void)
{
    read_cfg = new ReadExcelCfg;
    connect(this,&TkbmWidget::sig_send_eep_config,read_cfg,&ReadExcelCfg::get_eep_config);
    connect(read_cfg,&ReadExcelCfg::send_progress_status,this,[=](int status){
        switch (status) {
        case 0:set_eeprom_table();break;
        case 1:
            QMessageBox::information(this,"error",QString("缺少配置文件:\n[%1]\n程序关闭").arg(file_path));
            this->~TkbmWidget();
        case 2:
            QMessageBox::information(this,"error",QString("无法启用 MicroSoft Excel 或者 WPS"));
            this->~TkbmWidget();
        default:
            break;
        }
    });
    emit sig_send_eep_config(&eep_config);
    read_cfg->start();
}
/**    @breif: 读取EXCEL数据配置 EEPROM参数
*/
void ReadExcelCfg::read_excel_data()
{
    QString file_path = QApplication::applicationDirPath() + FILE_PATH_CONFIG;
    QFile config_file(file_path);
    eep_config->len = 0;
    if(IS_MUST_NEED_CFG != 1)
        return;
    if(config_file.exists() == false){
        send_progress_status(1);
        return;
    }

    OleInitialize(NULL);
    QAxObject *excel;
    excel_or_wps = EXCEL_USED_NAME;
    excel = new QAxObject(excel_or_wps);
    if(excel->isNull() == true){
        excel_or_wps = WPS_USED_NAME;
        excel = new QAxObject(excel_or_wps);
        if(excel->isNull() == true){           
            excel->dynamicCall("Quit(void)");
            delete excel;
            emit send_progress_status(2);
            return;
        }
    }

    excel->setProperty("Visible",false);
    QAxObject *pWorkbooks = excel->querySubObject("WorkBooks");
    pWorkbooks->dynamicCall("Open (Const QString&)",config_file.fileName());
    QAxObject *pWorkBook = excel->querySubObject("ActiveWorkBook");
    QAxObject *pSheet = pWorkBook->querySubObject("Sheets(int)",1);
    QAxObject *pCell;
    QVariant cell_val;
    QString val;
    do{
        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_NUM);     //获取参数编号
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        if(val.isEmpty() == true || val.isNull() == true){
            break;
        }
        eep_config->param[eep_config->len].s_num = val.toInt();       //获取数据编号

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_NAME);     //获取参数名
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        eep_config->param[eep_config->len].s_name = val;          //获取参数名
        if(QString::compare(val,param_ac_byte) == 0)
            epprom_set_ac_pos = eep_config->len;                 //如果得到交验行，获取行号

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_LEN);     //获取数据长度
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        eep_config->param[eep_config->len].o_byte_len = val.toInt();      //获取参数字节长度

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_FORMAT);     //获取显示格式
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        eep_config->param[eep_config->len].o_dis_format = val.toInt();    //获取参数显示格式

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_RATE);     //获取比例
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        eep_config->param[eep_config->len].o_rate = val.toDouble();       //获取参数放大系数

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_OFF);     //获取偏移
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        eep_config->param[eep_config->len].o_off = val.toInt();           //获取参数偏移

        pCell = pSheet->querySubObject("Cells(int,int)",FILE_CONFIG_ROW_START+eep_config->len,CONFIG_COL_PARAM_NOTE);     //获取参数注释
        cell_val = pCell->property("Value2");
        val = cell_val.toString();
        val += QString("\n\n比例:%1,偏移:%2").arg(eep_config->param[eep_config->len].o_rate).arg(eep_config->param[eep_config->len].o_off);
        eep_config->param[eep_config->len].s_note = val;          //获取参数注释

        eep_config->len++;               //参数总数计数器加1
    }while(true);
    pWorkBook->dynamicCall("Close(Boolen)",false);
    excel->dynamicCall("Quit(void)");
    delete excel;
    emit send_progress_status(0);
    OleUninitialize();
}

void ReadExcelCfg::get_eep_config(struct excel_param_organize_ext *eepConfig)
{
    eep_config = eepConfig;
}

void ReadExcelCfg::run()
{
    read_excel_data();
}
/**    @breif: 把EEPROM配置参数显示到屏幕
*/
void TkbmWidget::set_eeprom_table()
{
    int i;
    int row_num;
    for(i=0;i<eep_config.len;i++){
        row_num = ui->tb_eep_file->rowCount();
        ui->tb_eep_file->insertRow(row_num);
        ui->tb_eep_file->setVerticalHeaderItem(i,new QTableWidgetItem(QString("%1").arg(i)));
        ui->tb_eep_file->setItem(i,0,new QTableWidgetItem(eep_config.param[i].s_name));
        ui->tb_eep_file->setItem(i,1,new QTableWidgetItem);
        ui->tb_eep_file->setItem(i,2,new QTableWidgetItem);
    }
}

TkbmWidget::~TkbmWidget()
{
    if(can_bsp != NULL)
        delete can_bsp;
    delete bms_sub_info;
    delete read_cfg;
    delete bat_store;
    delete ui;
    QThread::sleep(2);
    exit(1);
}

void TkbmWidget::on_tb_eep_file_cellClicked(int row, int column)
{
    ui->tbs_note->setText(eep_config.param[row].s_note);
}

void TkbmWidget::closeEvent(QCloseEvent *event)
{
    emit sig_set_cthread_state(NULL);       //发送信号到CAN接收子线程，通知结束死循环
    emit sig_process_exit();
    monitor_dialog.hide();
    if(can_bsp != NULL)
        can_bsp->close();               //关闭CAN接收器
    QWidget::closeEvent(event);
}

void TkbmWidget::on_rb_exp_dis_clicked()
{
    if(ui->rb_exp_dis->isChecked())
    {
        monitor_dialog.show();
    }else{
        monitor_dialog.hide();
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
            if(can_data.len > CAN_RECV_BUFF_LEN_MAX){       //如果数据长度大于接收缓存长度，重置为零
                can_data.len = 0;
            }
            read_len = can_bps->get_recive_num();
            if(read_len == 0)   continue;                   //如果CAN设备中没有数据，则跳过此次接收
            if(can_data.len + read_len > CAN_RECV_BUFF_LEN_MAX){
                can_bps->clear_buff();
                read_len = 0;
                can_data.len = 0;
            }

            read_len = can_bps->can_rec_read(recv_buff,read_len);   //接收CAN数据
            mutex.lock();
            memcpy(&can_data.data[can_data.len],recv_buff,sizeof (VCI_CAN_OBJ)*read_len);   //复制数据
            can_data.len += read_len;
            mutex.unlock();

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
    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);       //同步BMS时间
}

void TkbmWidget::on_pb_modify_clicked()
{
    QTableWidgetItem *item = ui->tb_ctl_info->item(MSG_ACT_CONTAIN,0);
    QString text = item->text();
    bool ok;
    unsigned char data[8];
    unsigned int temp;
    float ac_contain,off_contain,write_soc;
    ac_contain = text.toFloat(&ok);
    if(ok == false){
        QMessageBox::information(this,"错误","[无效的参数] 实际容量");
        return;
    }
    item = ui->tb_ctl_info->item(MSG_OFF_CONTAIN,0);
    text = item->text();
    off_contain = text.toFloat(&ok);
    if(ok == false){
        QMessageBox::information(this,"错误","[无效的参数] 补偿容量");
        return;
    }
    item = ui->tb_ctl_info->item(MSG_SOC_SET,0);
    text = item->text();
    write_soc = text.toFloat(&ok);
    if(ok == false){
        QMessageBox::information(this,"错误","[无效的参数] SOC");
        return;
    }
    write_soc = write_soc * 10;
    ac_contain = ac_contain * 10;
    off_contain = off_contain * 10;

    data[0] = 0xF3;
    temp = (unsigned int)write_soc;
    data[1] = temp & 0xFF;
    data[2] = (temp >> 8) & 0xff;
    temp = (unsigned int)ac_contain;
    data[3] = temp & 0xFF;
    data[4] = (temp >> 8) & 0xFF;
    temp = (unsigned int)off_contain;
    data[5] = temp & 0xff;
    data[6] = (temp >> 8) & 0xFF;
    data[7] = 0xFF;
    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);   //修改BMS容量和SOC
}
/**    @breif: 切换报警控制和接触器控制内容
*/
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

    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);   //发送接触器控制报文
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

    can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);       //发送整车报警控制内容
}
/**  @breif:   从编辑框中获取当前选取的从板ID
*/
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
    if((main_is_test_mode == false) && (bid > bms_sub_info->unit_num-1)){           //运行模式将限制可视板ID最大值
        bid = bms_sub_info->each_board.begin()->bid;
    }
    //从从板列表中查找是否有输入的从板ID
    for(iter=bms_sub_info->each_board.begin();iter!=bms_sub_info->each_board.end();iter++){
        if(bid == iter->bid && iter->online_count != 0){
            bms_sub_info->cur_id = bid;
            j = -1;
        }
    }
    if(j != -1){        //如果没有查找到输入的从板ID
        for(iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
            if(iter->online_count != 0){        //尽检索在线的从板
                bms_sub_info->cur_id =iter->bid;    //第一个在线的从板ID设为当前ID                
                break;
            }
        }
    }
    ui->le_id_in->setText(QString("%1").arg(bms_sub_info->cur_id));
}
/**    @breif: 切换下一个从板ID
*/
void TkbmWidget::on_pb_after_clicked()
{
    bool ok;
    QVector<struct sub_each_board>::iterator iter;
    int bid = ui->le_id_in->text().toInt(&ok);
    if(ok == false){
        QMessageBox::information(this,"ERROR","无效的从板 ID");
        return;
    }

    for(iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
        if(bid <= iter->bid){
            bid = (iter+1)->bid;
            break;
        }
    }

    ui->le_id_in->setText(QString("%1").arg(bid));
    emit ui->le_id_in->editingFinished();
}
/**    @breif: 切换到上一个从板ID
*/
void TkbmWidget::on_pb_before_clicked()
{
    bool ok;
    QVector<struct sub_each_board>::iterator iter;
    int bid = ui->le_id_in->text().toInt(&ok);
    if(ok == false){
        QMessageBox::information(this,"ERROR","无效的从板 ID");
        return;
    }
    for(iter = bms_sub_info->each_board.begin();iter != bms_sub_info->each_board.end();iter++){
        if(bid <= iter->bid){
            bid = (iter-1)->bid;
            break;
        }
    }
    ui->le_id_in->setText(QString("%1").arg(bid));
    emit ui->le_id_in->editingFinished();
}
/**    @breif: 进入测试模式
*/
void TkbmWidget::on_pb_mian_into_t_clicked(bool checked)
{
    static QString style;
    if(main_is_test_mode == false){
        main_is_test_mode = true;
        style = ui->pb_mian_into_t->styleSheet();
        ui->pb_mian_into_t->setStyleSheet("background-color: rgb(255,255,170)");
    }else{
        main_is_test_mode = false;
        ui->pb_mian_into_t->setStyleSheet(style);
    }
}
/**    @breif: 启动接收EEPROM配置数据
*/
void TkbmWidget::on_pb_eep_read_clicked(bool click)
{
    eeprom_read_btn_state = !eeprom_read_btn_state;
    QTableWidgetItem *item1 = ui->tb_state->item(0,0);
    item1->setText(QString("EEPROM 数据\0"));
    eeprom_launch_send(eeprom_read_btn_state);
}
/**    @breif: 控制是否接受EEPROM数据
 *      @param:
 *          state:  状态控制
*/
void TkbmWidget::eeprom_launch_send(bool state)
{
    static QString style;
    QTableWidgetItem *item = ui->tb_state->item(0,1);
    unsigned char data[8] = {0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    if(state == true){
        eeprom_is_send_data = true;
        memset(&eeprom_info,0,sizeof(struct eeprom_data_info_discription));
        data[0] = 0xF1;
        can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);
        style = ui->pb_eep_read->styleSheet();
        ui->pb_eep_read->setStyleSheet("background-color: rgb(255,255,170)");
        item->setText("正在接收");
    }else{
        eeprom_is_send_data = false;
        data[0] = 0xF0;
        can_bsp->send_one_msg(UPME_SET_MAIN_PARAM_ID,8,data);
        ui->pb_eep_read->setStyleSheet(style);
        item->setText("接收完成");
    }
}
/**    @breif: 导出EEPROM 数据
*/
void TkbmWidget::on_pb_out_data_clicked()
{
    int i,j=0,k=0;
    if(IS_MUST_NEED_CFG != 1)
        return;
    QString fn = QFileDialog::getSaveFileName(this,"Save","./","(*.xls*)");
    if(fn.isEmpty() || fn.isNull()){
        return;
    }
    if(eeprom_info.recv_finish_flag == false){
        QMessageBox::information(this,"ERROR","[保存失败] 无效的数据\0");
        return;
    }
    QAxObject excel(excel_or_wps);
    excel.setProperty("Visible",false);
    QAxObject *pWorkbooks = excel.querySubObject("WorkBooks");
    pWorkbooks->dynamicCall("Add");
    QAxObject *workbook = excel.querySubObject("ActiveWorkBook");
    QAxObject *Sheets = workbook->querySubObject("Sheets");
    QAxObject *sheet = Sheets->querySubObject("Item (int)",1);
    QAxObject *pCell;
#if 0
    for(i=0;i<EEP_XLS_COL_LENGTH;i++){          //设置列表头
        pCell = sheet->querySubObject("Cells(int,int)",1,EEPROM_DATA_EXCEL_COL_S+i);
        pCell->setProperty("HorizontalAlignment",-4108);
        pCell->setProperty("Value",QString("%1").arg(i,2,16).toUpper());
        QAxObject *pborder = pCell->querySubObject("Borders");
        pborder->setProperty("Color",QColor(255,150,45));
    }
#endif
    do{
        if(j >= EEPROM_DATA_LENGTH)
            break;
        pCell = sheet->querySubObject("Cells(int,int)",j+1,1);        //设置行表头
        pCell->setProperty("HorizontalAlignment",-4108);
        pCell->setProperty("Value",QString("%1").arg(j));
        QAxObject *pborder = pCell->querySubObject("Borders");
        pborder->setProperty("Color",QColor(255,150,45));

        pCell = sheet->querySubObject("Cells(int,int)",j+1,EEPROM_DATA_EXCEL_COL_S);
        pCell->setProperty("HorizontalAlignment",-4108);
        pCell->setProperty("Value",QString("%1").arg(eeprom_info.eeprom[j],2,16,QLatin1Char('0')).toUpper());

        j++;
    }while(true);

    workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fn));
    workbook->dynamicCall("Close()");
    excel.dynamicCall("Quit(void)");
}
/**    @breif: 导入EEPROM数据
*/
void TkbmWidget::on_pb_in_data_clicked()
{
    int i,j=0,k=0;
    QString msg;
    bool ok = false;
    if(IS_MUST_NEED_CFG != 1)
        return;
    QString fp = QFileDialog::getOpenFileName(this,"Choice","./","*.xls*");
    if(fp.isEmpty() || fp.isNull())
        return;
    file_path = QFileInfo(fp).fileName();
    QAxObject excel(excel_or_wps);
    excel.setProperty("Visible",false);
    QAxObject *pWorkBooks = excel.querySubObject("WorkBooks");
    pWorkBooks->dynamicCall("Open (const QString&)",QDir::toNativeSeparators(fp));
    QAxObject *pWorkbook = excel.querySubObject("ActiveWorkBook");
    QAxObject *Sheet = pWorkbook->querySubObject("Sheets(int)",1);
    QAxObject *pCell;
    QVariant cell_val;

    do{
        if(j >= EEPROM_DATA_LENGTH)
            break;
        pCell = Sheet->querySubObject("Cells(int,int)",1+j,EEPROM_DATA_EXCEL_COL_S);
        cell_val = pCell->property("Value2");
        msg = cell_val.toString();
        if(msg.isEmpty() || msg.isNull()){
            QMessageBox::information(this,"ERROR","遇到空行,跳过\0");
            continue;
        }
        eeprom_info.eepcfg[j] = (unsigned char)msg.toUInt(&ok,16);
        if(ok == false){
            QMessageBox::information(this,"ERROR",QString("[转换失败]\nR:%1-C:%2").arg(1+j).arg(EEPROM_DATA_EXCEL_COL_S));
        }
        j++;
    }while(true);

    pWorkbook->dynamicCall("Close()");
    excel.dynamicCall("Quit(void)");
    QTableWidgetItem *item1 = ui->tb_state->item(0,0);
    item1->setText(file_path);
    if(ok == true)
        tb_eeprom_file_setting(eeprom_info.eepcfg,EEP_TB_FILE);
}
/**    @breif: 确认把修改的EEPROM信息发送到BMS
*/
void TkbmWidget::on_pb_ensure_clicked()
{
    QTableWidgetItem *item;
    int i;
    QString msg;
    if(IS_MUST_NEED_CFG != 1)
        return;
    item = ui->tb_state->item(0,EEP_MOD_TB_POS);
    msg = item->text();
    if(QString::compare(msg,QString(EEP_MOD_PASSWD)) != 0){
        QMessageBox::information(this,"ERROR","确认失败\0");
        item->setText("");
        return;
    }
    item->setText("");
    eeprom_after_modify();
    for(i=0;i<EEPROM_DATA_SEND_MARK;i++)
        eeprom_info.send_mark[i] = 0;
    for(i=0;i<EEPROM_DATA_SEND_MARK;i++){
        eeprom_send_array[i].Data[0] = (unsigned char)i;
        memcpy(eeprom_send_array[i].Data+1,eeprom_info.eepcfg_cp+i*7,sizeof(char)*7);
        eeprom_send_array[i].DataLen = 8;
        eeprom_send_array[i].ExternFlag = 1;
        eeprom_send_array[i].ID = UPME_SET_MAIN_PARAM_ID;
        eeprom_send_array[i].RemoteFlag = 0;
        eeprom_send_array[i].SendType = 0;
    }
    eeprom_recv_data_flag = true;
    item = ui->tb_state->item(0,2);
    eeprom_launch_send(true);
    item->setText("启动发送\0");
}
/**    @breif:     修改EEPROM后重新分析
*/
unsigned char TkbmWidget::eeprom_after_modify()
{
    QTableWidgetItem *item;
    QString msg;
    int i,j,k=0;
    int temp;
    unsigned int utmp;
    double dtmp;
    bool ok;
    int row_count = ui->tb_eep_file->rowCount();

    for(i=0;i<row_count;i++){
        item = ui->tb_eep_file->item(i,EEP_TB_FILE);
        msg = item->text();
        if(msg.isEmpty() || msg.isNull()){
            QMessageBox::information(this,"ERROR",QString("不接收空参数 [row: %1]").arg(i));
            return 0;
        }
        temp = msg.lastIndexOf(' ');
        msg = msg.mid(temp+1);
        if(eep_config.param[i].o_dis_format == 10){         //10进制数据可能含有小数，需要单独分析
            dtmp = msg.toDouble(&ok);               //转换为浮点
            dtmp -= (double)eep_config.param[i].o_off;      //减去偏移
            temp = (int)(1.0 / eep_config.param[i].o_rate);     //系数除法转换为乘法
            utmp = (unsigned int)(dtmp * temp);
        }else{
            temp = msg.toInt(&ok,eep_config.param[i].o_dis_format);
            utmp = (unsigned int)((temp - eep_config.param[i].o_off) / eep_config.param[i].o_rate);
        }
        if(ok == false){
            QMessageBox::information(this,"ERROR",QString("转换失败 [row: %1]").arg(i));
            return 0;
        }
        for(j=0;j<eep_config.param[i].o_byte_len;j++){      //装载修改过的数据
            eeprom_info.eepcfg_cp[k] = (utmp >> (8*j)) & 0xFF;
            k++;
            if(k > EEPROM_DATA_LENGTH)
                break;
        }
    }
    temp = 0;
    for(j=0;j<EEPROM_DATA_DATLENG;j++){             //计算校验
        temp += eeprom_info.eepcfg_cp[j];
        if(eeprom_info.eepcfg_cp[j] != eeprom_info.eeprom[j])
            std::cout<<"byte:"<<j<<"eeprom:"<<QString("%1").arg(eeprom_info.eeprom[j]).toStdString()<<"eeprom_cp:"<<QString("%1").arg(eeprom_info.eepcfg_cp[j]).toStdString()<<std::endl;
    }
    eeprom_info.eepcfg_cp[EEPROM_DATA_DATLENG] = (unsigned char)temp&0xff;
    return temp;
}

void BatteryStore::slot_get_store_obj(QVector<struct sub_each_board> *send_data,QVector<struct breif_info_for_bms> *breif_info)
{
    for(int i=0;i<send_data->size();i++){
       bat_unit.push_back(send_data->at(i));
    }
    for(int i=0;i<breif_info->size();i++)
        bms_breif_info.push_back(breif_info->at(i));
}
/**    @breif: 线程开始处理数据保存
 *      @attention:
 *          每次保存数据视作一次新的数据存储过程:
 *          1.判断文件是否存在，如不存在则创建文件
 *          2.打开文件
 *          3.按id查找表单，如果没有此id则创建新表单，并设置表单名为当前id
 *          4.进入该表单,索引使用行数以便设置数据的起始位置
 *          5.开始记录数据,每张表单第一次写入先写入一次电池标号
 *          6.循环写入数据到完成
 *          7.保存退出
 *      使用批量写入Excel，但远程调用Excel仍需大量时间
*/
void BatteryStore::run()
{
    OleInitialize(NULL);
    int i;
    int num;

    if(bat_unit.size() == 0)
        return;

    QString msg;
    if(EXCEL_FILENAME_FORMAT == 0){
        msg = QString("yy_MM_dd");
    }else if(EXCEL_FILENAME_FORMAT == 1){
        msg = QString("yy_MM_dd#hh");
    }else if(EXCEL_FILENAME_FORMAT == 2){
        msg = QString("yy_MM_dd#hh_mm");
    }else{
        msg = QString("yy_MM_dd#hh_mm_ss");
    }
    QString fn = mechine_first_launch_time.toString(msg) + ".xlsx";
    QString fp = QCoreApplication::applicationDirPath() + "/store_file/";
    QString fnp = fp + fn;
    QDateTime d_time;
    bool fe = QFile::exists(fnp);
    QDir dir(fp);
    if(dir.exists() == false)
        dir.mkdir(fp);

    QAxObject excel(excel_or_wps);
    excel.setProperty("Visible",false);
    excel.setProperty("DisplayAlerts",false);
    QAxObject *pWorkbooks = excel.querySubObject("WorkBooks");
    if(fe == false){
        pWorkbooks->dynamicCall("Add");                     //如果文件不存在，则创建文件
    }else{
        pWorkbooks->dynamicCall("Open (const QString&)",QDir::toNativeSeparators(fnp));     //如果文件存在，则打开
    }

    QAxObject *pWorkBook = excel.querySubObject("ActiveWorkBook");
    QAxObject *pSheets = pWorkBook->querySubObject("Sheets");
    QAxObject *pSheet;
    num = pSheets->dynamicCall("Count").toInt();
    if(fe == false){                        //如果是新建的表格，则删除表单仅保留1张
        for(i=0;i<num;i++){
            pSheet = pSheets->querySubObject("Item(int)",num-i);
            pSheet->dynamicCall("Delete");
        }
        pSheet = pSheets->querySubObject("Item(int)",1);
        pSheet->setProperty("Name",QString(BREIF_INFO_FIGURE_NAME));
    }

    if(VOLTAG_SAVE_MODE == 0){
        save_voltag_use_id(pSheets);
    }else{
        save_voltag_as_one(pSheets);
    }
    save_breif_info(pSheets);

    if(fe == false){
        pWorkBook->dynamicCall("SaveAs (const QString&)",QDir::toNativeSeparators(fnp));
    }else{
        pWorkBook->dynamicCall("Save");
        QFile file(fnp);
        file.open(QIODevice::ReadOnly);
        if(file.size() > (voltag_store_file_len_max*MBYTE_TO_BYTE))
            mechine_first_launch_time = QDateTime::currentDateTime();
        file.close();
    }
    pWorkBook->dynamicCall("Close()");
    excel.dynamicCall("Quit(void)");
    OleUninitialize();
    bat_unit.clear();
    bms_breif_info.clear();
}
/**    @breif: 所有电压保存为一张表格
 *      @param:
 *          pSheets:    待输入的表单集
*/
void BatteryStore::save_voltag_as_one(QAxObject *pSheets)
{
    int num,len;
    int i,j;
    QString bid,sbid = QString(SAVE_AS_ONE_DATA_NAME);
    QAxObject *pSheet;
    QAxObject *pUsePage;
    QList<QVariant> rowData;
    QString msg;

    num = pSheets->dynamicCall("Count").toInt();
    for(i=1;i<=num;i++){             //查找表格中是否有以 SAVE_AS_ONE_DATA_NAME 为表单名的表单
        pSheet = pSheets->querySubObject("Item(int)",i);
        bid = pSheet->property("Name").toString();
        if(QString::compare(sbid,bid) == 0)
            break;
    }
    if(i > num){               //如果没有找到表单，则添加1张表单,添加到最后之前
        pSheet = pSheets->querySubObject("Item(int)",num);
        QAxObject *pNSheet = pSheets->querySubObject("Add(QVariant)",pSheet->asVariant());
        pNSheet->setProperty("Name",sbid);
        for(i=0;i<BMS_BAT_COUNT_IN_EXCEL;i++){
            rowData.push_back(QVariant(QString("BAT %1").arg(i+1)));
        }
        QAxObject *pCells = pNSheet->querySubObject("Cells(int,int)",1,2);
        QAxObject *pCelle = pNSheet->querySubObject("Cells(int,int)",1,1+BMS_BAT_COUNT_IN_EXCEL);
        QAxObject *pRange = pNSheet->querySubObject("Range(QVariant,QVariant)",pCells->asVariant(),pCelle->asVariant());
        QAxObject *pInterior = pRange->querySubObject("Interior");
        pInterior->setProperty("Color",QColor(255,255,230));
        pRange->dynamicCall("SetValue (const QVariant&)",QVariant(rowData));
        pRange->setProperty("HorizontalAlignment",-4108);
    }

    if(i > num){
        pSheet = pSheets->querySubObject("Item(int)",num);
    }else{
        pSheet = pSheets->querySubObject("item(int)",i);
    }
    for(auto iter=bat_unit.begin();iter!=bat_unit.end();iter++){
        rowData.clear();
        if(iter->chinnel_start == 0){
            rowData.push_back(QVariant(iter->data_time.toString("hh:mm:ss")));
        }
        pUsePage = pSheet->querySubObject("UsedRange");
        QAxObject *col = pUsePage->querySubObject("Columns(int)",iter->chinnel_start+3);
        QAxObject *srow = col->querySubObject("Rows");
        num = srow->property("Count").toInt();
        len = 0;
        for(i=0;i<iter->modu_num;i++)
            len += iter->per_chinnal[i];
        QAxObject *pCells;
        QAxObject *pCelle;
        if(iter->chinnel_start == 0){
            pCells = pSheet->querySubObject("Cells(int,int)",num+1,iter->chinnel_start+1);
            pCelle = pSheet->querySubObject("Cells(int,int)",num+1,iter->chinnel_start+len+1);
        }else{
            pCells = pSheet->querySubObject("Cells(int,int)",num,iter->chinnel_start+2);
            pCelle = pSheet->querySubObject("Cells(int,int)",num,iter->chinnel_start+len+1);
        }

        QAxObject *pRange = pSheet->querySubObject("Range(QVariant,QVariant)",pCells->asVariant(),pCelle->asVariant());
        if(iter->modu_num>0){
            for(j=0;j<iter->per_chinnal[0];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j])));
        }
        if(iter->modu_num>1){
            for(j=0;j<iter->per_chinnal[1];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j+EACH_MODULE_CHINNEL])));
        }
        if(iter->modu_num>2){
            for(j=0;j<iter->per_chinnal[2];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j+EACH_MODULE_CHINNEL*2])));
        }
        if(iter->modu_num>3){
            for(j=0;j<iter->per_chinnal[3];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j+EACH_MODULE_CHINNEL*3])));
        }
        pRange->dynamicCall("SetValue (const QVariant&)",QVariant(rowData));
        pRange->setProperty("HorizontalAlignment",-4108);
    }
}
/** @brief: 保存概述信息
 *  @param:
 *      pSheets:    待修改的表单集
*/
void BatteryStore::save_breif_info(QAxObject *pSheets)
{
    int rowCount,num;
    int i,j=0;
    QString msg;
    QAxObject *pSheet;
    QList<QVariant> lData;
    num = pSheets->dynamicCall("Count").toInt();
    for(i=1;i<=num;i++){
        pSheet = pSheets->querySubObject("Item(int)",i);
        msg = pSheet->property("Name").toString();
        if(QString::compare(msg,QString(BREIF_INFO_FIGURE_NAME)) == 0){
            QAxObject *pRow = pSheet->querySubObject("UsedRange")->querySubObject("Rows");
            QAxObject *pCells;
            QAxObject *pCelle;
            QAxObject *pRange;
            rowCount = pRow->property("Count").toInt();
            if(rowCount < 2){
                QVector<QString> value = bms_breif_info.begin()->tital;
                for(auto siter = value.begin();siter != value.end();siter++){
                    lData.push_back(QVariant(*siter));
                }
                pCells = pSheet->querySubObject("Cells(int,int)",1,2);
                pCelle = pSheet->querySubObject("Cells(int,int)",1,value.size()+1);
                pRange = pSheet->querySubObject("Range(const QVariant&,const QVariant&)",pCells->asVariant(),pCelle->asVariant());
                QAxObject *pInterior = pRange->querySubObject("Interior");
                pInterior->setProperty("Color",QColor(255,255,230));
                pRange->dynamicCall("SetValue (const QVariant&)",QVariant(lData));
                pRange->setProperty("HorizontalAlignment",-4108);
                pRange->setProperty("ColumnWidth",18);
            }
            rowCount = pRow->property("Count").toInt();
            for(auto iter = bms_breif_info.begin();iter != bms_breif_info.end();iter++){
                lData.clear();
                lData.push_back((iter->time.toString("hh:mm:ss")));                
                for(auto siter=iter->value.begin();siter!=iter->value.end();siter++){
                    lData.push_back(QVariant(*siter));
                }                
                pCells = pSheet->querySubObject("Cells(int,int)",rowCount+j+1,1);
                pCelle = pSheet->querySubObject("Cells(int,int)",rowCount+j+1,iter->value.size()+1);
                pRange = pSheet->querySubObject("Range(const QVariant&,const QVariant&)",pCells->asVariant(),pCelle->asVariant());
                pRange->dynamicCall("SetValue (const QVariant&)",QVariant(lData));
                pRange->setProperty("HorizontalAlignment",-4108);
                j++;
            }
          break;
        }
    }
}
/** @brief: 按从板ID分页保存电压值
 *  @param:
 *      pSheets:    待修改的表单集
*/
void BatteryStore::save_voltag_use_id(QAxObject *pSheets)
{
    QVector<int> id_table;
    QVector<struct sub_each_board>::iterator iter;
    int num,colct;
    QString bid,sbid;
    int i,j,k=0;
    QAxObject *pSheet;
    QString msg;

    for(iter = bat_unit.begin();iter != bat_unit.end();iter++){
        num = pSheets->dynamicCall("Count").toInt();
        sbid = QString("BID%1").arg(iter->bid);
        for(i=1;i<=num;i++){             //查找表格中是否有以 从板ID为表单名的表单
            pSheet = pSheets->querySubObject("Item(int)",i);
            bid = pSheet->property("Name").toString();
            if(QString::compare(sbid,bid) == 0)
                break;
        }
        if(i > num){               //如果没有找到从板ID的表单，则添加1张表单,添加到最后之前
            pSheet = pSheets->querySubObject("Item(int)",num);
            QAxObject *pNSheet = pSheets->querySubObject("Add(QVariant)",pSheet->asVariant());
            pNSheet->setProperty("Name",sbid);
        }
        for(auto iditer = id_table.begin();iditer != id_table.end();iditer++){      //判断是否是首次写入
            if(*iditer == iter->bid){
                k = -1;
                break;
            }
        }

        //---------开始记录数据-----------
        if(i > num){
            pSheet = pSheets->querySubObject("Item(int)",num);
        }else{
            pSheet = pSheets->querySubObject("item(int)",i);
        }
        QAxObject *usedPage;
        QAxObject *Rows;
        int start_row;
        QAxObject *pCells;
        QAxObject *pCelle;
        QAxObject *pRange;
        QList<QVariant> rowData;
        if(k != -1){         //每次记录电压时先写一次电池标号
            usedPage = pSheet->querySubObject("UsedRange");
            Rows = usedPage->querySubObject("Rows");
            start_row = usedPage->property("Row").toInt();
            num = start_row + Rows->property("Count").toInt();      //获取位置
            colct = 0;
            for(j=0;j<iter->modu_num;j++)
                colct += iter->per_chinnal[j];
            for(j=0;j<colct;j++){
                msg = QString("BAT %1").arg(iter->chinnel_start + j + 1);
                rowData.push_back(QVariant(msg));
            }
            pCells = pSheet->querySubObject("Cells(int,int)",num,2);
            pCelle = pSheet->querySubObject("Cells(int,int)",num,colct+1);
            pRange = pSheet->querySubObject("Range(const QVariant&,const QVariant&)",pCells->asVariant(),pCelle->asVariant());
            QAxObject *pInterior = pRange->querySubObject("Interior");
            pInterior->setProperty("Color",QColor(255,255,230));
            pRange->dynamicCall("SetValue (const QVariant&)",QVariant(rowData));
            pRange->setProperty("HorizontalAlignment",-4108);
            id_table.push_back(iter->bid);
        }
        //写数据
        usedPage = pSheet->querySubObject("UsedRange");
        Rows = usedPage->querySubObject("Rows");
        start_row = usedPage->property("Row").toInt();
        num = start_row + Rows->property("Count").toInt();
        rowData.clear();

        rowData.push_back(QVariant(iter->data_time.toString("hh:mm:ss")));

        if(iter->modu_num>0){
            for(j=0;j<iter->per_chinnal[0];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j])));
            colct = iter->per_chinnal[0];
        }
        if(iter->modu_num>1){
            for(j=0;j<iter->per_chinnal[1];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j+EACH_MODULE_CHINNEL])));
            colct += iter->per_chinnal[1];
        }
        if(iter->modu_num>2){
            for(j=0;j<iter->per_chinnal[2];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j+EACH_MODULE_CHINNEL*2])));
            colct += iter->per_chinnal[2];
        }
        if(iter->modu_num>3){
            for(j=0;j<iter->per_chinnal[3];j++)
                rowData.push_back(QVariant(QString("%1").arg(iter->each_volt[j+EACH_MODULE_CHINNEL*3])));
            colct += iter->per_chinnal[3];
        }
        pCells = pSheet->querySubObject("Cells(int,int)",num,1);
        pCelle = pSheet->querySubObject("Cells(int,int)",num,colct+1);
        pRange = pSheet->querySubObject("Range(const QVariant&,const QVariant&)",pCells->asVariant(),pCelle->asVariant());
        pRange->dynamicCall("SetValue (const QVariant&)",QVariant(rowData));
        pRange->setProperty("HorizontalAlignment",-4108);
        rowData.clear();
        k=0;
    }
    id_table.clear();
}
/** @brief: 修改EEPROM表单后重新分析数据
*/
void TkbmWidget::on_tb_eep_file_itemChanged(QTableWidgetItem *item)
{
    unsigned char temp;
    QString msg;
    QTableWidgetItem *itemo;
    if(item->column() == EEP_TB_FILE && eeprom_set_launch_setting == true){
        itemo = ui->tb_eep_file->item(epprom_set_ac_pos,EEP_TB_FILE);
        temp = eeprom_after_modify();
        msg = QString("H: %1").arg(temp,2,16,QLatin1Char('0')).toUpper();
        itemo->setText(msg);
    }
}
/** @brief: 响应数据修改
*/
void TkbmWidget::on_tb_brief_itemChanged(QTableWidgetItem *item)
{
    bool ok;
    int val;
    if(item->column() == 0 && item->row() == 0){
        val = item->text().toInt(&ok);
        if(ok == true)
            VOLTAG_SAVE_TIME = val / 50;
    }
}
