
// showbmp.h : showbmp 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

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
// 有关此类的实现，请参阅 showbmp.cpp
//

class CshowbmpApp : public CWinApp
{
public:
	CshowbmpApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CshowbmpApp theApp;
