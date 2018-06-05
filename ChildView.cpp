
// ChildView.cpp : CChildView ���ʵ��
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

//ChildView��GdiPlus������ʾ�������͵�ͼƬ
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

// CChildView ��Ϣ�������

BOOL CChildView::OnEraseBkgnd(CDC *pDC)
{
	//__super::OnEraseBkgnd(pDC);
	return TRUE; 
}

//���ձ��û�����������������øı�ı����ڵĴ�С����Ϣ
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

//��ChildView�����ڡ�˫��������������showBig��noneShowBig��ʾģʽ��ת��
void	CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	showBig = !showBig;
	initialShow = true;

	InvalidateRect(NULL);
}

//���������£���ʼ�϶�showBigͼƬ��ChildView������ʾ�Ĳ���
void	CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	lbtonDown = true;
	lbtonDownInitialX = point.x;
	lbtonDownInitialY = point.y;
}

//���������ϣ��϶����
void	CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	lbtonDown = false;
}

//�����������º���ƶ�����
void	CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	//�������ƺ�֮ˮ����������������������
	//��������϶�ʱ�Ƴ����ں�Ĳ���
	TRACKMOUSEEVENT   tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = m_hWnd;
	_TrackMouseEvent(&tme);

	if (!lbtonDown || !showBig)
		return;

	//���ChildView���ڵĳ��Ϳ�ĳߴ����ͼƬ��ʾ�ĳߴ磬�����ƶ�ͼƬ
	if (g_viewWidth >= fileCurrentWidth && g_viewHeight >= fileCurrentHeight)
		return;

	//���ChildView���ڵĿ�С��ͼƬ����ʾ��ȣ����������ƶ�
	if (g_viewWidth < fileCurrentWidth)
		fileCurrentCX += point.x - lbtonDownInitialX;

	//���ChildView���ڵĸ߶�С��ͼƬ����ʾ�߶ȣ����������ƶ�
	if (g_viewHeight < fileCurrentHeight)
		fileCurrentCY += point.y - lbtonDownInitialY;

	lbtonDownInitialX = point.x;
	lbtonDownInitialY = point.y;

	//����ͼƬ����ʾ��ʼ�����꣬��ChildView�ڻ���ͼ��
	InvalidateRect(NULL);
}

//���������϶��������Ƴ���ChildView����������������״̬
LRESULT CChildView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	lbtonDown = false;
	return 0;
}

//�ѵ�ǰͼƬͼ����ʾ�ڣ�viewWidth��viewHeight����С�Ĵ��ڵ�������
void CChildView::fixCurrentFile(FileInfoDetail & info)
{
	info.fileCurrentPicType = info.filePathName.Right(
		g_strCurrentFileName.GetLength() - g_strCurrentFileName.ReverseFind('.') - 1);
	info.fileCurrentPicType.MakeUpper();

	int width = info.filePicWidth;
	int height = info.filePicHeight;

	int showWidth, showHeight;

	//�����showBigģʽ����ͼƬ��ԭʼ�ߴ���ʾ
	if (showBig) {
		showWidth = width;
		showHeight = height;
	}
	//���򣬰�ͼƬ��viewWidth��viewHeight��С��ChildView��������ʾ
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

	//�ó�ͼƬ��ChildView�е�ʵ����ʾ�Ĵ�С
	fileCurrentWidth = showWidth;
	fileCurrentHeight = showHeight;

	//ȱʡģʽΪinitialShow=true����ͼƬ��ʾ�ڴ��ڵ�����
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

	//����showBigģʽ����涨ͼƬ��������ʾ�ı߽�ֵ
	if (g_viewWidth < fileCurrentWidth) {
		if (fileCurrentCX >= 0) {
			fileCurrentCX = 0;
		}
		if (fileCurrentCX + fileCurrentWidth < g_viewWidth) {
			fileCurrentCX = g_viewWidth - fileCurrentWidth;
		}
	}

	//����showBigģʽ����涨ͼƬ��������ʾ�ı߽�ֵ
	if (g_viewHeight < fileCurrentHeight) {
		if (fileCurrentCY >= 0) {
			fileCurrentCY = 0;
		}
		if (fileCurrentCY + fileCurrentHeight < g_viewHeight) {
			fileCurrentCY = g_viewHeight - fileCurrentHeight;
		}
	}
}

