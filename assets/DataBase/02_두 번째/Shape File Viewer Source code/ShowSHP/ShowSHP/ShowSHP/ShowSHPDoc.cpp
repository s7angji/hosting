
// ShowSHPDoc.cpp : CShowSHPDoc Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "ShowSHP.h"
#endif

#include "ShowSHPDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CShowSHPDoc

IMPLEMENT_DYNCREATE(CShowSHPDoc, CDocument)

BEGIN_MESSAGE_MAP(CShowSHPDoc, CDocument)
END_MESSAGE_MAP()


// CShowSHPDoc ����/�Ҹ�

CShowSHPDoc::CShowSHPDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
	SHPfp = NULL;
	DBFfp = NULL;

	listNum = 0;

	moveX = 0;
	moveY = 0;
}

CShowSHPDoc::~CShowSHPDoc()
{
}

BOOL CShowSHPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.
	if(SHPfp != NULL){
		fclose(SHPfp);
		SHPfp = NULL;

		listN.RemoveAll();
		listPart.RemoveAll();
		listPoint.RemoveAll();
	}
	if(DBFfp != NULL){
		fclose(DBFfp);
		DBFfp = NULL;
	}
	listNum = 0;

	moveX = 0;
	moveY = 0;
	return TRUE;
}




// CShowSHPDoc serialization

void CShowSHPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}

#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void CShowSHPDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// �˻� ó���⸦ �����մϴ�.
void CShowSHPDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CShowSHPDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CShowSHPDoc ����

#ifdef _DEBUG
void CShowSHPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CShowSHPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CShowSHPDoc ���


void CShowSHPDoc::OnCloseDocument()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(SHPfp != NULL){
		fclose(SHPfp);
		SHPfp = NULL;
	}
	if(DBFfp != NULL){
		fclose(DBFfp);
		DBFfp = NULL;
	}
	CDocument::OnCloseDocument();
}


BOOL CShowSHPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	//���� SHP ���� ��θ� �̿��Ͽ� DBF ���ϵ� ���� ���� �ֱ�
	CString m_strDBFPath = lpszPathName;
	int tmp = m_strDBFPath.GetLength() - 4;
	m_strDBFPath = m_strDBFPath.Mid(0, tmp) + ".dbf";
	AfxMessageBox(m_strDBFPath);

	DBFfp = fopen(m_strDBFPath, "rb");
	SHPfp = fopen(lpszPathName, "rb");

	if(DBFfp == NULL){
		::AfxMessageBox("Error : File Open");
		return FALSE;
	}
	else if(DBFfp != NULL){
		::AfxMessageBox("DBF File Open Success");
	}

	if(SHPfp == NULL){
		::AfxMessageBox("Error : File Open");
		return FALSE;
	}

	//Shape Type
	int shapeType;
	fseek(SHPfp, 32L, SEEK_SET);
	fread(&shapeType, sizeof(int), 1, SHPfp); //2�� Endian ����

	if(shapeType != 5){
		::AfxMessageBox("Error : File Type is not Polygon");
		return FALSE;
	}

	//ȭ�� �������� ��� ��
	double xmin, ymin, xmax, ymax;
	fread(&xmin, sizeof(int), 1, SHPfp);
	fread(&ymin, sizeof(int), 1, SHPfp);
	fread(&xmax, sizeof(int), 1, SHPfp);
	fread(&ymax, sizeof(int), 1, SHPfp);

	//��� ���� �ǳ� �ٱ�
	fseek(SHPfp, 100L, SEEK_SET);

	//SHP���� �޸𸮿� �����ϱ�
	int *part = NULL;
	CPoint *point = NULL;

	while(int n = GetNextPolygon(&part, &point)){
		listN.AddTail(n);
		listPart.AddTail(part);
		listPoint.AddTail(point);
	}

	//DBF Type
	char dbfType;

	char record_tag;
	char *record;

		fread(&dbfType, 1, 1, DBFfp);

	if(dbfType != 3){
		fclose(DBFfp);
		::AfxMessageBox("���� ���� �ʴ� DBF File Version");
		return FALSE;
	}

	//DBF ���� ��� �о����
	DBF_FILEHEADER fHeader;
	fseek(DBFfp, 0, SEEK_SET);
	fread(&fHeader, sizeof(DBF_FILEHEADER), 1, DBFfp);

	//�ʵ��� ���� ���ϱ�
	int nfield = (fHeader.header_length - sizeof(DBF_FILEHEADER))
					/ sizeof(DBF_FILEDDESCRIPTOR);

	//�ʵ��� ������ŭ Field descriptor �����ϱ�
	DBF_FILEDDESCRIPTOR* pField = new DBF_FILEDDESCRIPTOR[nfield];
	//�ʵ� ���� �о��
	fread(pField, sizeof(DBF_FILEDDESCRIPTOR) * nfield, 1, DBFfp);

	//File Header�� Field Descriptor, 0x0D �ǳʶٱ�
	int offset = fHeader.header_length;
	fseek(DBFfp, offset, SEEK_SET);

	//Dialog�� ListBox�ȿ� �� ������ listDBF�� size ����
	listNum = fHeader.recordn;
	listDBF.SetSize(listNum);

	for(int j = 0; j < fHeader.recordn; j++){
		fseek(DBFfp, 1, SEEK_CUR); //ù ���� ����Ʈ �ǳʶٱ�
		for(int i = 0; i < nfield; i++){
			record = new char[pField[i].field_length + 1];
			fread(record, pField[i].field_length, 1, DBFfp);
			record[pField[i].field_length] = '\0'; //NULL ���� �߰� ù ���� ����Ʈ �ǳʶٱ�
			removeSpace(record, pField[i].field_length); //��������
			listDBF[j] += record;
			listDBF[j] += " , ";
			delete[] record;
		}
	}

	return TRUE;
}


int CShowSHPDoc::GetNextPolygon(int **part, CPoint **point)
{
	int numParts;
	int numPoints;

	// ����� �ǳ� ��
	fseek(SHPfp, sizeof(int)*2, SEEK_CUR);

	//shapeŸ�԰� box�� �ǳ� �� 14page
	fseek(SHPfp, sizeof(int)*1 + sizeof(double)*4, SEEK_CUR);

	//NumParts �� �б�
	fread(&numParts, sizeof(int), 1, SHPfp);

	//numPoints �� �б�
	if(fread(&numPoints, sizeof(int), 1, SHPfp) == 0)
		return 0; //view�� while��

	*part = new int[numParts];
	fread(*part, sizeof(int), numParts, SHPfp);

	double *pts =new double[numPoints *2];
	fread(pts, sizeof(double), numPoints* 2, SHPfp);

	*point = new CPoint[numPoints]; //long double ����

	for(int i= 0; i<numPoints; i++){
		//���⼭ ������ ������ ũ�� ����
		(*point)[i].x=(long)((pts[i*2] +180 + moveX) *3.5f); //
		(*point)[i].y= (long)((90 -pts[i*2 +1] + moveY) *3.5f); //
	}

	for(int i= 0; i< numParts -1; i++) //���� , ���� [0,5] ,[5,5]
		(*part)[i]=(*part)[i+1] - (*part)[i];
	(*part)[numParts -1] = numPoints -(*part)[numParts -1];

	delete[] pts;

	return numParts;
}

void CShowSHPDoc::Rewind()
{
	fseek(SHPfp, 100L, SEEK_SET); //���� ��� �ǳ� �ٱ�
}


void CShowSHPDoc::removeSpace(char *rec, int length){
	for(short int i = length - 1; i > 0; i--){
		if(rec[i] == ' ')
			rec[i] = 0;
		else
			break;
	}
}