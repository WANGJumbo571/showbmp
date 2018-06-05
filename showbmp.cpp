
// showbmp.cpp : ����Ӧ�ó��������Ϊ��
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

// CshowbmpApp ����

CshowbmpApp::CshowbmpApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���: 
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("showbmp.AppID.NoVersion"));

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CshowbmpApp ����

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

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Showbmp"));


	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame * pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->fixWindowSizeByViewWidthHeight();

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

int CshowbmpApp::ExitInstance()
{
	//TODO:  �����������ӵĸ�����Դ
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
	// ��ȡ��ֵ����    
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

// CshowbmpApp ��Ϣ�������



