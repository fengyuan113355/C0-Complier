#include "optimize.h"
#include<stdio.h>
/*
enum middleinstr{
	add,sub,mul,divv,
	                       _beq,_neq,_geq,_gtr,_leq,_lss,
	oar,iar,
	scf,prf,fdset,         toset,endset,fsset,
	pset,nset,
	push,     //////ret,//////���һ�����������ʱ���迼��ret����dagͼʱ���Ǽ���   call,
    eqint,eqstr,para,          j
};
*/

optimize::optimize(){tree.reserve(500);}

struct treenode* optimize::search_index(struct tablenode* for_index){
	//Ѱ��index���������򷵻�����ֵ��û�����½�һ��treenode��������ָ��
	for(int i=0;i<index.size();i++){
		if(index.at(i)->node==for_index){
			return index.at(i)->nowplace;
		}
	}
	struct treenode* newgentree = new treenode();
	struct indexnode* newgenindex = new indexnode();
	newgenindex->node = for_index;
	newgenindex->nowplace = newgentree;
	newgentree->self.push_back(for_index);//�����еĽڵ��¼��Ϣ
	newgentree->isleaf = true;
	index.push_back(newgenindex);
	tree.push_back(newgentree);
	return newgentree;
}

void optimize::update_index(struct treenode* uptreenode,struct tablenode* uptablenode){
	for(int i=0;i<index.size();i++){
		if(index.at(i)->node==uptablenode){
			index.at(i)->nowplace = uptreenode;
			return;
		}
	}
	//û���ҵ����ڽڵ�����½�
	struct indexnode* fornew = new indexnode();
	fornew->node = uptablenode;
	fornew->nowplace = uptreenode;
	index.push_back(fornew);
}


vector<MiddleItem*> optimize::dag(vector<MiddleItem*> init){
	vector<int> start;//��¼init�е��м����Ļ����鿪ʼ���
	start.push_back(0);//��ʼ��0�����Ϊ������
	for(int i=1;i<init.size();i++){
		if(init.at(i)->instr==_beq||
		init.at(i)->instr==_neq||
		init.at(i)->instr==_geq||
		init.at(i)->instr==_gtr||
		init.at(i)->instr==_leq||
		init.at(i)->instr==_lss||
		init.at(i)->instr==call||
		init.at(i)->instr==j){
			//��һ�������������
			start.push_back(i+1);
		}else if(init.at(i)->instr==toset||
			init.at(i)->instr==endset||
			init.at(i)->instr==fsset||
			init.at(i)->instr==fdset){
			//�������������
			start.push_back(i);
		}else{
			continue;
		}
	}
	//printf("divide basic block\n");
	//printf("start praser sum--%d\n",start.size());
	//����õ���start�п���������ֵ��ͬ
	int inflow;
	int nextflow;
	for(int i=0;i<start.size()-1;i++){
		inflow = start.at(i);//init�������±�Ϊinflow����
		nextflow = start.at(i+1);
		if(inflow==nextflow){
			//printf("enter compound\n");
			continue;
		}
		else{
			//printf("---##create tree##---\n");
			create_tree(init,inflow,nextflow);
		}
	}
	if(start.size()>0){
		inflow = start.at(start.size()-1);
		for(;inflow<init.size();inflow++){
			end.push_back(init.at(inflow));
		}
	}
	return end;
}


