#ifndef FILEOPERAOR_H
#define FILEOPERAOR_H
#include "Common_def.h"
#include "util/UtilExport.h"
#include <string>
#include <string.h>


BEGINE_NAMESPACE(mmrUtil)

//创建文件夹如果不存在
COMMON_FUN_API bool creatDirIfNotExist(const char* szPath);







END_NAMESPACE(mmrUtil)
#endif
