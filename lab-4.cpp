#include <iostream>
#include <cmath>
#include <string>
#include <sys/types.h>

using namespace std;

// ���Ǽ��������ַΪ32λ��һ�����̿�Ĵ�СΪ1KB���߼���n��Ӧ�����n��
// ÿ�ּ�ӿ����10��Ԫ�أ��ļ���˳��ռ�������.
// ע�����ﶨ���inode�Լ���ӿ���Ҫ��Ϊ�˱��������ϵĽṹ������ʵinode����ȫ��ͬ
typedef unsigned int size_32;
const int BLOCK_ITEM_SIZE = 10;
const int BLOCK_SIZE = 1024;

// һ����ӿ�
struct singleIndirectBlock
{
	int iaddr[BLOCK_ITEM_SIZE];

	singleIndirectBlock()
	{
		for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
			iaddr[i] = -1;
	}
};

// ������ӿ�
struct doubleIndirectBlock
{
	struct singleIndirectBlock singleIndirectBlocks[BLOCK_ITEM_SIZE];
};

// ������ӿ�
struct tripleIndirectBlock
{
	struct doubleIndirectBlock doubleIndirectBlocks[BLOCK_ITEM_SIZE];
};

struct inode
{
	int mode; // ����Ȩ��
	string owners; // ������
	int timeStamps; // ����ʱ��
	int iaddr[BLOCK_ITEM_SIZE];
	struct singleIndirectBlock sib;
	struct doubleIndirectBlock dib;
	struct tripleIndirectBlock tib;

	inode() : mode(-1), owners(""), timeStamps(-1)
	{
		for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
			iaddr[i] = -1;
	}
};

void initInode(struct inode& node)
{
	node.mode = 644;
	node.owners = "mjx";
	node.timeStamps = 20160606;
	// ��ʼ��ֱ��������һ�μ�ӿ�Ͷ��μ�ӿ�
	int count = 0;
	for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
		node.iaddr[i] = count++;
	for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
		node.sib.iaddr[i] = count++;
	for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
	for (int j = 0; j < BLOCK_ITEM_SIZE; ++j)
		node.dib.singleIndirectBlocks[i].iaddr[j] = count++;

	// ���μ�ӿ��ݲ�ʹ��
}

void printInode(const struct inode& node)
{
	cout << "mode: " << node.mode << endl;
	cout << "owners: " << node.owners << endl;
	cout << "timeStamps: " << node.timeStamps << endl;
	cout << "direct blocks: ";
	for (int i = 0; node.iaddr[i] != -1 && i < BLOCK_ITEM_SIZE; ++i)
		cout << node.iaddr[i] << " ";
	cout << endl;
}

int convertToPhysicalAddr(size_32 logicAddr, const struct inode& node,
	size_32& physicalBlockNo, size_32& physicalBlockOffset)
{
	size_32 idx = logicAddr / BLOCK_SIZE;
	physicalBlockOffset = logicAddr % BLOCK_SIZE;

	// ֱ������
	if (idx < 10)
	{
		if (node.iaddr[idx] == -1)
			return -1;
		physicalBlockNo = node.iaddr[idx];
		return 0;
	}
	// һ�μ�ӿ�
	else if (idx < 10 + BLOCK_ITEM_SIZE)
	{
		idx = idx - 10;
		if (node.sib.iaddr[idx] == -1)
			return -1;
		physicalBlockNo = node.sib.iaddr[idx];
		return 0;
	}
	// ���μ�ӿ�
	else if (idx < 10 + BLOCK_ITEM_SIZE + BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE)
	{
		int i, j;
		idx = idx - 10 - BLOCK_ITEM_SIZE;
		i = idx / BLOCK_ITEM_SIZE;
		j = idx % BLOCK_ITEM_SIZE;
		if (node.dib.singleIndirectBlocks[i].iaddr[j] == -1)
			return -1;
		physicalBlockNo = node.dib.singleIndirectBlocks[i].iaddr[j];
		return 0;
	}
	// ���μ�ӿ�
	else if (10 + BLOCK_ITEM_SIZE + BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE + BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE)
	{
		int i, j, k;
		idx = idx - 10 - BLOCK_ITEM_SIZE - BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE;

		i = idx / (BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE);
		idx %= (BLOCK_ITEM_SIZE * BLOCK_ITEM_SIZE);
		j = idx / BLOCK_ITEM_SIZE;
		k = idx % BLOCK_ITEM_SIZE;
		if (node.tib.doubleIndirectBlocks[i].singleIndirectBlocks[j].iaddr[k] == -1)
			return -1;
		physicalBlockNo = node.tib.doubleIndirectBlocks[i].singleIndirectBlocks[j].iaddr[k];
		return 0;
	}
	else
		return -1;

	return 0;
}

int main(int argc, char *argv[])
{
	struct inode node;
	initInode(node);
	printInode(node);

	size_32 logicAddr;
	size_32 physicalBlockNo;
	size_32 physicalBlockOffset;

	cout << "please input the logicAddr" << endl;
	cin >> hex >> logicAddr;

	int ret = convertToPhysicalAddr(logicAddr, node, physicalBlockNo, physicalBlockOffset);

	cout << "result: " << ret << endl;
	if (ret == 0)
	{
		cout << "physicalBlockNo: " << physicalBlockNo << endl;
		cout << "physicalBlockOffset: " << physicalBlockOffset << endl;
	}

	return 0;
}

