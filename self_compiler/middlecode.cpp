#include "middlecode.h"
#include <string>


//middlecode::middlecode(){}
/////////
void middlecode::generate(middleinstr instr,struct tablenode * des,struct tablenode * src1,struct tablenode * src2){
	MiddleItem* form = new MiddleItem();
	form->instr = instr;
	form->des = des;
	form->src1 = src1;
	form->src2 = src2;
	middlesave.push_back(form);
	//goal.mipgenerate(instr,des,src1,src2);////////////////////////////////////////
}
/////////
void middlecode::generate(middleinstr instr,string label,struct tablenode * src1,struct tablenode * src2){
	MiddleItem* form = new MiddleItem();
	form->instr = instr;
	form->label = label;
	form->src1 = src1;
	form->src2 = src2;
	middlesave.push_back(form);
	//goal.mipgenerate(instr,label,src1,src2);/////////////////////////////////////
}
/////////////////
void middlecode::generate(middleinstr instr,struct tablenode * des,int a){
	MiddleItem* form = new MiddleItem();
	form->instr = instr;
	form->des = des;
	form->forconst = a;
	middlesave.push_back(form);
	//goal.mipgenerate(instr,des,a);/////////////////////////////////////
}
////////////////////
void middlecode::generate(middleinstr instr,struct tablenode * des,string constr){
	MiddleItem* form = new MiddleItem();
	form->instr = instr;
	form->des = des;
	form->forstring = constr;
	middlesave.push_back(form);
	//goal.mipgenerate(instr,des,constr);////////////////////////////
}

//void middlecode::generate(middleinstr instr,string)
void middlecode::clear(){
	middlesave.clear();
}

MiddleItem::MiddleItem() {
	this->des = nullptr;
	this->src1 = nullptr;
	this->src2 = nullptr;
}