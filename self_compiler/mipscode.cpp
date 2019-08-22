#include "mipscode.h"
#include <sstream>
#include <algorithm>
//����ͬ��ʽ��MIPS�������ۺ�Ϊһ�ָ�ʽ�������Ҫ���м����תĿ�����ʱ������д�ú�������

//���ּĴ������д���Ƿ��ű��нڵ��ָ������Ӧȫ����ֲ����������add
mipscode::mipscode(){
	for(int i=0;i<8;i++){
		globalpool.push_back(nullptr);
	}//ȫ�ּĴ���$s0---$s7
	for(int i=0;i<8;i++){
		temppool.push_back(nullptr);
	}//��ʱ�Ĵ���$t0---$t7,$t8,$t9
	for_test = fopen("middle_mips.txt","w");
}

void mipscode::clear(){
	/*for(int i=0;i<8;i++){
		globalpool[i] = nullptr;
	}//ȫ�ּĴ���$s0---$s7*/
	for(int i=0;i<8;i++){
		temppool[i] = nullptr;
	}//��ʱ�Ĵ���$t0---$t7,$t8,$t9
}

void mipscode::clear_all(){
	for(int i=0;i<8;i++){
		globalpool[i] = nullptr;
	}//ȫ�ּĴ���$s0---$s7
	for(int i=0;i<8;i++){
		temppool[i] = nullptr;
	}//��ʱ�Ĵ���$t0---$t7,$t8,$t9
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
	}//ȫ�ּĴ���$s0---$s7*/
	for(int i=0;i<8;i++){
		p = temppool.at(i);
		if(p==nullptr){continue;}
		if(p->level==level_global){
			generate("la"," $k1,",p->name,"","","");
			generate("sw"," $t"+change(i)+",","","",change(0),"($k1)");
		}else{
			generate("sw"," $t"+change(i)+",","","","-"+change(p->offset+4),"($sp)");
		}
	}//��ʱ�Ĵ���$t0---$t7,$t8,$t9
}
//�ں�������ʱ���øú���������ȫ�ֺ���ʱ�Ĵ���
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
	}//ȫ�ּĴ���$s0---$s7
	for(int i=0;i<8;i++){
		p = temppool.at(i);
		if(p==nullptr){continue;}
		if(p->level==level_global){
			generate("la"," $k1,",p->name,"","","");
			generate("sw"," $t"+change(i)+",","","",change(0),"($k1)");
		}else{
			generate("sw"," $t"+change(i)+",","","","-"+change(p->offset+4),"($sp)");
		}
	}//��ʱ�Ĵ���$t0---$t7,$t8,$t9
}

//��ÿ�������Ŀ�ͷΪ��ǰ��������ȫ�ּĴ���
//�������ü�����Ҳ��ֻ��Ծֲ����ű��еı�������ȫ�ּĴ�����global���з���Ҳ����ʹ�����ͳ�Ƶ��ǲ������������Ż���
/*
void mipscode::assign_s_reg(){
	//localtable
	if(localtable->symbollist.size()==0){return;}
	int i = 0;
	int outcome = 0;//�Ѿ������ȥ��s�Ĵ����ĸ���
	for(;i<localtable->symbollist.size();i++){
		if(localtable->symbollist.at(i).addr<0){
			continue;
		}
		if(localtable->symbollist.at(i).length>0){
			continue;
		}//���������鲻����Ĵ���
		//Ϊ��i�����$s(outcome)
		if(localtable->symbollist.at(i).level==level_global){
			generate("la"," $t8,",localtable->symbollist.at(i).name,"","","");
			generate("lw"," $s"+change(outcome)+",","","",change(0),"($t8)");
		}else{
			generate("lw"," $s"+change(outcome)+",","","","-"+change(localtable->symbollist.at(i).offset+4),"($sp)");
		}
		//�޸ļĴ������еļ�¼
		globalpool[outcome] = &(localtable->symbollist.at(i));
		outcome++;
		if(outcome>7){
			break;
		}
	}
}*/

//����ȽϺ���
bool compare_node(struct forupper upperx,struct forupper uppery){
	if(upperx.copynode.usesum>uppery.copynode.usesum){
		return true;
	}
	return false;
}

//����ע�Ͳ�����û�в������ü���ʱ���������ȫ�������ͬ���ĺ���
void mipscode::assign_s_reg(){
	//localtable
	if(localtable->symbollist.size()==0){return;}
	int i = 0;
	int outcome = 0;//�Ѿ������ȥ��s�Ĵ����ĸ���
	vector<struct forupper> transcp;
	//�Ծֲ����ű�����������ڻ��ж�ԭ���ָ�����ã�Ϊ��ֹ�������Ը���һ��������������
	for(;i<localtable->symbollist.size();i++){
		if(localtable->symbollist.at(i).addr<0){
			continue;
		}
		if(localtable->symbollist.at(i).length>0){
			continue;
		}//���������鲻����Ĵ���
		struct forupper* copybab = new forupper();
		copybab->index = i;
		copybab->copynode = localtable->symbollist.at(i);
		transcp.push_back(*copybab);
	}
	sort(transcp.begin(),transcp.end(),compare_node);
	//��������˶Ը����ĸ���
	for(int j=0;j<transcp.size();j++){
		int realindex;
		realindex = transcp.at(j).index;
		if(localtable->symbollist.at(realindex).level==level_global){
			generate("la"," $t8,",localtable->symbollist.at(realindex).name,"","","");
			generate("lw"," $s"+change(outcome)+",","","",change(0),"($t8)");
		}else{
			generate("lw"," $s"+change(outcome)+",","","","-"+change(localtable->symbollist.at(realindex).offset+4),"($sp)");
		}
		//�޸ļĴ������еļ�¼
		globalpool[outcome] = &(localtable->symbollist.at(realindex));
		outcome++;
		if(outcome>7){
			break;
		}
	}
}

