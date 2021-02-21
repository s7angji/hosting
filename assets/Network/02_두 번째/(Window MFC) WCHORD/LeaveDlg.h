#pragma once
#include "afxwin.h"
#include "WCHORDDoc.h"

// CLeaveDlg 대화 상자입니다.

class CLeaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLeaveDlg)

public:
	CLeaveDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLeaveDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LEAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	struct CWCHORDDoc::nodeInfo curNodes;
	CString selectedNode;
	int nodeIndex;

	CComboBox m_ctrlSelectNode;
	afx_msg void OnCbnSelchangeCombo1();
	virtual BOOL OnInitDialog();
};
