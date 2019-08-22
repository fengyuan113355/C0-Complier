#include "mipscode.h"
#include <sstream>
//����ͬ��ʽ��MIPS�������ۺ�Ϊһ�ָ�ʽ�������Ҫ���м����תĿ�����ʱ������д�ú�������add

tra_mipscode::tra_mipscode(){}

void tra_mipscode::generate(string instr,string rd,string rs,string rt,string immediate,string base){
	struct instruction* p = new instruction();
	p->instr = instr;
	p->rd = rd;
	p->rs = rs;
	p->rt = rt;
	p->immediate = immediate;
	p->base = base;
	mips_mars.push_back(*p);
}
string tra_mipscode::change(int i){
	stringstream temp_name;
	temp_name<<i;
	return temp_name.str();
}

void tra_mipscode::branch(MiddleItem* in){
	if(in->instr==add||
		in->instr==sub||
		in->instr==mul||
		in->instr==divv||
		in->instr==pset||
		in->instr==nset||
		in->instr==oar||
		in->instr==iar||
		in->instr==para||
		in->instr==push||
		in->instr==call||
		in->instr==ret||
		in->instr==scf||
		in->instr==prf){mipgenerate(in->instr,in->des,in->src1,in->src2);}
	else if(in->instr==eqint){mipgenerate(in->instr,in->des,in->forconst);}
	else if(in->instr==eqstr){mipgenerate(in->instr,in->des,in->forstring);}
	else{mipgenerate(in->instr,in->label,in->src1,in->src2);}
}
//�����������Ԫʽ��������Ŀ�����
//add sub mul divv pset nset oar iar para push call ret scf prf
void tra_mipscode::mipgenerate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2){
	static int pushnum = 0;
	static int deef = 0;
	switch(instr){
	case fsset:{
		pushnum = 0;
		if(deef==0){
			generate("move"," $fp,","$sp","","","");//move $fp,$sp\nj main\nnop\n
			generate("j","","",""," main","");
			generate("nop","","","","","");
			deef++;
		}
		break;
			   }
	case prf:{
		//addr==-3  �ַ���
		if(des->addr==-3){
			generate("la"," $a0,",des->name,"","","");
			generate("li"," $v0,","","",change(4),"");
			generate("syscall","","","","","");
		}else{
			if(des->level==level_global){
				generate("la"," $t0,",des->name,"","","");
				generate("lw"," $a0,","","",change(0),"($t0)");
			}else{
				generate("lw"," $a0,","","","-"+change(des->offset+4),"($sp)");
			}
			if(des->type==type_int){
				generate("li"," $v0,","","",change(1),"");
			}else{
				generate("li"," $v0,","","",change(11),"");
			}
			generate("syscall","","","","","");
		}
		break;
	}
	case scf:{
		if(des->type==type_int){
			generate("li"," $v0,","","",change(5),"");
			generate("syscall","","","","","");
			//generate("sw"," $v0,","","","-"+change(des->offset+4),"($sp)");
		}
		else{
			generate("li"," $v0,","","",change(12),"");
			generate("syscall","","","","","");
			//generate("sw"," $v0,","","","-"+change(des->offset+4),"($sp)");
		}
		if(des->level==level_global){
			generate("la"," $t0,",des->name,"","","");
			generate("sw"," $v0,","","",change(0),"($t0)");
		}else{
			generate("sw"," $v0,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	case ret:{
		//printf("{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}\n");
		//printf(now_func.c_str());
		//printf("\n");
		if(now_func=="main"){
			generate("j"," mainend","","","","");
			generate("nop","","","","","");//������ֱ������mainend����
		}else{
			if(des==nullptr){//�޷���ֵ
				generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
				generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
				generate("jr"," $ra","","","","");
				generate("nop","","","","","");
			}else{//�з���ֵ
				if(des->level==level_global){
					generate("la"," $t0,",des->name,"","","");
					generate("lw"," $v0,","","",change(0),"($t0)");
				}else{
					generate("lw"," $v0,","","","-"+change(des->offset+4),"($sp)");
				}
				//generate("lw"," $v0,","","","-"+change(des->offset+4),"($sp)");
				generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
				generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
				generate("jr"," $ra","","","","");
				generate("nop","","","","","");
			}
		}
		break;
	}
	case call:{
		generate("move",""," $t0,","$sp","","");
		generate("li"," $t1,","","",change(nowsize+4+4),"");
		generate("sub"," $t2,","$t0,","$t1","","");////////////////////////////////cha
		generate("move"," $fp,","$t2","","","");//���������ĺ����Ļ��¼�Ļ���ַ������$fp��
		if(des==nullptr){
			generate("jal"," ",src1->name.c_str(),"","","");
			generate("nop","","","","","");
		}//û�з���ֵ����
		else{
			generate("jal"," ",src1->name.c_str(),"","","");
			generate("nop","","","","","");
			if(des->level==level_global){
				generate("la"," $t0,",des->name,"","","");
				generate("sw"," $v0,","","",change(0),"($t0)");
			}else{
				generate("sw"," $v0,","","","-"+change(des->offset+4),"($sp)");
			}
			//generate("sw"," $v0,","","","-"+change(des->offset+4),"($sp)");
		}//�з���ֵ����
		pushnum = 0;/////////////�����˲������½��Ļ��¼�д�ͷλ�ÿ�ʼ�洢
		break;
	}
	case push:{
		if(des->level==level_global){
			generate("la"," $t1,",des->name,"","","");
			generate("lw"," $t0,","","",change(0),"($t1)");
		}else{
			generate("lw"," $t0,","","","-"+change(des->offset+4),"($sp)");
		}
		generate("sw"," $t0,","","","-"+change(nowsize+4+4+4+pushnum*4),"($sp)"); 
		pushnum++;
		break;
    }
	case para:{
		break;
	}
	case iar:{
		if(src1->level==level_global){
			generate("la"," $t0,",src1->name,"","","");
			if(src2->level==level_global){
				generate("la"," $t3,",src2->name,"","","");
				generate("lw"," $t1,","","",change(0),"($t3)");
			}
			else{
				generate("lw"," $t1,","","","-"+change(src2->offset+4),"($sp)");
			}
			generate("sll"," $t2,","$t1,","",change(2),"");
			generate("addu"," $t0,","$t0,","$t2","","");
			if(des->level==level_global){
				generate("la"," $t3,",des->name,"","","");
				generate("lw"," $t2,","","",change(0),"($t3)");
			}
			else{
				generate("lw"," $t2,","","","-"+change(des->offset+4),"($sp)");
			}
			generate("sw"," $t2,","","",change(0),"($t0)");
		}
		else{
			generate("li"," $t0,","","",change(src1->offset+4),"");
			if(src2->level==level_global){
				generate("la"," $t3,",src2->name,"","","");
				generate("lw"," $t1,","","",change(0),"($t3)");
			}
			else{
				generate("lw"," $t1,","","","-"+change(src2->offset+4),"($sp)");
			}
			generate("sll"," $t2,","$t1,","",change(2),"");
			generate("addu"," $t0,","$t0,","$t2","","");
			generate("sub"," $t0,","$sp,","$t0","","");
			if(des->level==level_global){
				generate("la"," $t3,",des->name,"","","");
				generate("lw"," $t2,","","",change(0),"($t3)");
			}
			else{
				generate("lw"," $t2,","","","-"+change(des->offset+4),"($sp)");
			}
			generate("sw"," $t2,","","",change(0),"($t0)");
		}
		break;
	}
	case oar:{
		if(src1->level==level_global){
			generate("la"," $t0,",src1->name,"","","");
			if(src2->level==level_global){
				generate("la"," $t3,",src2->name,"","","");
				generate("lw"," $t1,","","",change(0),"($t3)");
			}
			else{
				generate("lw"," $t1,","","","-"+change(src2->offset+4),"($sp)");
			}
			generate("sll"," $t2,","$t1,","",change(2),"");
			generate("addu"," $t0,","$t0,","$t2","","");
			generate("lw"," $t1,","","",change(0),"($t0)");
			if(des->level==level_global){
				generate("la"," $t3,",des->name,"","","");
				generate("sw"," $t1,","","",change(0),"($t3)");
			}
			else{
				generate("sw"," $t1,","","","-"+change(des->offset+4),"($sp)");
			}
		}else{
			generate("li"," $t0,","","",change(src1->offset+4),"");
			if(src2->level==level_global){
				generate("la"," $t3,",src2->name,"","","");
				generate("lw"," $t1,","","",change(0),"($t3)");
			}
			else{
				generate("lw"," $t1,","","","-"+change(src2->offset+4),"($sp)");
			}
			generate("sll"," $t2,","$t1,","",change(2),"");
			generate("addu"," $t0,","$t0,","$t2","","");
			generate("sub"," $t0,","$sp,","$t0","","");
			generate("lw"," $t1,","","",change(0),"($t0)");
			if(des->level==level_global){
				generate("la"," $t3,",des->name,"","","");
				generate("sw"," $t1,","","",change(0),"($t3)");
			}
			else{
				generate("sw"," $t1,","","","-"+change(des->offset+4),"($sp)");
			}
		}
		break;
	}
	case nset:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t0,","","",change(0),"($t3)");
		}
		else{
			generate("lw"," $t0,","","","-"+change(src1->offset+4),"($sp)");
		}
		generate("sub"," $t1,","$0,","$t0","","");
		if(des->level==level_global){
			generate("la"," $t3,",des->name,"","","");
			generate("sw"," $t1,","","",change(0),"($t3)");
		}
		else{
			generate("sw"," $t1,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	case pset:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t0","","",change(0),"($t3)");
		}
		else{
			generate("lw"," $t0,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(des->level==level_global){
			generate("la"," $t3,",des->name,"","","");
			generate("sw"," $t0,","","",change(0),"($t3)");
		}else{
			generate("sw"," $t0,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	case add:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("addu"," $t0,","$t1,","$t2","","");
		if(des->level==level_global){
			generate("la"," $t3,",des->name,"","","");
			generate("sw"," $t0,","","",change(0),"($t3)");
		}
		else{
			generate("sw"," $t0,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	case sub:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("sub"," $t0,","$t1,","$t2","","");
		if(des->level==level_global){
			generate("la"," $t3,",des->name,"","","");
			generate("sw"," $t0,","","",change(0),"($t3)");
		}
		else{
			generate("sw"," $t0,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	case mul:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("mult",""," $t1,","$t2","","");
		generate("mflo"," $t0","","","","");
		if(des->level==level_global){
			generate("la"," $t3,",des->name,"","","");
			generate("sw"," $t0,","","",change(0),"($t3)");
		}
		else{
			generate("sw"," $t0,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	case divv:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("div",""," $t1,","$t2","","");
		generate("mflo"," $t0","","","","");
		if(des->level==level_global){
			generate("la"," $t3,",des->name,"","","");
			generate("sw"," $t0,","","",change(0),"($t3)");
		}
		else{
			generate("sw"," $t0,","","","-"+change(des->offset+4),"($sp)");
		}
		break;
	}
	}
}
//_neq _beq _leq _lss _geq _gtr toset j fsset fdset
void tra_mipscode::mipgenerate(middleinstr instr,string label,struct tablenode * src1,struct tablenode * src2){
	switch(instr){
	case _neq:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("bne"," $t1,","$t2,",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _beq:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("beq"," $t1,","$t2,",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _leq:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("sub"," $t3,","$t1,","$t2","","");
		generate("blez"," $t3,",",",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _lss:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("sub"," $t3,","$t1,","$t2","","");
		generate("bltz"," $t3,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _geq:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("sub"," $t3,","$t1,","$t2","","");
		generate("bgez"," $t3,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _gtr:{
		if(src1->level==level_global){
			generate("la"," $t3,",src1->name,"","","");
			generate("lw"," $t1,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t1,","","","-"+change(src1->offset+4),"($sp)");
		}
		if(src2->level==level_global){
			generate("la"," $t3,",src2->name,"","","");
			generate("lw"," $t2,","","",change(0),"($t3)");
		}else{
			generate("lw"," $t2,","","","-"+change(src2->offset+4),"($sp)");
		}
		generate("sub"," $t3,","$t1,","$t2","","");
		generate("bgtz"," $t3,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case toset:{
		generate("",label,":","","","");
		break;
			   }
	case endset:{
		generate("",label,":","","","");
		break;		
				}
	case j:{
	    generate("j ",label,"","","","");
		generate("nop","","","","","");
		break;
		   }
	case fsset:{
		now_func = label;//��¼��ǰ���Ǹ�����������Ŀ�����
		//printf("************+++++++++++++***************\n");
		//printf(label.c_str());
		//printf("\n");
		//printf(now_func.c_str());
		generate("",label,":","","","");
		generate("sw"," $sp,","","","-"+change(nowsize+8),"($fp)");
		generate("move"," $sp,","$fp","","","");
		generate("sw"," $ra,","","","-"+change(nowsize+4),"($sp)");
		break;
			   }
	case fdset:{
		//printf("&&&&&&^^^^^^^^^^^&&&&&&&&&&&&\n");
		//printf(now_func.c_str());
		//printf("\n");
		if(now_func=="main"){
			generate("j"," mainend","","","","");
			generate("nop","","","","","");//������ֱ������mainend����
		}
		else{
			generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
			generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
			generate("jr"," $ra","","","","");
			generate("nop","","","","","");
		}
		//������������������return���ĺ����������ĳЩ��return���ĺ����п��ܲ���ִ��
		generate("",label,":","","","");
		break;
			   }
	}
}
//eqint
void tra_mipscode::mipgenerate(middleinstr instr,struct tablenode * des,int a){
	generate("li"," $t0,","","",change(a),"");
	if(des->level==level_global){
		generate("la"," $t3,",des->name,"","","");
		generate("sw"," $t0,","","",change(0),"($t3)");
	}
	else{
		generate("sw"," $t0,","","","-"+change(des->offset+4),"($sp)");
	}
}
//eqstr
void tra_mipscode::mipgenerate(middleinstr instr,struct tablenode * des,string constr){
	data.push_back(des->name+": .asciiz \""+constr+"\"");
}
