// WCHORDDoc.cpp : CWCHORDDoc Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "WCHORD.h"
#endif

#include "WCHORDDoc.h"

#include "InitializeDlg.h"
#include "LookupDlg.h"
#include "JoinDlg.h"
#include "LeaveDlg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWCHORDDoc

IMPLEMENT_DYNCREATE(CWCHORDDoc, CDocument)

BEGIN_MESSAGE_MAP(CWCHORDDoc, CDocument)
	ON_COMMAND(ID_STEP3_LOOKUP, &CWCHORDDoc::OnStep3Lookup)
	ON_COMMAND(ID_STEP3_JOIN, &CWCHORDDoc::OnStep3Join)
	ON_COMMAND(ID_STEP3_LEAVE, &CWCHORDDoc::OnStep3Leave)
END_MESSAGE_MAP()


// CWCHORDDoc ����/�Ҹ�

void CWCHORDDoc::initSetting() {
	CInitializeDlg dlg;
	dlg.setNodeNum(twoN/4);
	dlg.setFileNum(twoN/4);
	if(dlg.DoModal() == IDOK){
		initNode = dlg.getNodeNum();
		initFile = dlg.getFileNum();
	}
	else {
		initNode = 1;
		initFile = 1;
		AfxMessageBox("������� ��ҷ� ������ ���� �����մϴ�.\ninitNode = 1\ninitFile = 1");
	}

	printf("*****************************************************************\n");
	printf("* DHT-Based P2P Protocol (CHORD) Functional Simulator*\n");
	printf("* Ver. 0.3 Aug. 22, 2014*\n");
	printf("* (c) Kim, Tae-Hyong*\n");
	printf("*****************************************************************\n\n");

	printf("Note: This CHORD simulator has 3 steps.\n");
	printf("-----------------------------------------------------------------\n");
	printf("STEP 1: Initialization of CHORD Networks\n");
	printf(" ** Node Initialization Mode : Automatic Mode\n");
	printf(" A. Input the number of initial nodes : %d\n", initNode);
	/**////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	while (selNum < initNode) { //����ڰ� ��������ϴ� ����ŭ�� ��带 ����� ����
		sameExist = 0;
		for (i=0; i < 4; i++) /* Random IP address generation */
			randIPsec[i] = rand()%255+1; //unsigned char randIPsec[4];

		//inet_ntoa() �Լ� : ��Ʈ��ũ ����Ʈ ������ 32��Ʈ ���� Dotted-Decimal Notation�� �ּҰ����� ��ȯ
		//*�Լ����� : char *inet_ntoa(struct in_addr addr)
		//*���� �� : ���� �� = ��ȯ�� �ش� ���ڿ��� ������, ���н� = -1

		curIP.S_un.S_addr = randIPsec[3]*256*256*256 + //struct in_addr curIP;
			randIPsec[2]*256*256 + randIPsec[1]*256 + randIPsec[0];
		curIPstr = inet_ntoa(curIP); //char *curIPstr;
		curID = str_hash(curIPstr); //hash fucntion�� IP xxx.xxx.xxx.xxx�� �־� ID���� ���´�.

		for (i = 0; i < curNodes.nodeNum; i++) //���� ���� �����ϰ� ���� ��尡 �浹�ϴ��� �׽�Ʈ�� ���� ������ ���� ����� ������
			if (curID == curNodes.Node[i].ID) {
				sameExist = 1; /* Already Exists */
				break; /* Choose Other */ //�浹 ������ �ٽ� ÷���� while���� ������ ���� !!
			}

			if (sameExist == 0) { //�浹�� ���ٸ� ���� IP�� ID�� ���� �־�����
				curNodes.Node[curNodes.nodeNum].ipaddr = curIP;
				curNodes.Node[curNodes.nodeNum].ID = curID;
				selNum++;
				curNodes.nodeNum++;
				printf(" Generated Node %d: %s (ID:%d)\n", selNum,
					curIPstr, curID);
			}
	}

	/* Variable Re-Initialization */
	selNum = 0; //������ ���� ����ڰ� �Է��� initNode ����ŭ ���������� 0���� �����

	printf(" ** File Initialization Mode : Automatic\n");

	printf(" B. Input the number of initial files : %d\n", initFile);
	/**////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	printf(" * In Automatic Mode, File Name will be random within F0~F%d \n", twoN-1);
	curFiles.fileNum = 0;

	while (selNum < initFile) { //����ڰ� ��������ϴ� ����ŭ�� ������ ����� ����
		sameExist = 0;
		curOwner = curNodes.Node[rand()%curNodes.nodeNum]; //�� ������ ������(���� �����ڴ� ���� ������ ������ �ǹǷ� �浹�� �Ű� �Ⱦ�)
		curName[0] = 'F'; //������ �̸��� F�� �����ϵ��� �����.
		do {
			randfnum = rand()%twoN;
			itoa(randfnum, curName+1, 10); //randfnum�� ���ڸ� ������ �̸� F �ٷ� ���ʿ� ���ڷ� �ٲ� ���� 10������ ǥ���ϰڴ�.
			for (i = 0; i < selNum; i++) //selNum�� ������� ���� ������ ���̰�, �̶����� ���� �����̶� �̸��� ��ġ���� Ȯ���ϰڴ�.
				if (strcmp(curFiles.File[selNum].Name, curName) == 0) { //strcmp()�Լ��� ���ڿ� ��
					sameExist = 1; //�浹���� do while���� Ż���Ҽ��ִ� �����̹Ƿ� �������� �ؾ���
					break; //�Ȱ��� �������� do while�� �ٽ� ���ƶ�
				}
		} while (sameExist);

		curKey = str_hash(curName); //hash �Լ��� ���� �̸��� �־� key�� ����
		for (i = 0; i < curFiles.fileNum; i++) //���ݱ��� ���� ������ ���� �˻��ؼ� �浹�ϴ� key�� ã�Ƴ���
			if (curKey == curFiles.File[i].key) {
				sameExist = 1; /* Already Exists */
				break; /* Choose Other */ //��¿�� ���� �ٽ� ������ '���� ó��' while������ ���ư���
			}

			if (sameExist == 0) { //�浹�� ���ٸ� ���� ������, �����̸�, key �� �־�����
				curFiles.File[selNum].Owner = curOwner;
				strcpy(curFiles.File[selNum].Name, curName);
				curFiles.File[selNum].key = curKey;
				selNum++;
				curFiles.fileNum++;
				printf(" Generated File %d: %s (Key:%d) Owner:%d\n",
					selNum, curName, curKey, curOwner.ID);
			}
	}

	printf("-----------------------------------------------------------------\n");
	printf("STEP 2: Construct CHORD Tables\n");

	printf(" A. Construction of CHORD Table \n");

	//inet_ntoa() �Լ� : ��Ʈ��ũ ����Ʈ ������ 32��Ʈ ���� Dotted-Decimal Notation�� �ּҰ����� ��ȯ
	//*�Լ����� : char *inet_ntoa(struct in_addr addr)
	//*���� �� : ���� �� = ��ȯ�� �ش� ���ڿ��� ������, ���н� = -1

	for (i=0; i<curNodes.nodeNum; i++) { //���� STEP 1���� ������� �ʱ� ������ Join�� ���۵ȴ�
		printf(" * Join Node %d : %s (ID: %d) \n", i+1,
			inet_ntoa(curNodes.Node[i].ipaddr), curNodes.Node[i].ID);

		CHORD[i].Self = curNodes.Node[i];
		initialize(curNodes.Node[i], curNodes.Node[0], i); //������ ���� �����൵ ��������� ���� ���� ���³��� ������ Join���� ��� �����Ƿ� ó�� �� ���� �����ְ��Ѵ�.

		stabilize(curNodes.Node[i]); //���� ���ͼ� ���峭 �κ��� succ�� pre�� ��������

		for (j=0; j<=i; j++) //�ΰŵ��� ��������
			fix_finger(curNodes.Node[j]);

		for (j=0; j<=i; j++) {
			//nID2CPos() : ���� ���� CHORD �迭�� �ִ� �� ���κ��� nodeID�� �迭 ���° �����ִ��� �� ����. �׷��� ���� ���� CHORD �迭�� ������ �˾Ƴ� �� ����!
			printf(" - Finger Table (Node %d): Pre(%d) ",
				curNodes.Node[j].ID,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.Pre.ID); //�� �ڽ��� ��� ID�� pre�� ���
			for (k=0; k<N; k++)
				printf("%d(%d) ", k,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.finger[k].ID); //���� �ΰŵ��� ���
			printf("\n");
		}
	}

	printf(" B. Addition of File Information in CHORD Table \n");
	printf(" B1. File Own Information \n");

	//CHORD ���̺� ���� ���� ���� �߰�
	for (i = 0; i < curFiles.fileNum; i++) {
		//���� STEP 1.���� ����� ���� ���ϵ��� �Ǽ����ڿ��� �־��ִ� ��
		tempID = curFiles.File[i].Owner.ID;
		CHORD[nID2CPos(tempID)].ownFile.File[CHORD[nID2CPos(tempID)].ownFile.fileNum++] = curFiles.File[i]; //++�����ڸ� �ߺ���. ������ ��������鼭 �Ŀ� ������ �־��ٰ� �����Ѵ�. 1Ÿ 2��
		printf(" (%d) Node (ID = %3d) has %5s (key = %3d).\n", i,
			curFiles.File[i].Owner.ID, curFiles.File[i].Name,
			curFiles.File[i].key); //�� ��尡 �� ������ ������ �ִٸ� ���
	}

	printf(" B2. File Reference Information \n");

	for (i=0; i<curFiles.fileNum; i++) {
		//���� STEP 1.���� ����� ���� ���ϵ��� ���۷����� ���� �־���ϴ� ����� Succesor�� ã�� �־��ִ� �� 
		tempID = find_successor(curNodes.Node[0].ID,
			curFiles.File[i].key).ID; //key�� succ ������ ã������. ���⼭ �ƹ� ��带 �־ ã�� �� �� ������ STEP 1.���� ���� ���� ������ ���� ���� �ִ�.
		CHORD[nID2CPos(tempID)].ownFRef.File[CHORD[nID2CPos(tempID)]
		.ownFRef.fileNum++] = curFiles.File[i]; //++�����ڸ� �ߺ���. ������ ��������鼭 �Ŀ� ������ �־��ٰ� �����Ѵ�. 1Ÿ 2��
		printf(" (%d) Node (ID = %3d) has reference info. of %5s(key = %3d).\n", i,
			find_successor(curNodes.Node[0].ID, curFiles.File[i].key).ID,
			curFiles.File[i].Name, curFiles.File[i].key); //� ��尡 � ������ ���۷��� ������ ������ �ִٸ� ���
	}
	printf(" * CHORD Network Setup Completed!\n\n");

	printf("-----------------------------------------------------------------\n");
	printf("STEP 3: Handling User Selected Commands: \n");
}

