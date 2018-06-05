
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ChildView.h"
#include "afxwin.h"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	
	// 对话框数据
	enum { IDD = IDD_ABOUTDIALOG };

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnPaint();

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic birding;
};

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();

protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 操作
public:
	void fixWindowSizeByViewWidthHeight();
	void fitOperation();
	void onFit();

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void showCurrentIndex();

protected:  // 控件条嵌入成员
//	CToolBar				m_wndToolBar;
	CMFCMenuBar		m_wndMenuBar;
	CChildView			m_wndView;

// 生成的消息映射函数
protected:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnSetFocus(CWnd *pOldWnd);
	afx_msg void		OnOpenFile();
	afx_msg void		OnFirst();
	afx_msg void		OnLast();
	afx_msg void		OnFit();
	afx_msg void		OnAppAbout();
	DECLARE_MESSAGE_MAP()
};


