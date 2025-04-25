#ifndef MMR_NET_EXPORT_H
#define MMR_NET_EXPORT_H

#include "common/include/Common_def.h"

#if defined(OS_MMR_WIN)
		//#pragma execution_character_set("utf-8")
	#ifdef MMR_NET_EXPORT
		#define MMR_NET_CLASS_API __declspec(dllexport)
		#define MMR_NET_FUN_API extern "C" __declspec(dllexport)
	#else
		#define MMR_NET_CLASS_API  __declspec(dllimport)
		#define MMR_NET_FUN_API extern "C" __declspec(dllimport)
	#endif
#else
	#define MMR_NET_CLASS_API 
	#define MMR_NET_FUN_API extern "C" 
#endif

#endif

