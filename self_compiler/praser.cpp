#include "word_analyze.h"
#include "praser.h"
#include <iostream>
#include "errorhandler.h"
#include <sstream>
#include <vector>
//const int name_max = 50;///////////////////////////////////pset factor warn_notfit
/*
#define kind_const 1
#define kind_var 2
#define kind_para 3
#define kind_func 4

#define type_int 1
#define type_char 2
#define type_void 3

#define level_global 1
#define level_local 2
*/
using namespace std;
typedef map<string,tablemanage*>::value_type organize_form;

//�ֲ����ű���²���
void praser::transfer(string key){
	still.insert(organize_form(key,local));//����ǰ���ű�ŵ�still�д洢���Ժ������������´�������
	//local = tablemanage();//�½�һ���յ�local���ű�exrbrack
	local = new tablemanage();
}

//���������ű���Ѱ����Ϊgoal�ı�ʶ������������ָ��toset
struct tablenode * praser::useident(string goal){
	//����local���ű� local->symbollist
	for(int i=0;i<local->symbollist.size();i++){
		if(strcmp(goal.c_str(),local->symbollist.at(i).name.c_str())==0){
			if(cyclic==0){local->symbollist.at(i).usesum = local->symbollist.at(i).usesum+1;}
			else{local->symbollist.at(i).usesum = local->symbollist.at(i).usesum+5*cyclic;}
			/////����ʵ���˶Է��ű��б�������Ȩ�ص��޸ģ�����Ҳ�޸���Ϊ��ʵ��һ�£��Գ���ûӰ������Ϊ���ú�����usesum��
			return &(local->symbollist.at(i));
		}
	}
	//����global���ű� global.symbollist
	for(int i=0;i<global.symbollist.size();i++){
		if(strcmp(goal.c_str(),global.symbollist.at(i).name.c_str())==0){
			if(cyclic==0){global.symbollist.at(i).usesum = global.symbollist.at(i).usesum+1;}
			else{global.symbollist.at(i).usesum = global.symbollist.at(i).usesum+5*cyclic;}
			return &(global.symbollist.at(i));
		}
	}
	return nullptr;
}

//���ڱ��ʽ�м�����ı�ʾ������һ���ڵ�ָ��geq->
/*struct tablenode * praser::gentemp(){
	struct tablenode* p = new tablenode();
	static int temp_number = 0;
	string base = "gentemp_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	p->name = temp_name.str();
	p->addr = -1;//addr==-1��������ʱ�������м����������Դ�����е���ʱ����
	p->kind = kind_var;//�����м���ʱ����
	temp_number++;
	return p;
}*/

struct tablenode * praser::gentemp(){
	static int temp_number = 0;
	string base = "gentemp_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	local->table_insert(temp_name.str(),kind_var,type_int,0,level_local,0,-1);//��ʱ����ͳһ���ó�int����֮���������޸�
	temp_number++;
	return &(local->symbollist.at(local->symbollist.size()-1));
}
//���ڱ��ʽ�е����������ַ�����ident�����Ա����ã�Ϊ��ʹ��Ԫʽͳһ
/*struct tablenode * praser::genrep(int a){
	struct tablenode* p = new tablenode();
	static int temp_number = 0;
	string base = "genrep_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	p->name = temp_name.str();
	p->addr = -2;//addr==-2�����Ƕ����������ַ�������
	p->value = a;
	temp_number++;
	return p;
}*/

struct tablenode * praser::genrep(int a){
	static int temp_number = 0;
	string base = "genrep_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	local->table_insert(temp_name.str(),kind_var,type_int,a,level_local,0,-2);//��Ĭ�����ó�int֮���������޸�
	temp_number++;
	return &(local->symbollist.at(local->symbollist.size()-1));
}


//������ident��ʽ��ʾ�ַ�������pset call  fdset
/*struct tablenode * praser::genstr(string constr){
	struct tablenode* p = new tablenode();
	static int temp_number = 0;
	string base = "genstr_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	p->name = temp_name.str();
	p->addr = -3;//addr==-3�����Ƕ��ַ���������
	p->str = constr;
	temp_number++;
	return p;
}*/

struct tablenode * praser::genstr(string constr){
	static int temp_number = 0;
	string base = "genstr_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	global.table_insert(temp_name.str(),kind_var,type_char,0,level_local,0,-3);
	global.symbollist.at(global.symbollist.size()-1).str = constr;
	temp_number++;
	return &(global.symbollist.at(global.symbollist.size()-1));
}

//���ɱ�ǩ
string praser::setlabel(symbol ins){
	static int number = 0;
	if(ins==ifsy){
		string base = "label_if";
		stringstream temp_name;
		temp_name<<base<<number;
		number++;
		return temp_name.str();
	}
	if(ins==dosy){
		string base = "label_do";
		stringstream temp_name;
		temp_name<<base<<number;
		number++;
		return temp_name.str();
	}
	if(ins==casesy){
		string base = "label_case";
		stringstream temp_name;
		temp_name<<base<<number;
		number++;
		return temp_name.str();
	}
}

int praser::erjump(int error){
	//����;��ִ����ú�����sy==';'����eof
	if(error==1){
		for(;sy!=semicolon&&sy!=eofsy;){insymbol();}
		return 0;
	}
	//����;������һ����
	if(error==2){
		for(;sy!=semicolon&&sy!=eofsy;){insymbol();}
		if(sy==semicolon){insymbol();}
		return 0;
	}
	//����,����;�����ȵ�,�򷵻�1���ȵ��ֺ��򷵻�2
	if(error==3){
		for(;sy!=comma&&sy!=semicolon&&sy!=eofsy;){insymbol();}
		if(sy==comma){return 1;}
		else if(sy==semicolon){return 2;}
		else{return 0;}
	}
	//����)����һ����
	if(error==4){
		for(;sy!=rparent&&sy!=eofsy;){insymbol();}
		if(sy==rparent){insymbol();}
		return 0;
	}
	//����}�����
	if(error==5){
		for(;sy!=rrf&&sy!=eofsy;){insymbol();}
		if(sy==rrf){insymbol();}
		return 0;
	}
	//����)�����
	if(error==6){
		for(;sy!=rparent&&sy!=eofsy;){insymbol();}
	}
}

praser::praser(int x,FILE * show){
	hasfault = 0;
	cyclic = 0;//��ʼʱ����ѭ��Ƕ�׵Ĳ���Ϊ0
	now = x;
	output = show;//output���ڽ��м���Ϣ�����һ��ָ�����ļ�
	global = tablemanage();
	local = new tablemanage();
	gen = middlecode();
	error = errorhandler();
}

