// ChildView.h : CChildView 类的接口
//

#include <gdiplus.h> 
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

#pragma once

// CChildView 窗口

class CMainFrame;

class CChildView : public CWnd
{
// 构造
public:
	CChildView();
	bool afterSetPos = false;
	bool checkedFit = false;
	CMainFrame * parent;
	int width, height;

// 特性
public:
	virtual BOOL PreTranslateMessage(MSG * pMsg);

// 操作
public:
	void TryLoadImage(CString pathName);
	void fixCurrentFile(FileInfoDetail & info);

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

private:
	bool isAnimated = false;
	Image *img = NULL;
	int frame_count;
	int frame_pos;
	PropertyItem *pro_item;
	ULONG_PTR token;

	void PaintWindowImpl();
	bool IsAnimate();

	bool initialShow = true;
	bool showBig = false;
	bool lbtonDown = false;
	int lbtonDownInitialX = 0;
	int lbtonDownInitialY = 0;

	int fileCurrentCX;
	int fileCurrentCY;
	int fileCurrentWidth;
	int fileCurrentHeight;

public:
	void initializeShowParams();

	// 生成的消息映射函数
protected:
	afx_msg void			OnPaint();
	afx_msg LRESULT	OnUserPaint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL		OnMouseWheel(UINT   nFlags, short   zDelta, CPoint   pt);
	afx_msg void			OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL		OnEraseBkgnd(CDC *pDC);
	afx_msg void			OnTimer(UINT_PTR);
	DECLARE_MESSAGE_MAP()
};

