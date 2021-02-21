#pragma once
#include "afxwin.h"
#include "WCHORDDoc.h"

// CFileStatusDlg ��ȭ �����Դϴ�.

class CFileStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileStatusDlg)

public:
	CFileStatusDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFileStatusDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FILESTATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int keyIndex;
	CString selectedFile;
	struct CWCHORDDoc::fileInfo curFiles;
	CComboBox m_ctrlSelectFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