//��������        ::= �ۣ������ݣ��޷�������������
int praser::integer(){
	if(sy==zerocon){
		insymbol();
		return 0;
	}
	else if(sy==plu){
		insymbol();
		if(sy==unsignintcon){
			int inte_temp = sum;
			insymbol();
			return inte_temp;
		}
		else{
			hasfault = 1;
			error.out(exunsigned,lines);
			erjump(1);/////////////////////////////////////////////////////////////
			return 0;
		}
	}
	else if(sy==minu){
		insymbol();
		if(sy==unsignintcon){
			int inte_temp = sum;
			insymbol();
			return (-1*inte_temp);
		}
		else{
			hasfault = 1;
			error.out(exunsigned,lines);
			erjump(1);/////////////////////////////////////////////////////////////
			return 0;
		}
	}
	else{
		if(sy==unsignintcon){
			int inte_temp = sum;
			insymbol();
			return inte_temp;
		}
		else{
			hasfault = 1;
			error.out(exunsigned,lines);
			erjump(1);//////////////////////////////////////////////////////////////
			return 0;
		}
	}
}

//���������壾   ::=   int����ʶ��������������{,����ʶ��������������}| char����ʶ���������ַ���{,����ʶ���������ַ���}
void praser::constdef(){
	int type;
	int kind = kind_const;
	if(sy==intsy){
		for(;;){
			type = type_int;//��ʶ������ת�Ƶ�type
			insymbol();
			if(sy==ident){
				string temp = string(name);//��ʶ������ת�Ƶ��¶���temp
				insymbol();
				if(sy==becomes){
					insymbol();
					int temp_value = integer();
					struct tablenode * item1;
					if(now==analyze_global){
						if(global.table_insert(temp,kind,type,temp_value,level_global,0,lines)<0){
							hasfault = 1;
							error.out(redefine,lines);
						}
						item1 = global.table_search(temp);//���������Ƿ����ɹ�������һ����search��
					}else{
						if(local->table_insert(temp,kind,type,temp_value,level_local,0,lines)<0){
							hasfault = 1;
							error.out(redefine,lines);
						}
						item1 = local->table_search(temp);//���������Ƿ����ɹ�������һ����search��
					}
					gen.generate(eqint,item1,temp_value);///////////////////////////
					fprintf(output,"constdef--%d\n",lines);
					if(sy==comma){continue;}
					else{break;}
				}//������ű�
				else{
					hasfault = 1;
					error.out(constnotinit,lines);
					erjump(1);////////////////////////////////////////////////////////////////////////
					break;
				}
			}
			else{
				hasfault = 1;
				error.out(exident,lines);
				erjump(1);/////////////////////////////////////////////////////////////////////////////
				break;
			}
		}
	}
	else if(sy==charsy){
		for(;;){
			type = 	type_char;
			insymbol();
			if(sy==ident){
				string temp(name);
				insymbol();
				if(sy==becomes){
					insymbol();
					if(sy==charcon){
						struct tablenode * item1;
						if(now==analyze_global){
							if(global.table_insert(temp,kind,type,name[0],level_global,0,lines)<0){
								hasfault = 1;
								error.out(redefine,lines);
							}
							item1 = global.table_search(temp);//���������Ƿ����ɹ�������һ����search��
						}else{
							if(local->table_insert(temp,kind,type,name[0],level_local,0,lines)<0){
								hasfault = 1;
								error.out(redefine,lines);
							}
							item1 = local->table_search(temp);//���������Ƿ����ɹ�������һ����search��
						}
						gen.generate(eqint,item1,name[0]);/////////////////////////
						insymbol();
						fprintf(output,"constdef--%d\n",lines);
						if(sy==comma){continue;}
						else{break;}
					}else{
						hasfault = 1;
						error.out(excharcon,lines);
						erjump(1);/////////////////////////////////////////////////////////
						break;
					}
				}else{
					hasfault = 1;
					error.out(constnotinit,lines);
					erjump(1);///////////////////////////////////////////////////
					break;
				}
			}else{
				hasfault = 1;
				error.out(exident,lines);
				erjump(1);////////////////////////////////////////////////////////////////
				break;
			}
		}
	}else{
		hasfault = 1;
		error.out(exintorchar,lines);
		erjump(1);//////////////////////////////////////////////////////////////////////
	}
}

//������˵���� ::=  const���������壾;{ const���������壾;}
void praser::constdec(){
	for(;;){
		if(sy==constsy){
			insymbol();
			constdef();
			if(sy==semicolon){
				insymbol();
				continue;
			}else{
				hasfault = 1;
				error.out(exsemicolon,lines);
				erjump(2);////////////////////////////////////////////////////////////
				break;
			}
		}else{
			break;
		}
	}
}


