
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "showbmp.h"
#include "MainFrm.h"
#include "sys/stat.h"

extern void fixCurrentDirList(CString choosenFile);

extern CList<FileInfoDetail, FileInfoDetail &> * g_currentDirList;
extern int			g_listCnt, g_currentIndex;
extern int			g_viewWidth, g_viewHeight;
extern CString	g_strCurrentFileName;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_OPENFILE,		&CMainFrame::OnOpenFile)
	ON_COMMAND(ID_FIRST,			&CMainFrame::OnFirst)
	ON_COMMAND(ID_LAST,				&CMainFrame::OnLast)
	ON_COMMAND(ID_FIT,				&CMainFrame::OnFit)
	ON_COMMAND(ID_APPABOUT,	&CMainFrame::OnAppAbout)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO:  �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("δ�ܴ����˵���\n");
		return -1;      // δ�ܴ���
	}

	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
			CRect(0, 0, g_viewWidth, g_viewHeight), 
			this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}

	m_wndView.parent = this;

	//showbmp�տ�ʼ����ʱ�ĵ�һ������ͼƬ����
	fixCurrentDirList(g_strCurrentFileName);

	//���س�ʼ���õ�ͼƬ����g_strCurrentFileNameָ����
	m_wndView.TryLoadImage(g_strCurrentFileName);
	m_wndView.initializeShowParams();

	m_wndView.ModifyStyle(WS_BORDER, 0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	//cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CFrameWnd::OnSetFocus(pOldWnd);
	// ������ǰ�Ƶ���ͼ����
	//m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//�Ѹ������յ���KeyBoard��Ϣ������ChildView���ڴ���
BOOL CMainFrame::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		return m_wndView.PreTranslateMessage(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//ȷ����Ҫ��ʾ�ĸ�index�ŵ�ͼƬ����ChildView����ʾ��
void CMainFrame::showCurrentIndex()
{
	//��DirList������ȡ��index��ָʾ���ĸ�ͼƬ����Ϣ
	POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
	FileInfoDetail nextFile = g_currentDirList->GetAt(pos);

	if (g_strCurrentFileName.Compare(nextFile.filePathName) == 0)
		return;

	g_strCurrentFileName = nextFile.filePathName;

	//���ز���ʾͼƬ
	m_wndView.TryLoadImage(g_strCurrentFileName);
	m_wndView.initializeShowParams();

	//�����showFit��ʾģʽ
	if (m_wndView.checkedFit) {
		fitOperation();
		return;
	}

	//�����ȱʡ���͵���ʾģʽ
	m_wndView.InvalidateRect(NULL);
}

//��ʾ���еĵ�һ��ͼƬ
void CMainFrame::OnFirst()
{
	if (g_listCnt == 0)
		return;

	g_currentIndex = 0;
	showCurrentIndex();
}

//��ʾ���е����һ��ͼƬ
void CMainFrame::OnLast()
{
	if (g_listCnt == 0)
		return;

	g_currentIndex = g_listCnt - 1;
	showCurrentIndex();
}

//��showFitģʽ��ʾͼƬ
void CMainFrame::onFit()
{
	OnFit();
}

void CMainFrame::OnFit()
{
	//��showFit��NoneShowFitģʽ��ת��
	m_wndView.checkedFit = !m_wndView.checkedFit;

	if (m_wndView.checkedFit) {
		//showFitģʽ�˵���
		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FIT, MF_BYCOMMAND | MF_CHECKED);
		fitOperation();
	}
	else {
		//NoneShowFitģʽ��ȡ���˵���
		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FIT, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

//showFitģʽ�����㵱ǰindexͼƬӦ����ʾ�ĳ����С(viewWidth��viewHeight)
void CMainFrame::fitOperation()
{
	int width = m_wndView.width;
	int height = m_wndView.height;
	
	int maxWidth = 1916;
	int maxHeight = 984;

	int showWidth, showHeight;

	if (width >= maxWidth) {
		if (height >= maxHeight) {
			if (((double)width) / height >= ((double)maxWidth) / maxHeight) {
				showWidth = maxWidth;
				showHeight = (int)(((double)(showWidth)) / width*height);
			}
			else {
				showHeight = maxHeight;
				showWidth = (int)(((double)(showHeight)) / height * width);
			}
		}
		else {
			showWidth = maxWidth;
			showHeight = (int)(((double)(showWidth)) / width*height);
		}
	}
	else {
		if (height >= maxHeight) {
			showHeight = maxHeight;
			showWidth = (int)(((double)(showHeight)) / height * width);
		}
		else {
			showWidth = width;
			showHeight = height;
		}
	}

	if (g_viewWidth == showWidth && g_viewHeight == showHeight) {
		m_wndView.InvalidateRect(NULL);
		return;
	}

	g_viewWidth = showWidth;
	g_viewHeight = showHeight;

	//�������沿��ȷ����viewWidth��viewHeight��ʾChildView�е�����
	fixWindowSizeByViewWidthHeight();
}

void CMainFrame::OnOpenFile()
{
	// TODO: Add your control notification handler code here   
	// ���ù�����   
	TCHAR szFilter[] = _T("ͼ���ļ�(*.jpg;*.jpeg;*.gif;*.png;*.ico)|*.jpg;*.jpeg;*.gif;*.png;*.ico|");
	// ������ļ��Ի���   
	CFileDialog fileDlg(TRUE, _T("jpg"), NULL, 0, szFilter, this);

	// ��ʾ���ļ��Ի���   
	if (IDOK == fileDlg.DoModal())
	{
		g_strCurrentFileName = fileDlg.GetPathName();
		
		//���ҽ�����ʼ��ʱ���û�ͨ��OpenFile�˵�����ָ����Ŀ¼����ʾͼƬʱ���ô�����
		fixCurrentDirList(g_strCurrentFileName);

		//Ŀ¼�������ʾͼƬ���غ��˺�֪ͨm_wndView����ˢ������
		m_wndView.TryLoadImage(g_strCurrentFileName);
		//����showBig�йصĲ���
		m_wndView.initializeShowParams();

		//�����showFit
		if (m_wndView.checkedFit) {
			fitOperation();
			return;
		}

		//���ﴦ��ȱʡ��ʽ����ʾ
		m_wndView.InvalidateRect(NULL);
	}
}

//ͨ��Ӧ�ÿ�ʼʱ�趨�Ĵ��ڴ�С������viewWidth��viewHeight��=��1024,768�����ô��ڳߴ�
void CMainFrame::fixWindowSizeByViewWidthHeight()
{
	int frameWidth  = g_viewWidth + 20;
	int frameHeight = g_viewHeight + 63;
	int startx = 1920 / 2 - frameWidth / 2;
	int starty = 1080 / 2 - frameHeight / 2 - 25;
	if (startx < 0)		startx = 0;
	if (starty < 0)		starty = 0;

	SetWindowPos(NULL, startx, starty, frameWidth, frameHeight, SWP_SHOWWINDOW);
	m_wndView.afterSetPos = true;
	m_wndView.SetWindowPos(NULL, 0, 0, g_viewWidth, g_viewHeight, SWP_SHOWWINDOW);
}

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

/*
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDI_BIRDING, birding);
}
*/

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CMainFrame::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

//����AboutDialog���ڵ�����
void CAboutDlg::OnPaint()
{
	CDialog::OnPaint();

	CStatic		*pName = (CStatic*)GetDlgItem(ID_STATIC4);
	CStatic		*pSize = (CStatic*)GetDlgItem(ID_STATIC5);
	CStatic		*pCreateTime = (CStatic*)GetDlgItem(ID_STATIC6);
	CString		showSize = CString("");
	CString		showName = CString("");

	if (g_currentIndex >= 0 && g_listCnt > 0)
	{
		POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
		FileInfoDetail currentFile = g_currentDirList->GetAt(pos);

		if (!currentFile.filePathName.IsEmpty()) {
			showSize.Format(L"%s �ļ� %d X %d ����",
				currentFile.fileCurrentPicType, currentFile.filePicWidth, currentFile.filePicHeight);
			pSize->SetWindowText(showSize);

			showName.Format(L"�ļ�����%s", currentFile.fileName);
			pName->SetWindowText(showName);

			struct stat sb;
			int len = 2*currentFile.filePathName.GetLength();
			char *cstr = new char[len + 1];
			cstr[len] = 0;

			WideCharToMultiByte(CP_OEMCP, 0, currentFile.filePathName, -1, cstr, len, NULL, NULL);
			stat(cstr, &sb);

			struct tm t;
			localtime_s(&t, &(sb.st_mtime));

			CString showTime = CString("");

			showTime.Format(L"����ʱ�� %04d-%02d-%02d %02d:%02d:%02d",
				t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
				t.tm_hour, t.tm_min, t.tm_sec);
			pCreateTime->SetWindowText(showTime);

			delete[] cstr;
			return;
		}
	}
	
	pSize->SetWindowText(CString(""));
	pName->SetWindowText(CString(""));
}

BOOL CAboutDlg::OnInitDialog()
{
	/*
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, 145, 2);
	*/

	if (g_currentIndex >= 0 && g_listCnt > 0)
	{
		POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
		FileInfoDetail currentFile = g_currentDirList->GetAt(pos);

		if (!currentFile.filePathName.IsEmpty()) {
			int len = currentFile.fileName.GetLength();
			int frameWidth = 565;
			int frameHeight;
			int actualLen = 0;
			//������ʾͼƬ����ռ�õ��������ٸ�����ֵȷ��Dialog���ڵĴ�С
			for (int i = 0; i < len; i++) {
				WCHAR w = currentFile.fileName.GetAt(i);
				if (w < 128) {
					actualLen++;
				}
				else {
					actualLen += 2;
				}
			}
			int numLines = actualLen / 60;
			if (numLines <= 4) {
				frameHeight = 200;
			}
			else {
				frameHeight = 210 + (numLines - 4) * 15;
			}
			int startx = 1920 / 2 - frameWidth / 2;
			int starty = 1080 / 2 - frameHeight / 2 - 25;
			if (startx < 0)		startx = 0;
			if (starty < 0)		starty = 0;
			SetWindowPos(NULL, startx, starty, frameWidth, frameHeight, SWP_SHOWWINDOW);
		}
	}

	return TRUE;

	/*
	HINSTANCE hInst = LoadLibrary(L"User32.DLL"); //��ʽ����DLL
	if (hInst)
	{
		typedef BOOL(WINAPI *MYFUNC) (HWND, COLORREF, BYTE, DWORD);
		MYFUNC fun = NULL;

		//ȡ��SetLayeredWindowAttributes����ָ��     
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)
			fun(this->GetSafeHwnd(), 0, 145, 2);     //ͨ�����������������ô���͸���̶�
		FreeLibrary(hInst);
	}
	*/
}
