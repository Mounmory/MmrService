#ifndef COMMONS_H
#define COMMONS_H

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QLibrary>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QGridLayout>

//PTCView
#include "interface/IPTCViewWidget.h"

//C++标准库头文件
#include <io.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>


#define MAX_STRING_LEN 1024

//*************添加Pro/Tookit头文件
#ifdef PRO_USE_VAR_ARGS
#include <ProToolkit.h>
#include <ProUtil.h>
#include <ProArray.h>
#include <ProDimension.h>
#include <ProWindows.h>
#include <ProSolid.h>
#include <ProParameter.h>
#include <ProParamval.h>
#include <ProMessage.h>
#include <ProMenu.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProModelitem.h>
#include <ProDrawing.h>
#include <ProFeature.h>
#include <ProFeatType.h>
#include <ProDrawing.h>
#include <ProNotify.h>
#include <ProRefInfo.h>
#include <ProRelSet.h>
#include <ProSelection.h>
#include <ProUICmd.h>
#include <ProNotify.h>
#include <ProPopupmenu.h>
#include <ProSelbuffer.h>
#include <ProAsmcomp.h>
#include <ProAsmcomppath.h>
#include <afxdisp.h>
#include <afxcontrolbars.h>
//////清除工作目录重复文件专有
#include <ProUICmd.h>
#include <ProUIMessage.h>
#include <ProToolkitDll.h>
////一键着色专有
#include <ProSurface.h>
#include <afxdisp.h>
#endif


#ifndef MAX_STR_LEN
#define MAX_STR_LEN 1024
#endif
namespace CommonFunctios 
{
	IPTCViewWidget* createPTCViewWidget(QWidget* parent = nullptr);

	//获取动态库路径,带文件分隔符“\”
	std::string getDllPath();

	std::string getAppFullPath();

	std::string getAppPath();
}

#endif // !COMMONS_H