CWCHORDDoc::CWCHORDDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
	curNodes.nodeNum = 0;
	curFiles.fileNum = 0;
	for(i = 0; i < twoN; i++) {
		CHORD[i].ownFile.fileNum = 0;
		CHORD[i].ownFRef.fileNum = 0;
	}

	nodeTypeClear(&joinNode);
	isNodesFull = FALSE;

	existChooseLeaveNode = FALSE;
	isNodesEmpty = FALSE;

	Found = 0;
	tempNum = 0; //tempID : 'CHORD ���̺� ���� ���� ���� �߰�'���� ������� �ӽú���

	curName[0] = '\0';
	tempStr[0] = '\0';
	tempIPstr[0] = '\0';
	noFile = 0;
	selNum = 0; //selNum�� ���� ���� �ʱ�ȭ���� �̿�, randfnum�� ���� �ʱ�ȭ���� �̿�(randfnum = rand()%twoN)
	i = 0;
	j = 0;
	k = 0;

	USel = 0;
	USel2 = 0;
	USel3 = 0;
	srand(time(NULL)); //srand(10); test�� ���� �켱 ������ ���Ƿ� ��������

	initSetting();
}

CWCHORDDoc::~CWCHORDDoc()
{
}

BOOL CWCHORDDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CWCHORDDoc serialization

void CWCHORDDoc::Serialize(CArchive& ar)
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
void CWCHORDDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CWCHORDDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CWCHORDDoc::SetSearchContent(const CString& value)
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

// CWCHORDDoc ����

#ifdef _DEBUG
void CWCHORDDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWCHORDDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CWCHORDDoc ���

int CWCHORDDoc::twoPow(int power) //2^power
{
	int i;
	int result = 1;

	if(power >= 0)
		for(i=0; i < power; i++)
			result *= 2;
	else
		result -1;

	return result;
}

int CWCHORDDoc::modMinus(int modN, int minuend, int subtrand) //���� ť ���� ���ڰ� ��ȯ�Ҷ��� ����
{
	if((minuend - subtrand) >= 0)
		return minuend - subtrand;
	else
		return (modN - subtrand) + minuend;
}

int CWCHORDDoc::modPlus(int modN, int addend1, int addend2) //circular ����
{
	if((addend1 + addend2) < modN)
		return addend1 + addend2;
	else
		return (addend1 + addend2) - modN;
}