void optimize::create_tree(vector<MiddleItem*> init,int inflow,int nextflow){
	//��end�зŽ��
	//tree��index�����������ڹ���dagͼʱʹ��
	for(;inflow<nextflow;inflow++){
		MiddleItem* nowhandle = init.at(inflow);
		if(nowhandle->instr==add||
		nowhandle->instr==sub||
		nowhandle->instr==mul||
		nowhandle->instr==divv||
		nowhandle->instr==oar){
			struct treenode* left = search_index(nowhandle->src1);
			//printf("create_left--->%d\n",left);
			struct treenode* right = search_index(nowhandle->src2);
			//printf("create_right--->%d\n",right);
			struct treenode* mid = nullptr;
			for(int i=0;i<tree.size();i++){
				if(tree.at(i)->lson==left&&tree.at(i)->rson==right&&tree.at(i)->instr==nowhandle->instr){
					mid = tree.at(i);
					mid->self.push_back(nowhandle->des);
					update_index(mid,nowhandle->des);
					break;
				}
			}
			if(mid==nullptr){
				struct treenode* mid = new treenode();
				mid->instr = nowhandle->instr;
				mid->lson = left;
				mid->rson = right;
				mid->self.push_back(nowhandle->des);
				update_index(mid,nowhandle->des);
				left->parent.push_back(mid);
				right->parent.push_back(mid);
				tree.push_back(mid);
				//printf("new node created for +-*/\n");
			}
			//printf("+-*/oar\n");
		}
		else if(nowhandle->instr==iar){
			struct treenode* left = search_index(nowhandle->des);
			struct treenode* right = search_index(nowhandle->src2);
			struct treenode* mid = nullptr;
			if(mid==nullptr){
				//����iar�Ľڵ㣬��ֻ�ܴ���һ�����飬���Ƕ���oar�Ľڵ㣬����Դ���������
				struct treenode* mid = new treenode();
				mid->instr = nowhandle->instr;
				mid->lson = left;
				mid->rson = right;
				mid->self.push_back(nowhandle->src1);
				update_index(mid,nowhandle->src1);
				left->parent.push_back(mid);
				right->parent.push_back(mid);
				tree.push_back(mid);
			}
			//printf("iar\n");
		}
		else if(nowhandle->instr==nset||
			nowhandle->instr==pset){
			//��des---src1�Ĳ���
			struct treenode* left = search_index(nowhandle->src1);
			struct treenode* mid = nullptr;
			for(int i=0;i<tree.size();i++){
				if(tree.at(i)->lson==left&&tree.at(i)->instr==nowhandle->instr){
					mid = tree.at(i);
					mid->self.push_back(nowhandle->des);
					update_index(mid,nowhandle->des);
					break;
				}
			}
			if(mid==nullptr){
				struct treenode* mid = new treenode();
				mid->instr = nowhandle->instr;
				mid->lson = left;
				mid->self.push_back(nowhandle->des);
				update_index(mid,nowhandle->des);
				left->parent.push_back(mid);
				tree.push_back(mid);
			}
			//printf("--->nset\n");
		}
		/*else if(nowhandle->instr==pset){


			//��������²����ɽ�㣬��һ��DAGͼ���Ż�Ч��������ֻ�ܽ�pset��nset����һ��
			struct treenode* left = search_index(nowhandle->src1);
			struct treenode* mid = nullptr;
			
			mid = left;
			mid->self.push_back(nowhandle->des);
			update_index(mid,nowhandle->des);
					
			if(mid==nullptr){
				struct treenode* mid = new treenode();
				mid->instr = nowhandle->instr;
				mid->lson = left;
				mid->self.push_back(nowhandle->des);
				update_index(mid,nowhandle->des);
				left->parent.push_back(mid);
				tree.push_back(mid);
			}
			//printf("--->pset\n");



		}*/
		else if(nowhandle->instr==eqint){
			struct treenode* left = new treenode();
			left->isleaf = true;
			left->isconst = true;
			left->selfvalue = nowhandle->forconst;
			tree.push_back(left);
			struct treenode* mid = nullptr;
			if(mid==nullptr){
				struct treenode* mid = new treenode();
				mid->instr = nowhandle->instr;
				mid->lson = left;
				mid->self.push_back(nowhandle->des);
				update_index(mid,nowhandle->des);
				left->parent.push_back(mid);
				tree.push_back(mid);
			}
			//printf("eqint\n");
		}
		else{
			//������ʽ�㷨�����м�������end��
			//һ��tree���ܲ�����ͨͼ�����ǵ����м������Ҫһ����ͨͼ��������ϵ���
			//tablenode��addr����С��0��Ϊ���ɵ��м��������ȫΪ�м������ȡ�����еĵ�һ��
			//printf("create one tree and prepare code\n");
			inspire();
			end.push_back(nowhandle);
		}
	}
	//������ʽ�㷨�����м�������end��
}

