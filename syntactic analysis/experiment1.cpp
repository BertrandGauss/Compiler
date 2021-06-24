#ifndef _EXPERIMENT1_H  
#define _EXPERIMENT1_H  
#include <string>
#include <map> 
using namespace std;
#define GRAMMAR_BASE 40 //动态生成的基值  
struct node{
	string word;
	string type;
	int number; 
	int row;
	int col;
	node* next;
};


void initKeyword();
void initDelimiter();
void initOperate();
void init();
node* lex();
int* getRowCol();


#endif 
