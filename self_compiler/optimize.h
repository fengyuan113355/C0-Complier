#ifndef OPTIMIZE_H
#define OPTIMIZE_H
#include <vector>
#include "symboltable.h"
#include "middlecode.h"

struct treenode{
	bool isleaf;//������Ҷ�ӽڵ�ĸ��ڵ����
	bool isconst;
	int selfvalue;//�����ڵ�ʱ������ֵ
	middleinstr instr;//����Ҷ�ӽڵ��middleinstr��������
	vector<struct treenode*> parent;//���ڵ�����Щ
	vector<struct tablenode*> self;//����ڵ�����������нڵ���е�ʲô
	struct treenode* lson;//���ӽڵ�
	struct treenode* rson;//���ӽڵ�
	bool hasouted;
	treenode(){
		//����Ϊ�½��ڵ�ʱ��Ĭ��ֵ
		isleaf = false;
		isconst = isconst;
		selfvalue = 0;
		instr = add;
		lson = nullptr;
		rson = nullptr;
		hasouted = false;
	}
};

struct indexnode{
	struct tablenode* node;
	struct treenode* nowplace;
};

class optimize{
public:
	vector<MiddleItem*> end;//ȫ�ִ�Ż������м����
	vector<MiddleItem*> cache;//ȫ�ִ�Ż������м����
	vector<struct treenode*> tree;//��vector��push_back��ʱ���ǻ��½��ģ�����ֵ��������
	vector<struct indexnode*> index;//��Ӧtablenode�����µ�treenode

	vector<MiddleItem*> smallhole;//�����Ż���

	optimize();
	vector<MiddleItem*> dag(vector<MiddleItem*> init);
	void create_tree(vector<MiddleItem*> init,int inflow,int nextflow);
	struct treenode* search_index(struct tablenode* for_index);
	void update_index(struct treenode* uptreenode,struct tablenode* uptablenode);
	void inspire();
	struct tablenode* standfor(struct treenode* treeself);
	bool allleaf();
	void deleteparent(struct treenode* root);
	bool allhasout();

	//�����Ż�����
	vector<MiddleItem*> forhole(vector<MiddleItem*> inithole);
};



#endif