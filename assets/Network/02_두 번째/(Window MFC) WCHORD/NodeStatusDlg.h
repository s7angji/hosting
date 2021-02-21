#pragma once
#include "afxwin.h"
#include "WCHORDDoc.h"

// CNodeStatusDlg 대화 상자입니다.

class CNodeStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNodeStatusDlg)

public:
	CNodeStatusDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNodeStatusDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NODESTATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int nodeIndex;
	CString selectedNode;
	struct CWCHORDDoc::nodeInfo curNodes;
	CComboBox m_ctrlSelectNode;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