int CWCHORDDoc::modIn(int modN, int targNum, int range1, int range2, int leftmode, int rightmode)
//leftmode, rightmode : 0 => range boundary not included
//                      1 => range boundary included
{
	int result = 0;

	if(range1 == range2){ //������ ũ�Ⱑ 0�̰� ���� �������� �ε�ȣ�� ���� �� targNum�� ������ ������ ���Ѵ�.
		if((leftmode == 0) || (rightmode == 0))
			return 0;
	}

	if(modPlus(twoN, range1, 1) == range2){ //������ ũ�Ⱑ 1������ ���� ������ �ε�ȣ�� ��� targNum�� ������ ������ ���Ѵ�.
		if((leftmode == 0) && (rightmode == 0))
			return 0;
	}

	if(leftmode == 0) //������ �ε�ȣ�� ���ٸ� ������ �ε�ȣ�� ���� �� �ֵ��� ��ĭ ����
		range1 = modPlus(twoN, range1, 1);
	if(rightmode == 0) //�����ʿ� �ε�ȣ�� ���ٸ� �������� �ε�ȣ�� ���� �� �ֵ��� ��ĭ ����
		range2 = modMinus(twoN, range2, 1);

	if(range1 < range2) { //rage1 =< targNum =< rage2 �̴ϱ� �翬�� �����ȿ� �����.
		if((targNum >= range1) && (targNum <= range2))
			result = 1;
	}
	else if(range1 > range2){ //���ڵ��� 0~2^m�����̰� circular�ϰ� ���ư��Ƿ� �� ��쿡�� ������ ���� �Ǵ��ϰ� �����ȿ� ������� �� �� �ִ�.
		if(((targNum >= range1) && (targNum < modN))
			|| ((targNum >= 0) && (targNum <= range2))) //range1 <= targNum < modN, or 0 <= targNum <= range2
			result = 1;
	}

	else if((targNum == range1) && (targNum == range2)) //rage1, rage2 �� ���̰� �����ε� �� ���� Ÿ���̴ϱ� �翬�� ���� ��
		result = 1;

	return result;
}

unsigned CWCHORDDoc::str_hash(char *s) //Simple Hash Function from String to Key
{
	unsigned hashval;
	
	for(hashval = 0; *s != '\0'; s++) //���ڿ� s�� ���̸�ŭ �ݺ�(char�� 1Byte�̹Ƿ� +1�� �����ϰ� ������ ���� '\0'�� ������ ����)
		hashval = *s + 31 * hashval;
		//���� *s�� �����ϴ� ��������(random) ��

	return hashval % twoN;
}

void CWCHORDDoc::printNodes(void)
{
	int i;
	printf("	- Currently there are %d nods: \n", curNodes.nodeNum);
	for (i=0; i<curNodes.nodeNum; i++)
		printf("	(%2d) Node %15s (ID=%3d)\n", i+1,
		inet_ntoa(curNodes.Node[i].ipaddr), curNodes.Node[i].ID);
}

void CWCHORDDoc::printFiles(void)
{
	int i, j, k, refID = -1, found = 0;
	printf("	- Currently there are %d files: \n", curFiles.fileNum);
	for (i=0; i<curFiles.fileNum; i++) { //��� ������ ã�� ���� ��� ���� �� ��ŭ ����.
		for (j=0; j<curNodes.nodeNum; j++) { //�� ������ ���۷����� ���� ��带 ã�� ���� ��� ����� ����ŭ ����.
			found = 0; //�ݺ����� ��������� "ã�� ������ ���۷��� ��带 ã�Ҵ�!"�� checking�ϴ� ����
			for (k=0; k<CHORD[j].ownFRef.fileNum; k++)
				if (curFiles.File[i].key == CHORD[j].ownFRef.File[k].key) //ã�� ������ ���۷����� �˰��ִ� ��带 ã���� ��
				{
					refID = CHORD[j].Self.ID;
					found = 1;
					break;
				}
				if (found) break; //������ ��ġ�� �˾����� ���� ���ϵ� ã�ƾ���? ���� ���Ϸ� ����
		}
		printf("	(%2d) File %3s (Key=%3d) Owner ID: %3d, Ref Owner ID:%3d\n", i+1, curFiles.File[i].Name,
			curFiles.File[i].key, curFiles.File[i].Owner.ID, refID);
	}
}

/////////////////////////////////////////////////////// �����ؾ� �� �Լ��� //////////////////////////////////////////////////////////////////////
int CWCHORDDoc::randomCurID(void)
{
	int i, curID, collision = 1;

	while(collision) { //�浹�� ���� �ٽ� curID�� ����� ����
		curID = rand()%twoN;
		for (i = 0; i < curNodes.nodeNum; i++){
			if (curID == curNodes.Node[i].ID) {
				collision = 1;
				break; //�浹 !!
			}
			else if(i == curNodes.nodeNum - 1) { //������ ������ �浹�� ���� �ʾ����� ������Ѵ�.
				collision = 0;
				break;
			}
		}
	}
	return curID;
}

int CWCHORDDoc::lookup(int nodeID, int fileKey, CWCHORDDoc::nodeType *foundNode, int *refInfo)
{
	int i, Pos;
	int Store = 0;

	Pos = nID2CPos(nodeID); //nodeID��(���� ����) CHORD �迭�� ��� index�� �ִ��� �˾Ƴ���
	if ( Pos < 0) //�迭�� ���� ���̴� ����
		return -1; /* Node Error */

	for (i=0; i<CHORD[Pos].ownFile.fileNum; i++) //�켱 ���� ������ �ִ� ���ϵ��� ���� ���� ������ ������ ���̴ϱ�
		if (fileKey == CHORD[Pos].ownFile.File[i].key)
			return -2; /* Store the File itself */

	for (i=0; i<CHORD[Pos].ownFRef.fileNum; i++) //�� ������ ���ٸ� ������ �� ������ ��ġ���� �� ���ִ� ������ �ִ��� ��������
		if (fileKey == CHORD[Pos].ownFRef.File[i].key) {
			*refInfo = i;
			return -3; /* Store the File Reference itself */
		}
		*foundNode = find_successor(nodeID, fileKey); //fileKey�� �ƴϱ� �� ������ ���۷����� ����� successor�� ã�ƴ޶�� �Լ����� ��Ű��
		return 0; /* Success */

}

