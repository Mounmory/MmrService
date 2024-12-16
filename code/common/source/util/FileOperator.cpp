#include "util/FileOperator.h"

#ifdef OS_MMR_WIN
#include <corecrt_io.h>	//_access头文件
#include <windows.h>
#elif defined OS_MMR_LINUX
#include <sys/stat.h>  //新建文件夹头文件
#include <sys/types.h> //新建文件夹头文件
#include <unistd.h>
#endif

using namespace mmrUtil;

COMMON_FUN_API bool creatDirIfNotExist(const char* szPath)
{
#ifdef OS_MMR_WIN
	if (_access(szPath, 0) == -1)//如果文件夹不存在，则创建
	{
		if (CreateDirectory(szPath, 0) == false)
		{
			return false;
		}
	}
#elif defined OS_MMR_LINUX
	if (access(szPath, 0) != F_OK) //检查文件夹是否存在，不存在则创建
	{
		mkdir(szPath, S_IRWXO); //所有人都有权限读写

		if (access(szPath, 0) != F_OK)
		{
			return false;
		}
	}
#endif
	return true;
}
