
// showbmp.h : showbmp Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

class FileInfoDetail
{
public:

	CString filePathName;
	CString fileName;

	int		filePicWidth;
	int		filePicHeight;
	CString fileCurrentPicType;

public:
	FileInfoDetail() { 
		fileCurrentPicType = CString("JPG"); 
	}
};

// CshowbmpApp:
// �йش����ʵ�֣������ showbmp.cpp
//

class CshowbmpApp : public CWinApp
{
public:
	CshowbmpApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CshowbmpApp theApp;
