#ifndef _EXPERIMENT3_H  
#define _EXPERIMENT3_H  
#include <string>
#include <map> 
using namespace std;


struct code{//三地址语句（四元式） 
	string op;
	int arg1;
	int arg2;
	int result;
	pair<int,bool> a1;//记录操作数1的下次引用信息和活跃信息 
	pair<int,bool> a2;
	pair<int,bool> r;
};

vector<code> SemAnalysis();


#endif 

