#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include<string.h>
#include<vector>
#include<string>
#include"word_analyze.h"

#define kind_const 1
#define kind_var 2
#define kind_para 3
#define kind_func 4

#define type_int 1
#define type_char 2
#define type_void 3

#define level_global 1
#define level_local 2

#define node_max  200
#define name_max  200
using namespace std;

struct tablenode{
	string name;//�������������--��ʶ��
	int kind;//�ñ�����const,var,parameter����function
	int type;//�ñ�����int����char����void
	int value;//�ñ�����ֵ��char����Ҳ��int��������
	int level;//�ñ�����ȫ�ֵĻ��Ǿֲ��ں�����---�����ϻ������ι�ϵ����Ϊ�ķ�ֻ֧�����㣬ѭ���ڲ����ٶ����±���
	int length;//�������������������鳤��
	int addr;//������ĵ�ַ��Դ�����еڼ���lines
	string str;
	int offset;//�������Լ�������
	bool issingle;//�жϱ��ʽ�Ƿ���һ����ֵ̬
	int usesum;//�������ü���
};

class tablemanage{
public:
	vector<struct tablenode> symbollist;//���ű�
	int off_sum;

	tablemanage();
	int table_insert(string name,int kind,int type,int value,int level,int length,int addr);//����ű��в���һ���ڵ㲢����λ��
	struct tablenode * table_search(string name);//�ڷ��ű��и���nameѰ�ҽڵ㲢����λ��
	void clear();
};


#endif

