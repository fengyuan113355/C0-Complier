#include"symboltable.h"


tablemanage::tablemanage(){symbollist.reserve(500);off_sum=0;}

int tablemanage::table_insert(string name,int kind,int type,int value,int level,int length,int addr){//����ű��в���һ���ڵ㲢����λ��
	//����vector<struct tablenode> symbollist
	//�ڲ����б���ͬ�������������ͬ�����ܲ��뷵��-1
	//������Բ����򷵻ز����ڵ���±�
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
	node->issingle = false;//�������ж��Ƕ�̬��
	node->usesum = 0;//���н���ʼʱ��usesumΪ0
	if(length>0&&kind!=kind_func){off_sum = off_sum+4*length;}//�����ڷ��ű���ռ�������ַ
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
//�������֡�������ͺͳ����ڷ��ű�symbollist��Ѱ��,û���ҵ�����-1���ҵ������±�
struct tablenode * tablemanage::table_search(string name){//�ڷ��ű��и���nameѰ�ҽڵ㲢����λ��
	for(int i=0;i<symbollist.size();i++){
		if(strcmp(symbollist.at(i).name.c_str(),name.c_str())==0){
			return &(symbollist.at(i));
		}
	}
	return nullptr;
}

//��շ��ű�
void tablemanage::clear(){
	symbollist.clear();
}












