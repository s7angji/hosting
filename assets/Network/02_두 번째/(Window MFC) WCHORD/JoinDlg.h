#pragma once


// CJoinDlg ��ȭ �����Դϴ�.

class CJoinDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJoinDlg)

public:
	CJoinDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CJoinDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_JOIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int addr1;
	int addr2;
	int addr3;
	int addr4;
};