void CWCHORDDoc::join(CWCHORDDoc::nodeType joinNode, CWCHORDDoc::nodeType helpNode, int order)
	/* When there is no help node, helpNode = joinNode */
{
	int i;
	CWCHORDDoc::nodeType mySucc_sPre; //mySucc_sPre = my successor's predecessor

	initialize(joinNode, helpNode, order); //�켱 x�� �������� ���� succ�� �˾� ����
	move_keys(joinNode.ID,
		CHORD[nID2CPos(joinNode.ID)].fingerInfo.finger[0].ID); //ã�� ���� succ�� ���� ���� ������ �� key�� �޾� ����
	stabilize(joinNode); //���� succ�� �ٸ����ϰ� succ�� pre�� ��ġ��

	/********************************* �ּ� ó�� ���� *********************************/
	//stabilize�� �ѹ��� �ʿ��� �����
	mySucc_sPre = CHORD[nID2CPos(CHORD[nID2CPos(joinNode.ID)].fingerInfo.finger[0].ID)].fingerInfo.Pre;
	if(mySucc_sPre.ID != joinNode.ID) {
		printf(" �� join : mySucc_sPre.ID != joinNode.ID �� \n");
		AfxMessageBox(" �� join : mySucc_sPre.ID != joinNode.ID �� \n");
		stabilize(mySucc_sPre);
	}
	/**********************************************************************************/

	for (i = 0; i<curNodes.nodeNum; i++) { //��� ������ finger���� ��ġ��
		fix_finger(curNodes.Node[i]);
	}
}

int CWCHORDDoc::nID2CPos(int nodeID)
{
	int i;

	for(i = 0; i < twoN; i++){
		if(nodeID == CHORD[i].Self.ID)
			return i;
	}

	return -1; //error �� ã��
}

CWCHORDDoc::nodeType CWCHORDDoc::find_successor(int curNodeID, int IDKey)
{
	CWCHORDDoc::nodeType errorNode;
	int i, Pos, key_sPre; //key_sPre(key's Predecesssor)

	Pos = nID2CPos(curNodeID);
	if(Pos < 0){
		printf("CHORDInfo �迭�� ���� ��带 ã�� �ֽ��ϴ�.[find_successor()]\n");
		errorNode.ID = -1;
		return errorNode; //error
	}

	if(curNodeID == IDKey)
		return CHORD[Pos].Self;
	else{
		key_sPre = find_predecessor(curNodeID, IDKey);
		return CHORD[nID2CPos(key_sPre)].fingerInfo.finger[0];
	}
}

int CWCHORDDoc::find_predecessor(int curNodeID, int IDKey)
{
	int oldNodeID;

	while(!modIn(twoN, IDKey, curNodeID, CHORD[nID2CPos(curNodeID)].fingerInfo.finger[0].ID, 0, 1))
	{
		oldNodeID = curNodeID;
		curNodeID = find_closest_predecessor(curNodeID, IDKey);
		
		if(oldNodeID == curNodeID) //�� ����? �׷� ���� ������ Predecessor �� �� ���ݾ�
			break;
	}

	return curNodeID;
}

int CWCHORDDoc::find_closest_predecessor(int curNodeID, int IDKey)
{
	int i;
	
	for(i = N-1; i >= 0; i--){
		if(modIn(twoN, CHORD[nID2CPos(curNodeID)].fingerInfo.finger[0].ID, curNodeID, IDKey, 0, 0))
			return CHORD[nID2CPos(curNodeID)].fingerInfo.finger[0].ID;
	}

	return curNodeID;
}

void CWCHORDDoc::initialize(CWCHORDDoc::nodeType joinNode, CWCHORDDoc::nodeType helpNode, int order)
{
	CHORD[order].Self = joinNode;

	//nID2CPos(joinNode.ID)�� �ε����� �˾Ƴ� �� ���� �ֳ��ϸ� ���� CHORD�迭�� ������ �ƴϴ�. �׷��� order�� �ʿ���
	CHORD[order].fingerInfo.Pre.ID = -1;

	if(nID2CPos(helpNode.ID) < 0)
		CHORD[order].fingerInfo.finger[0] = joinNode;
	else
		CHORD[order].fingerInfo.finger[0] = find_successor(helpNode.ID, joinNode.ID);
}

void CWCHORDDoc::move_keys(int toNodeID, int fromNodeID)
{
	int i, j, tempNum;

	if(toNodeID == fromNodeID) { //�� �Լ��� ������ �ʿ䰡 ����.
		return;
	}

	for(i = 0; i < CHORD[nID2CPos(fromNodeID)].ownFRef.fileNum; i++)
	{
		if(modIn(twoN, toNodeID, CHORD[nID2CPos(fromNodeID)].ownFRef.File[i].key, fromNodeID, 1, 1))
		{
			//move(k to node x)
			tempNum = CHORD[nID2CPos(toNodeID)].ownFRef.fileNum++; ////////////////////////////////////////////////////////////
			CHORD[nID2CPos(toNodeID)].ownFRef.File[tempNum] = CHORD[nID2CPos(fromNodeID)].ownFRef.File[i];

			//from node's refinfo delete
			tempNum = --CHORD[nID2CPos(fromNodeID)].ownFRef.fileNum; //////////////////////////////////////////////////////////
			for(j = i; j < tempNum; j++)
				CHORD[nID2CPos(fromNodeID)].ownFRef.File[j] = CHORD[nID2CPos(fromNodeID)].ownFRef.File[j + 1];
			//������ �� ���� CHORD[nID2CPos(fromNodeID)].ownFRef.File[j]�� ���� 0����
			for(;j < fMax; j++) ///////////////////////////////////////////////////////////////////////////////////////////////
				CHORD[nID2CPos(fromNodeID)].ownFRef.File[j].key = 0;
			i--; //for���� fileNum�� ������ �ǵ�����Ƿ� i�� ���󰡾��� ������ ���ϵ� �˻��Ѵ�.
		}
	}
}

void CWCHORDDoc::stabilize(CWCHORDDoc::nodeType Node)
{
	CWCHORDDoc::nodeType mySucc_sPre; //mySucc_sPre = my successor's predecessor
	mySucc_sPre = CHORD[nID2CPos(CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0].ID)].fingerInfo.Pre;

	if(mySucc_sPre.ID < 0){
		printf(" �� stabilize : ���� succ�� Pre ������ �����ϴ�.. !! �� \n");
		notify(Node, CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0]);
		return;
		//CHORD[nID2CPos(Node.ID)].fingerInfo.Pre = Node;
	}
	else if(modIn(twoN, mySucc_sPre.ID, Node.ID, CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0].ID, 0, 0))
		CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0] = mySucc_sPre;
	
	else{
		CHORD[nID2CPos(mySucc_sPre.ID)].fingerInfo.finger[0] = Node;
		notify(mySucc_sPre, Node);
	}

	notify(Node, CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0]);
}

