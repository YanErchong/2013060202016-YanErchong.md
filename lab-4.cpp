#include <iostream>
#include <cmath>
#include <string>
#include <sys/types.h>

using namespace std;

// 我们假设虚拟地址为32位，一个磁盘块的大小为1KB，逻辑块n对应物理块n，
// 每种间接块包含10个元素，文件按顺序占用物理块.
// 注：这里定义的inode以及间接块主要是为了表明概念上的结构，与真实inode不完全相同
typedef unsigned int size_32;
const int BLOCK_ITEM_SIZE = 10;
const int BLOCK_SIZE = 1024;

// 一级间接块
struct singleIndirectBlock
{
	int iaddr[BLOCK_ITEM_SIZE];

	singleIndirectBlock()
	{
		for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
			iaddr[i] = -1;
	}
};

// 二级间接块
struct doubleIndirectBlock
{
	struct singleIndirectBlock singleIndirectBlocks[BLOCK_ITEM_SIZE];
};

// 三级间接块
struct tripleIndirectBlock
{
	struct doubleIndirectBlock doubleIndirectBlocks[BLOCK_ITEM_SIZE];
};

struct inode
{
	int mode; // 访问权限
	string owners; // 持有者
	int timeStamps; // 创建时间
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
	// 初始化直接索引，一次间接块和二次间接块
	int count = 0;
	for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
		node.iaddr[i] = count++;
	for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
		node.sib.iaddr[i] = count++;
	for (int i = 0; i < BLOCK_ITEM_SIZE; ++i)
	for (int j = 0; j < BLOCK_ITEM_SIZE; ++j)
		node.dib.singleIndirectBlocks[i].iaddr[j] = count++;

	// 三次间接块暂不使用
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

	// 直接索引
	if (idx < 10)
	{
		if (node.iaddr[idx] == -1)
			return -1;
		physicalBlockNo = node.iaddr[idx];
		return 0;
	}
	// 一次间接块
	else if (idx < 10 + BLOCK_ITEM_SIZE)
	{
		idx = idx - 10;
		if (node.sib.iaddr[idx] == -1)
			return -1;
		physicalBlockNo = node.sib.iaddr[idx];
		return 0;
	}
	// 二次间接块
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
	// 三次间接块
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

