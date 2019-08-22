#include"symboltable.h"


tablemanage::tablemanage(){symbollist.reserve(500);off_sum=0;}

int tablemanage::table_insert(string name,int kind,int type,int value,int level,int length,int addr){//向符号表中插入一个节点并返回位置
	//操作vector<struct tablenode> symbollist
	//内部进行变量同名分析，如果有同名则不能插入返回-1
	//如果可以插入则返回插入后节点的下标
	//static int off_sum = 0;
	struct tablenode* node = new tablenode();
	node->name = name;
	node->kind = kind;
	node->type = type;
	node->value = value;
	node->level = level;
	node->length = length;
	node->addr = addr;
	node->offset = off_sum;
	node->issingle = false;//假设所有都是动态的
	node->usesum = 0;//所有结点初始时的usesum为0
	if(length>0&&kind!=kind_func){off_sum = off_sum+4*length;}//数组在符号表中占用虚拟地址
	else{off_sum = off_sum + 4;}
	for(int i=0;i<symbollist.size();i++){
		if(strcmp(symbollist.at(i).name.c_str(),name.c_str())==0){
			///////////////////////////////////////error
			return -1;
		}
	}
	symbollist.push_back(*node);
	return (symbollist.size()-1);
}
//根据名字、类别、类型和长度在符号表symbollist中寻找,没有找到返回-1，找到返回下标
struct tablenode * tablemanage::table_search(string name){//在符号表中根据name寻找节点并返回位置
	for(int i=0;i<symbollist.size();i++){
		if(strcmp(symbollist.at(i).name.c_str(),name.c_str())==0){
			return &(symbollist.at(i));
		}
	}
	return nullptr;
}

//清空符号表
void tablemanage::clear(){
	symbollist.clear();
}












