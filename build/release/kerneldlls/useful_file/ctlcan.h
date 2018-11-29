#ifndef CTLCAN_H
#define CTLCAN_H

#include "ControlCAN.h"
#pragma comment(lib,"ControlCAN.lib")

#define REC_ARRY_LEN    1024

class CtlCan
{
public:
    CtlCan(int type=VCI_USBCAN_2E_U,int index=0,int port=0,int udpport=0);

    int rec_buff_length;
    VCI_CAN_OBJ rec_buff[REC_ARRY_LEN];
    unsigned char send_data[8];

    enum baudRate{
        Kbps1000=0,Kbps800=1,Kbps500=2,Kbps250=3,Kbps125=4,Kbps100=5,
        Kbps50=6,Kbps20=7,Kbps10=8,Kbps5=9
    };

    enum refType{
        baudrate=0,
        setfilter=1,
        launchfilter=2,
        clearfilter=3,
        delayms=4
    };

    enum recMode{
        nomal=0,
        listenOnly=1
    };

    enum isExtFram{
        std_frame=0,
        ext_frame=1
    };

    enum sendType{
        send_nomal=0,
        send_once=1,
        send_self=2,
        send_self_once=3
    };

    enum isRomete{
        frame_data=0,
        frame_romete=1
    };

    enum port_state{
        off=0,
        on=1
    }state;

 //   static enum recMode useMode;
 //   int init()
    int open(baudRate rate,recMode mode=CtlCan::nomal);        //open device
    int reopen();                                               //use old param open device
    int close();                                                //close device
    int reset();                                                //reset deivce
    int start();
    int get_state();
    int clear_buff();                                           //clear data buff

    int get_recive_num();                                      //get recive buffer length
    int can_rec_read(PVCI_CAN_OBJ pData,int read_len,int outtime=-1);       //read CAN data

    int rec_data_update();                                     //recive CAN data and update to data buff
    int read_buff_data(PVCI_CAN_OBJ *pData,int read_len=0);       //read buffer data

    int send_msg(int len,PVCI_CAN_OBJ obj);                    //CAN data send
    //Only send one frame data to can bus
    int send_one_msg(unsigned int ID,char len,unsigned char *data,isExtFram frame=ext_frame,isRomete romete=frame_data,sendType type=send_nomal);

    int read_board_info(PVCI_BOARD_INFO info);
    int read_error_info(PVCI_ERR_INFO info);
    int read_can_state(PVCI_CAN_STATUS status);

    int set_filter(isExtFram type,unsigned long startId,unsigned long endId);
    int add_one_filter(unsigned long id,isExtFram type=ext_frame);
    int clear_filter();
    int set_retry_delay(int delay_ms);

private:
    int devType;
    int devIndex;
    int canIndex;
    int udpPort;
    baudRate store_baudRate;

    int dev_open();
    int dev_close();
    int dev_init(recMode mode=CtlCan::nomal);
    int dev_start();
    int set_baudrate(baudRate rate);
};

#endif // CTLCAN_H
