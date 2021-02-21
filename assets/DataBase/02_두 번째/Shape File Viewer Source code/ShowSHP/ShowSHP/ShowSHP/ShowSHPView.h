
// ShowSHPView.h : CShowSHPView Ŭ������ �������̽�
//

#pragma once


class CShowSHPView : public CView
{
protected:
	int curSel;

protected: // serialization������ ��������ϴ�.
	CShowSHPView();
	DECLARE_DYNCREATE(CShowSHPView)

// Ư���Դϴ�.
public:
	CShowSHPDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CShowSHPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSorry();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // ShowSHPView.cpp�� ����� ����
inline CShowSHPDoc* CShowSHPView::GetDocument() const
   { return reinterpret_cast<CShowSHPDoc*>(m_pDocument); }
#endif

