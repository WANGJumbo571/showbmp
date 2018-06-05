
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once
#include "ChildView.h"
#include "afxwin.h"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	
	// �Ի�������
	enum { IDD = IDD_ABOUTDIALOG };

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnPaint();

	// ʵ��
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

// ����
public:
	void fixWindowSizeByViewWidthHeight();
	void fitOperation();
	void onFit();

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void showCurrentIndex();

protected:  // �ؼ���Ƕ���Ա
//	CToolBar				m_wndToolBar;
	CMFCMenuBar		m_wndMenuBar;
	CChildView			m_wndView;

// ���ɵ���Ϣӳ�亯��
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


