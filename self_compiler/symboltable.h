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
	string name;//存入变量的名字--标识符
	int kind;//该变量是const,var,parameter还是function
	int type;//该变量是int还是char还是void
	int value;//该变量的值，char类型也用int类型来存
	int level;//该变量是全局的还是局部于函数的---本质上还代表层次关系，因为文法只支持两层，循环内不能再定义新变量
	int length;//函数参数个数或者数组长度
	int addr;//存变量的地址：源程序中第几行lines
	string str;
	int offset;//变量在自己函数中
	bool issingle;//判断表达式是否是一个静态值
	int usesum;//代表引用计数
};

class tablemanage{
public:
	vector<struct tablenode> symbollist;//符号表
	int off_sum;

	tablemanage();
	int table_insert(string name,int kind,int type,int value,int level,int length,int addr);//向符号表中插入一个节点并返回位置
	struct tablenode * table_search(string name);//在符号表中根据name寻找节点并返回位置
	void clear();
};


#endif

