#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "word_analyze.h"
#include "praser.h"
#include <iostream>
#include "mipscode.h"
#include "optimize.h"

#define line_max 400//源文件中每一行最大值
#define name_max 200//标识符、无符号整数、字符串的最大长度

int words_sum = 0;
char ch;
int cc,ll;
int lines = 0;
char line[line_max];
char name[name_max];
int sum;//无符号整数的数值
symbol sy;

FILE * in;
FILE * out;
FILE * papage;
FILE * middlepage;

void nextch(){
	if(cc==ll){
		char c;
		int i = 0;
		lines++;
		for(;(c=getc(in))!=EOF;i++){
			if(c=='\n'){
				line[i] = '\n';
				break;
			}else{
				line[i] = c;
			}
		}
		if(c==EOF){
			line[i] = EOF;
		}
		ll = i+1;//0到ll-1部分才是有用的
		cc = 0;
	}
	ch = line[cc];//回车字符可以取出
	cc = cc + 1;
}

int special_key(){
	//根据name数组对sy进行重新赋值
	//下列是程序中可能出现的保留字情况
	char a[] = "const";
	char b[] = "int";
	char c[] = "char";
	char d[] = "void";
	char e[] = "if";
	char f[] = "do";
	char g[] = "while";
	char h[] = "switch";
	char i[] = "case";
	char j[] = "scanf";
	char k[] = "printf";
	char l[] = "return";
	char temp[name_max];
	for(int i=0;i<name_max;i++){
		if(isupper(name[i])){
			temp[i] = tolower(name[i]);
		}else{
			temp[i] = name[i];
		}
	}
	if(strcmp(temp,a)==0){sy = constsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"constsy",name);return 1;}
	else if(strcmp(temp,b)==0){sy = intsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"intsy",name);return 1;}
	else if(strcmp(temp,c)==0){sy = charsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"charsy",name);return 1;}
	else if(strcmp(temp,d)==0){sy = voidsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"voidsy",name);return 1;}
	else if(strcmp(temp,e)==0){sy = ifsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"ifsy",name);return 1;}
	else if(strcmp(temp,f)==0){sy = dosy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"dosy",name);return 1;}
	else if(strcmp(temp,g)==0){sy = whilesy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"whilesy",name);return 1;}
	else if(strcmp(temp,h)==0){sy = switchsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"switchsy",name);return 1;}
	else if(strcmp(temp,i)==0){sy = casesy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"casesy",name);return 1;}
	else if(strcmp(temp,j)==0){sy = scanfsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"scanfsy",name);return 1;}
	else if(strcmp(temp,k)==0){sy = printfsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"printfsy",name);return 1;}
	else if(strcmp(temp,l)==0){sy = returnsy;fprintf(out,"%d key::%s,value::%s\n",++words_sum,"returnsy",name);return 1;}
	else{return 0;}
}


