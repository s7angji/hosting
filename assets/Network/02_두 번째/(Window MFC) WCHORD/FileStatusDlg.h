#pragma once
#include "afxwin.h"
#include "WCHORDDoc.h"

// CFileStatusDlg 대화 상자입니다.

class CFileStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileStatusDlg)

public:
	CFileStatusDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFileStatusDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILESTATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int keyIndex;
	CString selectedFile;
	struct CWCHORDDoc::fileInfo curFiles;
	CComboBox m_ctrlSelectFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