//���������壾  ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]��) }
//�÷������ں����ڲ��������壬����ȫ�ֱ������������ø÷�����Ϊ�䲻�����ֱ�������ͺ�������
void praser::vardef(){
	int type;
	int kind = kind_var;
	int length;
	string temp;
	if(sy==intsy){
		type = type_int;
		for(;;){
			insymbol();
			if(sy==ident){
				fprintf(output,"vardef--%d\n",lines);
				temp = string(name);
				insymbol();
				if(sy==comma){
					if(local->table_insert(temp,kind_var,type,0,level_local,0,0)<0){
						hasfault = 1;
						error.out(redefine,lines);
					}
					continue;
				}
				else if(sy==lbrack){
					insymbol();
					if(sy==unsignintcon){
						length = sum;
						insymbol();
						if(sy==rbrack){
							if(local->table_insert(temp,kind_var,type,0,level_local,length,0)<0){
								hasfault = 1;
								error.out(redefine,lines);
							}
							insymbol();
							if(sy==comma){
								continue;
							}
							else{return;}
						}else{
							hasfault = 1;
							error.out(exrbrack,lines);
							int status = erjump(3);
							if(status==1){continue;}///////////////////////////////////
							else if(status==2){return;}////////////////////////////////////////////
							else{return;}///////////////////////////////////////////
						}
					}else{
						hasfault = 1;
						error.out(exunsigned,lines);
						int status = erjump(3);
						if(status==1){continue;}///////////////////////////////////
						else if(status==2){return;}////////////////////////////////////////////
						else{return;}/////////////
					}
				}
				else{
					if(local->table_insert(temp,kind_var,type,0,level_local,0,0)<0){
						hasfault = 1;
						error.out(redefine,lines);
					}
					return;
				}
			}else{
				hasfault = 1;
				error.out(exident,lines);
				int status = erjump(3);
				if(status==1){continue;}///////////////////////////////////
				else if(status==2){return;}////////////////////////////////////////////
				else{return;}/////////////
			}
		}
	}
	else if(sy==charsy){
		type = type_char;
		for(;;){
			insymbol();
			if(sy==ident){
				fprintf(output,"vardef--%d\n",lines);
				temp = string(name);
				insymbol();
				if(sy==comma){
					if(local->table_insert(temp,kind_var,type,0,level_local,0,0)<0){
						hasfault = 1;
						error.out(redefine,lines);
					}
					continue;
				}
				else if(sy==lbrack){
					insymbol();
					if(sy==unsignintcon){
						length = sum;
						insymbol();
						if(sy==rbrack){
							if(local->table_insert(temp,kind_var,type,0,level_local,length,0)<0){
								hasfault = 1;
								error.out(redefine,lines);
							}
							insymbol();
							if(sy==comma){
								continue;
							}
							else{return;}
						}else{
							hasfault = 1;
							error.out(exrbrack,lines);
							int status = erjump(3);
							if(status==1){continue;}///////////////////////////////////
							else if(status==2){return;}////////////////////////////////////////////
							else{return;}/////////////
						}
					}else{
						hasfault = 1;
						error.out(exunsigned,lines);
						int status = erjump(3);
						if(status==1){continue;}///////////////////////////////////
						else if(status==2){return;}////////////////////////////////////////////
						else{return;}/////////////
					}
				}
				else{
					if(local->table_insert(temp,kind_var,type,0,level_local,0,0)<0){
						hasfault = 1;
						error.out(redefine,lines);
					}
					return;
				}
			}else{
				hasfault = 1;
				error.out(exident,lines);
				int status = erjump(3);
				if(status==1){continue;}///////////////////////////////////
				else if(status==2){return;}////////////////////////////////////////////
				else{return;}/////////////
			}
		}
	}
	else{
		//(������voidsy�����ܱ���ֻ��ͨ������������һ��)	
	}
}

//������˵����  ::= ���������壾;{���������壾;}
void praser::vardec(){
	//����֮ǰ��һ��sy�Ѿ�����
	for(;;){
		if(sy==intsy||sy==charsy){
			vardef();
			if(sy==semicolon){
				insymbol();
				continue;
			}
			else{
				hasfault = 1;
				error.out(exsemicolon,lines);
				erjump(2);////////////////////////////////////////////
				break;
			}
		}
		else{
			return;//������ֱ�ӷ���
		}
	}
}

//����ȫ�ֱ����Ķ��壬�����ź�ı���������ű����ڲ��жϷֺţ�����program()�������жϷֺ�
void praser::global_var(int type){
	//��ʼ����sy==comma
	fprintf(output,"vardef--%d\n",lines);
	string temp;
	int length;
	for(;;){
		insymbol();
		if(sy==ident){
			fprintf(output,"vardef--%d\n",lines);
			temp = string(name);
			insymbol();
			if(sy==comma){
				if(global.table_insert(temp,kind_var,type,0,level_global,0,0)<0){
					hasfault = 1;
					error.out(redefine,lines);
				}
				//insert global table
				continue;
			}
			else if(sy==lbrack){
				insymbol();
				if(sy==unsignintcon){
					length = sum;
					insymbol();
					if(sy==rbrack){
						if(global.table_insert(temp,kind_var,type,0,level_global,length,0)<0){
							hasfault = 1;
							error.out(redefine,lines);
						}
						//insert global table
						insymbol();
						if(sy==comma){
							continue;
						}
						else{break;}
					}
					else{
						hasfault = 1;
						error.out(exrbrack,lines);
						int status = erjump(3);
						if(status==1){continue;}///////////////////////////////////
						else if(status==2){return;}////////////////////////////////////////////
						else{return;}/////////////
					}
				}
				else{
					hasfault = 1;
					error.out(exunsigned,lines);
					int status = erjump(3);
					if(status==1){continue;}///////////////////////////////////
					else if(status==2){return;}////////////////////////////////////////////
					else{return;}/////////////
				}
			}
			else{
				if(global.table_insert(temp,kind_var,type,0,level_global,0,0)<0){
					hasfault = 1;		
					error.out(redefine,lines);
				}
				break;
			}
		}
		else{
			hasfault = 1;
			error.out(exident,lines);
			int status = erjump(3);
			if(status==1){continue;}///////////////////////////////////
			else if(status==2){return;}////////////////////////////////////////////
			else{return;}/////////////
			break;
		}
	}
}

//��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ��������в�����������Ķ��壬���������ÿ���A[1]��ʽ
int praser::paratable(){
	int singles = 0;
	int type;
	for(;sy==intsy||sy==charsy;){
		if(sy==intsy){type=type_int;}
		else{type=type_char;}
		insymbol();
		if(sy==ident){
			string temp = string(name);
			struct tablenode * item1;
			if(local->table_insert(temp,kind_para,type,0,level_local,0,0)<0){
				hasfault = 1;
				error.out(redefine,lines);
			}
			//insert local table
			item1 = local->table_search(temp);//���������Ƿ����ɹ�������item1�������ǿ�ָ��
			gen.generate(para,item1,nullptr,nullptr);
		}
		else{
			hasfault = 1;
			error.out(exident,lines);
			erjump(4);/////////////////////////////////////////////////////////////
			return singles;
		}
		insymbol();
		singles++;
		if(sy==comma){
			insymbol();
			if(sy==intsy||sy==charsy){continue;}
			else{
				hasfault = 1;
				error.out(exintorchar,lines);
				erjump(4);/////////////////////////////////////////////////////////////
				return singles;
			}
		}
		else{break;}
	}
	if(sy==rparent){
		insymbol();
		fprintf(output,"parametertable--%d--singles::%d\n",lines,singles);
		return singles;
	}
	else{
		hasfault = 1;
		error.out(exrparent,lines);
		erjump(4);/////////////////////////////////////////////////////////////////////////
		return singles;
	}
}


