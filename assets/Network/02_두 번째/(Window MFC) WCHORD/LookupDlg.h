#pragma once
#include "afxwin.h"

#include "WCHORDDoc.h"

// CLookupDlg 대화 상자입니다.

class CLookupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLookupDlg)

public:
	CLookupDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLookupDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOOKUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//Document에서 현재 노드와 파일의 상황을 넘겨 받을 변수
	struct CWCHORDDoc::nodeInfo curNodes;
	struct CWCHORDDoc::fileInfo curFiles;

	//사용자가 찾아달라고 선택한 노드와 파일
	CString selectedNode;
	CString selectedFile;
	int nodeIndex;
	int keyIndex;

	// 사용자가 선택할 node
	CComboBox m_ctrlSearchNode;
	// 사용자가 선택할 file
	CComboBox m_ctrlSearchFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo2();
};
