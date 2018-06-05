
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "showbmp.h"
#include "ChildView.h"
#include <afxpriv.h>
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CList<FileInfoDetail, FileInfoDetail &> * g_currentDirList;
extern int			g_listCnt, g_currentIndex;
extern int			g_viewWidth, g_viewHeight;
extern CString	g_strCurrentFileName;

//ChildView用GdiPlus库来显示各种类型的图片
CChildView::CChildView()
{
	GdiplusStartupInput in;
	GdiplusStartup(&token, &in, NULL);
}

CChildView::~CChildView()
{
	GdiplusShutdown(token);
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_MESSAGE(WM_USER_PAINT, &CChildView::OnUserPaint)
	ON_MESSAGE(WM_MOUSELEAVE, &CChildView::OnMouseLeave)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CChildView 消息处理程序

BOOL CChildView::OnEraseBkgnd(CDC *pDC)
{
	//__super::OnEraseBkgnd(pDC);
	return TRUE; 
}

//接收被用户鼠标拖拉或重新设置改变的本窗口的大小的信息
void CChildView::OnSize(UINT nType, int cx, int cy)
{
	if (afterSetPos == false)
		return;

	initializeShowParams();

	g_viewWidth = cx;
	g_viewHeight = cy;
	InvalidateRect(NULL);
}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	//cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

//在ChildView窗口内“双击”鼠标左键，在showBig和noneShowBig显示模式间转换
void	CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	showBig = !showBig;
	initialShow = true;

	InvalidateRect(NULL);
}

//鼠标左键按下，开始拖动showBig图片在ChildView窗口显示的部分
void	CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	lbtonDown = true;
	lbtonDownInitialX = point.x;
	lbtonDownInitialY = point.y;
}

//鼠标左键弹上，拖动完毕
void	CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	lbtonDown = false;
}

//鼠标在左键按下后的移动处理
void	CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	//君不见黄河之水天上来，奔流到海不复回
	//处理鼠标拖动时移出窗口后的部分
	TRACKMOUSEEVENT   tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = m_hWnd;
	_TrackMouseEvent(&tme);

	if (!lbtonDown || !showBig)
		return;

	//如果ChildView窗口的长和宽的尺寸大于图片显示的尺寸，则不能移动图片
	if (g_viewWidth >= fileCurrentWidth && g_viewHeight >= fileCurrentHeight)
		return;

	//如果ChildView窗口的宽小于图片的显示宽度，则处理左右移动
	if (g_viewWidth < fileCurrentWidth)
		fileCurrentCX += point.x - lbtonDownInitialX;

	//如果ChildView窗口的高度小于图片的显示高度，则处理上下移动
	if (g_viewHeight < fileCurrentHeight)
		fileCurrentCY += point.y - lbtonDownInitialY;

	lbtonDownInitialX = point.x;
	lbtonDownInitialY = point.y;

	//根据图片的显示起始点坐标，在ChildView内画出图像
	InvalidateRect(NULL);
}

//如果鼠标在拖动过程中移出了ChildView，则重置鼠标左键的状态
LRESULT CChildView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	lbtonDown = false;
	return 0;
}