void CWCHORDDoc::notify(CWCHORDDoc::nodeType askNode, CWCHORDDoc::nodeType selfNode)
{
	if(CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID == -1 ///////////////////////////////////////////////
		|| CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID == selfNode.ID
		|| modIn(twoN, askNode.ID, CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID, selfNode.ID, 1, 0)) ///////////////�������� 1�� �غ���.. x == (Pre,N) ���� x�� Pre�̸� ����Լ��� ������ �θ��ϱ�. x�� pre�϶� N�� �ڽ��� succ�� �¾Ƶ� ������ 0�̸� else���� �׾�� �θ� ���̴�.
	{
		CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre = askNode;
	}
	else if(askNode.ID == selfNode.ID) { //ex ��� ��Ȳ�� 22, 4�� ���� [22](Pre4)(22)(22)(22)(22)(22), [4](Pre-1)(22)(22)(22)(22)(22)(22) (BUG(srand(10) : 1, 2, 3, 24, 4, 123, 242, 124)
		printf(" �� notify : askNode == selfNodeID (ID = %d) ���Ƽ� �����մϴ�. (selfNode�� Pre = %d) ��\n",
			selfNode.ID, CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID);
		CHORD[nID2CPos(selfNode.ID)].fingerInfo.finger[0] = CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre;
		notify(selfNode, CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre);
	}
	else { //�� predecessor�� �ʹ��� �����ϹǷ� ���� askNode�� succ�� �� �� ���� �ٽ� stabilize�� �ض� !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		stabilize(askNode);
		printf(" �� notify : ��� %d�� succ�� �ٽ� ã�ƾ� �մϴ�.(succ �ƴ� ���� %d) �� \n", askNode.ID, selfNode.ID);
	}
}

void CWCHORDDoc::fix_finger(CWCHORDDoc::nodeType Node)
{
	int i, j;
	for(i = 1; i < N; i++){ //i = 1���� �����ص� ��
		j = Node.ID + twoPow(i);
		if(j >= twoN)
			j -= twoN;
		CHORD[nID2CPos(Node.ID)].fingerInfo.finger[i] = find_successor(Node.ID, j);
	}
}

void CWCHORDDoc::nodeTypeClear(CWCHORDDoc::nodeType *node) {
	char *zeroIP = "0";

	node->ID = -1;
	inet_aton(zeroIP, &node->ipaddr);
}

void CWCHORDDoc::fileTypeClear(CWCHORDDoc::fileType *file) {
	char *zeroIP = "0";

	file->key = -1;
	file->Name[0] = '\0';
	file->Owner.ID = -1;
	inet_aton(zeroIP, &file->Owner.ipaddr);
}

void CWCHORDDoc::fileTypeCopy(CWCHORDDoc::fileType *copy, CWCHORDDoc::fileType original) {
	int i;
	copy->key = original.key;
	for(i = 0; i < 32; i++) {
		copy->Name[i] = original.Name[i];
	}
	copy->Owner = original.Owner;
}

void CWCHORDDoc::Remove_that_reference_info(int nID2CPosIndex, int leaveNodeID) { //�� ��� �� �ϳ��� �ε����� �޾� ������ �����ڰ� ������ �� �̻� ���� ���� �� reference�� �����.
	int i= 0, j;
	int curNodeID;
	int found;

	curNodeID = CHORD[nID2CPosIndex].Self.ID;

	//CHORD Update
	do{
		found = 0;

		for(; i < CHORD[nID2CPosIndex].ownFRef.fileNum; i++) {
			if(CHORD[nID2CPosIndex].ownFRef.File[i].Owner.ID == leaveNodeID) { //���������� ������ �ε����� ã��
				found = 1;
				break;
			}
		}
		if(i == (CHORD[nID2CPosIndex].ownFRef.fileNum - 1)) { //������ ������ �迭�� ������ �κп� �־����Ƿ� �����ϰ� �����Ѵ�.
			fileTypeClear(&CHORD[nID2CPosIndex].ownFRef.File[i]);
			CHORD[nID2CPosIndex].ownFRef.fileNum--;
		}
		else if(i < (CHORD[nID2CPosIndex].ownFRef.fileNum - 1)) { //������ �����ϸ� �迭�� ������ �κ��� �ƴϹǷ� �������� ���ܿ;��Ѵ�.
			for(j = i; j < (CHORD[nID2CPosIndex].ownFRef.fileNum - 1); j++) {
				CHORD[nID2CPosIndex].ownFRef.File[j] = CHORD[nID2CPosIndex].ownFRef.File[j + 1];
			}
			CHORD[nID2CPosIndex].ownFRef.fileNum--;
		}

		if(found) //CHORD[nID2CPosIndex].ownFRef.fileNum--�� ���������Ƿ� i�� ��������
			i--;
		if(i < (CHORD[nID2CPosIndex].ownFRef.fileNum - 1))
			fileTypeClear(&CHORD[nID2CPosIndex].ownFRef.File[j]); //������ �κ��� �ǹ̾��� ������ ��������. ������� ���Ǹ� ���ؼ�
	}while(i < CHORD[nID2CPosIndex].ownFRef.fileNum);

}

void CWCHORDDoc::Remove_curFiles_Update(int leaveNodeID) //curFiles ���� �����ؾ��� ������ ������Ʈ�Ѵ�.
{
	int i = 0, j;
	int found;

	do{
		found = 0;

		for(; i < curFiles.fileNum; i++) {
			if(curFiles.File[i].Owner.ID == leaveNodeID) {
				found = 1;
				break;
			}
		}
		if(i == (curFiles.fileNum - 1)) { //������ ������ �迭�� ������ �κп� �־����Ƿ� �����ϰ� �����Ѵ�.
			fileTypeClear(&curFiles.File[i]);
			curFiles.fileNum--;
		}
		else if(i < (curFiles.fileNum - 1)) { //������ �����ϸ� �迭�� ������ �κ��� �ƴϹǷ� �������� ���ܿ;��Ѵ�.
			for(j = i; j < (curFiles.fileNum - 1); j++) {
				curFiles.File[j] = curFiles.File[j + 1];
			}
			curFiles.fileNum--;
		}

		if(found) //curFiles.fileNum--�� ���������Ƿ� i�� ��������
			i--;
		if(i < (curFiles.fileNum - 1))
			fileTypeClear(&curFiles.File[j]); //���� �ǹ̾��� �迭�� ������ ������.. ����� �Ҷ� �ް����ϱ�
	}while(i < curFiles.fileNum);
}