//每调用一次便从源文件中提取一个词
int insymbol(){
	//上次没有融入的字符作为每次insymbol的开始字符
	//文法为对于标识符不区分大小写
	//if(ch==EOF){sy = eofsy;return 0;}
	for(;(ch==' ')||(ch=='\t')||(ch=='\n');){
		nextch();
		continue;
	}
	if(ch==EOF){sy = eofsy;return 0;}
	if((ch=='_')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')){//ident
		int i = 0;
		for(;i<name_max;i++){
			if(isupper(ch)){
				ch = tolower(ch);	
			}
			if((ch=='_')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||(ch>='A'&&ch<='Z')){
				name[i] = ch;
				nextch();
				continue;
			}else{
				name[i] = '\0';
				sy = ident;
				if (special_key()==0){
					fprintf(out,"%d key::%s,value::%s\n",++words_sum,"ident",name);
					//printf("%d key::%s,value::%s\n",words_sum,"ident",name);
				}
				break;
			}//词法分析获得的标识符没有全部转化为小写字母而是在special_key中用了一个temp数组
		}
	}//ident
	else if(ch=='0'){//0
		sum = 0;
		sy = zerocon;
		fprintf(out,"%d key::%s,value::%d\n",++words_sum,"zerocon",sum);
		//printf("%d key::%s,value::%d\n",words_sum,"zerocon",sum);
		nextch();
	}
	else if(ch>='1'&&ch<='9'){//unsignnumber
		sum = 0;
		for(;(ch>='0')&&(ch<='9');){
			sum = 10*sum+(ch-'0');
			nextch();
			continue;
		}
		sy = unsignintcon;
		fprintf(out,"%d key::%s,value::%d\n",++words_sum,"unsignintcon",sum);
		//printf("%d key::%s,value::%d\n",words_sum,"unsignintcon",sum);
	}//number
	else if(ch=='\''){
		nextch();
		if((ch=='+')||(ch=='-')||(ch=='*')||(ch=='/')||(ch>='0'&&ch<='9')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch=='_')){
			name[0] = ch;
			name[1] = '\0';
			nextch();
			if(ch=='\''){
				sy = charcon;
				fprintf(out,"%d key::%s,value::%s\n",++words_sum,"charcon",name);
				//printf("%d key::%s,value::%s\n",words_sum,"charcon",name);
				nextch(); 
			}else{
				//printf("wrong");
				sy = wrongsy;
				fprintf(out,"wrong");
			}
		}else{
			//printf("wrong");
			sy = wrongsy;
			fprintf(out,"wrong");
		}
	}
	else if(ch=='"'){
		nextch();
		int i = 0;
		for(;(ch==32)||(ch==33)||(ch>=35&&ch<=126);){
			if(ch=='\\'){
				name[i] = ch;
				i++;
			}
			name[i] = ch;
			i++;
			nextch();
		}
		if(ch=='"'){
			name[i] = '\0';
			sy = stringcon;
			fprintf(out,"%d key::%s,value::%s\n",++words_sum,"stringcon",name);
			//printf("%d key::%s,value::%s\n",words_sum,"stringcon",name);
			nextch();
		}else{
			//printf("wrong");
			sy = wrongsy;
			fprintf(out,"wrong");
		}
	}
	else if(ch==':'){//colon
		sy = colon;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"colon",':');
		//printf("%d key::%s,value::%c\n",words_sum,"colon",':');
		nextch();
	}//colon
	else if(ch==';'){//semicolon
		sy = semicolon;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"semicolon",';');
		//printf("%d key::%s,value::%c\n",words_sum,"semicolon",';');
		nextch();
	}//semicolon
	else if(ch==','){//comma
		sy = comma;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"comma",',');
		//printf("%d key::%s,value::%c\n",words_sum,"comma",',');
		nextch();
	}//comma
	else if(ch=='('){
		sy = lparent;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"lparent",'(');
		//printf("%d key::%s,value::%c\n",words_sum,"lparent",'(');
		nextch();
	}
	else if(ch==')'){
		sy = rparent;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"rparent",')');
		//printf("%d key::%s,value::%c\n",words_sum,"rparent",')');
		nextch();
	}
	else if(ch=='['){//lbrack, rbrack, llf, rrf
		sy = lbrack;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"lbrack",'[');
		//printf("%d key::%s,value::%c\n",words_sum,"lbrack",'[');
		nextch();
	}
	else if(ch==']'){
		sy = rbrack;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"rbrack",']');
		//printf("%d key::%s,value::%c\n",words_sum,"rbrack",']');
		nextch();
	}
	else if(ch=='{'){
		sy = llf;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"llf",'{');
		//printf("%d key::%s,value::%c\n",words_sum,"llf",'{');
		nextch();
	}
	else if(ch=='}'){
		sy = rrf;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"rrf",'}');
		//printf("%d key::%s,value::%c\n",words_sum,"rrf",'}');
		nextch();
	}
	else if(ch=='='){
		nextch();
		if(ch=='='){//eql, neq, gtr, geq, lss, leq,
			sy = eql;
			fprintf(out,"%d key::%s,value::%s\n",++words_sum,"eql","==");
			//printf("%d key::%s,value::%s\n",words_sum,"eql","==");
			nextch();
		}else{
			sy = becomes;
			fprintf(out,"%d key::%s,value::%c\n",++words_sum,"becomes",'=');
			//printf("%d key::%s,value::%c\n",words_sum,"becomes",'=');
		}
	}
	else if(ch=='>'){
		nextch();
		if(ch=='='){
			sy = geq;
			fprintf(out,"%d key::%s,value::%s\n",++words_sum,"gep",">=");
			//printf("%d key::%s,value::%s\n",words_sum,"gep",">=");
			nextch();
		}else{
			sy = gtr;
			fprintf(out,"%d key::%s,value::%c\n",++words_sum,"gtr",'>');
			//printf("%d key::%s,value::%c\n",words_sum,"gtr",'>');
		}
	}
	else if(ch=='<'){
		nextch();
		if(ch=='='){
			sy = leq;
			fprintf(out,"%d key::%s,value::%s\n",++words_sum,"leq","<=");
			//printf("%d key::%s,value::%s\n",words_sum,"leq","<=");
			nextch();
		}else{
			sy = lss;
			fprintf(out,"%d key::%s,value::%c\n",++words_sum,"lss",'<');
			//printf("%d key::%s,value::%c\n",words_sum,"lss",'<');
		}
	}
	else if(ch=='!'){
		nextch();
		if(ch=='='){
			sy = neq;
			fprintf(out,"%d key::%s,value::%s\n",++words_sum,"neq","!=");
			//printf("%d key::%s,value::%s\n",words_sum,"neq","!=");
			nextch();
		}else{
			sy = wrongsy;
			//printf("wrong");
			fprintf(out,"wrong");
		}
	}
	else if(ch=='+'){
		sy = plu;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"plu",'+');
		//printf("%d key::%s,value::%c\n",words_sum,"plu",'+');
		nextch();
	}
	else if(ch=='-'){
		sy = minu;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"minu",'-');
		//printf("%d key::%s,value::%c\n",words_sum,"minu",'-');
		nextch();
	}
	else if(ch=='*'){
		sy = times;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"times",'*');
		//printf("%d key::%s,value::%c\n",words_sum,"times",'*');
		nextch();
	}
	else if(ch=='/'){
		sy = divs;
		fprintf(out,"%d key::%s,value::%c\n",++words_sum,"div",'/');
		//printf("%d key::%s,value::%c\n",words_sum,"div",'/');
		nextch();
	}
	else{
		//printf("wrong!");
		sy = wrongsy;
		fprintf(out,"wrong!");
	}
	return 1;
}