//把当前图片图像显示在（viewWidth，viewHeight）大小的窗口的正中央
void CChildView::fixCurrentFile(FileInfoDetail & info)
{
	info.fileCurrentPicType = info.filePathName.Right(
		g_strCurrentFileName.GetLength() - g_strCurrentFileName.ReverseFind('.') - 1);
	info.fileCurrentPicType.MakeUpper();

	int width = info.filePicWidth;
	int height = info.filePicHeight;

	int showWidth, showHeight;

	//如果是showBig模式，则按图片的原始尺寸显示
	if (showBig) {
		showWidth = width;
		showHeight = height;
	}
	//否则，把图片在viewWidth，viewHeight大小的ChildView窗口里显示
	else {
		if (width >= g_viewWidth) {
			if (height >= g_viewHeight) {
				if (((double)width) / height >= ((double)g_viewWidth) / g_viewHeight) {
					showWidth = g_viewWidth;
					showHeight = (int)(((double)(showWidth)) / width*height);
				}
				else {
					showHeight = g_viewHeight;
					showWidth = (int)(((double)(showHeight)) / height * width);
				}
			}
			else {
				showWidth = g_viewWidth;
				showHeight = (int)(((double)(showWidth)) / width*height);
			}
		}
		else {
			if (height >= g_viewHeight) {
				showHeight = g_viewHeight;
				showWidth = (int)(((double)(showHeight)) / height * width);
			}
			else {
				showWidth = width;
				showHeight = height;
			}
		}
	}

	//得出图片在ChildView中的实际显示的大小
	fileCurrentWidth = showWidth;
	fileCurrentHeight = showHeight;

	//缺省模式为initialShow=true，把图片显示在窗口的中央
	if (initialShow) {
		fileCurrentCX = g_viewWidth / 2 - fileCurrentWidth / 2;
		fileCurrentCY = g_viewHeight / 2 - fileCurrentHeight / 2;

		if (!showBig) {
			if (fileCurrentCX < 0)
				fileCurrentCX = 0;
			if (fileCurrentCY < 0)
				fileCurrentCY = 0;
		}

		initialShow = false;
		return;
	}

	//如是showBig模式，则规定图片的左右显示的边界值
	if (g_viewWidth < fileCurrentWidth) {
		if (fileCurrentCX >= 0) {
			fileCurrentCX = 0;
		}
		if (fileCurrentCX + fileCurrentWidth < g_viewWidth) {
			fileCurrentCX = g_viewWidth - fileCurrentWidth;
		}
	}

	//如是showBig模式，则规定图片的上下显示的边界值
	if (g_viewHeight < fileCurrentHeight) {
		if (fileCurrentCY >= 0) {
			fileCurrentCY = 0;
		}
		if (fileCurrentCY + fileCurrentHeight < g_viewHeight) {
			fileCurrentCY = g_viewHeight - fileCurrentHeight;
		}
	}
}

//鼠标滚轮消息处理
BOOL CChildView::OnMouseWheel(UINT   nFlags, short   zDelta, CPoint   pt)
{
	static short zDeltaSoFar = 0;

	if (g_currentIndex < 0 || g_listCnt <= 0) {
		g_strCurrentFileName = CString("");
		return TRUE;
	}

	zDeltaSoFar += zDelta;

	//鼠标滑轮运作时相应地前后遍历
	if (zDeltaSoFar >= 120) {
		zDeltaSoFar = 0;
		GetParent()->PostMessage(WM_KEYDOWN, VK_LEFT);
	} 
	else {
		if (zDeltaSoFar <= -120) {
			zDeltaSoFar = 0;
			GetParent()->PostMessage(WM_KEYDOWN, VK_RIGHT);
		}
	}
	return TRUE; 
}

//处理ChildView窗口接收到的键盘消息
BOOL CChildView::PreTranslateMessage(MSG * pMsg)
{
	FileInfoDetail nextFile;

	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam)
		{
		case VK_F1:
			parent->onFit();
			break;

		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			//前后遍历
			if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP) {
				if (g_currentIndex > 0)
					g_currentIndex--;
			}
			else {
				if (g_currentIndex < (g_listCnt - 1))
					g_currentIndex++;
			}

			POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
			nextFile = g_currentDirList->GetAt(pos);

			if (g_strCurrentFileName.Compare(nextFile.filePathName) == 0)
				break;

			g_strCurrentFileName = nextFile.filePathName;

			//用GdiPlus::Image::FromFile加载图片，并重新画窗口内容
			TryLoadImage(g_strCurrentFileName);
			initializeShowParams();

			if (checkedFit) {
				parent->fitOperation();
			}
			else {
				InvalidateRect(NULL);
			}
			break;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//重置显示模式状态
void CChildView::initializeShowParams()
{
	initialShow = true;
	showBig = false;
	lbtonDown = false;

	lbtonDownInitialX = 0;
	lbtonDownInitialY = 0;
}

//负责加载当前图片
void CChildView::TryLoadImage(CString pathName)
{
	KillTimer(1212);

	if (img != NULL) {
		delete img;
		img = NULL;
	}

	//GdiPlus::Image::FromFile可对付多种类型的图片
	img = Image::FromFile(pathName);
	
	if (img == NULL) return;
	
	width = img->GetWidth();
	height = img->GetHeight();
	frame_pos = 0;
	isAnimated = IsAnimate();

	//如果是gif文件且有多高frame（动画图片)，设置Timer例程显示它
	if (isAnimated) {
		long pause = ((long *)pro_item->value)[frame_pos];
		SetTimer(1212, pause, NULL);
	}
}

