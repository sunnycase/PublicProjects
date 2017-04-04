#pragma once
#include "CameraPipeline.h"

// CVideoBox

class CVideoBox : public CStatic
{
	DECLARE_DYNAMIC(CVideoBox)

public:
	CVideoBox();
	virtual ~CVideoBox();

	void SetCameraPipeline(CES::CameraPipeline* cameraPipeline);
protected:
	DECLARE_MESSAGE_MAP()
private:
	WRL::ComPtr<CES::CameraPipeline> _cameraPipeline;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen);
};


