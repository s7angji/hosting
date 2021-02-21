
// WCHORDDoc.h : CWCHORDDoc Ŭ������ �������̽�
//

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<WinSock2.h>
#include<string.h>

#pragma once


#pragma comment(lib, "ws2_32.lib") //error LNK2019: __imp__inet_ntoa@4 �ذ�

#define N 5 //Space Base
#define twoN 32 //Space Size
#define fMax 10 //Max number of Files at each node

class CWCHORDDoc : public CDocument
{
protected:
	void initSetting();

public:
	/********** ����ü ���� **********/
	struct nodestru{ //Node Structure
		int ID; //Node's ID
		struct in_addr ipaddr; //Node's IPv4 Adress
	};

	typedef struct nodestru nodeType;

	struct filestru{ //File Structure
		nodeType Owner; //Node having the actual file
		char Name[32]; //File name
		int key; //File key
	};

	typedef struct filestru fileType;

	struct fileInfo{ //Global Information of Current Files
		int fileNum; //The Number of Current Files
		fileType File[fMax]; //Information of Each File
	};

	struct fingertable{ //Finger Table
		nodeType Pre; //Predecessor
		nodeType finger[N]; //Finger(Successor)
	};

	struct CHORDInfo{ //CHORD Infomation
		nodeType Self; //Node itself
		struct fingertable fingerInfo; //Finger Table
		struct fileInfo ownFile; //File Own Information
		struct fileInfo ownFRef; //File Ref Own Information
	};

	struct nodeInfo{ //Global Information of Current Nodes
		int nodeNum; //The Number of Current Nodes
		nodeType Node[twoN]; //Information of Each Node
	};
	/*********************************/

private:
	struct nodeInfo curNodes; //For handling the current nodes
	struct fileInfo curFiles; //For handling the current files
	struct CHORDInfo CHORD[twoN]; //Main CHORD database

	/********** �Լ� ������Ÿ�� **********/
	//http://blog.naver.com/jisangryul7/140020071124 ����
	int inet_aton(const char *cp, struct in_addr *ap);

	void fileTypeCopy(fileType *copy, fileType original);
	void fileTypeClear(fileType *file);
	void nodeTypeClear(nodeType *node);

	//leave()���� ����� �Լ� 4����
	void Remove_curFiles_Update(int leaveNodeID); //curFiles ���� �����ؾ��� ������ ������Ʈ�Ѵ�.
	void Remove_that_reference_info(int nID2CPosIndex, int leaveNodeID); //�� ����� �ϳ��� �ε����� �޾� ������ �����ڰ� ������ �� �̻� ���� ���� �� reference�� �����.
	void Remove_leaveNode(int leaveNodeID); //CHORD�迭�� curNodes ���� ���� ��带 �����Ѵ�.
	void Move_All_Keys(int toNodeID, int fromNodeID); //fromNode�� ��� reference�� toNode���� �±��.

	void fix_finger(nodeType Node);
	//for Fix_Finger of a Node; May Return the result

	nodeType find_successor(int curNodeID, int IDKey);
	//For finding successor of IDkey for a node with curNodeID

	int find_predecessor(int curNodeID, int IDKey);
	//For finding predecessor of IDKey for a node with curNodeID

	int find_closest_predecessor(int curNodeID, int IDKey);
	//For finding closest predecessor of IDKey for a node with curNodeID

	int lookup(int nodeID, int fileKey, nodeType *foundNode, int *refInfo);
	//For looking up a file with filekey at a node with nodeID
	//Found node information is stored at foundNode
	//reference node information is stored at refInfo
	//May return the result

	void stabilize(nodeType Node);
	//For stabilizing the Predecessor and the successor around Node

	void notify(nodeType askNode, nodeType selfNode);
	//For selfNode's asking askNode to change its predecessor

	void join(nodeType joinNode, nodeType helpNode, int order);
	//For Joining a joinNode with a help of helpNode
	//order indicates the order of joining for initial sequential joining

	void initialize(nodeType joinNode, nodeType helpNode, int order);
	//For Initializing a join node, joinNode with a help of helpNode
	//order indicates the order of joining for initial sequential joining

	void move_keys(int toNodeID, int fromNodeID);
	//For moving file keys from Node with fromNodeID to Node with toNodeID
	//when joining or leaving a node

	void leave(int leaveNodeID);
	// For leaving a leaveNode

	unsigned str_hash(char *);
	//A Simple Hash Function from a string to the ID/key space

	int nID2CPos(int nodeID); //���� ���� CHORD �迭�� �ִ� �� ���κ��� nodeID�� �迭 ���° �����ִ��� �� ����
	//For getting the index at a CHORD array from a node ID, nodeID

	int twoPow(int power);
	//For getting a power of 2

	int modMinus(int modN, int minuend, int subtrand);
	//For modN modular operation of "mined - subtrand"

	int modPlus(int modN, int addend1, int addend2);
	//For modN modular operation of "addend1 + addend2"

	int modIn(int modN, int targNum, int range1, int range2, int leftmode, int rightmode);
	//For checking if targNum is "in" the range using left and right modes
	//under modN modular environment


	int randomCurID(void);
	/*************************************/

	/********** ����� ������ **********/
	int nodeIndex, keyIndex; //lookup�� ����� �Է� ���� ����
	nodeType foundNodeLU; //lookup output
	int refInfoLU; //lookup output
	int lookupResult; //lookup return value
	int match; //foundNodeLU�� refFile�� ������ ã�����ϴ� ���ϰ� ��ġ�ϴ��� �Ǵ��ϰ� ���� ���������� ����

	nodeType joinNode;
	BOOL isNodesFull;

	int leaveNodeID; //leave���� ����ڿ��� ���� leaving Node ID�� �ӽ� ����
	BOOL existChooseLeaveNode;
	BOOL isNodesEmpty;

	int initNode; // No of Initial Nodes
	int initFile; // No of Initial Files
	int curID, tempID, Found, tempNum; //tempID : 'CHORD ���̺� ���� ���� ���� �߰�'���� ������� �ӽú���
	struct in_addr curIP;
	char *curIPstr;
	nodeType curOwner, foundNode; //curOwner�� ���� �������� �����ϰ� ������ ������ ����� ���� �� ������
	char curName[32], tempStr[32], tempIPstr[16];
	int curKey, noFile;
	int selNum, randfnum, refInfo; //selNum�� ���� ���� �ʱ�ȭ���� �̿�, randfnum�� ���� �ʱ�ȭ���� �̿�(randfnum = rand()%twoN)
	int i, j, k;
	int sameExist;
	unsigned char randIPsec[4];
	int USel, USel2, USel3;
	/***********************************/

public:
	struct nodeInfo getCurNodes() { return curNodes; };
	struct fileInfo getCurFiles() { return curFiles; };
	struct CHORDInfo getCHORD(int index) { return CHORD[index]; };

	void printFiles(void); //For printing File Information
	void printNodes(void); //For printing Node Information
	int refOwnerID(int key);


protected: // serialization������ ��������ϴ�.
	CWCHORDDoc();
	DECLARE_DYNCREATE(CWCHORDDoc)

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
	virtual ~CWCHORDDoc();
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
	afx_msg void OnStep3Lookup();
	afx_msg void OnStep3Join();
	afx_msg void OnStep3Leave();
};
