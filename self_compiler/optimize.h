#ifndef OPTIMIZE_H
#define OPTIMIZE_H
#include <vector>
#include "symboltable.h"
#include "middlecode.h"

struct treenode{
	bool isleaf;//不关心叶子节点的父节点情况
	bool isconst;
	int selfvalue;//常数节点时常数的值
	middleinstr instr;//不是叶子节点的middleinstr才有意义
	vector<struct treenode*> parent;//父节点有哪些
	vector<struct tablenode*> self;//本身节点代表编译过程中节点表中的什么
	struct treenode* lson;//左子节点
	struct treenode* rson;//右子节点
	bool hasouted;
	treenode(){
		//如下为新建节点时的默认值
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
	vector<MiddleItem*> end;//全局存放化简后的中间代码
	vector<MiddleItem*> cache;//全局存放化简后的中间代码
	vector<struct treenode*> tree;//向vector中push_back的时候是会新建的，即传值而非引用
	vector<struct indexnode*> index;//相应tablenode的最新的treenode

	vector<MiddleItem*> smallhole;//窥孔优化用

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

	//窥孔优化函数
	vector<MiddleItem*> forhole(vector<MiddleItem*> inithole);
};



#endif