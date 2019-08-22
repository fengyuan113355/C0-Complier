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

//局部符号表更新操作
void praser::transfer(string key){
	still.insert(organize_form(key,local));//将当前符号表放到still中存储，以函数名索引，新创建对象
	//local = tablemanage();//新建一个空的local符号表exrbrack
	local = new tablemanage();
}

//在两个符号表中寻找名为goal的标识符并返回其结点指针toset
struct tablenode * praser::useident(string goal){
	//先找local符号表 local->symbollist
	for(int i=0;i<local->symbollist.size();i++){
		if(strcmp(goal.c_str(),local->symbollist.at(i).name.c_str())==0){
			if(cyclic==0){local->symbollist.at(i).usesum = local->symbollist.at(i).usesum+1;}
			else{local->symbollist.at(i).usesum = local->symbollist.at(i).usesum+5*cyclic;}
			/////上面实现了对符号表中变量出现权重的修改（函数也修改是为了实现一致，对程序没影响是因为不用函数的usesum）
			return &(local->symbollist.at(i));
		}
	}
	//再找global符号表 global.symbollist
	for(int i=0;i<global.symbollist.size();i++){
		if(strcmp(goal.c_str(),global.symbollist.at(i).name.c_str())==0){
			if(cyclic==0){global.symbollist.at(i).usesum = global.symbollist.at(i).usesum+1;}
			else{global.symbollist.at(i).usesum = global.symbollist.at(i).usesum+5*cyclic;}
			return &(global.symbollist.at(i));
		}
	}
	return nullptr;
}

//用于表达式中间变量的表示，返回一个节点指针geq->
/*struct tablenode * praser::gentemp(){
	struct tablenode* p = new tablenode();
	static int temp_number = 0;
	string base = "gentemp_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	p->name = temp_name.str();
	p->addr = -1;//addr==-1代表是临时产生的中间变量而不是源程序中的临时变量
	p->kind = kind_var;//生成中间临时变量
	temp_number++;
	return p;
}*/

struct tablenode * praser::gentemp(){
	static int temp_number = 0;
	string base = "gentemp_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	local->table_insert(temp_name.str(),kind_var,type_int,0,level_local,0,-1);//临时变量统一设置成int，但之后会有相关修改
	temp_number++;
	return &(local->symbollist.at(local->symbollist.size()-1));
}
//对于表达式中的整数或者字符生成ident名字以便引用，为了使四元式统一
/*struct tablenode * praser::genrep(int a){
	struct tablenode* p = new tablenode();
	static int temp_number = 0;
	string base = "genrep_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	p->name = temp_name.str();
	p->addr = -2;//addr==-2代表是对整数或者字符的引用
	p->value = a;
	temp_number++;
	return p;
}*/

struct tablenode * praser::genrep(int a){
	static int temp_number = 0;
	string base = "genrep_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	local->table_insert(temp_name.str(),kind_var,type_int,a,level_local,0,-2);//先默认设置成int之后会有相关修改
	temp_number++;
	return &(local->symbollist.at(local->symbollist.size()-1));
}


