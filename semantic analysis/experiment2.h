#ifndef _EXPERIMENT2_H  
#define _EXPERIMENT2_H  

#include <string>
#include <map> 
#include <vector>
#include <stack> 
using namespace std;

const int MAX=1000;

struct symbolNode{//符号表节点
	string name;//名字 
	string type; //类型 
	int param;//是不是形参，如果不是填-1 ,反之填1 
	int val;//符号的值 
	int pos;//存储地址 
	int dimension;//维数
	int defineRow;//定义行
	vector<int> useRows;//使用行	 
};
struct normalNode{//分析树节点 
	string type;
	string name;
	int use;//使用的产生式的编号 
	int row;//所在位置 
	int number;//该节点的标号 
	normalNode* pre;//该节点的父节点 
	normalNode* bro;//该节点的右边的兄弟 
	normalNode* next[MAX];//该节点的子节点集合 
};
struct midnode{//中间的节点可能含有的所有综合属性 
	int type;//类型 
	string op; //将进行的操作 
	int val;//中间节点当前可以计算的值 
	int num;//在函数调用时用到的记录识别出来的实参个数 
	int entry;//指向符号表中的临时变量,即位置 
	int offset;//指向符号表中的临时变量 
	int* turelist;//真出口 
	int* falselist;//假出口 
	int* nextlist;
	stack<int>* stack;
	int quad;//记录三地址 
};

struct treeNode{//抽象语法 
	string name;
	int number;//该节点的标号 
	treeNode* pre;//该节点的父节点 
	treeNode* bro;//该节点的右边的兄弟 
	treeNode* next[MAX];//该节点的子节点集合 
};
normalNode* SynAnalysis();
midnode program(normalNode * root,treeNode* tree);//程序块
midnode FunctionDefinition_or_DeclarationStatement(normalNode * p,treeNode* tree);//函数定义或者变量声明语句 
midnode Type(normalNode *p);//类型
midnode functionDefinition_or_Declaration(normalNode*p,int t,treeNode* tree); //函数定义或者变量声明
midnode defineParameters(normalNode*p,treeNode * tree);//定义函数的参数列表 
midnode ARray(normalNode *p,treeNode* tree);//数组
midnode variableClosure(normalNode* p,int in,treeNode* tree,int num);//变量闭包,in是继承属性，
midnode variable(normalNode* p,int in,treeNode * tree,int num);//变量 
midnode defineParametersClosure(normalNode* p,treeNode* tree,int num);//参数列表闭包
midnode expression(normalNode* p ,treeNode* tree);//表达式 expression -> factors  term
midnode factors(normalNode * p,treeNode * tree);//factors,因式 factors -> factor factorRecursion 
midnode factor(normalNode* p,treeNode* tree);//因子
midnode factorRecursion(normalNode * p,treeNode * tree);//因式递归
midnode term(normalNode * p,treeNode * tree);//项 
midnode number(normalNode * p);//数字
midnode bitwiseOperators(normalNode * p);//位运算符 
midnode AssignmentFunction(normalNode * p,treeNode * tree);
midnode Assignment_or_FunctionCall(normalNode* p,treeNode* tree);
midnode rightValue(normalNode * p, treeNode* tree);//右值 
midnode Boolean(normalNode * p);//bool
midnode ParametersList(normalNode * p,treeNode * tree);//参数列表 ParametersList -> Parameter parameterClosure
midnode Parameter(normalNode * p,treeNode *tree,int numB);
midnode parameterClosure(normalNode* p,treeNode* tree,int numB);//参数闭包
midnode whileLoop(normalNode* p,treeNode * tree);//whileLoop -> while ( M relation_or_logicalExpression ) { M Program } ;
midnode M(normalNode* p);
midnode relation_or_logicalExpression(normalNode* p,treeNode* tree);
midnode relation_or_logicalOperator(normalNode* p);
midnode relationalOperators(normalNode* p);
midnode logicalOperators(normalNode* p);
midnode ConditionalStatement(normalNode* p,treeNode* tree);
midnode otherwiseStatement(normalNode * p,treeNode* tree);
midnode returnStatement(normalNode*p,treeNode * tree);
int* makelist(int n);

#endif 

