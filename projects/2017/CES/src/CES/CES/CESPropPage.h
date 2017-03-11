#pragma once

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
};

