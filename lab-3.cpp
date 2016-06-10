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
int pageshift = 12;//ҳ��ƫ�Ƶ�ַλ��
u64 addr;
u64 shiftmask; //ҳ��ƫ������
u64 tmp[64]; //��ʱ���ɵ�ӳ���ϵ
u64 ids[64]; //����ҳ��

void printtable(){//��ӡ����ҳ���ӳ���ϵ
	printf("ҳ�����£�\n");
	for (int i = 0; i<lev - 1; i++){
		printf("��%d��ҳ���0x%lX��Ϊ��0x%lX��������һ��ҳ�����ʼ��ַ\n",
			i + 1, ids[i], tmp[i]);
	}
	printf("��%d��ҳ���0x%lX�����������ţ�0x%lX\n\n", lev, ids[lev - 1], tmp[lev - 1]);
}

inline int getPageshift(int x){
	for (int i = 0; i<32; i++){
		if ((x >> i) == 0)return i - 1;
	}
	return 0;
}

u64 mapping(){//ӳ�䵽�����ַ
	int levsize = getPageshift(pagesz * 1024 / page_recsz);  //���˵�1����ÿ����λ��
	u64 mask = (1 << levsize) - 1;
	ids[0] = addr >> (levsize*(lev - 1) + pageshift);
	for (int i = 1; i<lev; i++){//��ȡ����ҳ��
		ids[i] = (mask << (pageshift + (lev - 1 - i)*levsize)) & addr;
		ids[i] = ids[i] >> (pageshift + (lev - 1 - i)*levsize);
	}

	set<u64> repeater;
	for (int i = 0; i<lev - 1; i++){//�������ҳ������
		int x = rand();
		while (repeater.find(x) != repeater.end()){
			x = rand();
		}//ȥ��
		repeater.insert(x);
		tmp[i] = x;
	}
	//������ĿҪ�����յ������ַ=�߼���ַ
	//�������һ����ŵ���������ҳ��ƫ��ƴ�����γ��߼���������ַ
	tmp[lev - 1] = ((~shiftmask)&addr) >> pageshift;
	printtable();
	return tmp[lev - 1];
}

int main(){
	//freopen("1.txt", "r", stdin);
	srand(time(NULL));

	while (true){
		cout << "�����룺1.ҳ��¼��С  2.ҳ��С��KB��  3.ҳ����" << endl;
		cin >> page_recsz >> pagesz >> lev;
		if (page_recsz<8){
			cout << "64λϵͳ��ҳ��¼��С����Ϊ8B" << endl;
			continue;
		}
		pageshift = 10 + getPageshift(pagesz);
		shiftmask = (1 << pageshift) - 1;
		int com;
		u64 paddr, innershift;
		cout << "1. ����16�����߼���ַ" << endl;
		cin >> com;
		if (com == 1){
			scanf_s("%lX", &addr);
			innershift = shiftmask&addr;
			paddr = mapping();
			paddr = (paddr << pageshift) | innershift;//ҳ����ҳ��ƫ��ƴ�ӣ��γ������ַ
			printf("�����ַ��0x%lX  �����ţ�0x%lX  ҳ��ƫ�ƣ�0x%lX\n\n", paddr,
				((~shiftmask)&paddr) >> pageshift, innershift);
			printtable();
		}
		else break;
	}
OUT:

	return 0;
}
