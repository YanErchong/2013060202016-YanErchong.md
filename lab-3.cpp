#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <set>
#include <cmath>
#include <cassert>

using namespace std;

typedef unsigned long long u64;

int page_recsz = 4, pagesz = 4, lev = 1;
int pageshift = 12;//页内偏移地址位数
u64 addr;
u64 shiftmask; //页内偏移掩码
u64 tmp[64]; //临时生成的映射关系
u64 ids[64]; //各级页号

void printtable(){//打印各级页表的映射关系
	printf("页表如下：\n");
	for (int i = 0; i<lev - 1; i++){
		printf("第%d级页表第0x%lX项为：0x%lX，它是下一级页表的起始地址\n",
			i + 1, ids[i], tmp[i]);
	}
	printf("第%d级页表第0x%lX项内是物理块号：0x%lX\n\n", lev, ids[lev - 1], tmp[lev - 1]);
}

inline int getPageshift(int x){
	for (int i = 0; i<32; i++){
		if ((x >> i) == 0)return i - 1;
	}
	return 0;
}

u64 mapping(){//映射到物理地址
	int levsize = getPageshift(pagesz * 1024 / page_recsz);  //除了第1级外每级的位数
	u64 mask = (1 << levsize) - 1;
	ids[0] = addr >> (levsize*(lev - 1) + pageshift);
	for (int i = 1; i<lev; i++){//获取各级页号
		ids[i] = (mask << (pageshift + (lev - 1 - i)*levsize)) & addr;
		ids[i] = ids[i] >> (pageshift + (lev - 1 - i)*levsize);
	}

	set<u64> repeater;
	for (int i = 0; i<lev - 1; i++){//随机生成页表内容
		int x = rand();
		while (repeater.find(x) != repeater.end()){
			x = rand();
		}//去重
		repeater.insert(x);
		tmp[i] = x;
	}
	//根据题目要求最终的物理地址=逻辑地址
	//所以最后一级存放的物理块号与页内偏移拼接能形成逻辑（物理）地址
	tmp[lev - 1] = ((~shiftmask)&addr) >> pageshift;
	printtable();
	return tmp[lev - 1];
}

int main(){
	//freopen("1.txt", "r", stdin);
	srand(time(NULL));

	while (true){
		cout << "请输入：1.页记录大小  2.页大小（KB）  3.页表级数" << endl;
		cin >> page_recsz >> pagesz >> lev;
		if (page_recsz<8){
			cout << "64位系统的页记录大小至少为8B" << endl;
			continue;
		}
		pageshift = 10 + getPageshift(pagesz);
		shiftmask = (1 << pageshift) - 1;
		int com;
		u64 paddr, innershift;
		cout << "1. 输入16进制逻辑地址" << endl;
		cin >> com;
		if (com == 1){
			scanf_s("%lX", &addr);
			innershift = shiftmask&addr;
			paddr = mapping();
			paddr = (paddr << pageshift) | innershift;//页号与页内偏移拼接，形成物理地址
			printf("物理地址：0x%lX  物理块号：0x%lX  页内偏移：0x%lX\n\n", paddr,
				((~shiftmask)&paddr) >> pageshift, innershift);
			printtable();
		}
		else break;
	}
OUT:

	return 0;
}
