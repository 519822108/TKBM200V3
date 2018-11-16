#include "ctlcan.h"
#include <iostream>
#include <string.h>
#define COUT    std::cout << "[" << __FILE__ << ":" << __LINE__ << "] "
#define ENDL    std::endl

int brArry[10] = {0x060003,0x060004,0x060007,0x1C0008,0x1C0011,0x160023,0x1C002C,0x1600B3,0x1C00E0,0x1C01C1};
//char *strArry[10] = {"1000Kbps","800Kbps","500Kbps","250Kbps","125Kbps","100Kbps","50Kbps","20Kbps","10Kbps","5Kbps"};

CtlCan::CtlCan(int type,int index,int port,int udpport)
{
    devType = type;                     //device type
    devIndex = index;                   //device index,use for multex can devices
    canIndex = port;                    //device port number,use for a device have multex ports
    udpPort = udpport;                  //can udp device using
    rec_buff_length = 0;
    state = CtlCan::off;
}

int CtlCan::dev_open()
{
   return !VCI_OpenDevice(this->devType,this->devIndex,udpPort);
}

int CtlCan::dev_close()
{
    return !VCI_CloseDevice(this->devType,this->devIndex);
}

int CtlCan::dev_init(recMode mode)
{
    VCI_INIT_CONFIG canConfig;
    canConfig.Mode = mode;
    return !VCI_InitCAN(devType,devIndex,canIndex,&canConfig);
}

int CtlCan::dev_start()
{
    int err=0;
    err = VCI_StartCAN(devType,devIndex,canIndex);
    if(err) state = CtlCan::on;
    return !err;
}
/***    @breif: set CAN bus baudrate
 *      @param:
 *          rate:   the value of baudrate
 *      @return:    achieve return 0,otherwise return 1
*/
int CtlCan::set_baudrate(baudRate rate)
{
    if((int)rate > 9)   return 1;
    return !VCI_SetReference(devType,devIndex,canIndex,(int)CtlCan::baudrate,(void *)&brArry[(int)rate]);
}
/***    @breif: config CAN bus filter
 *      @param:
 *          type:   external frame or standered frame
 *          startId:    start id of filter
 *          endId:      end id of filter
 *      @return:    achieve return 0,otherwise return 1
*/
int CtlCan::set_filter(isExtFram type, unsigned long startId, unsigned long endId)
{
    VCI_FILTER_RECORD filter;
    int errInfo;
    filter.ExtFrame = (int)type;
    filter.Start = startId;
    filter.End = endId;
    errInfo = VCI_SetReference(devType,devIndex,canIndex,(int)CtlCan::setfilter,&filter);
    errInfo &= VCI_SetReference(devType,devIndex,canIndex,(int)CtlCan::launchfilter,NULL);
    return !errInfo;
}
/***    @breif: add one frame to filter
 *      @param:
 *          type:   It's a external frame or standered frame,default is external faram
 *      @return:    achiece return 0,else return 1
*/
int CtlCan::add_one_filter( unsigned long id,isExtFram type)
{
    return set_filter(type,id,id);
}
/**
 * @brief :     clear the filter
 * @return :    achieve return 0,otherwise return 1
 */
int CtlCan::clear_filter()
{
    return !VCI_SetReference(devType,devIndex,canIndex,(int)CtlCan::clearfilter,NULL);
}
/**
 * @brief : set outtime retransf delay
 * @param delay_ms :    time value
 * @return :    achieve return 0,otherwise return 1
 */
int CtlCan::set_retry_delay(int delay_ms)
{
    return !VCI_SetReference(devType,devIndex,canIndex,(int)CtlCan::delayms,(void *)&delay_ms);
}
/**
 *    @breif: open a can device
 *    @param
 *      rate:   configuration of baudrate
 *      mode:   nomal run or listen mode
 *    @return:  achieve return 0,otherwise return 1
*/
int CtlCan::open(baudRate rate, recMode mode)
{
    if(this->dev_open()){
        COUT << "error to open device"<<ENDL;
        return 1;
    }
    if(this->set_baudrate(rate)){
        COUT << "error to set reference"<<ENDL;
        this->dev_close();
        return 2;
    }
    if(this->dev_init(mode)){
        COUT << "error to init device"<<ENDL;
        this->dev_close();
        return 3;
    }

    if(this->dev_start()){
        COUT << "error to start device"<<ENDL;
        this->dev_close();
        return 4;
    }
    state = CtlCan::on;
    store_baudRate = rate;
    return 0;
}

int CtlCan::reopen()
{
    int err;
    err = open(store_baudRate);
    if(!err)    state = CtlCan::on;
    return err;
}

