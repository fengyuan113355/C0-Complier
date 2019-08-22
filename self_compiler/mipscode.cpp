#include "mipscode.h"
#include <sstream>
#include <algorithm>
//将不同格式的MIPS汇编代码综合为一种格式，因此需要在中间代码转目标代码时认真填写该函数参数

//两种寄存器池中存的是符号表中节点的指针以适应全局与局部重名的情况add
mipscode::mipscode(){
	for(int i=0;i<8;i++){
		globalpool.push_back(nullptr);
	}//全局寄存器$s0---$s7
	for(int i=0;i<8;i++){
		temppool.push_back(nullptr);
	}//临时寄存器$t0---$t7,$t8,$t9
	for_test = fopen("middle_mips.txt","w");
}

void mipscode::clear(){
	/*for(int i=0;i<8;i++){
		globalpool[i] = nullptr;
	}//全局寄存器$s0---$s7*/
	for(int i=0;i<8;i++){
		temppool[i] = nullptr;
	}//临时寄存器$t0---$t7,$t8,$t9
}

void mipscode::clear_all(){
	for(int i=0;i<8;i++){
		globalpool[i] = nullptr;
	}//全局寄存器$s0---$s7
	for(int i=0;i<8;i++){
		temppool[i] = nullptr;
	}//临时寄存器$t0---$t7,$t8,$t9
}
void mipscode::save(){
	struct tablenode * p;
	/*for(int i=0;i<8;i++){
		p = globalpool.at(i);
		if(p==nullptr){continue;}
		if(p->level==level_global){
			generate("la"," $k1,",p->name,"","","");
			generate("sw"," $s"+change(i)+",","","",change(0),"($k1)");
		}else{
			generate("sw"," $s"+change(i)+",","","","-"+change(p->offset+4),"($sp)");
		}
	}//全局寄存器$s0---$s7*/
	for(int i=0;i<8;i++){
		p = temppool.at(i);
		if(p==nullptr){continue;}
		if(p->level==level_global){
			generate("la"," $k1,",p->name,"","","");
			generate("sw"," $t"+change(i)+",","","",change(0),"($k1)");
		}else{
			generate("sw"," $t"+change(i)+",","","","-"+change(p->offset+4),"($sp)");
		}
	}//临时寄存器$t0---$t7,$t8,$t9
}
//在函数调用时调用该函数，保存全局和临时寄存器
void mipscode::save_all(){
	struct tablenode * p;
	for(int i=0;i<8;i++){
		p = globalpool.at(i);
		if(p==nullptr){continue;}
		if(p->level==level_global){
			generate("la"," $k1,",p->name,"","","");
			generate("sw"," $s"+change(i)+",","","",change(0),"($k1)");
		}else{
			generate("sw"," $s"+change(i)+",","","","-"+change(p->offset+4),"($sp)");
		}
	}//全局寄存器$s0---$s7
	for(int i=0;i<8;i++){
		p = temppool.at(i);
		if(p==nullptr){continue;}
		if(p->level==level_global){
			generate("la"," $k1,",p->name,"","","");
			generate("sw"," $t"+change(i)+",","","",change(0),"($k1)");
		}else{
			generate("sw"," $t"+change(i)+",","","","-"+change(p->offset+4),"($sp)");
		}
	}//临时寄存器$t0---$t7,$t8,$t9
}

//在每个函数的开头为当前函数分配全局寄存器
//增加引用计数后也是只针对局部符号表中的变量分配全局寄存器（global表中符号也进行使用情况统计但是不利用其结果做优化）
/*
void mipscode::assign_s_reg(){
	//localtable
	if(localtable->symbollist.size()==0){return;}
	int i = 0;
	int outcome = 0;//已经分配出去的s寄存器的个数
	for(;i<localtable->symbollist.size();i++){
		if(localtable->symbollist.at(i).addr<0){
			continue;
		}
		if(localtable->symbollist.at(i).length>0){
			continue;
		}//函数、数组不分配寄存器
		//为第i项分配$s(outcome)
		if(localtable->symbollist.at(i).level==level_global){
			generate("la"," $t8,",localtable->symbollist.at(i).name,"","","");
			generate("lw"," $s"+change(outcome)+",","","",change(0),"($t8)");
		}else{
			generate("lw"," $s"+change(outcome)+",","","","-"+change(localtable->symbollist.at(i).offset+4),"($sp)");
		}
		//修改寄存器池中的记录
		globalpool[outcome] = &(localtable->symbollist.at(i));
		outcome++;
		if(outcome>7){
			break;
		}
	}
}*/

