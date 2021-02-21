#pragma once


// CInitializeDlg 대화 상자입니다.

class CInitializeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInitializeDlg)

public:
	CInitializeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInitializeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INITIALIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	// 사용자에게서 초기에 생성할 node의 갯수를 받아 온 것
	int m_nodeNum;
	// 사용자에게서 초기에 생성할 file의 갯수를 받아 온 것
	int m_fileNum;
public:
	int getNodeNum() { return m_nodeNum; };
	int getFileNum() { return m_fileNum; };
	void setNodeNum(int nodeNum) { m_nodeNum = nodeNum; };
	void setFileNum(int fileNum) { m_fileNum = fileNum; };
};
