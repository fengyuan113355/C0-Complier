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
	int hasfault;//1代表词法或语法或语义分析有错，不需生成目标代码
	int cyclic;//循环嵌套的层数（用于引用计数）
	FILE * output;
	tablemanage global;  //全局符号表
	tablemanage* local;   //局部符号表
	map<string, tablemanage*> still;   //用于存放每个函数自己的局部符号表

	middlecode gen;//中间代码生成
	//mipscode goal;
	errorhandler error;//输出错误类型
	int now;//显示当前语法制导翻译是全局还是局部1---全局  0---局部
	
	int erjump(int error);//根据错误种类进行跳词
	void transfer(string key);//每调用一次transfer都把当前local存到still中并重新生成local
	struct tablenode * useident(string goal);//每次在语句中使用标识符时调用useident返回其在两个符号表中结点的指针
	struct tablenode * gentemp();//用于表达式中间变量的表示，返回一个节点指针
	struct tablenode * genrep(int a);//用于将表达式中出现的整数或者字符也分配一个ident名字进行保存
	struct tablenode * genstr(string constr);//用于生成一个变量以表示字符串常量
	string setlabel(symbol ins);//根据标识符生成标签
	praser(int x,FILE * show);//默认构造器
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