struct tablenode* optimize::standfor(struct treenode* treeself){
	for(int i=0;i<treeself->self.size();i++){
		if(treeself->self.at(i)->addr>=0){
			return treeself->self.at(i);
		}
	}
	return treeself->self.at(0);
}

void optimize::inspire(){
	//printf("TREE________SIZE---%d\n",tree.size());
	while(true){
		//������ʽ�㷨�����м����Ĺ����У�ֻ�涨����Ҫ�����Ľڵ����������,˳���Լ�����
		for(int i=tree.size()-1;i>=0;i--){
			if(tree.at(i)->hasouted==false&&tree.at(i)->parent.size()==0&&tree.at(i)->isleaf==false){
				//printf("in--->one--->this__I--%d\n",i);
				//�����ýڵ���м����
				//printf("operator---%d\n",tree.at(i)->instr);
				//printf("var___%s\n",tree.at(i)->self.at(0)->name.c_str());
				switch(tree.at(i)->instr){
				case add:
				case sub:
				case mul:
				case divv:
				case oar:{
					//�����м���룬��ȥ�ӽڵ��parent���Ѹ����ڵ�ȥ��
					struct treenode* left = tree.at(i)->lson;
					//printf("left--%d\n",left);
					struct treenode* right = tree.at(i)->rson;
					//printf("right--%d\n",right);
					struct tablenode* tableleft = standfor(left);
					struct tablenode* tableright = standfor(right);
					int record = 0;
					for(int index=0;index<tree.at(i)->self.size();index++){
						if(tree.at(i)->self.at(index)->addr>=0){
							record = 1;
							MiddleItem* form = new MiddleItem();
							form->instr = tree.at(i)->instr;
							form->des = tree.at(i)->self.at(index);
							form->src1 = tableleft;
							form->src2 = tableright;
							cache.push_back(form);
						}
					}
					if(record==0){
						MiddleItem* form = new MiddleItem();
						form->instr = tree.at(i)->instr;
						form->des = tree.at(i)->self.at(0);
						form->src1 = tableleft;
						form->src2 = tableright;
						cache.push_back(form);
					}
					deleteparent(tree.at(i));
					//printf("out~~~~~~ +-*/oar\n");
					break;
						 }
				case iar:{
					struct treenode* left = tree.at(i)->lson;//des
					struct treenode* right = tree.at(i)->rson;//src2
					struct tablenode* tableleft = standfor(left);
					struct tablenode* tableright = standfor(right);
					int record = 0;
					for(int index=0;index<tree.at(i)->self.size();index++){
						if(tree.at(i)->self.at(index)->addr>=0){
							record = 1;
							MiddleItem* form = new MiddleItem();
							form->instr = tree.at(i)->instr;
							form->des = tableleft;
							form->src1 = tree.at(i)->self.at(index);
							form->src2 = tableright;
							cache.push_back(form);
						}
					}
					if(record==0){//����ִ�е�ʱ���ǲ���ִ�е������
						//printf("something wrong when optimizing\n");
					}
					deleteparent(tree.at(i));
					//printf("out~~~~~~ iar\n");
					break;
						 }
				case pset:
				case nset:{
					struct treenode* left = tree.at(i)->lson;
					struct tablenode* tableleft = standfor(left);//src1
					int record = 0;
					for(int index=0;index<tree.at(i)->self.size();index++){
						if(tree.at(i)->self.at(index)->addr>=0){
							record = 1;
							MiddleItem* form = new MiddleItem();
							form->instr = tree.at(i)->instr;
							form->des = tree.at(i)->self.at(index);
							form->src1 = tableleft;
							cache.push_back(form);
						}
					}
					if(record==0){
						MiddleItem* form = new MiddleItem();
						form->instr = tree.at(i)->instr;
						form->des = tree.at(i)->self.at(0);
						form->src1 = tableleft;
						cache.push_back(form);
					}
					deleteparent(tree.at(i));
					//printf("out~~~~~~ pset---nset\n");
					break;
						  }
				case eqint:{
					MiddleItem* form = new MiddleItem();
					form->instr = tree.at(i)->instr;
					form->des = tree.at(i)->self.at(0);
					form->forconst = tree.at(i)->lson->selfvalue;
					cache.push_back(form);
					deleteparent(tree.at(i));
					//printf("out~~~~~~ eqint\n");
					//printf("--this-I--%d\n",i);
					break;
						   }
				}
				tree.at(i)->hasouted = true;
				//printf("out--->one\n");
				continue;
			}
		}
		if(allhasout()){
			//printf("----->>><<<-----\n");
			break;
		}
		//printf("is dead deep in while\n");
		//printf("while___---tree_size--->>>%d\n",tree.size());
	}
	//���浼�������и��ڵ��ϵı��������洦������Ҷ�ڵ㣬���Ҷ�ڵ����б����򵼳�
	/*for(int i=0;i<tree.size();i++){
		if(tree.at(i)->isleaf==true&&tree.at(i)->isconst==false){
			for(int j=1;j<tree.at(i)->self.size();j++){
				if(tree.at(i)->self.at(j)->addr<0){
					continue;
				}
				MiddleItem* form = new MiddleItem();
				form->instr = pset;
				form->des = tree.at(i)->self.at(j);
				form->src1 = tree.at(i)->self.at(0);
				cache.push_back(form);
			}
		}
	}*/
	/////////////////////////////////////////////////////////////
	for(int i=cache.size()-1;i>=0;i--){
		end.push_back(cache.at(i));
	}
	//printf("generate all code and clear vector\n");
	//һϵ����ղ���
	cache.clear();
	tree.clear();
	index.clear();
}

