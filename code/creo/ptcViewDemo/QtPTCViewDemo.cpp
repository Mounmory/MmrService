#include "Commons.h"

static QApplication* g_pApp = nullptr;

static uiCmdAccessState AccessDefault(uiCmdAccessMode access_mode)
{
	return(ACCESS_AVAILABLE);
}

#include <fstream>
//非模态对话框
QDialog* pDlg = nullptr;
void Test1()
{
	if (nullptr != pDlg)
	{
		delete pDlg;
		pDlg = nullptr;
	}

	QDialog* pDlg = new QDialog;
	QHBoxLayout* pLayout = new QHBoxLayout;
	IPTCViewWidget* pView = CommonFunctios::createPTCViewWidget();
	if (pView)
	{
		pView->setSourceUrl("C:/creoModel/d.prt");
		pLayout->addWidget(pView);
		pDlg->setLayout(pLayout);
		pDlg->setWindowTitle(u8"非模态对话框测试");
		pDlg->setMinimumHeight(300);
		pDlg->setMinimumWidth(500);
		pDlg->show();
	}

}

//模态对话框
void Test2()
{
	QDialog dlg;
	QHBoxLayout* pLayout = new QHBoxLayout;
	IPTCViewWidget* pView = CommonFunctios::createPTCViewWidget();
	if (pView)
	{
		pView->setSourceUrl("C:/creoModel/d.prt");
		pLayout->addWidget(pView);
		dlg.setLayout(pLayout);
		dlg.setWindowTitle(u8"模态对话框测试");
		dlg.setMinimumHeight(300);
		dlg.setMinimumWidth(500);
		dlg.setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
		//dlg.setWindowModality(Qt::ApplicationModal);
		dlg.exec();
	}
}

extern "C" int user_initialize()
{
	if (nullptr == g_pApp)
	{
		static int argc = 1;
		static char* szPath = new char[MAX_STR_LEN];
		std::string strPath = CommonFunctios::getAppFullPath();
		strcpy_s(szPath, MAX_STR_LEN, strPath.c_str());

		g_pApp = new QApplication(argc, &szPath);
		//static QApplication app(argc, &szPath);

		//避免主窗口隐藏时，子窗口退出造成程序退出
		QApplication::setQuitOnLastWindowClosed(false);
	}

	ProError status;
	ProFileName  MsgFile = L"IconMessage.txt";
	uiCmdCmdId PushButton1_cmd_id, PushButton2_cmd_id;
	status = ProMenubarMenuAdd("MainMenu", "Function", "Help", PRO_B_TRUE, MsgFile);

	ProCmdActionAdd("PushButton1_Act", (uiCmdCmdActFn)Test1, 12, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &PushButton1_cmd_id);
	ProMenubarmenuPushbuttonAdd("MainMenu", "PushButton", "FirstButton", "this button will show a message", NULL, PRO_B_TRUE, PushButton1_cmd_id, MsgFile);

	ProCmdActionAdd("PushButton2_Act", (uiCmdCmdActFn)Test2, uiCmdPrioDefault, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &PushButton2_cmd_id);
	ProMenubarmenuPushbuttonAdd("MainMenu", "PushButton2", "SecondButton", "this button will show a message", NULL, PRO_B_TRUE, PushButton2_cmd_id, MsgFile);
	return status;
}
extern  "C" void user_terminate()
{
	if (nullptr != g_pApp)
	{
		g_pApp->closeAllWindows();
		g_pApp->quitOnLastWindowClosed();
		g_pApp->exit();
		g_pApp->deleteLater();
		delete g_pApp;
		g_pApp = nullptr;
	}
}