// VideoBox.cpp : 实现文件
//

#include "stdafx.h"
#include "CES.h"
#include "VideoBox.h"


// CVideoBox

IMPLEMENT_DYNAMIC(CVideoBox, CStatic)

CVideoBox::CVideoBox()
{

}

CVideoBox::~CVideoBox()
{
}


BEGIN_MESSAGE_MAP(CVideoBox, CStatic)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CVideoBox 消息处理程序




void CVideoBox::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
		//需要判断一下，是最小化则退出。
		//如果是最小化，恢复的时候会BUG，因为整数除以0
		return;
	}

}