//�����ӣ�    ::= ����ʶ����������ʶ������[�������ʽ����]������������|���ַ��������з���ֵ����������䣾|��(�������ʽ����)��
struct tablenode* praser::ele(int species){
	if(sy==ident){
		string temp = string(name);
		insymbol();
		if(sy==lbrack){
			struct tablenode* item1;
			struct tablenode* item2;
			struct tablenode* item3;
			insymbol();
			item2 = expression(species);//expression()�ڲ����ɽ������������ָ�뷵��
			if(item2==nullptr){
				hasfault = 1;
				error.out(expexp,lines);
				erjump(species);//////////////////////////////////////////////////////////////////
			}
			if(sy==rbrack){
				item1 = useident(temp);
				if(item2->issingle){
					if((item2->value)<0||(item2->value)>=item1->length){
						hasfault = 1;
						error.out(overboundary,lines);
					}
				}
				if(item1==nullptr){
					hasfault = 1;
					error.out(undefine,lines);
					erjump(species);///////////////////////////////////////////////////////////////////
				}else{
					if(item1->kind==kind_func){
						hasfault = 1;
						error.out(wrongment,lines);
					}
					insymbol();
					item3 = gentemp();
					item3->type = item1->type;//���ʽҲ����int��char����֮�ֵ�
					gen.generate(oar,item3,item1,item2);
					return item3;
				}
			}else{
				hasfault = 1;
				error.out(exrbrack,lines);
				erjump(species);///////////////////////////////////////////////////////////////////
			}
		}else if(sy==lparent){
			insymbol();//
			std::vector<struct tablenode *> push_record;
			for(;sy!=rparent;){
				//insymbol();
				//if(sy==rparent){break;}
				struct tablenode* item1;
				item1 = expression(species);
				if(item1==nullptr){
					hasfault = 1;
					error.out(expexp,lines);
					erjump(species);///////////////////////////////////////////////////////////////////
				}
				//gen.generate(push,item1,nullptr,nullptr);
				push_record.push_back(item1);
				if(sy==comma){insymbol();continue;}
				else{break;}
			}////�漰�ķ��ĸ�д����Ϊfirst���ཻ��Ϊ�ռ�
			for(int i=0;i<push_record.size();i++){//
				gen.generate(push,push_record.at(i),nullptr,nullptr);//
			}//
			if(sy==rparent){
				struct tablenode* item2 = useident(temp);
				struct tablenode* item3 = gentemp();
				if(item2!=nullptr){
					item3->type = item2->type;//����Ϊ����ʱ�����͵��ں���������
				}
				if(item2==nullptr){
					hasfault = 1;
					error.out(undefine,lines);
					return nullptr;
				}
				if(item3->type==type_void){
					hasfault = 1;
					error.out(noretvalue,lines);
				}
				if(item2->kind!=kind_func){
					hasfault = 1;
					error.out(wrongment,lines);
				}
				insymbol();
				gen.generate(call,item3,item2,nullptr);
				return item3;
			}
		}
		else{
			struct tablenode* item1 = useident(temp);
			if(item1==nullptr){
				hasfault = 1;
				error.out(undefine,lines);
				return item1;///changeddddddd
			}
			if(item1->kind==kind_const){
				item1->issingle = true;
			}////////////////////////////////////////////////////////////////////////////////
			return item1;
		}
	}
	else if(sy==lparent){
		insymbol();
		struct tablenode* item1;
		item1 = expression(species);//���Ӳ���Ϊ����˲�����()������ʽ�ı��ʽ
		if(sy==rparent){insymbol();return item1;}
		else{
			hasfault = 1;
			error.out(exrparent,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
		}
	}
	else if(sy==charcon){
		struct tablenode* item1 = genrep(name[0]);
		item1->type = type_char;//���ӵ�����Ҫ����������ֻҪ���Ӳ��������������
		gen.generate(eqint,item1,name[0]);/////////////////////////////////////////////////
		insymbol();
		return item1;
	}
	else if(sy==zerocon||sy==plu||sy==minu||sy==unsignintcon){
		int a = integer();//�з���ֵ����Ϊ������ֵ
		struct tablenode* item1 = genrep(a);
		item1->issingle = true;///////////////////////////////////////////////////////////
		gen.generate(eqint,item1,a);//////////////////////////////////////////////////////////////
		return item1;
	}
	else{
		hasfault = 1;
		error.out(exfactor,lines);
		erjump(species);///////////////////////////////////////////////////////////////////
	}
	return nullptr;
}

//���     ::= �����ӣ�{���˷�������������ӣ�}
struct tablenode* praser::vector(int species){
	struct tablenode * item1 = ele(species);
	if(item1==nullptr){
		hasfault = 1;
		error.out(exele,lines);
		erjump(species);///////////////////////////////////////////////////////////////////
	}
	for(;sy==times||sy==divs;){
		bool ismul = (sy==times)? true:false; 
		insymbol();
		struct tablenode * item2 = ele(species);
		if(item2==nullptr){
			hasfault = 1;
			error.out(exele,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
		}
		struct tablenode * item3 = gentemp();
		if(ismul){
			gen.generate(mul,item3,item1,item2);
		}else{
			gen.generate(divv,item3,item1,item2);
		}
		item1 = item3;
	}
	return item1;
}

//species==1Ϊ;���ñ��ʽ��==2Ϊ()�����˱��ʽ
//�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}//��ʽ��ͷ��+-Ϊֻ��Կ�ͷ��һ��ļӼ�
struct tablenode* praser::expression(int species){
	fprintf(output,"expression--%d\n",lines);
	struct tablenode * item1 = nullptr;
	if(sy==plu){
		insymbol();
		item1 = vector(species);
		if(item1==nullptr){
			hasfault = 1;
			error.out(exvec,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
		}
	}
	else if(sy==minu){
		insymbol();
		struct tablenode * item2 = vector(species);
		if(item2==nullptr){
			hasfault = 1;
			error.out(exvec,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
		}
		struct tablenode * item3 = gentemp();
		if(item2->issingle){
			item3->issingle = true;
			item3->value = (item2->value)*(-1);
		}
		gen.generate(nset,item3,item2,nullptr);
		item1 = item3;
	}
	else{
		item1 = vector(species);
		if(item1==nullptr){
			hasfault = 1;
			error.out(exvec,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
		}
	}
	for(;sy==plu||sy==minu;){
		bool isplus = (sy==plu)? true:false;
		insymbol();
		struct tablenode * item2 = vector(species);
		struct tablenode * item3 = gentemp();
		if(item2==nullptr){
			hasfault = 1;
			error.out(exvec,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
			break;

		}
		if(isplus){
			gen.generate(add,item3,item1,item2);
		}else{
			gen.generate(sub,item3,item1,item2);
		}
		item1 = item3;
	}
	return item1;
}

//��������    ::=  �����ʽ������ϵ������������ʽ���������ʽ�� //���ʽΪ0����Ϊ�٣�����Ϊ��
void praser::condition(string label,symbol operate){
	struct tablenode* item1 = expression(6);
	if(item1==nullptr){
		hasfault = 1;
		error.out(expexp,lines);
	}
	struct tablenode* item2;
	if(sy==eql||sy==neq||sy==gtr||sy==geq||sy==lss||sy==leq){
		symbol index = sy;
		insymbol();
		item2 = expression(6);
		if(item2==nullptr){
			hasfault = 1;
			error.out(expexp,lines);
		}
		if(operate==ifsy){
			switch(index){
			case eql:{gen.generate(_neq,label,item1,item2);break;}
			case neq:{gen.generate(_beq,label,item1,item2);break;}
			case gtr:{gen.generate(_leq,label,item1,item2);break;}
			case geq:{gen.generate(_lss,label,item1,item2);break;}
			case lss:{gen.generate(_geq,label,item1,item2);break;}
			case leq:{gen.generate(_gtr,label,item1,item2);break;}
			}//if���������ȡ��
		}else{
			switch(index){
			case eql:{gen.generate(_beq,label,item1,item2);break;}
			case neq:{gen.generate(_neq,label,item1,item2);break;}
			case gtr:{gen.generate(_gtr,label,item1,item2);break;}
			case geq:{gen.generate(_geq,label,item1,item2);break;}
			case lss:{gen.generate(_lss,label,item1,item2);break;}
			case leq:{gen.generate(_leq,label,item1,item2);break;}
			}//while�����������ȡ��
		}
	}else{
		item2 = genrep(0);
		gen.generate(eqint,item2,0);
		if(operate==ifsy){
			gen.generate(_beq,label,item1,item2);
		}else{
			gen.generate(_neq,label,item1,item2);
		}
	}
}

//��������䣾  ::=  if ��(������������)������䣾
int praser::ifment(){
	//��ʱsyΪifsy
	fprintf(output,"ifment-start--%d\n",lines);
	string label = setlabel(ifsy);
	gen.generate(toset,label,nullptr,nullptr);
	int what = 0;
	insymbol();
	if(sy==lparent){
		insymbol();
		condition(label+"end",ifsy);
		if(sy==rparent){
			insymbol();
			what = ments();
			gen.generate(endset,label+"end",nullptr,nullptr);
			fprintf(output,"ifment-end--%d\n",lines);
		}
		else{
			hasfault = 1;
			error.out(exrparent,lines);
			erjump(4);////////////////////////////////////////////////////////////////////
		}
	}
	else{
		hasfault = 1;
		error.out(exlparent,lines);
		erjump(4);////////////////////////////////////////////////////////////////////
	}
	return what;
}

//��ѭ����䣾   ::=  do����䣾while ��(������������)��
int praser::whilement(){
	//����ʱsy==dosy
	int what = 0;
	cyclic = cyclic+1;//����ѭ������1
	fprintf(output,"whilement-start--%d\n",lines);
	string label = setlabel(dosy);
	gen.generate(toset,label,nullptr,nullptr);
	insymbol();
	what = ments();
	if(sy==whilesy){
		insymbol();
		if(sy==lparent){
			insymbol();
			condition(label,whilesy);
			if(sy==rparent){
				fprintf(output,"whilement-end--%d\n",lines);
				gen.generate(endset,label+"end",nullptr,nullptr);
				cyclic = cyclic-1;//ѭ��������Ƕ�׼�1
				insymbol();
			}else{
				hasfault = 1;
				error.out(exrparent,lines);
				erjump(4);////////////////////////////////////////////////////////////////
			}
		}else{
			hasfault = 1;
			error.out(exlparent,lines);
			erjump(4);////////////////////////////////////////////////////////////////
		}
	}
	else{
		hasfault = 1;
		error.out(exwhile,lines);
		erjump(4);////////////////////////////////////////////////////////////////////////
	}
	return what;
}

//��������   ::=  ��������|���ַ���
//���������䣾  ::=  case��������������䣾
int praser::switchbranch(struct tablenode* item1,string end){
	fprintf(output,"switchbranchment-start--%d\n",lines);
	string label = setlabel(casesy);
	gen.generate(toset,label,nullptr,nullptr);
	int what = 0;
	if(sy==casesy){
		insymbol();
		struct tablenode* item2;
		if(sy==charcon){
			item2 = genrep(name[0]);
			item2->type = type_char;
			gen.generate(eqint,item2,name[0]);////////////////////////////////////////////////
			insymbol();
		}else if(sy==plu||sy==minu||sy==zerocon||sy==unsignintcon){
			int a = integer();//�з���ֵ��Ϊ�����Ĵ�С
			item2 = genrep(a);
			item2->type = type_int;
			gen.generate(eqint,item2,a);
		}
		else{
			hasfault = 1;
			error.out(caseafwrong,lines);
			erjump(2);//////////////////////////////////////////////////////////////////////////////////
			return what;
		}
		if(item1==nullptr){
			hasfault = 1;
			error.out(exnotnull,lines);
			erjump(2);//////////////////////////////////////////////////////////////////////////////////
		}else{
			if(item1->type!=item2->type){
				error.out(warn_notfit,lines);
			}
		}
		gen.generate(_neq,label+"end",item1,item2);
		if(sy==colon){
			insymbol();
			what = ments();
			gen.generate(j,end,nullptr,nullptr);
			gen.generate(endset,label+"end",nullptr,nullptr);
			fprintf(output,"switchbranchment-end--%d\n",lines);
		}else{
			hasfault = 1;
			error.out(excolon,lines);
			erjump(2);//////////////////////////////////////////////////////////////////////////////////
		}
	}
	return what;
}

//�������   ::=  ���������䣾{���������䣾}
int praser::switchtable(struct tablenode* item1){
	string end = setlabel(casesy);
	int what = 0;
	if(item1==nullptr){
		hasfault = 1;
		error.out(exnotnull,lines);
		erjump(2);////////////////////////////////////////////////////////////////
	}
	if(sy==casesy){
		if(switchbranch(item1,end)!=0){
			what = 1;
		}
	}else{
		hasfault = 1;
		error.out(excasesy,lines);
		erjump(2);////////////////////////////////////////////////////////////
	}
	for(;sy==casesy;){
		if(switchbranch(item1,end)!=0){
			what = 1;
		}
	}
	gen.generate(toset,end,nullptr,nullptr);
	return what;
}


//�������䣾  ::=  switch ��(�������ʽ����)�� ��{��������� ��}��
int praser::switchment(){
	//����ʱsy==switchsy
	int what = 0;
	fprintf(output,"switchment-start-%d\n",lines);
	insymbol();
	if(sy==lparent){
		insymbol();
		struct tablenode * item1 = expression(6);
		if(item1==nullptr){
			hasfault = 1;
			error.out(expexp,lines);
			erjump(5);////////////////////////////////////////////////////////////
			return what;
		}
		if(sy==rparent){
			insymbol();
			if(sy==llf){
				insymbol();
				what = switchtable(item1);
				if(sy==rrf){
					fprintf(output,"switchment-end-%d\n",lines);
					insymbol();
				}else{
					hasfault = 1;
					error.out(exrrf,lines);
					erjump(5);////////////////////////////////////////////////////////////
					return what;
				}
			}else{
				hasfault = 1;
				error.out(exllf,lines);
				erjump(5);////////////////////////////////////////////////////////////
				return what;
			}
		}else{
			hasfault = 1;
			error.out(exrparent,lines);
			erjump(5);////////////////////////////////////////////////////////////
			return what;
		}
	}else{
		hasfault = 1;
		error.out(exlparent,lines);
		erjump(5);////////////////////////////////////////////////////////////
		return what;
	}
	return what;
}

//��ֵ������   ::= �����ʽ��{,�����ʽ��}�����գ�
//���з���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
//���޷���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
//�����ʵ���漰�˶������ķ��ĸ�д
void praser::functionuse(string temp){
	//����ʱsy==lparent
	int parahow = 0;//Դ������ô��θ���
	struct tablenode * item1 = useident(temp);
	if(item1==nullptr){
		hasfault = 1;
		error.out(undefine,lines);
		erjump(4);
		return;
	}
	if(item1->kind!=kind_func){
		hasfault = 1;
		error.out(wrongment,lines);
		erjump(1);
		return;
	}
	if(sy==lparent){
		fprintf(output,"functionuse--%d\n",lines);
		insymbol();
		if(sy==rparent){
			if(item1==nullptr){
				hasfault = 1;
				error.out(undefine,lines);
				erjump(4);
				return;
			}
			if(item1->length!=0){
				hasfault = 1;
				error.out(wrongment,lines);
			}
			insymbol();
			gen.generate(call,nullptr,item1,nullptr);
			return;
		}
		std::vector<struct tablenode *> push_record;
		for(;;){
			struct tablenode * item2 = expression(6);
			if(item2==nullptr){
				hasfault = 1;
				error.out(expexp,lines);
				erjump(4);
				return;
			}
			parahow++;
			push_record.push_back(item2);//////
			//gen.generate(push,item2,nullptr,nullptr);
			if(sy==comma){insymbol();continue;}
			else{break;}
		}
		if(sy==rparent){
			if(item1==nullptr){
				hasfault = 1;
				error.out(undefine,lines);
				erjump(4);
				return;
			}
			if(item1->length!=parahow){
				hasfault = 1;
				error.out(wrongment,lines);
			}
			insymbol();
			for(int i=0;i<push_record.size();i++){//
				gen.generate(push,push_record.at(i),nullptr,nullptr);//
			}//
			gen.generate(call,nullptr,item1,nullptr);
			return;
		}
		else{
			hasfault = 1;
			error.out(exrparent,lines);
			erjump(4);
		}
	}
}
//����ֵ��䣾   ::=  ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
//�漰���ķ��ĸ�д����Ϊ�������ָ�ֵ�뺯������
void praser::assignment(string temp){
	//����ʱֻ����ident��һ��symbol,��sy��λ������=��[
	struct tablenode * item1 = useident(temp);
	///////
	if(item1==nullptr){
		hasfault = 1;
		error.out(undefine,lines);
		erjump(1);
		return;
	}
	///////
	if(sy==becomes){
		fprintf(output,"assignment--%d\n",lines);
		if(item1==nullptr){
			hasfault = 1;
			error.out(undefine,lines);
			erjump(1);
			return;
		}
		if(item1->kind==kind_const||item1->length>0||item1->kind==kind_func){//
			hasfault = 1;//
			error.out(wrongment,lines);//
		}//
		insymbol();
		struct tablenode * item2 = expression(1);
		if(item2==nullptr){
			hasfault = 1;
			error.out(expexp,lines);
			erjump(1);
			return;
		}else{
			gen.generate(pset,item1,item2,nullptr);
		}
	}
	else if(sy==lbrack){
		fprintf(output,"assignment--%d\n",lines);
		insymbol();
		struct tablenode * item2 = expression(1);
		if(item1->kind==kind_func||item1->length==0){
			hasfault = 1;
			error.out(wrongment,lines);
			erjump(1);
			return;
		}
		if(item2==nullptr){
			hasfault = 1;
			error.out(expexp,lines);
			erjump(1);
			return;
		}
		if(item2->issingle){
			if((item2->value)<0||(item2->value)>=item1->length){
				hasfault = 1;
				error.out(overboundary,lines);
			}
		}
		if(sy==rbrack){
			if(item1==nullptr){
				hasfault = 1;
				error.out(undefine,lines);
				erjump(1);
				return;
			}
			insymbol();
			if(sy==becomes){
				insymbol();
				struct tablenode * item3 = expression(1);
				if(item3==nullptr){
					hasfault = 1;
					error.out(expexp,lines);
					erjump(1);
					return;
				}
				gen.generate(iar,item3,item1,item2);
			}else{
				hasfault = 1;
				error.out(exbecomesy,lines);
				erjump(1);
				return;
			}
		}else{
			hasfault = 1;
			error.out(exrbrack,lines);
			erjump(1);
			return;
		}
	}
	else{
		hasfault = 1;
		error.out(wrongassign,lines);
		erjump(1);
		return;
	}

}

//������䣾    ::=  scanf ��(������ʶ����{,����ʶ����}��)��
void praser::readment(){
	fprintf(output,"readment--%d\n",lines);
	if(sy==scanfsy){
		insymbol();
		if(sy==lparent){
			insymbol();
			if(sy==ident){
				struct tablenode * item1 = useident(string(name));
				if(item1==nullptr){
					hasfault = 1;
					error.out(undefine,lines);
					erjump(4);
					return;
				}
				if(item1->kind==kind_func||item1->kind==kind_const||item1->length>0){
					hasfault = 1;
					error.out(wrongment,lines);
				}
				gen.generate(scf,item1,nullptr,nullptr);
				insymbol();
				for(;sy==comma;){
					insymbol();
					if(sy==ident){
						struct tablenode * item2 = useident(string(name));
						if(item2==nullptr){
							hasfault = 1;
							error.out(undefine,lines);
							erjump(4);
							return;
						}
						if(item2->kind==kind_func||item2->kind==kind_const||item2->length>0){
							hasfault = 1;
							error.out(wrongment,lines);
						}
						insymbol();
						gen.generate(scf,item2,nullptr,nullptr);
						continue;
					}
					else{
						hasfault = 1;
						error.out(exident,lines);
						erjump(4);
						return;
					}
				}
				if(sy==rparent){insymbol();return;}
				else{
					hasfault = 1;
					error.out(exrparent,lines);
					erjump(4);
					return;
				}
			}else{
				hasfault = 1;
				error.out(exident,lines);
				erjump(4);
				return;
			}
		}else{
			hasfault = 1;
			error.out(exlparent,lines);
			erjump(4);
			return;
		}
	}
}

//expression()������û��ƥ�䵽���ʽʱ�ᱨ��
//��д��䣾    ::=  printf��(�����ַ�����,�����ʽ����)��|printf ��(�����ַ�������)��|printf ��(�������ʽ����)��
void praser::writement(){
	fprintf(output,"writement--%d\n",lines);
	if(sy==printfsy){
		insymbol();
		if(sy==lparent){
			insymbol();
			if(sy==stringcon){
				struct tablenode * item1 = genstr(string(name));
				string for_s = string(name);
				//gen.generate(eqstr,item1,for_s);
				//gen.generate(prf,item1,nullptr,nullptr);
				insymbol();
				if(sy==comma){
					insymbol();
					struct tablenode * item2 = expression(6);
					if(item2==nullptr){
						hasfault = 1;
						error.out(expexp,lines);
						erjump(4);
						return;
					}
					if(sy==rparent){
						insymbol();
						gen.generate(eqstr,item1,for_s);
						gen.generate(prf,item1,nullptr,nullptr);
						gen.generate(prf,item2,nullptr,nullptr);
						return;
					}
					else{
						hasfault = 1;
						error.out(exrparent,lines);
						erjump(4);
						return;
					}
				}else{//ֻ���ַ���һ������
					if(sy==rparent){
						insymbol();
						gen.generate(eqstr,item1,for_s);
						gen.generate(prf,item1,nullptr,nullptr);
						return;
					}else{
						hasfault = 1;
						error.out(exrparent,lines);
						erjump(4);
						return;
					}
				}
			}
			else{
				struct tablenode * item1 =expression(6);
				if(item1==nullptr){
					hasfault = 1;
					error.out(expexp,lines);
					erjump(4);
					return;
				}
				gen.generate(prf,item1,nullptr,nullptr);
				if(sy==rparent){insymbol();return;}
				else{
					hasfault = 1;
					error.out(exrparent,lines);
					erjump(4);
					return;
				}
			}
		}else{
			hasfault = 1;
			error.out(exlparent,lines);
			erjump(4);
			return;
		}
	}
}

//��������䣾   ::=  return[��(�������ʽ����)��]
int praser::returnment(){
	fprintf(output,"returnment--%d\n",lines);
	if(sy==returnsy){
		insymbol();
		if(sy==lparent){
			insymbol();
			struct tablenode * item1 = expression(6);
			if(item1==nullptr){
				hasfault = 1;
				error.out(expexp,lines);
				erjump(4);
				return 0;
			}
			if(sy==rparent){insymbol();gen.generate(ret,item1,nullptr,nullptr);return 1;}
			else{
				hasfault = 1;
				error.out(exrparent,lines);
				erjump(4);
				return 0;
			}
		}else{
			gen.generate(ret,nullptr,nullptr,nullptr);/////////////////
			return 0;
		}
	}
}


//����䣾    ::= ��������䣾����ѭ����䣾��<������>|��{��������У���}�������з���ֵ����������䣾; 
//                      |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;����������䣾;
//�ò�������б�ʶ�������飬ÿ��ʹ�ñ�ʶ������Ҫ����Ƿ��ж���
//�����м����ʱֻ�迼����䲿��
int praser::ments(){
	int what = 0;
	if(sy==ifsy){what = ifment();}
	else if(sy==dosy){what = whilement();}
	else if(sy==switchsy){what = switchment();}
	else if(sy==llf){
		fprintf(output,"mentlists-start--%d\n",lines);
		insymbol();
		for(;sy==ifsy||sy==dosy||sy==switchsy||sy==llf||sy==ident||sy==scanfsy||sy==printfsy||sy==returnsy||sy==semicolon;){
			if(ments()!=0){
				what = 1;
			}
		}
		if(sy==rrf){fprintf(output,"mentlists-end--%d\n",lines);insymbol();}
		else{
			hasfault = 1;
			error.out(exrrf,lines);
			erjump(5);
		}
	}
	else if(sy==ident){
		string temp = string(name); 
		insymbol();
		if(sy==lparent){
			functionuse(temp);
		}
		else{
			assignment(temp);
		}
		if(sy==semicolon){insymbol();}
		else{
			hasfault = 1;
			error.out(exsemicolon,lines);
			erjump(2);
		}
	}//��/�޷���ֵ�������û��߸�ֵ���
	else if(sy==scanfsy){
		readment();
		if(sy==semicolon){insymbol();}
		else{
			hasfault = 1;
			error.out(exsemicolon,lines);
			erjump(2);
		}
	}
	else if(sy==printfsy){
		writement();
		if(sy==semicolon){insymbol();}
		else{
			hasfault = 1;
			error.out(exsemicolon,lines);
			erjump(2);
		}
	}
	else if(sy==semicolon){fprintf(output,"nullment--%d\n",lines);insymbol();}
	else if(sy==returnsy){
	    what = returnment();
		if(sy==semicolon){insymbol();}
		else{
			hasfault = 1;
			error.out(exsemicolon,lines);
			erjump(2);
		}
	}
	else{
		hasfault = 1;
		error.out(wrongment,lines);
		erjump(2);
	}
	return what;
}


//��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
//������У�   ::=������䣾��
int praser::complexment(){
	constdec();
	vardec();
	int what = 0;
	for(;sy==ifsy||sy==dosy||sy==switchsy||sy==llf||sy==ident||sy==scanfsy||sy==printfsy||sy==returnsy||sy==semicolon;){
		int retsum = ments();
		if(retsum==1){
			what = 1;
		}
	}
	return what;
}


//���ں����ķ������з���ֵ��������������ж��ķ��ĸ�д
//���������壾  ::=  ��(������������)�� ��{����������䣾��}������������ͷ�����ڸú�����ȷ������˴��ķ���ȥ��
void praser::functiondef(int type,string temp){
	fprintf(output,"functiondef--%d\n",lines);
	int length;
	int kind = kind_func;
	gen.generate(fsset,temp,nullptr,nullptr);
	if(sy==lparent){
		insymbol();
		length = paratable();
		//insert global table--->type temp kind length
		if(sy==llf){
			if(global.table_insert(temp,kind_func,type,0,level_global,length,0)<0){
				hasfault = 1;
				error.out(redefine,lines);
			}
			insymbol();
			int retsum = complexment();//����������
			if(type==type_void&&retsum!=0){
				hasfault = 1;
				error.out(noexretval,lines);
			}
			if((type==type_int||type==type_char)&&retsum==0){
				hasfault = 1;
				error.out(exbutnoretval,lines);
			}
			if(sy==rrf){
				gen.generate(fdset,temp+"end",nullptr,nullptr);//������transfer֮ǰ����Ϊ��˻���local��
				transfer(temp);//��ÿ����������ʱ����transfer()�������оֲ����ű�ĸ���
				insymbol();
				return;
			}else{
				hasfault = 1;
				error.out(exrrf,lines);
				erjump(5);
				return;
			}
		}
		else{
			hasfault = 1;
			error.out(exllf,lines);
			erjump(5);
			return;
		}
	}
	else{
		hasfault = 1;
		error.out(exlparent,lines);
		erjump(5);//����}����һ����
	}
}


//������    ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
void praser::program(){
	//����ó���ʱΪ�Ѿ�insymbol���ˣ��ȴ�ʹ��
	int type;
	int kind;
	int length;
	string temp;
	insymbol();
	for(;;){
		if(sy==constsy){
			constdec();
		}
		else{
			break;
		}
	}
	//������б���˵���ͺ���������﷨���������ڻ���ڻ��ݣ�������д�ķ�
	for(;sy==intsy||sy==charsy;){
		if(sy==intsy){type=type_int;}
		else{type=type_char;}
		insymbol();
		if(sy==ident){
			temp = string(name);
			insymbol();
			if(sy==comma){
				if(global.table_insert(temp,kind_var,type,0,level_global,0,0)<0){
					hasfault = 1;
					error.out(redefine,lines); 
				}
				//insert
				global_var(type);//�����ź�ı����������
				if(sy==semicolon){insymbol();continue;}
				else{
					hasfault = 1;
					error.out(exsemicolon,lines);
					erjump(2);
					break;
				}
			}
			else if(sy==lbrack){
				insymbol();
				if(sy==unsignintcon){
					length = sum;
					insymbol();
					if(sy==rbrack){
						if(global.table_insert(temp,kind_var,type,0,level_global,length,0)<0){
							hasfault = 1;
							error.out(redefine,lines);
						}
						insymbol();
						if(sy==comma){
							global_var(type);//�����ź�ı����������
							if(sy==semicolon){insymbol();continue;}
							else{
								hasfault = 1;
								error.out(exsemicolon,lines);
								erjump(2);
								break;
							}
						}
						else{
							if(sy==semicolon){insymbol();continue;}
							else{
								hasfault = 1;
								error.out(exsemicolon,lines);
								erjump(2);
								break;
							}
						}
					}
					else{
						hasfault = 1;
						error.out(exrbrack,lines);
						erjump(2);
						break;
					}
				}
				else{
					hasfault = 1;
					error.out(exunsigned,lines);
					erjump(2);
					break;
				}
			}
			else if(sy==lparent){
				now = analyze_local;//��ʱ�﷨��������ֲ�����
				functiondef(type,temp);////////
				break;//���ֺ���������֮��һ���������б����������������
			}
			else if(sy==semicolon){
				if(global.table_insert(temp,kind_var,type,0,level_global,0,0)<0){
					hasfault = 1;
					error.out(redefine,lines);
				}
				insymbol();
				continue;
			}
			else{
				hasfault = 1;
				error.out(globalvarwrong,lines);
				erjump(2);
				break;
			}
		}
		else{
			hasfault = 1;
			error.out(exident,lines);
			erjump(2);
			break;
		}
	}
	//����ȫ���Ǻ����ķ����������ڱ��������Ķ��壬�������������Ҫ�����ֲ����ű�
	now = analyze_local;//��ʱ���оֲ����﷨����
	for(;sy==voidsy||sy==intsy||sy==charsy;){
		if(sy==intsy){
			type = type_int;
			insymbol();
			if(sy==ident){
				temp = string(name);
				insymbol();
				functiondef(type,temp);
				continue;
			}
			else{
				hasfault = 1;
				error.out(exident,lines);
				erjump(5);
			}
		}
		else if(sy==charsy){
			type = type_char;
			insymbol();
			if(sy==ident){
				temp = string(name);
				insymbol();
				functiondef(type,temp);
				continue;
			}
			else{
				hasfault = 1;
				error.out(exident,lines);
				erjump(5);
			}
		}
		else{
			//�޷���ֵ����+������
			type = type_void;
			insymbol();
			if(sy==ident){
				temp = string(name);
				if(strcmp(name,"main")==0){
					//insert global table--->type temp 0
					fprintf(output,"mainfunction--%d\n",lines);
					gen.generate(fsset,"main",nullptr,nullptr);
					insymbol();
					if(sy==lparent){
						insymbol();
						if(sy==rparent){
							insymbol();
							if(sy==llf){
								insymbol();
								int retsum = complexment();
								if(retsum!=0){
									hasfault = 1;
									error.out(noexretval,lines);
								}
								if(sy==rrf){
									insymbol();
									gen.generate(fdset,"mainend",nullptr,nullptr);
									transfer("main");//
									break;
								}
								else{
									hasfault = 1;
									error.out(exrrf,lines);
									erjump(5);
									break;
								}
							}
							else{
								hasfault = 1;
								error.out(exllf,lines);
								erjump(5);
								break;
							}
						}
						else{
							hasfault = 1;
							error.out(exrparent,lines);
							erjump(5);
							break;
						}
					}else{
						hasfault = 1;
						error.out(exlparent,lines);
						erjump(5);
						break;
					}
					break;
				}
				else{
					insymbol();
					functiondef(type,temp);
					continue;
				}
			}
			else{
				hasfault = 1;
				error.out(exident,lines);
				erjump(5);
				break;
			}
		}
	}
	//////////////////////////////////////////////////�ϲ��ֽ���ʱsy==eofsy��Դ�ļ�����
	if(sy==eofsy){printf("compiler_success!");}
	else{printf("compiler_failure~");}
}

















