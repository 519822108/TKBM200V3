#ifndef DATA_ORGANIZE_H
#define DATA_ORGANIZE_H
#include <config.h>
#include <QString>

/*  s_ :源文件和显示区共有的成员
 *  o_ :源文件独有的成员
 *  d_ :显示区独有的成员
 */
struct excel_param_organize{
    int             s_num;                  //数据的编号
    QString         s_nane;                 //参数名
    QString         s_note;                 //参数注释
    int             o_byte_len;             //参数占有的字节长度
    int             o_dis_format;           //参数的显示格式, 16进制或10进制
    int             d_cur_val;             //参数当前值
    int             d_obj_val;             //参数目标值
    int             d_store_pos;            //参数存储位置
};

struct tb_breif_organize{
    QString         version;
};

#endif // DATA_ORGANIZE_H