void CChildView::OnTimer(UINT_PTR nID)
{
	KillTimer(1212);

	if (img == NULL) return;

	//立即显示
	Invalidate();
	UpdateWindow();

	long pause = ((long *)pro_item->value)[frame_pos];
	pause += 25;
	SetTimer(1212, pause, NULL);
}

//根据show模式不同显示图片
void CChildView::PaintWindowImpl()
{
	GetParent()->SetWindowText(g_strCurrentFileName);

	CPaintDC dc(this); // 用于绘制的设备上下文
	
	if (g_currentIndex < 0 || g_listCnt <= 0 || img == NULL)
	{
		CBrush brush(RGB(0, 0, 0));
		CRect rec0(0, 0, g_viewWidth, g_viewHeight);
		dc.FillRect(rec0, &brush);
		return;
	}
	
	CDC * mdc = new CDC();
	mdc->CreateCompatibleDC(&dc);
	
	//内存DC画图首先用DC选择BitMap做稿纸
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&dc, g_viewWidth, g_viewHeight);
	CBitmap * pOldBit = mdc->SelectObject(&memBitmap);

	//取出链表中的当前图片信息，以计算图片在窗口中的位置
	POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
	FileInfoDetail currentFile = g_currentDirList->GetAt(pos);

	currentFile.filePicHeight = height;
	currentFile.filePicWidth = width;

	//只对动画类型的GIF文件有效
	if (isAnimated) {
		GUID guid = FrameDimensionTime;
		frame_pos++;
		if (frame_pos >= frame_count)
			frame_pos = 0;
		img->SelectActiveFrame(&guid, frame_pos);
	}

	//计算图片在本窗口显示的尺寸，结果在fileCurrentCX,fileCurrentCY和
	//fileCurrentWidth，fileCurrentHeight变量中
	fixCurrentFile(currentFile);
	g_currentDirList->SetAt(pos, currentFile);

	//图片周围区域涂黑
	CBrush brush(RGB(0, 0, 0));

	CRect rec0(0, 0, g_viewWidth, fileCurrentCY);
	mdc->FillRect(rec0, &brush);

	CRect rec1(0, fileCurrentCY + fileCurrentHeight, g_viewWidth, g_viewHeight);
	mdc->FillRect(rec1, &brush);

	CRect rec2(0, fileCurrentCY, fileCurrentCX, fileCurrentCY + fileCurrentHeight);
	mdc->FillRect(rec2, &brush);

	CRect rec3(	fileCurrentCX + fileCurrentWidth, fileCurrentCY, 
				g_viewWidth, fileCurrentCY + fileCurrentHeight);
	mdc->FillRect(rec3, &brush);

	//用GDIPLUS的程序搞定多种图片类型
	HDC hdc = mdc->GetSafeHdc();
	Graphics graph(hdc);
	graph.DrawImage(img,
		fileCurrentCX, fileCurrentCY,
		fileCurrentWidth, fileCurrentHeight);

	//一次性地把内存里画图显示，以避免窗口闪烁
	dc.BitBlt(0, 0, g_viewWidth, g_viewHeight, mdc, 0, 0, SRCCOPY);

	mdc->SelectObject(pOldBit);
	memBitmap.DeleteObject();
	delete mdc;
}

afx_msg LRESULT CChildView::OnUserPaint(WPARAM wParam, LPARAM lParam)
{
	PaintWindowImpl();
	return 0;
}

afx_msg void CChildView::OnPaint()
{
	PaintWindowImpl();
}

//君不见高堂明镜悲白发，朝如青丝暮成雪
bool CChildView::IsAnimate()
{
	//本例程对所有图片类型都有效
	UINT cn = img->GetFrameDimensionsCount();
	GUID* pGuid = new GUID[cn];
	img->GetFrameDimensionsList(pGuid, cn);
	frame_count = img->GetFrameCount(&pGuid[0]);
	int sz = img->GetPropertyItemSize(PropertyTagFrameDelay);
	pro_item = (PropertyItem *)malloc(sz);
	img->GetPropertyItem(PropertyTagFrameDelay, sz, pro_item);
	delete pGuid;
	return frame_count > 1;
}