void CWCHORDDoc::Remove_leaveNode(int leaveNodeID) //CHORD�迭�� curNodes ���� ���� ��带 �����Ѵ�.
{
	int i, j;

	//CHORD delete
	for(i = 0; i < curNodes.nodeNum; i++) {
		if(CHORD[i].Self.ID == leaveNodeID) {
			break;
		}
	}

	if(i == (curNodes.nodeNum - 1)){ //������ ��尡 �迭�� ������ �κп� �־����Ƿ� �����ϰ� �����Ѵ�.
		CHORD[i].Self.ID = -1;
	}
	else if(i < (curNodes.nodeNum - 1)){ //��带 �����ϸ� �迭�� ������ �κ��� �ƴϹǷ� �������� ���ܿ;��Ѵ�.
		for(j = i; j < (curNodes.nodeNum - 1); j++)
			CHORD[j] = CHORD[j + 1];
	}
	else {
		printf("		������ ��尡 CHORD �迭�� �����ϴ�. Error\n");
		return; //�� �Լ��� �� ���Ƶ� �ǹ̰� ����. ����������
	}
	if(i < (curNodes.nodeNum - 1)) { //������ �κ� ������
		nodeTypeClear(&CHORD[j].Self);
	}
	
	//curNodes delete
	for(i = 0; i < curNodes.nodeNum; i++) {
		if(curNodes.Node[i].ID == leaveNodeID)
			break;
	}
	
	if(i == (curNodes.nodeNum - 1)) { //������ ��尡 �迭�� ������ �κп� �־����Ƿ� �����ϰ� �����Ѵ�.
		curNodes.Node[i].ID = -1;
		curNodes.nodeNum--;
	}
	else if(i < (curNodes.nodeNum - 1)) { //��带 �����ϸ� �迭�� ������ �κ��� �ƴϹǷ� �������� ���ܿ;��Ѵ�.
		for(j = i; j < (curNodes.nodeNum - 1); j++) {
			curNodes.Node[j] = curNodes.Node[j + 1];
		}
		curNodes.nodeNum--;
	}
	else
		printf("		������ ��尡 curNodes �迭�� �����ϴ�. Error\n");
	if(--i < (curNodes.nodeNum - 1)) { //������ �κ� ������, curNodes.nodeNum--;�� ���������Ƿ� i�� ���ҽ�Ű��
		nodeTypeClear(&curNodes.Node[j]);
	}
}

void CWCHORDDoc::Move_All_Keys(int toNodeID, int fromNodeID) //fromNode�� ��� reference�� toNode���� �±��.
{
	int i;
	if(toNodeID == fromNodeID) //���� ��尡 �������Ƿ� �Լ��� �׳� ������
		return;

	toNodeID = nID2CPos(toNodeID);
	fromNodeID = nID2CPos(fromNodeID);
	for(i = 0; i < CHORD[fromNodeID].ownFRef.fileNum ; i++)
		CHORD[toNodeID].ownFRef.File[CHORD[toNodeID].ownFRef.fileNum++] = CHORD[fromNodeID].ownFRef.File[i];
}

//�ؿ� 4���� �Լ��� leave()�� �����Ѵ�.
//void Remove_curFiles_Update(int leaveNodeID); //curFiles ���� �����ؾ��� ������ ������Ʈ�Ѵ�.
//void Remove_that_reference_info(int nID2CPosIndex, int leaveNodeID); //�� ����� �ϳ��� �ε����� �޾� ������ �����ڰ� ������ �� �̻� ���� ���� �� reference�� �����.
//void Remove_leaveNode(int leaveNodeID); //CHORD�迭�� curNodes ���� ���� ��带 �����Ѵ�.
//void Move_All_Keys(int toNodeID, int fromNodeID); //fromNode�� ��� reference�� toNode���� �±��.

void CWCHORDDoc::leave(int leaveNodeID) //���� ������ pre�� Ʋ�� ���� succ Ʋ�� ��尡 ���� ���̴�. ã�ƺ���!
{

	int i, j, k, l;
	CWCHORDDoc::nodeType tmpSucc;
	CWCHORDDoc::nodeType needS; //stabilize�� �ʿ��� ���

	Remove_curFiles_Update(leaveNodeID); //���� �����鼭 ������ �ִ� ���ϵ��� CHORD �󿡼� �������.
	Move_All_Keys(CHORD[nID2CPos(leaveNodeID)].fingerInfo.finger[0].ID, leaveNodeID); //���� succ���� ��� Ű�� ����
	Remove_leaveNode(leaveNodeID); //������ ���� ������.

	if(curNodes.nodeNum == 0) //���� �ִ� ��尡 �����Ƿ� �����ܰ� ������ ������
		return; //�Լ� Ż��

	for(i = 0; i < curNodes.nodeNum; i++) { //��Ʈ��ũ �ȿ� ���� �ִ� �� ������ ��������
		if(CHORD[i].fingerInfo.Pre.ID == leaveNodeID) { //� ����� predecessor�� ���� �����ٸ� null�� ������. ã�Ҵ�!
			nodeTypeClear(&CHORD[i].fingerInfo.Pre);
		}

		for(j = 0; j < N; j++) { //�� ��� finger ���̺��� �Ⱦ� �� ����
			if(CHORD[i].fingerInfo.finger[j].ID == leaveNodeID) { //���� �����Լ� Pong�� ���� �� �� ���̴�. ���� ���� ���� ����Ű�� finger�� ���� ������ ���� finger���� �����ϰ� ���ĺ���.
				if(j == N - 1) //������ finger�� ���� finger�� �����Ƿ� predecessor�� ��ü�Ѵ�.
					CHORD[i].fingerInfo.finger[j] = CHORD[i].fingerInfo.Pre;
				else if((0 < j) && (j < (N - 1))) { //�߰� finger���� ���� finger ������ ����
					CHORD[i].fingerInfo.finger[j] = CHORD[i].fingerInfo.finger[j + 1];
				}
				else if(j == 0){ //(j == 0)//ù��° finger�� succ �� succ�� �߸��� ����. ã�Ҵ�!
					needS = CHORD[i].Self; //stabilize �Լ��� �ʿ��� ��带 ã�Ҵ�
					
					for(k = 0; k < N; k++) { //���� ��带 ����Ű�� �߸��� ��� �ΰŵ��� �����Լ� �ִ� �������� �׳��� ������ succ�� �Ǵ� ���� �������
						tmpSucc = CHORD[i].fingerInfo.finger[k];

						if(tmpSucc.ID != leaveNodeID) //�߸��� ��� �ΰŵ��� �׳��� ���� ������ �ִ� �������� �ӽ� succ�� �� �� �ִ� ��带 ã�Ҵ� break
							break;
					}
					if (k != N) {
						for(l = 0; l < k; l++) {
							CHORD[i].fingerInfo.finger[l] = tmpSucc; //�߸��� �ΰſ� ã�Ƴ��� �ӽ� succ�� ����
						}
					}
					//k == N���� ���Դٸ� finger���� �� �� �߸��� ��.. pre�� ä������!!!
					if(k == N) {
						if(CHORD[i].fingerInfo.Pre.ID == leaveNodeID) {
							printf("		leave() : �ɰ��� Error ���� �κ��� ��� ȥ�� �����Ѵٴ°ǵ� �̹� �� �Լ� �պκп��� �Լ��� ������ ������µ� ����� ������ �ȵ�\n");
							return; //error�� �׳� �Լ� ����
						}
						//���� ����� ����
						//���� CHORD �� 20, 22�� �ִ�.
						//22�� pre�� finger�� ���� �� 20�̴�.
						//20�� ������... pre�� finger ��� ���ٴ� ��! �� ȥ�� �ִٴ°Ŵ�.
						else if(CHORD[i].fingerInfo.Pre.ID == -1) { //���⼭ �߿��Ѱ��� pre�� ���� succ�� ���ٴ� ���̴�. �׷��ٸ� �� ��Ʈ��ũ�󿡴� �� ȥ�� �ִ°��̴�.
							CHORD[i].fingerInfo.Pre.ID = CHORD[i].Self.ID;
							for(l = 0; l < N; l++) //finger���� �� �� pre�� �ٲٴ� �۾�
								CHORD[i].fingerInfo.finger[l] = CHORD[i].fingerInfo.Pre;
						}
						//���� ����� ����
						//���� CHORD �� 5, 6, 25, 28�� �ִ�.
						//6�� pre�� 5�̰�, finger�� ���� �� 25�̴�.
						//25�� ������... finger�� ��� ���� ���ٴ� ��! ���� ���� �����߿� Pre�� ����Ѵٴ� �Ҹ�.
						else {
							for(l = 0; l < N; l++) //finger���� �� �� pre�� �ٲٴ� �۾�
								CHORD[i].fingerInfo.finger[l] = CHORD[i].fingerInfo.Pre;
						}
					}
				}
				else
					printf("		Leave() : Error \n\n");
			}
		}
		//if(M has reference info of the file owned by N)
		//N�� �����鼭 �ʿ� ������ ������ û������.
		//	Remove_that_reference_info
		Remove_that_reference_info(i, leaveNodeID);
	}

	stabilize(needS); //succ�� �߸��� ��常 stabilize ���ָ� ���� �ذ�ȴ�.

	for (i = 0; i<curNodes.nodeNum; i++) { //��� ������ finger���� ��ġ��
		fix_finger(curNodes.Node[i]);
	}
}

