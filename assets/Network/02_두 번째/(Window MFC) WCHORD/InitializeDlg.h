#pragma once


// CInitializeDlg ��ȭ �����Դϴ�.

class CInitializeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInitializeDlg)

public:
	CInitializeDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CInitializeDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_INITIALIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
protected:
	// ����ڿ��Լ� �ʱ⿡ ������ node�� ������ �޾� �� ��
	int m_nodeNum;
	// ����ڿ��Լ� �ʱ⿡ ������ file�� ������ �޾� �� ��
	int m_fileNum;
public:
	int getNodeNum() { return m_nodeNum; };
	int getFileNum() { return m_fileNum; };
	void setNodeNum(int nodeNum) { m_nodeNum = nodeNum; };
	void setFileNum(int fileNum) { m_fileNum = fileNum; };
};
