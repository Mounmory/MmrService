#ifndef UTILCOMMON_H
#define UTILCOMMON_H

#include "common/include/Common_def.h"

#if defined(OS_MMR_WIN)
		//#pragma execution_character_set("utf-8")
	#ifdef MMR_COMMON_EXPORT
		#define COMMON_CLASS_API __declspec(dllexport)
		#define COMMON_FUN_API extern "C" __declspec(dllexport)
	#else
		#define COMMON_CLASS_API  __declspec(dllimport)
		#define COMMON_FUN_API extern "C" __declspec(dllimport)
	#endif
#else
	#define COMMON_CLASS_API 
	#define COMMON_FUN_API extern "C" 
#endif

#endif

