#pragma once


// CVideoBox

class CVideoBox : public CStatic
{
	DECLARE_DYNAMIC(CVideoBox)

public:
	CVideoBox();
	virtual ~CVideoBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