//http://blog.naver.com/jisangryul7/140020071124 ����
int CWCHORDDoc::inet_aton(const char *cp, struct in_addr *ap)
{
	int dots = 0;
	register u_long acc = 0, addr = 0;

	do{
		register char cc = *cp;
		switch(cc) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			acc = acc * 10 + (cc - '0');
			break;
		case '.':
			if(++dots > 3)
				return 0;

		/*Fall through*/
		case '\0':
			if(acc > 255)
				return 0;
			addr = addr << 8 | acc;
			acc = 0;
			break;
		default:
			return 0;
		}
	}while(*cp++);
	/*Store it if requested*/
	if(ap)
		ap->s_addr = htonl(addr);
	return 1;
}

void CWCHORDDoc::OnStep3Lookup()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if(curFiles.fileNum == 0) {
		printf("			���� ã�� ������ �����ϴ�!\n");
		AfxMessageBox("���� ã�� ������ �����ϴ�!");
		return;
	}

	//����ڿ��Լ� �˻��� ���, �˻��� ������ ������
	CLookupDlg dlg;
	
	dlg.curNodes = curNodes;
	dlg.curFiles = curFiles;

	CString str;
	int nSelected;

	int nodeIndex;
	int keyIndex;

	if(dlg.DoModal() == IDOK) {
		nodeIndex = dlg.nodeIndex;
		keyIndex = dlg.keyIndex;

		printf("\n      (1) File Search\n");
		printNodes();
		printFiles();
		printf("       - Search file %s (Key=%d) at Node %15s (ID=%d)\n", curFiles.File[keyIndex].Name, curFiles.File[keyIndex].key, inet_ntoa(curNodes.Node[nodeIndex].ipaddr), curNodes.Node[nodeIndex].ID);
		lookupResult = lookup(curNodes.Node[nodeIndex].ID, curFiles.File[keyIndex].key, &foundNodeLU, &refInfoLU);

		if(lookupResult == -1)
			printf("       - Node index out of bounds\n");
		else if(lookupResult == -2)
			printf("       - The file %s is stored at Node ID %d itself!\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
		else if(lookupResult == -3) {
			printf("       - Reference info of file %s is at Node ID %d itself!\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
			printf("       - Actual file %s is stored at Node %15s (ID=%d)\n", curFiles.File[keyIndex].Name, inet_ntoa(CHORD[nID2CPos(curNodes.Node[nodeIndex].ID)].ownFRef.File[refInfoLU].Owner.ipaddr), CHORD[nID2CPos(curNodes.Node[nodeIndex].ID)].ownFRef.File[refInfoLU].Owner.ID);
		}
		else { //lookupResult == 0

			printf("       - The file %s is not stored at Node ID %d.\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
			printf("       - Reference info of file %s is not at Node ID %d.\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
			printf("       - The Successor of Key %d : Node ID %d.\n", curFiles.File[keyIndex].key, foundNodeLU.ID);
			printf("       - Node ID %d has the reference of the file %s.\n", foundNodeLU.ID, curFiles.File[keyIndex].Name);

			//printf("       - The actual file %s is stored at Node %15s (ID=%d).\n", curFiles.File[keyIndex].Name, inet_ntoa(curFiles.File[keyIndex].Owner.ipaddr), curFiles.File[keyIndex].Owner.ID);
			//���� �ٷ� ���� printf() �ּ�ó�� �� ���� �̿��ϸ� ���������� ����� ������ �����ڸ� �ٷ� ã�� �� �� ������.. �̷�������
			//�˰��� ������ �̿��Ͽ� �˰����� ã���� ��带 �湮�ؼ� �Ѿ� ������ ������ ����
			for(i = 0; i < CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.fileNum; i++) {
				if(curFiles.File[keyIndex].key == CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.File[i].key)
					break;
			}
			printf("       - The actual file %s is stored at Node %15s (ID=%d).\n\n\n", curFiles.File[keyIndex].Name, inet_ntoa(CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.File[i].Owner.ipaddr), CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.File[i].Owner.ID);
		}

		AfxMessageBox("����� �ܼ�â�� !!");
	}
	else {
		printf("����ڰ� ����߽��ϴ�.\n");
		return;
	}
}

void CWCHORDDoc::OnStep3Join()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if(curNodes.nodeNum == twoN)
		isNodesFull = TRUE;
	else
		isNodesFull = FALSE;

	if(curNodes.nodeNum == 0)
		isNodesEmpty = TRUE;
	else
		isNodesEmpty = FALSE;


	if(curNodes.nodeNum == twoN) {
		printf("			�� �̻� ��带 ��ġ�� ������ �����ϴ�!\n");
		AfxMessageBox("�� �̻� ��带 ��ġ�� ������ �����ϴ�!\n");
		isNodesFull = TRUE;
		isNodesEmpty = FALSE;
		return;
	}
	//����ڿ��� Join�� ����� IP Address�� �޾ƿ´�.
	CJoinDlg dlg;
	CString str;
	do {
		sameExist = 0;

		if(dlg.DoModal() == IDOK) {
			printf("      (2) Node Join\n");
			printNodes();

			str.Format("%d.%d.%d.%d", dlg.addr1, dlg.addr2, dlg.addr3, dlg.addr4);
			char *addrBuf = (LPSTR)(LPCTSTR)str; //join���� ����ڿ��� ���� IP address �ӽ� ����

			printf("      2.1 Enter the IP address of the joining node : %s\n", addrBuf);

			inet_aton(addrBuf, &joinNode.ipaddr);
			curID = str_hash(inet_ntoa(joinNode.ipaddr)); //hash fucntion�� ID���� ���´�.

			for (i = 0; i < curNodes.nodeNum; i++){ //�̹� �ִ� ������ IP �ߺ� �˻�
				if(joinNode.ipaddr.S_un.S_addr == curNodes.Node[i].ipaddr.S_un.S_addr){
					printf("        �Է��� IP�� �̹� �����մϴ�.\n        �ٽ� �ѹ� joining Node�� IP address�� �Է����ּ���.\n");
					sameExist = 1;
					break;
					//����ڰ� �Ǽ��� ���� IP�� �Է��ߴ�. �ٽ� ÷���� ���ư���.
				}
			}

			if(sameExist != 1) {
				for (i = 0; i < curNodes.nodeNum; i++) { //�̹� �ִ� ������ ID�� �ߺ��Ǵ��� �˻�
					if (curID == curNodes.Node[i].ID) {
						printf("        �Է��� IP�� �ؽ��Լ��� ����� ID�� �̹� ���� �մϴ�.\n");
						printf("        ���Ƿ� ID�� ���ϰڽ��ϴ�.\n");
						curID = randomCurID();
						break;
					}
				}
			}
		}
		else {//IDCANCEL
			printf("����ڰ� ����߽��ϴ�.\n");
			return;
		}
	}while(sameExist);

	//���������� Join�� ����
	if(!isNodesFull) { //������ ��� �� ������ �� ���� �ʾ����� ���� ��带 ���� �� �ִ�.
		//ID�� ���� �־�����
		joinNode.ID = curID;

		//���� joinNode�� curNodes�� CHORD �迭�� ���Խ�Ŵ
		curNodes.Node[curNodes.nodeNum] = joinNode;
		CHORD[curNodes.nodeNum].Self = joinNode;
		curNodes.nodeNum++;

		printf("        - Node ID of %15s is %d\n", inet_ntoa(joinNode.ipaddr), joinNode.ID);
		printf("      2.2 CHORD Network Connection Update\n");
		printNodes();

		join(joinNode, curNodes.Node[0], nID2CPos(joinNode.ID));

		for (j=0; j<curNodes.nodeNum; j++) {
			//nID2CPos() : ���� ���� CHORD �迭�� �ִ� �� ���κ��� nodeID�� �迭 ���° �����ִ��� �� ����. �׷��� ���� ���� CHORD �迭�� ������ �˾Ƴ� �� ����!
			printf("        - Finger Table (Node %d): Pre(%d) ",
				curNodes.Node[j].ID,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.Pre.ID); //�� �ڽ��� ��� ID�� pre�� ���
			for (k=0; k<N; k++)
				printf("%d(%d) ", k,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.finger[k].ID); //���� �ΰŵ��� ���
			printf("\n");
		}

		printFiles();
		printf("Join�Ϸ� !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		UpdateAllViews(NULL);
	}
}

void CWCHORDDoc::OnStep3Leave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if(curNodes.nodeNum == twoN)
		isNodesFull = TRUE;
	else
		isNodesFull = FALSE;

	if(curNodes.nodeNum == 0)
		isNodesEmpty = TRUE;
	else
		isNodesEmpty = FALSE;


	if(curNodes.nodeNum == 0) {
		printf("			�� �̻� ���� ��尡 �������� �ʽ��ϴ�!\n");
		AfxMessageBox("�� �̻� ���� ��尡 �������� �ʽ��ϴ�!\n");
		isNodesFull = FALSE;
		isNodesEmpty = TRUE;
		return;
	}

	CLeaveDlg dlg;

	dlg.curNodes = curNodes;
	if(dlg.DoModal() == IDOK) {

		printNodes();
		printFiles();
		leaveNodeID = _ttoi(dlg.selectedNode);
		printf("      3.1 Enter the node ID of the leaving node : %d\n", leaveNodeID);
	}
	else {//IDCANCEL
		printf("����ڰ� ����߽��ϴ�.\n");
		return;
	}

	if(!isNodesEmpty) {
		printf("      3.2 CHORD Network Connection Update\n");

		leave(leaveNodeID); //��¥ leave�� �ٽ����� �κ� !!!!!!!!!!!!!!!!!!!!!

		printNodes();

		for (j=0; j<curNodes.nodeNum; j++) {
			//nID2CPos() : ���� ���� CHORD �迭�� �ִ� �� ���κ��� nodeID�� �迭 ���° �����ִ��� �� ����. �׷��� ���� ���� CHORD �迭�� ������ �˾Ƴ� �� ����!
			printf("        - Finger Table (Node %d): Pre(%d) ",
				curNodes.Node[j].ID,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.Pre.ID); //�� �ڽ��� ��� ID�� pre�� ���
			for (k=0; k<N; k++)
				printf("%d(%d) ", k,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.finger[k].ID); //���� �ΰŵ��� ���
			printf("\n");
		}

		printFiles();
		printf("Leave�Ϸ� !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		UpdateAllViews(NULL);
	}
}

int CWCHORDDoc::refOwnerID(int key)
{
	int j, k, refID = -1, found = 0;

	for (j=0; j<curNodes.nodeNum; j++) { //�� ������ ���۷����� ���� ��带 ã�� ���� ��� ����� ����ŭ ����.
		found = 0; //�ݺ����� ��������� "ã�� ������ ���۷��� ��带 ã�Ҵ�!"�� checking�ϴ� ����
		for (k=0; k<CHORD[j].ownFRef.fileNum; k++)
			if (key == CHORD[j].ownFRef.File[k].key) //ã�� ������ ���۷����� �˰��ִ� ��带 ã���� ��
			{
				refID = CHORD[j].Self.ID;
				found = 1;
				break;
			}
			if (found) break;
	}

	return refID;
}