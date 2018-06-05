
// showbmp.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "showbmp.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CshowbmpApp

BEGIN_MESSAGE_MAP(CshowbmpApp, CWinApp)
END_MESSAGE_MAP()

CString GetRegisterValue(HKEY hkey, LPCTSTR Rgspath, LPCTSTR ValueName, DWORD type);

// CshowbmpApp 构造

CshowbmpApp::CshowbmpApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("showbmp.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CshowbmpApp 对象

CshowbmpApp		theApp;

CList<FileInfoDetail, FileInfoDetail &> * g_currentDirList = NULL;
int				g_listCnt;
int				g_currentIndex;
int				g_viewWidth		= 1024;
int				g_viewHeight	= 768;
CString			g_strCurrentFileName;

BOOL CshowbmpApp::InitInstance()
{
	CString sCmdline = AfxGetApp()->m_lpCmdLine;
	if (sCmdline.GetLength() != 0) 
	{
		sCmdline.Replace(L"\"", L"");
		//AfxMessageBox(sCmdline);
		g_strCurrentFileName = sCmdline;
	}
	else {
		g_strCurrentFileName = GetRegisterValue(HKEY_CURRENT_USER, _T("Software\\Showbmp"), _T("defaultValue"), REG_SZ);
	}

	/*
	CString file_width	= GetRegisterValue(HKEY_CURRENT_USER, _T("Software\\Showbmp"), _T("width"), REG_SZ);
	CString file_height	= GetRegisterValue(HKEY_CURRENT_USER, _T("Software\\Showbmp"), _T("height"), REG_SZ);

	g_nCurrentWidth		= g_currentPicWidth		= _ttoi(file_width);
	g_nCurrentHeight	= g_currentPicHeight	= _ttoi(file_height);
	*/

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Showbmp"));


	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame * pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->fixWindowSizeByViewWidthHeight();

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

int CshowbmpApp::ExitInstance()
{
	//TODO:  处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

CString GetRegisterValue(HKEY hkey, LPCTSTR Rgspath, LPCTSTR ValueName, DWORD type)
{
	CString str = _T("");
	unsigned char path[255];
	HKEY hKEY;
	LONG ret = RegOpenKeyEx(hkey, Rgspath, 0, KEY_READ, &hKEY);

	if (ret != ERROR_SUCCESS)
	{
		RegCloseKey(hKEY);
		AfxMessageBox(_T("Error"));
		return str;
	}
	// 读取键值内容    
	DWORD dwInfoSize = 255;
	ret = RegQueryValueEx(hKEY, ValueName, NULL, &type, (BYTE*)path, &dwInfoSize);

	if (ret != ERROR_SUCCESS)
	{
		RegCloseKey(hKEY);
		return str;
	}

	WCHAR * wideArray = (WCHAR *)path;

	str = CString(wideArray);
	return str;
}

// CshowbmpApp 消息处理程序