//定义比较函数
bool compare_node(struct forupper upperx,struct forupper uppery){
	if(upperx.copynode.usesum>uppery.copynode.usesum){
		return true;
	}
	return false;
}

//上面注释部分是没有采用引用计数时的情况可完全替代下面同名的函数
void mipscode::assign_s_reg(){
	//localtable
	if(localtable->symbollist.size()==0){return;}
	int i = 0;
	int outcome = 0;//已经分配出去的s寄存器的个数
	vector<struct forupper> transcp;
	//对局部符号表进行排序，由于还有对原表的指针引用，为防止出错所以复制一个表进行排序操作
	for(;i<localtable->symbollist.size();i++){
		if(localtable->symbollist.at(i).addr<0){
			continue;
		}
		if(localtable->symbollist.at(i).length>0){
			continue;
		}//函数、数组不分配寄存器
		struct forupper* copybab = new forupper();
		copybab->index = i;
		copybab->copynode = localtable->symbollist.at(i);
		transcp.push_back(*copybab);
	}
	sort(transcp.begin(),transcp.end(),compare_node);
	//上面完成了对副本的复制
	for(int j=0;j<transcp.size();j++){
		int realindex;
		realindex = transcp.at(j).index;
		if(localtable->symbollist.at(realindex).level==level_global){
			generate("la"," $t8,",localtable->symbollist.at(realindex).name,"","","");
			generate("lw"," $s"+change(outcome)+",","","",change(0),"($t8)");
		}else{
			generate("lw"," $s"+change(outcome)+",","","","-"+change(localtable->symbollist.at(realindex).offset+4),"($sp)");
		}
		//修改寄存器池中的记录
		globalpool[outcome] = &(localtable->symbollist.at(realindex));
		outcome++;
		if(outcome>7){
			break;
		}
	}
}

//判断某一变量在当前寄存器池中是否存在，不存在返回-1；存在则返回寄存器的编号
int mipscode::search(vector<struct tablenode*> pool,struct tablenode* goal){
	for(int i=0;i<pool.size();i++){
		if(pool.at(i)==goal){
			return i;
		}
	}
	return -1;
}

