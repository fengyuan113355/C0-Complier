#ifndef MIPSCODE_H
#define MIPSCODE_H
#include <string>
#include <vector>
#include "middlecode.h"
using namespace std;


struct instruction{
	string instr;
	string rd;
	string rs;
	string rt;
	string immediate;//���ڼ�¼ָ���еĳ���
	string base;
	void toshow(FILE * in){
		string total = instr+rd+rs+rt+immediate+base;
		fprintf(in,total.c_str());
		fprintf(in,"\n");
	}
};

struct forupper{
	int index;
	struct tablenode copynode;
};

class mipscode{
public:
	vector<instruction> mips_mars;
	vector<string> data;
	int nowsize;
	string now_func;
	vector<struct tablenode*> globalpool;
	vector<struct tablenode*> temppool;
	FILE * for_test;
	tablemanage* localtable;//��������Ŀ�����ĺ����ľֲ����ű�
	//localtable��word_analyze�ṩ�޸�

	mipscode();
	//�Ĵ�������غ���
	void clear();
	void clear_all();
	void save();
	void save_all();
	void assign_s_reg();
	int search(vector<struct tablenode*> pool,struct tablenode* goal);
	int eliminate(struct tablenode* inreg,struct tablenode* use1,struct tablenode* use2);
	//bool compare_node(struct forupper upperx,struct forupper uppery);

	void generate(string instr,string rd,string rs,string rt,string immediate,string base);
	//֧�ֶ�̬�������ĸ����������м�������ʽ����MIPSĿ����룬ÿ����������Ҫ���������generate����
	void mipgenerate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2);
	void mipgenerate(middleinstr instr,string label,struct tablenode * src1,struct tablenode * src2);
	void mipgenerate(middleinstr instr,struct tablenode * des,int a);
	void mipgenerate(middleinstr instr,struct tablenode * des,string constr);
	void branch(MiddleItem* in);
	string change(int i);
};

/////////////////////////////////////////////
class tra_mipscode{
public:
	vector<instruction> mips_mars;
	vector<string> data;
	int nowsize;
	string now_func;

	tra_mipscode();
	void generate(string instr,string rd,string rs,string rt,string immediate,string base);
	//֧�ֶ�̬�������ĸ����������м�������ʽ����MIPSĿ����룬ÿ����������Ҫ���������generate����
	void mipgenerate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2);
	void mipgenerate(middleinstr instr,string label,struct tablenode * src1,struct tablenode * src2);
	void mipgenerate(middleinstr instr,struct tablenode * des,int a);
	void mipgenerate(middleinstr instr,struct tablenode * des,string constr);
	void branch(MiddleItem* in);
	string change(int i);
};

///////////////////////////////////////////////////
#endif
