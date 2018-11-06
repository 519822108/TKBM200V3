#ifndef CONFIG_H
#define CONFIG_H

#define FILE_PATH_CONFIG    "/config/BMS_config.xlsx"
#define FILE_CONFIG_COL_START   2               //配置文件数据区列起始坐标
#define FILE_CONFIG_COL_END     7               //配置文件数据区列终止坐标
#define FILE_CONFIG_ROW_START   3               //配置文件数据区行起始坐标
#define FILE_CONFIG_ROW_END     324             //配置文件数据区行终止坐标,此处不定

#define CONFIG_COL_PARAM_NUM    2               //参数编号的列号
#define CONFIG_COL_PARAM_NAME   3               //参数数据名的列号
#define CONFIG_COL_PARAM_LEN    4               //参数所占字节数的列号
#define CONFIG_COL_PARAM_FORMAT 5               //参数显示格式的列号
#define CONFIG_COL_PARAM_NOTE   8               //参数注释的列号

#define FILE_CONFIG_LIST_MAX_LEN    512

#endif // CONFIG_H