//将临时寄存器池中的某一变量剔除（存入活动记录），并返回剔除后可用寄存器（临时寄存器$t0---$t7）
//剔除后inreg所表示的变量就已经存入MIPS寄存器中，即生成了load指令
//当前栈的基地址是$sp
int mipscode::eliminate(struct tablenode* inreg,struct tablenode* use1,struct tablenode* use2){
	static int a = 0;
	struct tablenode* elimi;
	int remainder = a%8;
		for(;;){
			elimi = temppool.at(remainder);
			if(elimi==nullptr){a++;break;}
			else{
				if((elimi==use1)||(elimi==use2)){
					a++;
					remainder = a%8;
					continue;
				}else{a++;break;}
			}
		}
		//存入活动记录中
		if(elimi!=nullptr){
			if(elimi->level==level_global){
				generate("la"," $t8,",elimi->name,"","","");
				generate("sw"," $t"+change(remainder)+",","","",change(0),"($t8)");
			}else{
				generate("sw"," $t"+change(remainder)+",","","","-"+change(elimi->offset+4),"($sp)");
			}
		}
		//从活动记录中取出
		if(inreg->level==level_global){
			generate("la"," $t8,",inreg->name,"","","");
			generate("lw"," $t"+change(remainder)+",","","",change(0),"($t8)");
		}else{
			generate("lw"," $t"+change(remainder)+",","","","-"+change(inreg->offset+4),"($sp)");
		}
		//修改寄存器池中的记录
		temppool[remainder] = inreg;
		return remainder;
	/*else{//inreg是数组，需要申请得到寄存器的是数组中的第index项（从0开始，数组下标）
		for(;;){
			elimi = temppool.at(remainder);
			if(elimi.big==nullptr){break;}
			else{
				if(use1->length==0){
					if((elimi.big==use1)||(elimi.big==use2)){
						a++;
						remainder = a%8;
						continue;
					}else{break;}
				}else{
					if((elimi.big==use1&&elimi.small==index2)||(elimi.big==use2)){
						a++;
						remainder = a%8;
						continue;
					}else{break;}
				}
			}
		}
		if(elimi.big!=nullptr){
			if(elimi.big->length==0){
				if(elimi.big->level==level_global){
					generate("la"," $t8,",elimi.big->name,"","","");
					generate("sw"," $t"+change(remainder)+",","","",change(0),"($t8)");
				}else{
					generate("sw"," $t"+change(remainder)+",","","","-"+change(elimi.big->offset+4),"($sp)");
				}
			}else{//数组，需要考虑index
				if(elimi.big->level==level_global){
					generate("la"," $t8,",elimi.big->name,"","","");
					generate("li"," $t9,",change(elimi.small),"","","");
					generate("sll"," $t9,","$t9,","",change(2),"");
					generate("addu"," $t8,","$t8,","$t9","","");
					generate("sw"," $t"+change(remainder)+",","","",change(0),"($t8)");
				}
				else{
					generate("li"," $t8,","","",change(elimi.big->offset+4),"");
					generate("li"," $t9,",change(elimi.small),"","","");
					generate("sll"," $t9,","$t9,","",change(2),"");
					generate("addu"," $t8,","$t8,","$t9","","");
					generate("sub"," $t8,","$sp,","$t8","","");
					generate("sw"," $t"+change(remainder)+",","","",change(0),"($t0)");
				}
			}
		}
		//将inreg index1放入寄存器$t(remainder)中
		if(inreg->level==level_global){
			generate("la"," $t8,",inreg->name,"","","");
			generate("li"," $t9,",change(index1),"","","");
			generate("sll"," $t9,","$t9,","",change(2),"");
			generate("addu"," $t8,","$t8,","$t9","","");
			generate("lw"," $t"+change(remainder)+",","","",change(0),"($t8)");
		}else{
			generate("li"," $t8,","","",change(inreg->offset+4),"");
			generate("li"," $t9,",change(index1),"","","");
			generate("sll"," $t9,","$t9,","",change(2),"");
			generate("addu"," $t8,","$t8,","$t9","","");
			generate("sub"," $t8,","$sp,","$t8","","");
			generate("lw"," $t"+change(remainder)+",","","",change(0),"($t8)");
		}
		struct node * p = new node();
		p->big = inreg;
		p->small = index1;
		temppool[remainder] = *p;
		return remainder;
	}*/
}


//根据MIPS规则要求，生成MIPS汇编指令
void mipscode::generate(string instr,string rd,string rs,string rt,string immediate,string base){
	struct instruction* p = new instruction();
	p->instr = instr;
	p->rd = rd;
	p->rs = rs;
	p->rt = rt;
	p->immediate = immediate;
	p->base = base;
	mips_mars.push_back(*p);
	fprintf(for_test,"%s\n",(instr+rd+rs+rt+immediate+base).c_str());
}
//将数字转化为字符串
string mipscode::change(int i){
	stringstream temp_name;
	temp_name<<i;
	return temp_name.str();
}