//�ж�ĳһ�����ڵ�ǰ�Ĵ��������Ƿ���ڣ������ڷ���-1�������򷵻ؼĴ����ı��
int mipscode::search(vector<struct tablenode*> pool,struct tablenode* goal){
	for(int i=0;i<pool.size();i++){
		if(pool.at(i)==goal){
			return i;
		}
	}
	return -1;
}

//����ʱ�Ĵ������е�ĳһ�����޳���������¼�����������޳�����üĴ�������ʱ�Ĵ���$t0---$t7��
//�޳���inreg����ʾ�ı������Ѿ�����MIPS�Ĵ����У���������loadָ��
//��ǰջ�Ļ���ַ��$sp
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
		//������¼��
		if(elimi!=nullptr){
			if(elimi->level==level_global){
				generate("la"," $t8,",elimi->name,"","","");
				generate("sw"," $t"+change(remainder)+",","","",change(0),"($t8)");
			}else{
				generate("sw"," $t"+change(remainder)+",","","","-"+change(elimi->offset+4),"($sp)");
			}
		}
		//�ӻ��¼��ȡ��
		if(inreg->level==level_global){
			generate("la"," $t8,",inreg->name,"","","");
			generate("lw"," $t"+change(remainder)+",","","",change(0),"($t8)");
		}else{
			generate("lw"," $t"+change(remainder)+",","","","-"+change(inreg->offset+4),"($sp)");
		}
		//�޸ļĴ������еļ�¼
		temppool[remainder] = inreg;
		return remainder;
	/*else{//inreg�����飬��Ҫ����õ��Ĵ������������еĵ�index���0��ʼ�������±꣩
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
			}else{//���飬��Ҫ����index
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
		//��inreg index1����Ĵ���$t(remainder)��
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


//����MIPS����Ҫ������MIPS���ָ��
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
//������ת��Ϊ�ַ���
string mipscode::change(int i){
	stringstream temp_name;
	temp_name<<i;
	return temp_name.str();
}

//���м�������instr��Ϊ����
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
//�����������Ԫʽ��������Ŀ�����
//add sub mul divv pset nset oar iar para push call ret scf prf
void mipscode::mipgenerate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2){
	static int pushnum = 0;
	static int deef = 0;
	switch(instr){
	//������fsset����������Ԫʽ��������fssetʱ��Ҫ��������Ԫʽgenerate�е�pushnum����
	case fsset:{
		pushnum = 0;
		if(deef==0){
			save_all();
			clear_all();//��ʵû�б�Ҫ����Ϊ�Ǹտ�ʼ
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
			clear();//һ�����ܰ�ȫ�ּĴ�����գ������û���Ż�Ч����
			generate("j"," mainend","","","","");
			generate("nop","","","","","");//������ֱ������mainend����
		}else{
			if(des==nullptr){//�޷���ֵ
				save_all();
				clear();
				generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
				generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
				generate("jr"," $ra","","","","");
				generate("nop","","","","","");
			}else{//�з���ֵ
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
				clear();//��Ϊsave��ʱ���������գ����ڽ����µĻ�����ʱ��save�󣬻Ὣ�����ݸ��ǵ��ڴ�����Ӧλ��
				//��Ҫ��֤�ڽ���ÿһ��������ʱ�Ĵ����ض��ǿյ�
				//��������������ָ��֮ǰ����Ϊlw $sp���޸�sp������save()�������õ�$sp
				generate("lw"," $ra,","","","-"+change(nowsize+4),"($sp)");
				generate("lw"," $sp,","","","-"+change(nowsize+8),"($sp)");
				//��Ϊsave��ʱ���������գ����ڽ����µĻ�����ʱ��save�󣬻Ὣ�����ݸ��ǵ��ڴ�����Ӧλ��
				//��Ҫ��֤�ڽ���ÿһ��������ʱ�Ĵ����ض��ǿյ�
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
		generate("move"," $fp,","$t8","","","");//���������ĺ����Ļ��¼�Ļ���ַ������$fp��
		save_all();
		clear_all();
		if(des==nullptr){
			generate("jal"," ",src1->name.c_str(),"","","");
			generate("nop","","","","","");
			assign_s_reg();//�������Ǹ���˳��������ջ��load
			//clear();
		}//û�з���ֵ����
		else{
			generate("jal"," ",src1->name.c_str(),"","","");
			generate("nop","","","","","");
			//clear();
			assign_s_reg();//�������Ǹ���˳��������ջ��load
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
		}//�з���ֵ����
		pushnum = pushnum-(src1->length);/////////////�����˲������½��Ļ��¼�д�ͷλ�ÿ�ʼ�洢
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
	case iar:{//��������д��
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
	}//switch������
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
		generate("sub"," $t8,",reg_src1+",",reg_src2,"","");//save�в���ʹ��$t8����Ϊt8�д洢���������������$k1����ȫ�ֵ�ַ
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
		now_func = label;//��¼��ǰ���Ǹ�����������Ŀ�����
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
		assign_s_reg();//��ÿ��������ͷ����ȫ�ּĴ�������һ�η���
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
