//��������Ϣ����
BOOL CChildView::OnMouseWheel(UINT   nFlags, short   zDelta, CPoint   pt)
{
	static short zDeltaSoFar = 0;

	if (g_currentIndex < 0 || g_listCnt <= 0) {
		g_strCurrentFileName = CString("");
		return TRUE;
	}

	zDeltaSoFar += zDelta;

	//��껬������ʱ��Ӧ��ǰ�����
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

//����ChildView���ڽ��յ��ļ�����Ϣ
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
			//ǰ�����
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

			//��GdiPlus::Image::FromFile����ͼƬ�������»���������
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

//������ʾģʽ״̬
void CChildView::initializeShowParams()
{
	initialShow = true;
	showBig = false;
	lbtonDown = false;

	lbtonDownInitialX = 0;
	lbtonDownInitialY = 0;
}

//������ص�ǰͼƬ
void CChildView::TryLoadImage(CString pathName)
{
	KillTimer(1212);

	if (img != NULL) {
		delete img;
		img = NULL;
	}

	//GdiPlus::Image::FromFile�ɶԸ��������͵�ͼƬ
	img = Image::FromFile(pathName);
	
	if (img == NULL) return;
	
	width = img->GetWidth();
	height = img->GetHeight();
	frame_pos = 0;
	isAnimated = IsAnimate();

	//�����gif�ļ����ж��frame������ͼƬ)������Timer������ʾ��
	if (isAnimated) {
		long pause = ((long *)pro_item->value)[frame_pos];
		SetTimer(1212, pause, NULL);
	}
}

void CChildView::OnTimer(UINT_PTR nID)
{
	KillTimer(1212);

	if (img == NULL) return;

	//������ʾ
	Invalidate();
	UpdateWindow();

	long pause = ((long *)pro_item->value)[frame_pos];
	pause += 25;
	SetTimer(1212, pause, NULL);
}

//����showģʽ��ͬ��ʾͼƬ
void CChildView::PaintWindowImpl()
{
	GetParent()->SetWindowText(g_strCurrentFileName);

	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	
	if (g_currentIndex < 0 || g_listCnt <= 0 || img == NULL)
	{
		CBrush brush(RGB(0, 0, 0));
		CRect rec0(0, 0, g_viewWidth, g_viewHeight);
		dc.FillRect(rec0, &brush);
		return;
	}
	
	CDC * mdc = new CDC();
	mdc->CreateCompatibleDC(&dc);
	
	//�ڴ�DC��ͼ������DCѡ��BitMap����ֽ
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&dc, g_viewWidth, g_viewHeight);
	CBitmap * pOldBit = mdc->SelectObject(&memBitmap);

	//ȡ�������еĵ�ǰͼƬ��Ϣ���Լ���ͼƬ�ڴ����е�λ��
	POSITION pos = g_currentDirList->FindIndex(g_currentIndex);
	FileInfoDetail currentFile = g_currentDirList->GetAt(pos);

	currentFile.filePicHeight = height;
	currentFile.filePicWidth = width;

	//ֻ�Զ������͵�GIF�ļ���Ч
	if (isAnimated) {
		GUID guid = FrameDimensionTime;
		frame_pos++;
		if (frame_pos >= frame_count)
			frame_pos = 0;
		img->SelectActiveFrame(&guid, frame_pos);
	}

	//����ͼƬ�ڱ�������ʾ�ĳߴ磬�����fileCurrentCX,fileCurrentCY��
	//fileCurrentWidth��fileCurrentHeight������
	fixCurrentFile(currentFile);
	g_currentDirList->SetAt(pos, currentFile);

	//ͼƬ��Χ����Ϳ��
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

	//��GDIPLUS�ĳ���㶨����ͼƬ����
	HDC hdc = mdc->GetSafeHdc();
	Graphics graph(hdc);
	graph.DrawImage(img,
		fileCurrentCX, fileCurrentCY,
		fileCurrentWidth, fileCurrentHeight);

	//һ���Եذ��ڴ��ﻭͼ��ʾ���Ա��ⴰ����˸
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

//�����������������׷���������˿ĺ��ѩ
bool CChildView::IsAnimate()
{
	//�����̶�����ͼƬ���Ͷ���Ч
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
