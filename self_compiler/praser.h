#ifndef PRASER_H
#define PRASER_H
#include "symboltable.h"
#include "middlecode.h"
#include "errorhandler.h"
#include <map>
#include <vector>

#define analyze_global 1
#define analyze_local 0
class praser{
public:
	int hasfault;//1����ʷ����﷨����������д���������Ŀ�����
	int cyclic;//ѭ��Ƕ�׵Ĳ������������ü�����
	FILE * output;
	tablemanage global;  //ȫ�ַ��ű�
	tablemanage* local;   //�ֲ����ű�
	map<string, tablemanage*> still;   //���ڴ��ÿ�������Լ��ľֲ����ű�

	middlecode gen;//�м��������
	//mipscode goal;
	errorhandler error;//�����������
	int now;//��ʾ��ǰ�﷨�Ƶ�������ȫ�ֻ��Ǿֲ�1---ȫ��  0---�ֲ�
	
	int erjump(int error);//���ݴ��������������
	void transfer(string key);//ÿ����һ��transfer���ѵ�ǰlocal�浽still�в���������local
	struct tablenode * useident(string goal);//ÿ���������ʹ�ñ�ʶ��ʱ����useident���������������ű��н���ָ��
	struct tablenode * gentemp();//���ڱ��ʽ�м�����ı�ʾ������һ���ڵ�ָ��
	struct tablenode * genrep(int a);//���ڽ����ʽ�г��ֵ����������ַ�Ҳ����һ��ident���ֽ��б���
	struct tablenode * genstr(string constr);//��������һ�������Ա�ʾ�ַ�������
	string setlabel(symbol ins);//���ݱ�ʶ�����ɱ�ǩ
	praser(int x,FILE * show);//Ĭ�Ϲ�����
	void program();
	int integer();
	void constdef();
	void constdec();
	void vardef();
	void vardec();
	void global_var(int type);
	void functiondef(int type,string temp);
	int paratable();
	int complexment();
	int ments();
	int ifment();
	void condition(string label,symbol operate);
	struct tablenode* expression(int species);
	struct tablenode* ele(int species);
	struct tablenode* vector(int species);
	int whilement();
	int switchment();
	int switchtable(struct tablenode* item1);
	int switchbranch(struct tablenode* item1,string end);
	void functionuse(string temp);
	void assignment(string temp);
	void readment();
	void writement();
	int returnment();
};


#endif







