#pragma once
#include "afxwin.h"
#include <unordered_map>

// CESPropPage.h : CCESPropPage 属性页类的声明。


// CCESPropPage : 有关实现的信息，请参阅 CESPropPage.cpp。

class CCESPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CCESPropPage)
	DECLARE_OLECREATE_EX(CCESPropPage)

// 构造函数
public:
	CCESPropPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_CES };

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 消息映射
protected:
	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_cbPrimCam;
	CComboBox m_cbSecndCam;

	std::unordered_map<int, CString> _indexToSymbolicLink;
	std::unordered_map<std::wstring, int> _symbolicToIndexLink;
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
private:
	int m_selectedPrimCamIndex;
	int m_selectedSecndCamIndex;
};

