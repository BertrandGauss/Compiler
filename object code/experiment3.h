#ifndef _EXPERIMENT3_H  
#define _EXPERIMENT3_H  
#include <string>
#include <map> 
using namespace std;


struct code{//����ַ��䣨��Ԫʽ�� 
	string op;
	int arg1;
	int arg2;
	int result;
	pair<int,bool> a1;//��¼������1���´�������Ϣ�ͻ�Ծ��Ϣ 
	pair<int,bool> a2;
	pair<int,bool> r;
};

vector<code> SemAnalysis();


#endif 

