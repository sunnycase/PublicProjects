// VideoBox.cpp : ʵ���ļ�
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



// CVideoBox ��Ϣ�������


void CVideoBox::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
		//��Ҫ�ж�һ�£�����С�����˳���
		//�������С�����ָ���ʱ���BUG����Ϊ��������0
		return;
	}
	if (_cameraPipeline)
		_cameraPipeline->OnResize(GetSafeHwnd());
}


void CVideoBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	if (_cameraPipeline)
		_cameraPipeline->OnPaint(GetSafeHwnd(), dc.GetSafeHdc());
}

void CVideoBox::OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen)
{
	CStatic::OnDisplayChange(nImageDepth, cxScreen, cyScreen);
	if (_cameraPipeline)
		_cameraPipeline->OnDisplayChange();
}
