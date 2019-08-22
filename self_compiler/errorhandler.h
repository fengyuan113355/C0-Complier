#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H
#include <string.h>
#include <string>

//√∂æŸ¥ÌŒÛ¿‡–Õ
enum errorkind{
	undefine,globalvarwrong,
	exlparent,exrparent,
	exunsigned,redefine,constnotinit,exident,excharcon,exintorchar,exsemicolon,
	exrbrack,exfactor,exwhile,caseafwrong,excolon,excasesy,exrrf,exllf,exbecomesy,wrongassign,
	wrongment,noretvalue,noexretval,exbutnoretval,expexp,exele,exvec,exnotnull,overboundary,
	warn_notfit
};

class errorhandler{
public:
	FILE * errorshow;

	errorhandler();
	void out(errorkind a,int lines);
};


#endif







