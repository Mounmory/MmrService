/**
 * @file UtilFunc.h
 * @brief util模块公共函数
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef UTILFUNC_H
#define UTILFUNC_H

#include <common/include/util/UtilExport.h>
#include <string>
#include <string.h>


BEGINE_NAMESPACE(mmrUtil)

//UTF-8转本地字符
COMMON_FUN_API bool utf8ToLocalString(const std::string& strIn,std::string& strOut);

//本地字符转UTF-8
COMMON_FUN_API bool localStringToUtf8(const std::string& strIn, std::string& strOut);

////将字节序转换为编码字符
//COMMON_FUN_API bool stringToCode(const std::string strIn, std::string& strOut);

////将编码字符转换为字节序
//COMMON_FUN_API bool codeToString(const std::string strIn, std::string& strOut);

//获取计算机唯一ID
COMMON_FUN_API std::string getComputerID();

//路径带最后一个"/"
COMMON_FUN_API bool getAppPathAndName(std::string& filePath, std::string& exeName);

COMMON_FUN_API std::string getAppPath();

COMMON_FUN_API const char* getFileName(const char* szFullPath);

COMMON_FUN_API std::string generateGUID();

COMMON_FUN_API std::string getComplieTime();

COMMON_FUN_API std::string getComplierInfo();

COMMON_FUN_API std::string timeInt64ToString(int64_t llTime);

END_NAMESPACE(mmrUtil)
#endif
