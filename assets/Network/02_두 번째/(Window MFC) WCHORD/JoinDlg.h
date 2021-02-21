#pragma once


// CJoinDlg 대화 상자입니다.

class CJoinDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJoinDlg)

public:
	CJoinDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CJoinDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_JOIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int addr1;
	int addr2;
	int addr3;
	int addr4;
};
