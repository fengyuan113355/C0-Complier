#ifndef MIDDLECODE_H
#define MIDDLECODE_H
#include <vector>
#include <string>
#include "symboltable.h"
//#include "mipscode.h"

using namespace std;

enum middleinstr{
	add,sub,mul,divv,
	_beq,_neq,_geq,_gtr,_leq,_lss,
	oar,iar,
	scf,prf,toset,endset,fsset,fdset,
	pset,nset,
	call,push,ret,
	j,eqint,eqstr,para
};

struct MiddleItem{
	middleinstr instr;
	struct tablenode* des;
	struct tablenode* src1;
	struct tablenode* src2;
	string label;
	int forconst;
	string forstring;
	MiddleItem();
	void toprint(FILE * show){
		//fprintf(show,"%d\n",instr);
		switch(instr){
		case add:{fprintf(show,"%s = %s + %s\n",(des->name).c_str(),(src1->name).c_str(),(src2->name).c_str());break;}
		case sub:{fprintf(show,"%s = %s - %s\n",(des->name).c_str(),(src1->name).c_str(),(src2->name).c_str());break;}
		case mul:{fprintf(show,"%s = %s * %s\n",(des->name).c_str(),(src1->name).c_str(),(src2->name).c_str());break;}
		case divv:{fprintf(show,"%s = %s / %s\n",(des->name).c_str(),(src1->name).c_str(),(src2->name).c_str());break;}
		case _beq:{fprintf(show,"%s==%s %s\n",(src1->name).c_str(),(src2->name).c_str(),label.c_str());break;}
		case _neq:{fprintf(show,"%s!=%s %s\n",(src1->name).c_str(),(src2->name).c_str(),label.c_str());break;}
		case _geq:{fprintf(show,"%s>=%s %s\n",(src1->name).c_str(),(src2->name).c_str(),label.c_str());break;}
		case _gtr:{fprintf(show,"%s>%s %s\n",(src1->name).c_str(),(src2->name).c_str(),label.c_str());break;}
		case _leq:{fprintf(show,"%s<=%s %s\n",(src1->name).c_str(),(src2->name).c_str(),label.c_str());break;}
		case _lss:{fprintf(show,"%s<%s %s\n",(src1->name).c_str(),(src2->name).c_str(),label.c_str());break;}
		case oar:{fprintf(show,"%s=%s[%s]\n",(des->name).c_str(),(src1->name).c_str(),(src2->name).c_str());break;}
		case iar:{fprintf(show,"%s[%s]=%s\n",(src1->name).c_str(),(src2->name).c_str(),(des->name).c_str());break;}
		case scf:{fprintf(show,"scanf %s\n",(des->name).c_str());break;}
		case prf:{fprintf(show,"printf %s\n",(des->name).c_str());break;}
		case toset:{fprintf(show,"%s\n",label.c_str());break;}
		case endset:{fprintf(show,"%s\n",label.c_str());break;}
		case fsset:{fprintf(show,"%s\n",label.c_str());break;}//函数开始设置标签
		case fdset:{fprintf(show,"%s\n",label.c_str());break;}//函数结束设置标签
		case pset:{fprintf(show,"%s=%s\n",des->name.c_str(),src1->name.c_str());break;}
		case nset:{fprintf(show,"%s=-%s\n",(des->name).c_str(),(src1->name).c_str());break;}
		case call:{
			if(des==nullptr){
				fprintf(show,"call %s\n",(src1->name).c_str());
			}else{
				fprintf(show,"%s = call %s\n",(des->name).c_str(),(src1->name).c_str());
			}
			break;
		}
		case push:{fprintf(show,"push %s\n",(des->name).c_str());break;}
		case ret:{
			if(des!=nullptr){
				fprintf(show,"return %s\n",(des->name).c_str());
			}else{
				fprintf(show,"return\n");
			}
			break;
		}
		case j:{fprintf(show,"j %s\n",label.c_str());break;}
		case eqint:{fprintf(show,"%s = %d\n",(des->name).c_str(),forconst);break;}
		case eqstr:{fprintf(show,"%s = \"%s\"\n",(des->name).c_str(),forstring.c_str());break;}
		case para:{fprintf(show,"para %s\n",(des->name).c_str());break;}
		}
	}
};



class middlecode{
public:
	vector<MiddleItem*> middlesave;//存储每条中间代码的指针
	//mipscode goal;//生成MIPS目标代码的对象

	//middlecode();
	void generate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2);
	void generate(middleinstr instr,string label,struct tablenode * src1,struct tablenode * src2);
	void generate(middleinstr instr,struct tablenode * des,int a);
	void generate(middleinstr instr,struct tablenode * des,string constr);
	//void generate(middleinstr instr,string label);
	void clear();
};

#endif