//将中间代码根据instr分为四类
void mipscode::branch(MiddleItem* in){
	in->toprint(for_test);
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
//根据输入的四元式规则生成目标代码
//add sub mul divv pset nset oar iar para push call ret scf prf
void mipscode::mipgenerate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2){
	static int pushnum = 0;
	static int deef = 0;
	switch(instr){
	//并不是fsset属于这种四元式，而是在fsset时需要将这种四元式generate中的pushnum清零
	case fsset:{
		pushnum = 0;
		if(deef==0){
			save_all();
			clear_all();//其实没有必要，因为是刚开始
			generate("move"," $fp,","$sp","","","");//move $fp,$sp\nj main\nnop\n
			generate("j","","",""," main","");
			generate("nop","","","","","");
			deef++;
		}
		break;
			   }
	case prf:{
		//addr==-3  字符串
		if(des->addr==-3){
			generate("la"," $a0,",des->name,"","","");
			generate("li"," $v0,","","",change(4),"");
			generate("syscall","","","","","");
		}else{
			int glo = search(globalpool,des);
			if(glo==-1){
				glo = search(temppool,des);
				if(glo==-1){
					glo = eliminate(des,nullptr,nullptr);
					generate("move"," $a0,","$t"+change(glo),"","","");
				}else{
					generate("move"," $a0,","$t"+change(glo),"","","");
				}
			}else{
				generate("move"," $a0,","$s"+change(glo),"","","");
			}
			///////
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
		///////////////////////////////
		int glo = search(globalpool,des);
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,nullptr,nullptr);
				generate("move"," $t"+change(glo)+",","$v0","","","");
			}else{
				generate("move"," $t"+change(glo)+",","$v0","","","");
			}
		}else{
			generate("move"," $s"+change(glo)+",","$v0","","","");
		}
		///////////////////////////////
		break;
	}
	case ret:{
		//printf("{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}\n");
		//printf(now_func.c_str());
		//printf("\n");
		if(now_func=="main"){
			save_all();
			clear();//一定不能把全局寄存器清空，否则就没有优化效果了
			generate("j"," mainend","","","","");
			generate("nop","","","","","");//主函数直接跳到mainend即可
		}else{
			if(des==nullptr){//无返回值
				save_all();
				clear();
				generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
				generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
				generate("jr"," $ra","","","","");
				generate("nop","","","","","");
			}else{//有返回值
				int glo = search(globalpool,des);
				if(glo==-1){
					glo = search(temppool,des);
					if(glo==-1){
						glo = eliminate(des,nullptr,nullptr);
						generate("move"," $v0,","$t"+change(glo),"","","");
					}else{
						generate("move"," $v0,","$t"+change(glo),"","","");
					}
				}else{
					generate("move"," $v0,","$s"+change(glo),"","","");
				}
				//////////////////////////////////
				//generate("lw"," $v0,","","","-"+change(des->offset+4),"($sp)");
				save_all();
				clear();//因为save的时候如果不清空，则在进入新的基本块时先save后，会将脏数据覆盖到内存中相应位置
				//需要保证在进入每一个基本块时寄存器池都是空的
				//必须在下面两条指令之前，因为lw $sp会修改sp，但是save()函数会用到$sp
				generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
				generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
				//因为save的时候如果不清空，则在进入新的基本块时先save后，会将脏数据覆盖到内存中相应位置
				//需要保证在进入每一个基本块时寄存器池都是空的
				generate("jr"," $ra","","","","");
				generate("nop","","","","","");
			}
		}
		break;
	}
	case call:{
		generate("move",""," $t8,","$sp","","");
		generate("li"," $t9,","","",change(nowsize+4+4),"");
		generate("sub"," $t8,","$t8,","$t9","","");////////////////////////////////cha
		generate("move"," $fp,","$t8","","","");//计算出进入的函数的活动记录的基地址，存入$fp中
		save_all();
		clear_all();
		if(des==nullptr){
			generate("jal"," ",src1->name.c_str(),"","","");
			generate("nop","","","","","");
			assign_s_reg();//本质上是根据顺序重新由栈中load
			//clear();
		}//没有返回值调用
		else{
			generate("jal"," ",src1->name.c_str(),"","","");
			generate("nop","","","","","");
			//clear();
			assign_s_reg();//本质上是根据顺序重新由栈中load
			int glo = search(globalpool,des);
			if(glo==-1){
				glo = search(temppool,des);
				if(glo==-1){
					glo = eliminate(des,nullptr,nullptr);
					generate("move"," $t"+change(glo)+",","$v0","","","");
				}else{
					generate("move"," $t"+change(glo)+",","$v0","","","");
				}
			}else{
				generate("move"," $s"+change(glo)+",","$v0","","","");
			}
			////////
			//generate("sw"," $v0,","","","-"+change(des->offset+4),"($sp)");
		}//有返回值调用
		pushnum = pushnum-(src1->length);/////////////代表了参数在新建的活动记录中从头位置开始存储
		break;
	}
	case push:{
		int glo = search(globalpool,des);
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,nullptr,nullptr);
				generate("sw"," $t"+change(glo)+",","","","-"+change(nowsize+4+4+4+pushnum*4),"($sp)");
			}else{
				generate("sw"," $t"+change(glo)+",","","","-"+change(nowsize+4+4+4+pushnum*4),"($sp)"); 
			}
		}else{
			generate("sw"," $s"+change(glo)+",","","","-"+change(nowsize+4+4+4+pushnum*4),"($sp)"); 
		}
		pushnum++;
		break;
    }
	case para:{
		//
		//printf("arrive para\n");
		break;
	}
	case iar:{//向数组里写入
		if(src1->level==level_global){
			int glo = search(globalpool,src2);
			string reg_src2;
			if(glo==-1){
				glo = search(temppool,src2);
				if(glo==-1){
					glo = eliminate(src2,des,nullptr);
					generate("move"," $t9,","$t"+change(glo),"","","");
				}else{
					generate("move"," $t9,","$t"+change(glo),"","","");
				}
			}else{
				generate("move"," $t9,","$s"+change(glo),"","","");
			}

			
			generate("la"," $t8,",src1->name,"","","");
			generate("sll"," $t9,","$t9,","",change(2),"");
			generate("addu"," $t8,","$t8,","$t9","","");
			
			glo = search(globalpool,des);
			string reg_des;
			if(glo==-1){
				glo = search(temppool,des);
				if(glo==-1){
					glo = eliminate(des,src2,nullptr);
					reg_des = "$t"+change(glo);
				}else{
					reg_des = "$t"+change(glo);
				}
			}else{
				reg_des = "$s"+change(glo);
			}
			generate("sw"," "+reg_des+",","","",change(0),"($t8)");
		}
		else{
			int glo = search(globalpool,src2);
			string reg_src2;
			if(glo==-1){
				glo = search(temppool,src2);
				if(glo==-1){
					glo = eliminate(src2,des,nullptr);
					generate("move"," $t9,","$t"+change(glo),"","","");
				}else{
					generate("move"," $t9,","$t"+change(glo),"","","");
				}
			}else{
				generate("move"," $t9,","$s"+change(glo),"","","");
			}

			generate("li"," $t8,","","",change(src1->offset+4),"");
			generate("sll"," $t9,","$t9,","",change(2),"");
			generate("addu"," $t8,","$t8,","$t9","","");
			generate("sub"," $t8,","$sp,","$t8","","");
			glo = search(globalpool,des);
			string reg_des;
			if(glo==-1){
				glo = search(temppool,des);
				if(glo==-1){
					glo = eliminate(des,nullptr,nullptr);
					reg_des = "$t"+change(glo);
				}else{
					reg_des = "$t"+change(glo);
				}
			}else{
				reg_des = "$s"+change(glo);
			}
			generate("sw"," "+reg_des+",","","",change(0),"($t8)");
		}
		break;
	}
	case oar:{
		if(src1->level==level_global){
			int glo = search(globalpool,src2);
			string reg_src2;
			if(glo==-1){
				glo = search(temppool,src2);
				if(glo==-1){
					glo = eliminate(src2,des,nullptr);
					generate("move"," $t9,","$t"+change(glo),"","","");
				}else{
					generate("move"," $t9,","$t"+change(glo),"","","");
				}
			}else{
				generate("move"," $t9,","$s"+change(glo),"","","");
			}

			generate("la"," $t8,",src1->name,"","","");
			generate("sll"," $t9,","$t9,","",change(2),"");
			generate("addu"," $t8,","$t8,","$t9","","");
			generate("lw"," $t9,","","",change(0),"($t8)");
			glo = search(globalpool,des);
			string reg_des;
			if(glo==-1){
				glo = search(temppool,des);
				if(glo==-1){
					glo = eliminate(des,src2,nullptr);
					reg_des = "$t"+change(glo);
				}else{
					reg_des = "$t"+change(glo);
				}
			}else{
				reg_des = "$s"+change(glo);
			}
			generate("move"," "+reg_des+",","$t9","","","");
		}else{
			int glo = search(globalpool,src2);
			string reg_src2;
			if(glo==-1){
				glo = search(temppool,src2);
				if(glo==-1){
					glo = eliminate(src2,des,nullptr);
					generate("move"," $t9,","$t"+change(glo),"","","");
				}else{
					generate("move"," $t9,","$t"+change(glo),"","","");
				}
			}else{
				generate("move"," $t9,","$s"+change(glo),"","","");
			}

			generate("li"," $t8,","","",change(src1->offset+4),"");
			generate("sll"," $t9,","$t9,","",change(2),"");
			generate("addu"," $t8,","$t8,","$t9","","");
			generate("sub"," $t8,","$sp,","$t8","","");
			generate("lw"," $t9,","","",change(0),"($t8)");
			glo = search(globalpool,des);
			string reg_des;
			if(glo==-1){
				glo = search(temppool,des);
				if(glo==-1){
					glo = eliminate(des,nullptr,nullptr);
					reg_des = "$t"+change(glo);
				}else{
					reg_des = "$t"+change(glo);
				}
			}else{
				reg_des = "$s"+change(glo);
			}
			generate("move"," "+reg_des+",","$t9","","","");
		}
		break;
	}
	case nset:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,des,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		///////
		glo = search(globalpool,des);
		string reg_des;
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,src1,nullptr);
				reg_des = "$t"+change(glo);
			}else{
				reg_des = "$t"+change(glo);
			}
		}else{
			reg_des = "$s"+change(glo);
		}
		generate("sub"," "+reg_des+",","$0,",reg_src1,"","");
		break;
	}
	case pset:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,des,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		///////
		glo = search(globalpool,des);
		string reg_des;
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,src1,nullptr);
				reg_des = "$t"+change(glo);
			}else{
				reg_des = "$t"+change(glo);
			}
		}else{
			reg_des = "$s"+change(glo);
		}
		generate("move"," "+reg_des+",",reg_src1,"","","");
		break;
	}
	case add:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,des,src2);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,des,src1);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		//
		glo = search(globalpool,des);
		string reg_des;
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,src1,src2);
				reg_des = "$t"+change(glo);
			}else{
				reg_des = "$t"+change(glo);
			}
		}else{
			reg_des = "$s"+change(glo);
		}
		generate("addu"," "+reg_des+",",reg_src1+",",reg_src2,"","");
		break;
	}
	case sub:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,des,src2);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,des,src1);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		//
		glo = search(globalpool,des);
		string reg_des;
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,src1,src2);
				reg_des = "$t"+change(glo);
			}else{
				reg_des = "$t"+change(glo);
			}
		}else{
			reg_des = "$s"+change(glo);
		}
		generate("sub"," "+reg_des+",",reg_src1+",",reg_src2,"","");
		break;
	}
	case mul:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,des,src2);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,des,src1);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		glo = search(globalpool,des);
		string reg_des;
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,src1,src2);
				reg_des = "$t"+change(glo);
			}else{
				reg_des = "$t"+change(glo);
			}
		}else{
			reg_des = "$s"+change(glo);
		}
		generate("mult",""," "+reg_src1+",",reg_src2,"","");
		generate("mflo"," "+reg_des,"","","","");
		break;
	}
	case divv:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,des,src2);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,des,src1);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		glo = search(globalpool,des);
		string reg_des;
		if(glo==-1){
			glo = search(temppool,des);
			if(glo==-1){
				glo = eliminate(des,src1,src2);
				reg_des = "$t"+change(glo);
			}else{
				reg_des = "$t"+change(glo);
			}
		}else{
			reg_des = "$s"+change(glo);
		}
		generate("div",""," "+reg_src1+",",reg_src2,"","");
		generate("mflo"," "+reg_des,"","","","");
		break;
	}
	}//switch的括号
}
//_neq _beq _leq _lss _geq _gtr toset j fsset fdset
void mipscode::mipgenerate(middleinstr instr,string label,struct tablenode * src1,struct tablenode * src2){
	switch(instr){
	case _neq:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,src2,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,src1,nullptr);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		save();
		clear();
		generate("bne"," "+reg_src1+",",reg_src2+",",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _beq:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,src2,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,src1,nullptr);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		save();
		clear();
		generate("beq"," "+reg_src1+",",reg_src2+",",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _leq:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,src2,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,src1,nullptr);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		generate("sub"," $t8,",reg_src1+",",reg_src2,"","");//save中不能使用$t8是因为t8中存储着运算结果，因此用$k1代替全局地址
		save();
		clear();
		generate("blez"," $t8,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _lss:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,src2,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,src1,nullptr);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		generate("sub"," $t8,",reg_src1+",",reg_src2,"","");
		save();
		clear();
		generate("bltz"," $t8,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _geq:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,src2,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,src1,nullptr);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		generate("sub"," $t8,",reg_src1+",",reg_src2,"","");
		save();
		clear();
		generate("bgez"," $t8,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case _gtr:{
		int glo = search(globalpool,src1);
		string reg_src1;
		if(glo==-1){
			glo = search(temppool,src1);
			if(glo==-1){
				glo = eliminate(src1,src2,nullptr);
				reg_src1 = "$t"+change(glo);
			}else{
				reg_src1 = "$t"+change(glo);
			}
		}else{
			reg_src1 = "$s"+change(glo);
		}
		////
		glo = search(globalpool,src2);
		string reg_src2;
		if(glo==-1){
			glo = search(temppool,src2);
			if(glo==-1){
				glo = eliminate(src2,src1,nullptr);
				reg_src2 = "$t"+change(glo);
			}else{
				reg_src2 = "$t"+change(glo);
			}
		}else{
			reg_src2 = "$s"+change(glo);
		}
		generate("sub"," $t8,",reg_src1+",",reg_src2,"","");
		save();
		clear();
		generate("bgtz"," $t8,","",label,"","");
		generate("nop","","","","","");
		break;
			  }
	case toset:{
		save();
		clear();
		generate("",label,":","","","");
		break;
			   }
	case endset:{
		save();
		clear();
		generate("",label,":","","","");
		break;
				}
	case j:{
		save();
		clear();
	    generate("j ",label,"","","","");
		generate("nop","","","","","");
		break;
		   }
	case fsset:{
		now_func = label;//记录当前是那个函数在生成目标代码
		//printf("************+++++++++++++***************\n");
		//printf(label.c_str());
		//printf("\n");
		//printf(now_func.c_str());
		save_all();
		clear_all();
		generate("",label,":","","","");
		generate("sw"," $sp,","","","-"+change(nowsize+8),"($fp)");
		generate("move"," $sp,","$fp","","","");
		generate("sw"," $ra,","","","-"+change(nowsize+4),"($sp)");
		assign_s_reg();//在每个函数的头部对全局寄存器进行一次分配
		break;
			   }
	case fdset:{
		//printf("&&&&&&^^^^^^^^^^^&&&&&&&&&&&&\n");
		//printf(now_func.c_str());
		//printf("\n");
		save_all();
		clear_all();
		if(now_func=="main"){
			generate("j"," mainend","","","","");
			generate("nop","","","","","");//主函数直接跳到mainend即可
		}
		else{
			generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
			generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
			generate("jr"," $ra","","","","");
			generate("nop","","","","","");
		}
		//上面四条命令用于无return语句的函数，因此在某些有return语句的函数中可能不会执行
		generate("",label,":","","","");
		break;
			   }
	}
}
//eqint
void mipscode::mipgenerate(middleinstr instr,struct tablenode * des,int a){
	int glo = search(globalpool,des);
	string reg_des;
	if(glo==-1){
		glo = search(temppool,des);
		if(glo==-1){
			glo = eliminate(des,nullptr,nullptr);
			reg_des = "$t"+change(glo);
		}else{
			reg_des = "$t"+change(glo);
		}
	}else{
		reg_des = "$s"+change(glo);
	}
	//////////////////////
	generate("li"," "+reg_des+",","","",change(a),"");
}
//eqstr
void mipscode::mipgenerate(middleinstr instr,struct tablenode * des,string constr){
	data.push_back(des->name+": .asciiz \""+constr+"\"");
}
