bool optimize::allleaf(){
	for(int i=0;i<tree.size();i++){
		if(tree.at(i)->isleaf==false){
			return false;
		}
	}
	return true;
}

bool optimize::allhasout(){
	for(int i=0;i<tree.size();i++){
		if(tree.at(i)->hasouted==false&&tree.at(i)->isleaf==false){
			return false;
		}
	}
	return true;
}

void optimize::deleteparent(struct treenode* root){
	if(root->lson!=nullptr){
		int i=0;
		for(;i<root->lson->parent.size();i++){
			if(root->lson->parent.at(i)==root){
				vector<struct treenode*>::iterator it = root->lson->parent.begin()+i;
				root->lson->parent.erase(it);
				break;
			}
		}
	}
	if(root->rson!=nullptr){
		int i=0;
		for(;i<root->rson->parent.size();i++){
			if(root->rson->parent.at(i)==root){
				vector<struct treenode*>::iterator it = root->rson->parent.begin()+i;
				root->rson->parent.erase(it);
				break;
			}
		}
	}
}

//�����Ż�����
vector<MiddleItem*> optimize::forhole(vector<MiddleItem*> inithole){
	int i = 0;
	for(;i<inithole.size()-1;i++){
		if(inithole.at(i)->instr==add||
		inithole.at(i)->instr==sub||
		inithole.at(i)->instr==mul||
		inithole.at(i)->instr==divv){
			if(inithole.at(i+1)->instr==pset&&inithole.at(i)->des==inithole.at(i+1)->src1){
				MiddleItem* form = new MiddleItem();
				form->instr = inithole.at(i)->instr;
				form->des = inithole.at(i+1)->des;
				form->src1 = inithole.at(i)->src1;
				form->src2 = inithole.at(i)->src2;
				smallhole.push_back(form);
				i++;
			}else{
				smallhole.push_back(inithole.at(i));
			}
		}else{
			smallhole.push_back(inithole.at(i));
		}
	}
	if(inithole.size()!=0){
		smallhole.push_back(inithole.at(inithole.size()-1));
	}
	return smallhole;
}








