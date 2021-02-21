#pragma once
#include "afxwin.h"


// CListboxDlg 대화 상자입니다.

class CListboxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListboxDlg)

public:
	CListboxDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CListboxDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
