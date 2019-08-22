#include "errorhandler.h"
#include <string.h>
#include <string>
#include <iostream>

errorhandler::errorhandler(){
	errorshow = fopen("error_result.txt","w");
}

//将错误信息输出到指定文件中，其中传入lines标记错误出现在源程序中的位置
void errorhandler::out(errorkind a,int lines){
	switch(a){
	case undefine: {printf("ident not define--%d\n",lines);break;}
	case exrparent: {printf("expect a rparent--%d\n",lines);break;}
	case exunsigned: {printf("expect an unsignintcon--%d\n",lines);break;}
	case redefine: {printf("redefine ident--%d\n",lines);break;}
	case constnotinit: {printf("const not initial--%d\n",lines);break;}
	case exident: {printf("expect an ident--%d\n",lines);break;}
	case excharcon: {printf("expect a charcon--%d\n",lines);break;}
	case exintorchar: {printf("expect intsy or charsy--%d\n",lines);break;}
	case exsemicolon: {printf("expect a semicolon--%d\n",lines);break;}
	case exrbrack: {printf("expect a rbrack--%d\n",lines);break;}
	case exfactor: {printf("expect a factor--%d\n",lines);break;}
	case exlparent: {printf("expect a lparent--%d\n",lines);break;}
	case exwhile: {printf("expect a while--%d\n",lines);break;}
	case caseafwrong: {printf("expect charcon or integer after casesy--%d\n",lines);break;}
	case excolon: {printf("expect a colon--%d\n",lines);break;}
	case excasesy: {printf("expect a casesy--%d\n",lines);break;}
	case exrrf: {printf("expect a rrf--%d\n",lines);break;}
	case exllf: {printf("expect a llf--%d\n",lines);break;}
	case exbecomesy: {printf("expect a becomesy--%d\n",lines);break;}
	case wrongassign: {printf("expect a becomesy or lbrack--%d\n",lines);break;}
	case wrongment: {printf("there is a wrong ment--%d\n",lines);break;}
	case globalvarwrong: {printf("global var define wrong--%d\n",lines);break;}
	case noretvalue:{printf("want value but not return--%d\n",lines);break;}
	case noexretval:{printf("not expect return value--%d\n",lines);break;}
	case exbutnoretval:{printf("expect but not return value--%d\n",lines);break;}
	case expexp:{printf("expect expression but null--%d\n",lines);break;}
	case exele:{printf("expect an ele to make up vector--%d\n",lines);break;}
	case exvec:{printf("expect a vector to make up expression--%d\n",lines);break;}
	case exnotnull:{printf("expect a value but not--%d\n",lines);break;}
	case overboundary:{printf("boundary violation alarm--%d\n",lines);break;}

	case warn_notfit:{printf("[warning] type not fit--%d\n",lines);break;}
	}

}









