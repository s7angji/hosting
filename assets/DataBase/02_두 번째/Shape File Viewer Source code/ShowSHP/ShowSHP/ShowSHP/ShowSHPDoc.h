
// ShowSHPDoc.h : CShowSHPDoc Ŭ������ �������̽�
//


#pragma once


class CShowSHPDoc : public CDocument
{
protected:
	FILE *SHPfp, *DBFfp;
	
	typedef struct Dbf_header3{
		unsigned char		version;
		unsigned char		year;
		unsigned char		month;
		unsigned char		day;
		unsigned long		recordn;
		unsigned short int	header_length; //��� ��ü ũ��
		unsigned short int	record_length;
		unsigned char		ver10_reserved[20];
	}DBF_FILEHEADER;

	typedef struct Field_descriptor32{ //32 Byte Field Descriptor
		unsigned char		name[11];
		unsigned char		type;
		unsigned long		field_data_address;
		unsigned char		field_length;
		unsigned char		decimal_count;
		unsigned char		ver10_reserved[14];
	}DBF_FILEDDESCRIPTOR;

	void removeSpace(char *rec, int length);

public:
	BOOL IsReady(){return (SHPfp == NULL)? FALSE : TRUE;};
	bool IsDBFReady(){return (DBFfp == NULL)? FALSE : TRUE;};

	int GetNextPolygon(int **part, CPoint **point); //**����� �迭�� �Ѱ��� �� �̿�
	void Rewind();

	CStringArray listDBF;

	CList<CPoint *, CPoint *> listPoint;
	CList<int *, int *> listPart;
	CList<int , int &> listN;

	int listNum; //������ ���� == ����Ʈ �ڽ��� �� Item ����
	int moveX;
	int moveY;

protected: // serialization������ ��������ϴ�.
	CShowSHPDoc();
	DECLARE_DYNCREATE(CShowSHPDoc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~CShowSHPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