/*    @breif: close CAN device
*/
int CtlCan::close()
{
    state = CtlCan::off;
    return dev_close();
}
/*  @breif: get state of can device
*/
int CtlCan::get_state()
{
    return (int)this->state;
}
/***    @breif: reset CAN device ,cooperat to dev_start
*/
int CtlCan::reset()
{
    state = CtlCan::off;
    return !VCI_ResetCAN(devType,devIndex,canIndex);
}

int CtlCan::start()
{
    int err;
 //   err = reset();
    err = dev_start();
    return err;
}

int CtlCan::read_board_info(PVCI_BOARD_INFO info)
{
    return !VCI_ReadBoardInfo(devType,devIndex,info);
}

int CtlCan::read_error_info(PVCI_ERR_INFO info)
{
    return !VCI_ReadErrInfo(devType,devIndex,canIndex,info);
}

int CtlCan::read_can_state(PVCI_CAN_STATUS status)
{
    return !VCI_ReadCANStatus(devType,devIndex,canIndex,status);
}
/***    @breif: get CAN device data buff length which some not read
 *      @return:    the length for date not read
*/
int CtlCan::get_recive_num()
{
    return VCI_GetReceiveNum(devType,devIndex,canIndex);
}

/***    @brief: read can device data
 *      @param:
 *         pData: data buffer pointer
 *         read_len:    data length would be read
 *         outtime:     while data buffer not have data,the function wait time
 *      @return:    the read data length actually
*/
int CtlCan::can_rec_read(PVCI_CAN_OBJ pData, int read_len,int outtime)
{
    return VCI_Receive(devType,devIndex,canIndex,pData,read_len,outtime);
}

/***    @breif: update the can data buff
 *      @return:    achieve return 0,otherwise return error code
 *      @attention: this function must be call periodic,lower call period may lead the buffer to be overlapped
*/
int CtlCan::rec_data_update()
{
    int rec_len=0;
    rec_len = get_recive_num();
    if((rec_buff_length+rec_len) > REC_ARRY_LEN)
        rec_buff_length = 0;
    if(rec_len > REC_ARRY_LEN){
        clear_buff();
        return 1;
    }
    rec_len = VCI_Receive(devType,devIndex,canIndex,&rec_buff[rec_buff_length],rec_len,-1);
    if(rec_len == 0xFFFFFFFF)    return 2;
    rec_buff_length += rec_len;
    return 0;
}
/***    @breif: read buffer data
 *      @param:
 *          pData:  read data buffer pointer
 *          read_len:   read data length
 *      @return:   read data length actually
*/
int CtlCan::read_buff_data(PVCI_CAN_OBJ *pData, int read_len)
{
    int data_len;
    read_len = (read_len == 0)?(rec_buff_length):(read_len);
    data_len = (read_len > rec_buff_length) ? (rec_buff_length) : (read_len);
    memcpy(pData,&rec_buff[rec_buff_length-data_len],sizeof(VCI_CAN_OBJ)*data_len);
    rec_buff_length -= data_len;
    return data_len;
}
/***    @breif: clear CAN device buffer
*/
int CtlCan::clear_buff()
{
    return !VCI_ClearBuffer(devType,devIndex,canIndex);
}
/***    @breif: send mesg to CAN bus
 *      @param:
 *          sned_num:   frame number to be send
 *          obj:        date buffer
 *      @return:    frame number of achieve to be send
*/
int CtlCan::send_msg(int sned_num, PVCI_CAN_OBJ obj)
{
    return VCI_Transmit(devType,devIndex,canIndex,obj,sned_num);
}
/***    @breif: only send one msg to CAN bus
 *      @param:
 *          ID: CAN ID
 *          len:    data len,maximom value at 8
 *          data:   char-array store the data to be send
 *          frame:  whether the external frame or standered frame.default is external frame
 *          romete: whether is romete frame or is data frame,default is data frame
 *          type:   send mode,default is nomal send
 *      @return:    achieve return 0,otherwise return 1
*/
int CtlCan::send_one_msg(unsigned int ID, char len,unsigned char *data, isExtFram frame, isRomete romete, sendType type)
{
    VCI_CAN_OBJ obj;
    if(len > 8 || len < 0)
        return 1;
    obj.ID = ID;
    obj.SendType = (unsigned char)type;
    obj.RemoteFlag = (unsigned char)romete;
    obj.ExternFlag = (unsigned char)frame;
    obj.DataLen = len;
    memcpy(obj.Data,data,len);
    return !VCI_Transmit(devType,devIndex,canIndex,&obj,1);
}
