#include "stdafx.h"
#include "showbmp.h"


extern CList<FileInfoDetail, FileInfoDetail &> * g_currentDirList;
extern int			g_listCnt, g_currentIndex;
extern int			g_viewWidth, g_viewHeight;
extern CString	g_strCurrentFileName;

//找出currentFileName图片文件所在的目录里包含的所有文件，挑出
//JPG、JPEG、GIF、PNG等可以显示的图片文件并生成链表，供用户由
//鼠标滑轮前后遍历显示图片
void fixCurrentDirList(CString currentFileName) 
{
	if (g_currentDirList != NULL) {
		g_currentDirList->RemoveAll();
		delete g_currentDirList;
	}

	g_currentDirList = new CList<FileInfoDetail, FileInfoDetail &>;

	CString currentFilePathName = CString(currentFileName);
	CString currentPath = currentFilePathName.Left(currentFilePathName.ReverseFind('\\'));
	CString currentTmpFile = CString(currentPath);

	currentPath += CString("\\*.*");
	CFileFind ff;

	BOOL res = ff.FindFile(currentPath);
	while (res) {
		res = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots()) {
			CString strFile = ff.GetFileName();
			CString ext = strFile.Right(strFile.GetLength() - strFile.ReverseFind('.'));
			ext.MakeUpper();
			if (!(ext.Compare(CString(".JPG")) == 0 || 
				  ext.Compare(CString(".JPEG")) == 0 || 
				  ext.Compare(CString(".ICO")) == 0 || 
				  ext.Compare(CString(".GIF")) == 0 || 
				  ext.Compare(CString(".PNG")) == 0))
				continue;
			CString strPath = CString(currentTmpFile);
			strPath += CString("\\");
			strPath += strFile;

			FileInfoDetail detail;
			detail.filePathName = strPath;
			detail.fileName = strFile;

			g_currentDirList->AddTail(detail);
		}
	}

	g_listCnt = g_currentDirList->GetCount();
	g_currentIndex = -1;
	for (int i = 0; i < g_listCnt; i++) {
		POSITION pos = g_currentDirList->FindIndex(i);
		FileInfoDetail currentFile = g_currentDirList->GetAt(pos);
		if (currentFile.filePathName.Compare(g_strCurrentFileName) == 0) {
			g_currentIndex = i;
			return;
		}
	}
}