int main(){
	//test word_analyze
	int isopt;
	string path;
	cin>>path;
	in = fopen(path.c_str(),"r");//全局变量在praser中直接进行了使用，在该文件中进行了赋值
	scanf("%d",&isopt);//optimize==0不开启优化；其余值则开启优化
	//in = fopen("origin.txt","r");
	out = fopen("out.txt","w");
	papage = fopen("praser_result.txt","w");
	middlepage = fopen("middlecode.txt","w");
	if(in==NULL){return 0;}
	nextch();
	//for(;insymbol()!=0;){}//测试词法分析
	praser real = praser(analyze_global,papage);
	printf("start_praser_analyze\n");
	real.program();//测试语法分析
	/*for(unsigned int i=0;i<real.global.symbollist.size();i++){
		cout<<endl<<real.global.symbollist.at(i).name<<endl;
	}*///测试符号表中填写的情况
	////////测试中间代码生成情况
	if(real.hasfault==1){printf("Has Parser Error\n");return 0;}//
	vector<MiddleItem*> middle;
	tablemanage* x ;
	if(isopt>=0){
		if(isopt==0){
			for(int i=0;i<real.gen.middlesave.size();i++){
				real.gen.middlesave.at(i)->toprint(middlepage);
			}
			//下面不属于测试而是生成目标代码
			middle = real.gen.middlesave;//middle指向装有中间代码的容器
		}else{
			//进行优化，将优化后的中间代码toprint并存于middle
			//原来的四元式在real.gen.middlesave中，化简后放于middle中
			optimize obj = optimize();
			middle = obj.dag(real.gen.middlesave);
			if(isopt==2){
				middle = obj.forhole(middle);
			}
			for(int i=0;i<middle.size();i++){
				middle.at(i)->toprint(middlepage);
			}
		}
		mipscode final = mipscode();//用于生成目标代码，其内有容器
		tablemanage global = real.global;
		tablemanage* local = nullptr;
		for(int i=0;i<global.symbollist.size();i++){
			if(global.symbollist.at(i).kind==kind_func){continue;}
			if(global.symbollist.at(i).length==0){
				if(global.symbollist.at(i).addr!=-3){
					final.data.push_back(global.symbollist.at(i).name+": .word "+final.change(0));
				}
			}else{
				for(int j=0;j<global.symbollist.at(i).length;j++){
					if(j==0){
						final.data.push_back(global.symbollist.at(i).name+": .word "+final.change(0));
					}else{
						final.data.push_back(global.symbollist.at(i).name+"_"+final.change(j)+": .word "+final.change(0));
					}
				}
			}
		}
		for(int i=0;i<middle.size();i++){
			if(middle.at(i)->instr==fsset){
				local = real.still[middle.at(i)->label.c_str()];
				if(local->symbollist.size()==0){
					final.nowsize = 0;
				}
				else{
					final.nowsize = local->symbollist.at(local->symbollist.size()-1).offset+4;//
				}
				final.localtable = local;//
				final.mipgenerate(fsset,nullptr,nullptr,nullptr);//修改函数参数的计量
				final.branch(middle.at(i));//并不会将fsset中间代码分配到上面一条想去的地方
			}
			else{
				final.branch(middle.at(i));
			}
		}
		FILE * mips = fopen("mipscode.txt","w");
		fprintf(mips,".data\n");
		for(int i=0;i<final.data.size();i++){
			fprintf(mips,"%s\n",final.data.at(i).c_str());
		}
		fprintf(mips,".text\n");//在Mars中进行初始化move $fp,$sp\nj main\nnop\n
		for(int i=0;i<final.mips_mars.size();i++){
			final.mips_mars.at(i).toshow(mips);
		}
	}else{
		//打印中间代码
		for(int i=0;i<real.gen.middlesave.size();i++){
			real.gen.middlesave.at(i)->toprint(middlepage);
		}
		//isopt小于0时没有优化，并且为一次一内存式存取
		middle = real.gen.middlesave;//middle指向装有中间代码的容器
		tra_mipscode final = tra_mipscode();//用于生成目标代码，其内有容器
		tablemanage global = real.global;
		tablemanage* local = nullptr;
		for(int i=0;i<global.symbollist.size();i++){
			if(global.symbollist.at(i).kind==kind_func){continue;}
			if(global.symbollist.at(i).length==0){
				if(global.symbollist.at(i).addr!=-3){
					final.data.push_back(global.symbollist.at(i).name+": .word "+final.change(0));
				}
			}else{
				for(int j=0;j<global.symbollist.at(i).length;j++){
					if(j==0){
						final.data.push_back(global.symbollist.at(i).name+": .word "+final.change(0));
					}else{
						final.data.push_back(global.symbollist.at(i).name+"_"+final.change(j)+": .word "+final.change(0));
					}
				}
			}
		}
		for(int i=0;i<middle.size();i++){
			if(middle.at(i)->instr==fsset){
				local = real.still[middle.at(i)->label.c_str()];
				if(local->symbollist.size()==0){
					final.nowsize = 0;
				}
				else{
					final.nowsize = local->symbollist.at(local->symbollist.size()-1).offset+4;//
				}
				final.mipgenerate(fsset,nullptr,nullptr,nullptr);//修改函数参数的计量
				final.branch(middle.at(i));//并不会将fsset中间代码分配到上面一条想去的地方
			}
			else{
				final.branch(middle.at(i));
			}
		}
		FILE * mips = fopen("mipscode.txt","w");
		fprintf(mips,".data\n");
		for(int i=0;i<final.data.size();i++){
			fprintf(mips,"%s\n",final.data.at(i).c_str());
		}
		fprintf(mips,".text\n");//在Mars中进行初始化move $fp,$sp\nj main\nnop\n
		for(int i=0;i<final.mips_mars.size();i++){
			final.mips_mars.at(i).toshow(mips);
		}
	}
	return 0;
}

