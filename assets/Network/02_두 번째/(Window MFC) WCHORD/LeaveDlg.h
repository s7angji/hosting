#pragma once
#include "afxwin.h"
#include "WCHORDDoc.h"

// CLeaveDlg ��ȭ �����Դϴ�.

class CLeaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLeaveDlg)

public:
	CLeaveDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLeaveDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LEAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	struct CWCHORDDoc::nodeInfo curNodes;
	CString selectedNode;
	int nodeIndex;

	CComboBox m_ctrlSelectNode;
	afx_msg void OnCbnSelchangeCombo1();
	virtual BOOL OnInitDialog();
};
