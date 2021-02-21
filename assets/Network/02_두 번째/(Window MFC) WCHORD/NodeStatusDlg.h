#pragma once
#include "afxwin.h"
#include "WCHORDDoc.h"

// CNodeStatusDlg ��ȭ �����Դϴ�.

class CNodeStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNodeStatusDlg)

public:
	CNodeStatusDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNodeStatusDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NODESTATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int nodeIndex;
	CString selectedNode;
	struct CWCHORDDoc::nodeInfo curNodes;
	CComboBox m_ctrlSelectNode;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
