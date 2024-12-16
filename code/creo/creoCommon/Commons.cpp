#include "Commons.h"


IPTCViewWidget* CommonFunctios::createPTCViewWidget(QWidget* parent)
{
	static getPTCViewFuc func = nullptr;

	if (nullptr == func)
	{
		std::string strLibPath = getDllPath() + "ptcViewWidget.dll";
		static QLibrary myLib(strLibPath.c_str());
		if (myLib.load())
		{
			func = (getPTCViewFuc)myLib.resolve("getPTCView");
		}
		else 
		{
			QMessageBox::warning(nullptr, u8"¥ÌŒÛ", u8"º”‘ÿ PTCViewWidget.dll failed!", QMessageBox::Ok);
		}
	}

	if (nullptr != func)
		return func(parent);
	else
		return nullptr;
}

std::string CommonFunctios::getDllPath()
{
	char path[MAX_STR_LEN] = { 0 };
	HMODULE hModuleInstance = _AtlBaseModule.GetModuleInstance();
	uint32_t nlen = GetModuleFileName(hModuleInstance, path, MAX_STR_LEN);
	uint32_t ulLastIndex = 0;
	for (uint32_t index = 0 ; index < nlen ; ++index)
	{
		if ('\\' == path[index])
		{
			path[index] = '/';
			ulLastIndex = index;
		}
	}
	path[ulLastIndex + 1] = 0;
	return path;
}

std::string CommonFunctios::getAppFullPath()
{
	char path[MAX_STR_LEN] = { 0 };
	uint32_t nlen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	uint32_t ulLastIndex = 0;
	for (uint32_t index = 0; index < nlen; ++index)
	{
		if ('\\' == path[index])
		{
			path[index] = '/';
			ulLastIndex = index;
		}
	}
	return path;
}

std::string CommonFunctios::getAppPath()
{
	char path[MAX_STR_LEN] = { 0 };
	uint32_t nlen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	uint32_t ulLastIndex = 0;
	for (uint32_t index = 0; index < nlen; ++index)
	{
		if ('\\' == path[index])
		{
			path[index] = '/';
			ulLastIndex = index;
		}
	}
	path[ulLastIndex + 1] = 0;
	return path;
}


