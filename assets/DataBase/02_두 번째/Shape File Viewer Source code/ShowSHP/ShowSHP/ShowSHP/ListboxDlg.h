#pragma once
#include "afxwin.h"


// CListboxDlg ��ȭ �����Դϴ�.

class CListboxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListboxDlg)

public:
	CListboxDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CListboxDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox ctrlList;
	CString strList;
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnDblclkList1();

	CStringArray listDBF;
	int curSel;
	int listNum;
};
