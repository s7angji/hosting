#pragma once
#include "afxwin.h"

#include "WCHORDDoc.h"

// CLookupDlg ��ȭ �����Դϴ�.

class CLookupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLookupDlg)

public:
	CLookupDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLookupDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOOKUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	//Document���� ���� ���� ������ ��Ȳ�� �Ѱ� ���� ����
	struct CWCHORDDoc::nodeInfo curNodes;
	struct CWCHORDDoc::fileInfo curFiles;

	//����ڰ� ã�ƴ޶�� ������ ���� ����
	CString selectedNode;
	CString selectedFile;
	int nodeIndex;
	int keyIndex;

	// ����ڰ� ������ node
	CComboBox m_ctrlSearchNode;
	// ����ڰ� ������ file
	CComboBox m_ctrlSearchFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo2();
};
