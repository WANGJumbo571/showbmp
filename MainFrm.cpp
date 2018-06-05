
// MainFrm.cpp : CMainFrame 类的实现
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO:  在此添加成员初始化代码
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
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	// 创建一个视图以占用框架的工作区
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
			CRect(0, 0, g_viewWidth, g_viewHeight), 
			this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	m_wndView.parent = this;

	//showbmp刚开始运行时的第一次生成图片链表
	fixCurrentDirList(g_strCurrentFileName);

	//加载初始设置的图片（由g_strCurrentFileName指定）
	m_wndView.TryLoadImage(g_strCurrentFileName);
	m_wndView.initializeShowParams();

	m_wndView.ModifyStyle(WS_BORDER, 0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	//cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 诊断

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


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CFrameWnd::OnSetFocus(pOldWnd);
	// 将焦点前移到视图窗口
	//m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//把父窗口收到的KeyBoard消息传给子ChildView窗口处理
BOOL CMainFrame::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		return m_wndView.PreTranslateMessage(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//确定好要显示哪个index号的图片后，在ChildView中显示它
void CMainFrame::showCurrentIndex()
{
	//从DirList队列中取出index所指示的哪个图片的信息
	POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
	FileInfoDetail nextFile = g_currentDirList->GetAt(pos);

	if (g_strCurrentFileName.Compare(nextFile.filePathName) == 0)
		return;

	g_strCurrentFileName = nextFile.filePathName;

	//加载并显示图片
	m_wndView.TryLoadImage(g_strCurrentFileName);
	m_wndView.initializeShowParams();

	//如果是showFit显示模式
	if (m_wndView.checkedFit) {
		fitOperation();
		return;
	}

	//如果是缺省类型的显示模式
	m_wndView.InvalidateRect(NULL);
}

//显示队列的第一个图片
void CMainFrame::OnFirst()
{
	if (g_listCnt == 0)
		return;

	g_currentIndex = 0;
	showCurrentIndex();
}

//显示队列的最后一个图片
void CMainFrame::OnLast()
{
	if (g_listCnt == 0)
		return;

	g_currentIndex = g_listCnt - 1;
	showCurrentIndex();
}

//用showFit模式显示图片
void CMainFrame::onFit()
{
	OnFit();
}

void CMainFrame::OnFit()
{
	//在showFit和NoneShowFit模式间转换
	m_wndView.checkedFit = !m_wndView.checkedFit;

	if (m_wndView.checkedFit) {
		//showFit模式菜单打勾
		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FIT, MF_BYCOMMAND | MF_CHECKED);
		fitOperation();
	}
	else {
		//NoneShowFit模式，取消菜单打勾
		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FIT, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

//showFit模式，计算当前index图片应该显示的长宽大小(viewWidth和viewHeight)
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

	//按照上面部分确定的viewWidth和viewHeight显示ChildView中的内容
	fixWindowSizeByViewWidthHeight();
}

void CMainFrame::OnOpenFile()
{
	// TODO: Add your control notification handler code here   
	// 设置过滤器   
	TCHAR szFilter[] = _T("图像文件(*.jpg;*.jpeg;*.gif;*.png;*.ico)|*.jpg;*.jpeg;*.gif;*.png;*.ico|");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("jpg"), NULL, 0, szFilter, this);

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		g_strCurrentFileName = fileDlg.GetPathName();
		
		//当且仅当初始化时和用户通过OpenFile菜单命令指定新目录和显示图片时调用此例程
		fixCurrentDirList(g_strCurrentFileName);

		//目录链表和显示图片加载好了后，通知m_wndView窗口刷新内容
		m_wndView.TryLoadImage(g_strCurrentFileName);
		//重置showBig有关的参数
		m_wndView.initializeShowParams();

		//如果是showFit
		if (m_wndView.checkedFit) {
			fitOperation();
			return;
		}

		//这里处理缺省形式的显示
		m_wndView.InvalidateRect(NULL);
	}
}

//通过应用开始时设定的窗口大小变量（viewWidth，viewHeight）=（1024,768）定好窗口尺寸
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

// 用于运行对话框的应用程序命令
void CMainFrame::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

//画出AboutDialog窗口的内容
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
			showSize.Format(L"%s 文件 %d X %d 像素",
				currentFile.fileCurrentPicType, currentFile.filePicWidth, currentFile.filePicHeight);
			pSize->SetWindowText(showSize);

			showName.Format(L"文件名：%s", currentFile.fileName);
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

			showTime.Format(L"创建时间 %04d-%02d-%02d %02d:%02d:%02d",
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
			//计算显示图片名称占用的行数，再根据数值确定Dialog窗口的大小
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
	HINSTANCE hInst = LoadLibrary(L"User32.DLL"); //显式加载DLL
	if (hInst)
	{
		typedef BOOL(WINAPI *MYFUNC) (HWND, COLORREF, BYTE, DWORD);
		MYFUNC fun = NULL;

		//取得SetLayeredWindowAttributes函数指针     
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)
			fun(this->GetSafeHwnd(), 0, 145, 2);     //通过第三个参数来设置窗体透明程度
		FreeLibrary(hInst);
	}
	*/
}
