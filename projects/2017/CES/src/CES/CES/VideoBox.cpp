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


BEGIN_MESSAGE_MAP(CVideoBox, CStatic)
	ON_WM_SIZE()
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

}
