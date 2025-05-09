/**
 * @file FileOperator.h
 * @brief 文件操作相关接口
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef FILEOPERAOR_H
#define FILEOPERAOR_H
#include <common/include/Common_def.h>
#include <common/include/util/UtilExport.h>
#include <string>
#include <string.h>


BEGINE_NAMESPACE(mmrUtil)

//创建文件夹如果不存在
COMMON_FUN_API bool creatDirIfNotExist(const char* szPath);







END_NAMESPACE(mmrUtil)
#endif
