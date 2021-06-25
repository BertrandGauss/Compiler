#ifndef _EXPERIMENT2_H  
#define _EXPERIMENT2_H  

#include <string>
#include <map> 
#include <vector>
#include <stack> 
using namespace std;

const int MAX=1000;

struct symbolNode{//���ű�ڵ�
	string name;//���� 
	string type; //���� 
	int param;//�ǲ����βΣ����������-1 ,��֮��1 
	int val;//���ŵ�ֵ 
	int pos;//�洢��ַ 
	int dimension;//ά��
	int defineRow;//������
	vector<int> useRows;//ʹ����	 
};
struct normalNode{//�������ڵ� 
	string type;
	string name;
	int use;//ʹ�õĲ���ʽ�ı�� 
	int row;//����λ�� 
	int number;//�ýڵ�ı�� 
	normalNode* pre;//�ýڵ�ĸ��ڵ� 
	normalNode* bro;//�ýڵ���ұߵ��ֵ� 
	normalNode* next[MAX];//�ýڵ���ӽڵ㼯�� 
};
struct midnode{//�м�Ľڵ���ܺ��е������ۺ����� 
	int type;//���� 
	string op; //�����еĲ��� 
	int val;//�м�ڵ㵱ǰ���Լ����ֵ 
	int num;//�ں�������ʱ�õ��ļ�¼ʶ�������ʵ�θ��� 
	int entry;//ָ����ű��е���ʱ����,��λ�� 
	int offset;//ָ����ű��е���ʱ���� 
	int* turelist;//����� 
	int* falselist;//�ٳ��� 
	int* nextlist;
	stack<int>* stack;
	int quad;//��¼����ַ 
};

struct treeNode{//�����﷨ 
	string name;
	int number;//�ýڵ�ı�� 
	treeNode* pre;//�ýڵ�ĸ��ڵ� 
	treeNode* bro;//�ýڵ���ұߵ��ֵ� 
	treeNode* next[MAX];//�ýڵ���ӽڵ㼯�� 
};
normalNode* SynAnalysis();
midnode program(normalNode * root,treeNode* tree);//�����
midnode FunctionDefinition_or_DeclarationStatement(normalNode * p,treeNode* tree);//����������߱���������� 
midnode Type(normalNode *p);//����
midnode functionDefinition_or_Declaration(normalNode*p,int t,treeNode* tree); //����������߱�������
midnode defineParameters(normalNode*p,treeNode * tree);//���庯���Ĳ����б� 
midnode ARray(normalNode *p,treeNode* tree);//����
midnode variableClosure(normalNode* p,int in,treeNode* tree,int num);//�����հ�,in�Ǽ̳����ԣ�
midnode variable(normalNode* p,int in,treeNode * tree,int num);//���� 
midnode defineParametersClosure(normalNode* p,treeNode* tree,int num);//�����б�հ�
midnode expression(normalNode* p ,treeNode* tree);//���ʽ expression -> factors  term
midnode factors(normalNode * p,treeNode * tree);//factors,��ʽ factors -> factor factorRecursion 
midnode factor(normalNode* p,treeNode* tree);//����
midnode factorRecursion(normalNode * p,treeNode * tree);//��ʽ�ݹ�
midnode term(normalNode * p,treeNode * tree);//�� 
midnode number(normalNode * p);//����
midnode bitwiseOperators(normalNode * p);//λ����� 
midnode AssignmentFunction(normalNode * p,treeNode * tree);
midnode Assignment_or_FunctionCall(normalNode* p,treeNode* tree);
midnode rightValue(normalNode * p, treeNode* tree);//��ֵ 
midnode Boolean(normalNode * p);//bool
midnode ParametersList(normalNode * p,treeNode * tree);//�����б� ParametersList -> Parameter parameterClosure
midnode Parameter(normalNode * p,treeNode *tree,int numB);
midnode parameterClosure(normalNode* p,treeNode* tree,int numB);//�����հ�
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