//用于以ident形式表示字符串常量pset call  fdset
/*struct tablenode * praser::genstr(string constr){
	struct tablenode* p = new tablenode();
	static int temp_number = 0;
	string base = "genstr_";
	stringstream temp_name;
	temp_name<<base<<temp_number;
	p->name = temp_name.str();
	p->addr = -3;//addr==-3代表是对字符串的引用
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

//生成标签
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
	//跳到;即执行完该函数后sy==';'或者eof
	if(error==1){
		for(;sy!=semicolon&&sy!=eofsy;){insymbol();}
		return 0;
	}
	//跳到;后面那一个词
	if(error==2){
		for(;sy!=semicolon&&sy!=eofsy;){insymbol();}
		if(sy==semicolon){insymbol();}
		return 0;
	}
	//跳到,或者;，若先到,则返回1，先到分号则返回2
	if(error==3){
		for(;sy!=comma&&sy!=semicolon&&sy!=eofsy;){insymbol();}
		if(sy==comma){return 1;}
		else if(sy==semicolon){return 2;}
		else{return 0;}
	}
	//跳到)的下一个词
	if(error==4){
		for(;sy!=rparent&&sy!=eofsy;){insymbol();}
		if(sy==rparent){insymbol();}
		return 0;
	}
	//跳到}这个词
	if(error==5){
		for(;sy!=rrf&&sy!=eofsy;){insymbol();}
		if(sy==rrf){insymbol();}
		return 0;
	}
	//跳到)这个词
	if(error==6){
		for(;sy!=rparent&&sy!=eofsy;){insymbol();}
	}
}

praser::praser(int x,FILE * show){
	hasfault = 0;
	cyclic = 0;//初始时设置循环嵌套的层数为0
	now = x;
	output = show;//output用于将中间信息输出到一个指定的文件
	global = tablemanage();
	local = new tablemanage();
	gen = middlecode();
	error = errorhandler();
}

//＜整数＞        ::= ［＋｜－］＜无符号整数＞｜０
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

//＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
void praser::constdef(){
	int type;
	int kind = kind_const;
	if(sy==intsy){
		for(;;){
			type = type_int;//标识符类型转移到type
			insymbol();
			if(sy==ident){
				string temp = string(name);//标识符名字转移到新对象temp
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
						item1 = global.table_search(temp);//不管上面是否插入成功，这里一定能search到
					}else{
						if(local->table_insert(temp,kind,type,temp_value,level_local,0,lines)<0){
							hasfault = 1;
							error.out(redefine,lines);
						}
						item1 = local->table_search(temp);//不管上面是否插入成功，这里一定能search到
					}
					gen.generate(eqint,item1,temp_value);///////////////////////////
					fprintf(output,"constdef--%d\n",lines);
					if(sy==comma){continue;}
					else{break;}
				}//插入符号表
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
							item1 = global.table_search(temp);//不管上面是否插入成功，这里一定能search到
						}else{
							if(local->table_insert(temp,kind,type,name[0],level_local,0,lines)<0){
								hasfault = 1;
								error.out(redefine,lines);
							}
							item1 = local->table_search(temp);//不管上面是否插入成功，这里一定能search到
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

//＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
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


//＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
//该方法用于函数内部变量定义，对于全局变量，并不适用该方法因为其不能区分变量定义和函数定义
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
		//(可能是voidsy，不能报错，只是通过本函数过滤一下)	
	}
}

//＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
void praser::vardec(){
	//进入之前第一个sy已经读好
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
			return;//不报错，直接返回
		}
	}
}

//用于全局变量的定义，将逗号后的变量填入符号表，其内不判断分号，留在program()函数中判断分号
void praser::global_var(int type){
	//开始符号sy==comma
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

//＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞参数表中不允许有数组的定义，但函数调用可用A[1]形式
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
			item1 = local->table_search(temp);//不管上面是否插入成功，这里item1都不会是空指针
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


//＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
struct tablenode* praser::ele(int species){
	if(sy==ident){
		string temp = string(name);
		insymbol();
		if(sy==lbrack){
			struct tablenode* item1;
			struct tablenode* item2;
			struct tablenode* item3;
			insymbol();
			item2 = expression(species);//expression()内部生成结果变量并将其指针返回
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
					item3->type = item1->type;//表达式也是有int和char类型之分的
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
			}////涉及文法的改写，因为first集相交不为空集
			for(int i=0;i<push_record.size();i++){//
				gen.generate(push,push_record.at(i),nullptr,nullptr);//
			}//
			if(sy==rparent){
				struct tablenode* item2 = useident(temp);
				struct tablenode* item3 = gentemp();
				if(item2!=nullptr){
					item3->type = item2->type;//因子为函数时其类型等于函数的类型
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
		item1 = expression(species);//因子不能为空因此不能是()这种形式的表达式
		if(sy==rparent){insymbol();return item1;}
		else{
			hasfault = 1;
			error.out(exrparent,lines);
			erjump(species);///////////////////////////////////////////////////////////////////
		}
	}
	else if(sy==charcon){
		struct tablenode* item1 = genrep(name[0]);
		item1->type = type_char;//因子的类型要保留，但是只要因子参与运算就是整型
		gen.generate(eqint,item1,name[0]);/////////////////////////////////////////////////
		insymbol();
		return item1;
	}
	else if(sy==zerocon||sy==plu||sy==minu||sy==unsignintcon){
		int a = integer();//有返回值，即为整数的值
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

//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
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

//species==1为;处用表达式、==2为()里用了表达式
//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}//此式开头的+-为只针对开头那一项的加减
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

//＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
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
			}//if情况下条件取反
		}else{
			switch(index){
			case eql:{gen.generate(_beq,label,item1,item2);break;}
			case neq:{gen.generate(_neq,label,item1,item2);break;}
			case gtr:{gen.generate(_gtr,label,item1,item2);break;}
			case geq:{gen.generate(_geq,label,item1,item2);break;}
			case lss:{gen.generate(_lss,label,item1,item2);break;}
			case leq:{gen.generate(_leq,label,item1,item2);break;}
			}//while情况下条件不取反
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

//＜条件语句＞  ::=  if ‘(’＜条件＞‘)’＜语句＞
int praser::ifment(){
	//此时sy为ifsy
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

//＜循环语句＞   ::=  do＜语句＞while ‘(’＜条件＞‘)’
int praser::whilement(){
	//进入时sy==dosy
	int what = 0;
	cyclic = cyclic+1;//进入循环增加1
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
				cyclic = cyclic-1;//循环结束后嵌套减1
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

//＜常量＞   ::=  ＜整数＞|＜字符＞
//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
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
			int a = integer();//有返回值，为常量的大小
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

//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
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


//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞ ‘}’
int praser::switchment(){
	//进来时sy==switchsy
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

//＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
//＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
//＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
//下面的实现涉及了对上面文法的改写
void praser::functionuse(string temp){
	//进入时sy==lparent
	int parahow = 0;//源程序调用传参个数
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
//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
//涉及对文法的改写，因为不能区分赋值与函数调用
void praser::assignment(string temp){
	//进入时只读了ident这一个symbol,但sy的位置已在=或[
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

//＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
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

//expression()函数在没有匹配到表达式时会报错
//＜写语句＞    ::=  printf‘(’＜字符串＞,＜表达式＞‘)’|printf ‘(’＜字符串＞‘)’|printf ‘(’＜表达式＞‘)’
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
				}else{//只有字符串一个参数
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

//＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
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


//＜语句＞    ::= ＜条件语句＞｜＜循环语句＞｜<情况语句>|‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞; 
//                      |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
//该部分需进行标识符定义检查，每次使用标识符则需要查表看是否有定义
//生成中间代码时只需考虑语句部分
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
	}//有/无返回值函数调用或者赋值语句
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


//＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
//＜语句列＞   ::=｛＜语句＞｝
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


//用于函数的分析，有返回值函数定义分析，有对文法的改写
//＜函数定义＞  ::=  ‘(’＜参数表＞‘)’ ‘{’＜复合语句＞‘}’，其中声明头部不在该函数中确定，因此从文法中去掉
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
			int retsum = complexment();//复合语句分析
			if(type==type_void&&retsum!=0){
				hasfault = 1;
				error.out(noexretval,lines);
			}
			if((type==type_int||type==type_char)&&retsum==0){
				hasfault = 1;
				error.out(exbutnoretval,lines);
			}
			if(sy==rrf){
				gen.generate(fdset,temp+"end",nullptr,nullptr);//必须在transfer之前，因为后端还用local了
				transfer(temp);//在每个函数结束时调用transfer()函数进行局部符号表的更新
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
		erjump(5);//跳到}的下一个词
	}
}


//＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
void praser::program(){
	//进入该程序时为已经insymbol好了，等待使用
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
	//下面进行变量说明和函数定义的语法分析，由于会存在回溯，因此需改写文法
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
				global_var(type);//将逗号后的变量填入表中
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
							global_var(type);//将逗号后的变量填入表中
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
				now = analyze_local;//此时语法分析进入局部部分
				functiondef(type,temp);////////
				break;//出现函数定义则之后一定不会再有变量声明，因此跳出
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
	//下面全部是函数的分析，不存在变量或常量的定义，函数定义分析需要建立局部符号表
	now = analyze_local;//此时进行局部的语法分析
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
			//无返回值函数+主函数
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
	//////////////////////////////////////////////////上部分结束时sy==eofsy即源文件结束
	if(sy==eofsy){printf("compiler_success!");}
	else{printf("compiler_failure~");}
}

















