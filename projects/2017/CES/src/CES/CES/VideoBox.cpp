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

void CVideoBox::SetCameraPipeline(CES::CameraPipeline * cameraPipeline)
{
	_cameraPipeline = cameraPipeline;
}


BEGIN_MESSAGE_MAP(CVideoBox, CStatic)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DISPLAYCHANGE()
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
	if (_cameraPipeline)
		_cameraPipeline->OnResize(GetSafeHwnd());
}


void CVideoBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	if (_cameraPipeline)
		_cameraPipeline->OnPaint(GetSafeHwnd(), dc.GetSafeHdc());
}

void CVideoBox::OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen)
{
	CStatic::OnDisplayChange(nImageDepth, cxScreen, cyScreen);
	if (_cameraPipeline)
		_cameraPipeline->OnDisplayChange();
}
