#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <cstring> 
#include <sstream>
#include "experiment2.h"
#include<windows.h> 
using namespace std;
int tempBase=0;
const int Max=10000;
vector<symbolNode> symbolTable;//符号表
vector<int> topS;//块索引表 

extern map<string,int> keyword;//关键词 
extern map<string,int> Delimiter;//分割符 
extern map<string,int> oper;//运算符 
map<string,int> type;

int offset=0;
int top=0; 
int nextquad=0;
void initType(){
	type["null"]=-1;
	type["int"]=0;
	type["bool"]=1;
	type["void"]=2;
	type["int_func"]=3; 
	type["bool_func"]=7;
	type["func"]=8;
	type["Array"]=4;//数组
	type["int_array"]=5;//整数数组
	type["bool_array"]=6;//布尔数组 
	type["type_error"]=100;
}
treeNode* Troot=new treeNode();//抽象分析树的根节点 
treeNode* rnext=new treeNode();

struct code{//三地址语句（四元式） 
	string op;
	int arg1;
	int arg2;
	int result;
};
vector<code> codes;//中间代码 

string searchType(int i){
	if(i==-1) return "null";
	if(i==0) return "int";
	if(i==1) return "bool";
	if(i==2) return "void";
	if(i==3) return "int_func";
	if(i==4) return "Array";
	if(i==5) return "int_array";
	if(i==6) return "bool_array"; 
	if(i==7) return "bool_func";
	if(i==8) return "func";
	if(i==100) return "type_error"; 
}
void ErrorProc(int row,string result,string str){//错误处理，str为错误类型 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);//字体为红色 
	cout<<"[语义错误]"<<row<<" row "<<result<<" "<<str<<endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);//字体为白色 
} 
int findid(string name){
	int start=0;//topS[topS.size()-1];
	for(int i=start;i<symbolTable.size();i++){
		if(name==symbolTable[i].name)
			return i;
	}
	return -1;
} 
symbolNode* lookup(string name){
	int start=0;//topS[topS.size()-1];
	for(int i=start;i<symbolTable.size();i++){
		if(name==symbolTable[i].name)
			return &symbolTable[i];
	}
	symbolNode * s=new symbolNode();
	s->type="null";
	return s;
}
void addTable(string name,int useRow){
	int start=0;//topS[topS.size()-1];
	for(int i=start;i<top;i++){
		if(name==symbolTable[i].name)
			symbolTable[i].useRows.push_back(useRow);
	}	
}
int newtemp(int type,int pos,int val){//生成中间变量
	symbolNode s;

	string b;
    stringstream ss;  
    ss<<tempBase;
    ss>>b;
	s.name="temp"+b;
	s.type=searchType(type);
	s.pos=pos;
	s.val=val;
	tempBase++;
	symbolTable.push_back(s);
	top++;
	
	return symbolTable.size()-1;
}
int counter(int x,int y,string op){
	if(op=="+") return x+y;
	if(op=="-") return x-y;
	if(op=="*") return x*y;
	if(op=="/") return x/y;
	if(op=="%") return x%y;
	if(op=="&") return x&y;
	if(op=="|") return x|y; 
}
void outcode(int result,string op,int arg1,int arg2){//生成四元式 
	code c;
	c.arg1=arg1;
	c.arg2=arg2;
	c.op=op;
	c.result=result;
	codes.push_back(c);
	nextquad++;
}
void outcode(int result,string op,int arg1){//重载 
	code c;
	c.arg1=arg1;
	c.op=op; 
	c.result=result;
	codes.push_back(c);
	nextquad++;
}
void outcode(int result,string op){//重载 
	code c;
	c.result=result;
	c.op=op;
	codes.push_back(c);
	nextquad++;
}
void enterTable(string name,int type,int defineRow,int pos,int dimension,int param=-1){//将新定义的符号加入符号栈 
	symbolNode s;
	s.name=name;
	s.type=searchType(type);
	s.defineRow=defineRow;
	s.pos=pos;
	s.param=param;
	s.dimension=dimension;
	symbolTable.push_back(s);
	top++;
}
int num=0; 
midnode program(normalNode * root,treeNode* tree){//程序块 num表示该语句在为第几句 
	midnode FD,AS,WL,CS,VC,program1;
    midnode mid; 
	if(root->use==1) {//如果是函数定义或声明语句
		FD=FunctionDefinition_or_DeclarationStatement(root->next[0],tree);
		program1=program(root->next[1],tree);
		mid.nextlist=program1.nextlist; 
		if(FD.type==type["void"]&&program1.type==type["void"])
			mid.type=type["void"];
		else
			mid.type=type["type_error"];
    }
    else if(root->use==2) {//如果是赋值或函数调用 
		AS=AssignmentFunction(root->next[0],tree);
		program1=program(root->next[1],tree);
		mid.nextlist=program1.nextlist; 
		if(AS.type==type["void"]&&program1.type==type["void"])
			mid.type=type["void"];
		else
			mid.type=type["type_error"];
    }
    else if(root->use==3){//如果是while循环 
    	WL=whileLoop(root->next[0],tree);
		program1=program(root->next[1],tree);
		mid.nextlist=program1.nextlist; 
		if(WL.type==type["void"]&&program1.type==type["void"])
			mid.type=type["void"];
		else
			mid.type=type["type_error"];
	}
    else if(root->use==4){//如果是条件语句 
    	CS=ConditionalStatement(root->next[0],tree);
		program1=program(root->next[1],tree);
		mid.nextlist=program1.nextlist; 
		if(CS.type==type["void"]&&program1.type==type["void"])
			mid.type=type["void"];
		else
			mid.type=type["type_error"];
	}
	else if(root->use==5){//如果是get输入语句 
		treeNode* gets=new treeNode();
		treeNode* getsID=new treeNode();
		gets->name="get";
		gets->pre=tree;
		int i=0;
		while(tree->next[i]) i++;
		tree->next[i]=gets;
		gets->pre=tree; 
		gets->next[0]=getsID;
		getsID->name=root->next[2]->name;
		getsID->pre=gets;
		symbolNode* p=lookup(root->next[2]->name);
		int pos=findid(root->next[2]->name);
		if(p->type=="null"){
			ErrorProc(root->next[2]->row,root->next[2]->name,"未定义"); 
			mid.type=type["type_error"];
		}
		else{

			addTable(root->next[2]->name,root->next[2]->row);//增加符号表		
		}		
		VC=variableClosure(root->next[3],-1,gets,0);
		mid.stack=VC.stack; 
		mid.stack->push(pos);
		mid.num=VC.num+1;
		mid.type=type["void"];
		for(int i=0;i<mid.num;i++){
			int x=mid.stack->top();
			mid.stack->pop();
			outcode(x,"get");
		}
		program1=program(root->next[6],tree);
		mid.nextlist=program1.nextlist; 
		
		if(VC.type!=type["type_error"]&&program1.type!=type["type_error"]){
			
			mid.type=type["void"];
		}
			
		else
			mid.type=type["type_error"];
	}
	else if(root->use==6){//如果是put输出语句 
		treeNode* puts=new treeNode();
		treeNode* putsID=new treeNode();
		puts->name="put";
		puts->pre=tree;
		int i=0;
		while(tree->next[i]) i++;
		tree->next[i]=puts;
		puts->pre=tree; 
		puts->next[0]=putsID;
		putsID->name=root->next[2]->name;
		putsID->pre=puts;
		int pos=findid(root->next[2]->name);
		VC=variableClosure(root->next[3],-1,puts,0);
		mid.stack=VC.stack; 
		mid.stack->push(pos);
		mid.num=VC.num+1;
		mid.type=type["void"];
		for(int i=0;i<mid.num;i++){
			int x=mid.stack->top();
			mid.stack->pop();
			outcode(x,"put");
		}
		program1=program(root->next[6],tree);
		mid.nextlist=program1.nextlist; 
		symbolNode* p=lookup(root->next[2]->name);
		
		if(p->type=="null"){
			ErrorProc(root->next[2]->row,root->next[2]->name,"未定义");
			mid.type=type["type_error"];
		}else
			addTable(root->next[2]->name,root->next[2]->row);//增加符号表		
		
		if(VC.type==type["void"]&&program1.type==type["void"]){

			mid.type=type["void"];
		}
			
		else
			mid.type=type["type_error"];
	}
	else if(root->use==7){//如果是推出空
		mid.type=type["void"]; 
		mid.nextlist=makelist(nextquad);				
	}	
	else{//语法错误，但在实验二中已经处理了 
		cout<<"ERROR"<<endl;
	}
	return mid;
} 

midnode FunctionDefinition_or_DeclarationStatement(normalNode * p,treeNode* tree){//函数定义或声明
	midnode fd, TYpe,mid;

	treeNode* t=new treeNode();
	treeNode* id=new treeNode();
	t->name="null"; 
	id->name=p->next[1]->name;
	t->pre=tree;
	t->next[0]=id;
	id->pre=t;
	int i=0;
	while(tree->next[i]) i++;
	tree->next[i]=t;
	TYpe = Type(p->next[0]);
	fd = functionDefinition_or_Declaration(p->next[2],TYpe.type,t);
	symbolNode* s=lookup(p->next[1]->name);
	if(fd.type==type["int_func"]||fd.type==type["bool_func"]){//是函数声明 
		t->name=searchType(TYpe.type)+"_FunctionDefinition";
		id->name=p->next[1]->name;
		if(s->type=="func"){
			ErrorProc(p->next[1]->row,p->next[1]->name,"函数重复定义"); 
		}
		else if((TYpe.type==type["int"]&&fd.type!=type["int_func"])||(TYpe.type==type["bool"]&&fd.type!=type["bool_func"])){
			ErrorProc(p->next[1]->row,p->next[1]->name,"返回类型错误"); 
		}
		else{

			enterTable(p->next[1]->name,type["func"],p->next[1]->row,offset,0,-1);
			offset=offset+4;
			mid.type = type["void"];
		} 	
	}
	else if(fd.type==type["void"]){//是变量定义
		symbolNode* s=lookup(p->next[1]->name);
		if(s->type!="null"&&s->param==-1){//重复定义 
			ErrorProc(p->next[1]->row,p->next[1]->name,"变量重复定义"); 
		}
		else{
			t->name=searchType(TYpe.type);
				id->name=p->next[1]->name;
			enterTable(p->next[1]->name,TYpe.type,p->next[1]->row,offset,0,-1);
			if(TYpe.type==type["int"])				
				offset+=4;				
			else if(TYpe.type==type["bool"]) 
				offset=offset+1;
			mid.type=type["void"];
			mid.entry=symbolTable.size()-1;//得到当前定义变量在符号表中的位置 
		} 		
	} 
	else if(fd.type==type["array"]){//定义数组
		symbolNode* s=lookup(p->next[1]->name);
		if(s->type!="null"&&s->param==-1){//重复定义 
			ErrorProc(p->next[1]->row,p->next[1]->name,"变量重复定义"); 
		}
		else{
			t->name=searchType(TYpe.type);
			id->name=p->next[1]->name;
			if(TYpe.type==type["int"]){
				enterTable(p->next[1]->name,type["int_array"],p->next[1]->row,offset,1,-1);
				mid.type=type["void"];
				mid.offset=fd.offset;
				offset+=4*symbolTable[fd.offset].val;
			}				
			else if(TYpe.type==type["bool"]) {
				enterTable(p->next[1]->name,type["bool_array"],p->next[1]->row,offset,1,-1);
				mid.type=type["void"];
				mid.offset=fd.offset;
				offset+=symbolTable[fd.offset].val;
			}

		} 		
		
	}
	return mid;	
}
midnode Type(normalNode *p){
	midnode mid;
	if(p->use==63)
		mid.type=type["int"];
	else if(p->use==64)
		mid.type=type["bool"];
	else
		mid.type=type["type_error"];
	return mid;
		
}
midnode functionDefinition_or_Declaration(normalNode*p,int t,treeNode* tree){	
	midnode mid; 
	if(p->use==27){//函数定义 functionDefinition_or_Declaration -> ( defineParameters ) { Program returnStatement} ;
		treeNode* defT=new treeNode();
		tree->next[1]=defT;
		defT->name="params";

		midnode DP,pr,re;
		topS.push_back(symbolTable.size());//表示新子符号表 
		DP=defineParameters(p->next[1],defT);
		mid.num=DP.num;
		pr=program(p->next[4],tree);//程序块
		re=returnStatement(p->next[5],tree);
		
		//程序块结束
		for(int i=symbolTable.size()-1;i>=topS[topS.size()-1];i--){//把子表的局部变量出栈 
			if(symbolTable[i].param!=1){
				symbolTable[i].type="null";
			}
		} 
		top= symbolTable.size()-1;//重新设置栈顶指针
		offset= symbolTable[top].pos+1;//由于释放了变量，重新设置存储位置 
		topS.pop_back(); 
		if(DP.type==type["void"]&&pr.type==type["void"])
			mid.type=type[searchType(re.type)+"_func"];//表示是定义函数 
		else
			mid.type=type["type_error"];
		
	}
	else if(p->use==28){//变量定义 functionDefinition_or_Declaration -> array variableClosure ;
		midnode Array,VC; 
		Array=ARray(p->next[0],tree);
		if(Array.type==type["void"]){//只是变量
			mid.type=type["void"];
			VC=variableClosure(p->next[1],t,tree,0);
		} 
		else if(Array.type==type["array"]){//是数组 
			mid.type=type["array"];
			mid.offset=Array.offset;
			VC=variableClosure(p->next[1],t,tree,0);
		}
	} 
	return mid;	 
}
midnode returnStatement(normalNode*p,treeNode * tree){
	midnode mid;
	int f=findid(p->next[1]->name);
	if(f==-1){
		mid.type=-1; 
		ErrorProc(p->next[1]->row,p->next[1]->name,"返回值不存在");
	}else{
		mid.type=type[symbolTable[f].type];
	}

	return mid;
} 
midnode defineParameters(normalNode*p,treeNode * tree){//定义函数的参数列表 defineParameters -> type id defineParametersClosure 
	midnode TYpe,DC,mid;
	treeNode* Param=new treeNode();
	TYpe=Type(p->next[0]);
	Param->name=searchType(TYpe.type);
	Param->pre=tree;
	tree->next[0]=Param;
	treeNode* ParamID=new treeNode();
	ParamID->name=p->next[1]->name;
	ParamID->pre=Param;
	Param->next[0]=ParamID;
	DC=defineParametersClosure(p->next[2],tree,1);
	enterTable(p->next[1]->name,TYpe.type,p->next[1]->row,offset,1,1);
	if(TYpe.type!=type["type_error"]&&DC.type!=type["type_error"]) {
		mid.type=type["void"];
	}else 
		mid.type=type["type_error"];
	mid.num=DC.num+1;
	return mid;
}
midnode ARray(normalNode *p,treeNode* tree){//数组
	midnode mid;
	if(p->use==22){//有数组 使用array -> [ expression ] 
		midnode ex;
		treeNode* a=new treeNode();
		int i=0;
		while(tree->next[i]) i++;
		tree->next[i]=a;
		a->pre=tree; 
		a->name="[]";
		ex=expression(p->next[1],a);
		if(ex.type==type["int"]){

			mid.entry=ex.entry;
			mid.val=ex.val;
			mid.type=type["array"];
			//outcode(mid.entry,"=",ex.entry);
		} 
		else{
			mid.type=type["type_error"];
		}
	} 
	else if(p->use==23){//array -> ε
		mid.type=type["void"];
	}
	return mid;
} 
midnode variableClosure(normalNode* p,int in,treeNode* tree,int num){//变量闭包,in是继承属性，用来传递定义变量时的变量类型，如果仅仅时引用变量时，该值置为-1 
	midnode V,VC,mid;
	if(p->use==24){//variableClosure -> , variable variableClosure
		V=variable(p->next[1],in,tree,num+1);
		VC=variableClosure(p->next[2],in,tree,num+1);
		if(V.type!=type["type_error"]&&VC.type!=type["type_error"]){
			mid.type=V.type;
			mid.stack=VC.stack;
			mid.stack->push(V.entry);
			mid.num=VC.num+1;
		}else{
			mid.type=type["type_error"];
		}
	}else if(p->use==25){//variableClosure -> ε
		mid.type=type["void"];
		stack<int>* s=new stack<int>();
		mid.stack=s;
	}else{
		mid.type=type["type_error"];
	}
	return mid;
}
midnode variable(normalNode* p,int in,treeNode * tree,int num){//变量 
	symbolNode* s;
	midnode Array,mid;
	Array=ARray(p->next[1],tree);
	s=lookup(p->next[0]->name);
	treeNode * var=new treeNode();
	int i=0;
	while(tree->next[i]) i++;
	if(type[s->type]==type["null"]&&in!=-1){//是在定义该变量，且符号表中没有该变量 
		var->name=p->next[0]->name;
		var->pre=tree;
		tree->next[num]=var;
		if(Array.type==type["void"]){//定义的是变量 
			enterTable(p->next[0]->name,in,p->next[0]->row,offset,0,-1);
			if(in==type["int"]){
				offset+=4;
				mid.type=type["in"];
			}else if(in==type["bool"]){
				offset+=1;
				mid.type=type["bool"];
			}else
				mid.type=type["type_error"];			
		}else if(Array.type==type["array"]){//定义的是数组 
			if(in==type["int"]){
				enterTable(p->next[0]->name,type["int_array"],p->next[0]->row,offset,1,-1);
				offset+=Array.val*4;
				mid.entry=newtemp(type["int"],offset,offset-Array.val*4);//中间变量存放基址 
			
				offset+=4;//存放了一个整型的中间变量 
				
				mid.offset=newtemp(type["int"],offset,Array.val*4);// 中间变量存放偏移量 
				offset+=4;
				outcode(mid.offset,"=4*",Array.entry);
				mid.type=type["int"];
			}else if(in==type["bool"]){
				enterTable(p->next[0]->name,type["bool_array"],p->next[0]->row,offset,1,-1);
				offset+=Array.val;
				mid.entry=newtemp(type["bool"],offset,offset);
				
				offset+=1;
				//outcode(mid.entry,"=",&symbolTable[symbolTable.size()-1]);
				mid.offset=newtemp(type["bool"],offset,symbolTable[Array.entry].val);
				offset+=1;
				outcode(mid.offset,"=",Array.entry);
				mid.type=type["bool"];
			}else
				mid.type=type["type_error"];
		}}
		else if(s->type!="null"&&in==-1){//已定义，在引用该变量
			var->name=p->next[0]->name;
			var->pre=tree;
			tree->next[num]=var;
			mid.type=type[s->type];
			mid.val=s->val; 
			mid.entry=findid(s->name);
			addTable(p->next[0]->name,p->next[0]->row);//修改符号表 			
		}
		else if(type[s->type]==type["null"]&&in==-1){//未定义
			var->name=p->next[0]->name;
			var->pre=tree;
			tree->next[0]=var;
			ErrorProc(p->next[0]->row,p->next[0]->name,"变量未定义"); 	
			mid.type=type["type_error"];		
		}else if(type[s->type]!=type["null"]&&in!=-1){//重复定义 
			var->name=p->next[0]->name;
			var->pre=tree;
			mid.type=type["type_error"];	
		}
		//mid.entry=findid(s->name);
		return mid;
	
	
}
midnode defineParametersClosure(normalNode* p,treeNode* tree,int num){//参数列表闭包,num表示是第几个参数 
	midnode mid;
	if(p->use==30){//defineParametersClosure -> , type id defineParametersClosure
		midnode TYpe,DP;
		treeNode* Param=new treeNode();
		TYpe=Type(p->next[1]);
		Param->name=searchType(TYpe.type);
		Param->pre=tree;
		tree->next[num]=Param;
		treeNode* ParamID=new treeNode();
		ParamID->name=p->next[2]->name;
		ParamID->pre=Param;
		Param->next[0]=ParamID;
		DP=defineParametersClosure(p->next[3],tree,num+1);

		enterTable(p->next[2]->name,TYpe.type,p->next[2]->row,offset,0,1);//是参数
		if(TYpe.type==type["int"])	
			offset+=4;
		else if(TYpe.type==type["bool"])
			offset+=1;
		for(int i=symbolTable.size()-1;;i--){
			if(symbolTable[i].param==1)
				mid.num=mid.num+1;
			else
				break;
		}
	} 
	else if(p->use==31){//defineParametersClosure -> ε
		mid.num=0;
		mid.type=type["void"];	
	}
	return mid;
	
}
midnode expression(normalNode* p ,treeNode* tree){//表达式 expression -> factors  term
	midnode mid,fas,te; 
	treeNode* exop=new treeNode();
    treeNode* arg1=new treeNode();
    treeNode* arg2=new treeNode();
	int i=0;
	while(tree->next[i]) i++;
	tree->next[i]=exop;
	fas=factors(p->next[0],arg1);
	te=term(p->next[1],arg2);
	if(te.type==type["void"]){
		int i=0,j=0;
		exop->name=arg1->name;
		while(arg1->next[j]){
			exop->next[j]=arg1->next[j];
			j++;
		}
		mid.op=fas.op;
		mid.entry=fas.entry;
		mid.type=fas.type;
		mid.val=fas.val; 
	}else if(te.type==type["int"]&&fas.type==type["int"]){
		tree->next[i]=exop;
		exop->name=te.op;

		exop->next[0]=arg1;
		exop->next[1]=arg2;
		arg1->pre=exop;
		arg2->pre=exop;
		mid.val=counter(fas.val,te.val,te.op);
		mid.entry=newtemp(type["int"],offset,mid.val);
		offset+=4;
		mid.type=type["int"];
		outcode(mid.entry,te.op,fas.entry,te.entry);
	}else{
		mid.type=type["type_error"];
	} 
	int x[Max],y[Max];
	x[0]=nextquad;
	x[1]=-1;
	y[0]=nextquad;
	y[1]=-1;	
	mid.turelist=x;//makelist(nextquad);
	mid.falselist=y;//makelist(nextquad);
	return mid;
}
midnode factors(normalNode * p,treeNode * tree){//factors,因式 factors -> factor factorRecursion 
	midnode fa,faR,mid;
	treeNode* op=new treeNode();
	treeNode* arg1=new treeNode();
	treeNode* arg2=new treeNode();
	fa= factor(p->next[0],arg1);
	faR=factorRecursion(p->next[1],arg2);
	if(faR.type==type["void"]){
		//tree=op;
		int i=0;
		tree->name=arg1->name;
		while(tree->next[i]) i++; 
		while(arg1->next[i]){
			tree->next[i]=arg1->next[i];	
			i++;
		}
		mid.type=fa.type;
		mid.entry=fa.entry;

	}else if(faR.type==type["int"]&&fa.type==type["int"]){
		int i=0;
		while(tree->next[i]) i++;
		tree->name=faR.op;
		arg1->pre=tree;
		arg2->pre=tree;
		tree->next[0]=arg1;
		tree->next[1]=arg2; 
		mid.val=counter(fa.val,faR.val,faR.op);
		mid.entry=newtemp(type["int"],offset,mid.val);

		offset+=4;
		mid.type=type["int"];
		outcode(mid.entry,faR.op,fa.entry,faR.entry);
	}else{
		tree->next[0]=op;
		op->next[0]=arg1;
		op->next[1]=arg2;
		mid.type=type["type_error"];
	} 

	return mid;	
}
midnode factor(normalNode* p,treeNode* tree){//因子
	midnode mid;
	if(p->use==18){//factor  -> ( expression )
		midnode ex=expression(p->next[1],tree); 
		mid.entry=ex.entry;
		mid.type=ex.type;
	} 
	else if(p->use==19){//factor  -> variable
        int i=0;
        while(tree->next[i]) i++;
		midnode va=variable(p->next[0],-1,tree,i);
		tree->name=tree->next[0]->name;
		tree->next[0]=NULL;
		mid.type=va.type;
		mid.val=va.val;
		mid.entry=va.entry;
	}else if(p->use==20){//factor  -> number
		int i=0;   
        treeNode* t=new treeNode();
		midnode num=number(p->next[0]);
		mid.type=type["int"];
		mid.val=num.val;
		stringstream ss; 
		string b; 
    	ss<<num.val;
    	ss>>b;
		tree->name=b;
		mid.entry=num.entry;//(mid.type,offset,num.val);
		offset+=4;	
	}
	 return mid;	
}
midnode factorRecursion(normalNode * p,treeNode * tree){//因式递归
	midnode mid,fa,faR; 
	if(p->use==17){//factorRecursion -> ε

		mid.type=type["void"];
		mid.val=0;
	}else{
		treeNode* arg1=new treeNode();
		treeNode* arg2=new treeNode();
		fa=factor(p->next[1],arg1);
		faR=factorRecursion(p->next[2],arg2);
		
		if(faR.type==type["void"]){
		    int i=0;
		    tree->name=arg1->name;
		    while(arg1->next[i]){
			    tree->next[i]=arg1->next[i];	
			    i++;
		   }
			mid.type=fa.type;
			mid.entry=fa.entry;
			mid.val=fa.val;
		}else if(faR.type==type["int"]){
			tree->name=faR.op;
			arg1->pre=tree;
			arg2->pre=tree;
			tree->next[0]=arg1;
			tree->next[1]=arg2; 
			mid.type==type["int"];
			mid.val=counter(fa.val,faR.val,faR.op);
			mid.entry=newtemp(type["int"],offset,mid.val);
			offset+=4;
			outcode(mid.entry,faR.op,fa.entry,faR.entry);
		}else{
			mid.type=type["type_error"];
		}
		if(p->use==10){//factorRecursion -> * factor  factorRecursion
			mid.op="*";	
		}else if(p->use==11){//factorRecursion -> / factor  factorRecursion
			mid.op="/"; 
		}else if(p->use==12){//factorRecursion -> % factor  factorRecursion
			mid.op="%";
		}
	}

	return mid;
}
midnode term(normalNode * p,treeNode * tree){//项
	midnode mid,fas, te,bo;
	if(p->use==16){//term -> ε
		mid.type=type["void"];
		mid.val=0;
	}
	else if(p->use==15){//term -> bitwiseOperators factors term
		treeNode* arg1=new treeNode();
		treeNode* arg2=new treeNode();
		bo=bitwiseOperators(p->next[0]);
		fas=factors(p->next[1],arg1);
		te=term(p->next[2],arg2);
		tree->name=te.op;

		if(fas.type==type["int"]&&te.type==type["int"]){
			arg1->pre=tree;
		    arg2->pre=tree;
		    tree->next[0]=arg1;
		    tree->next[1]=arg2;
			mid.op=bo.op;
			mid.val=counter(fas.val,te.val,te.op);
			mid.type=type["int"];
			mid.entry=newtemp(type["int"],offset,mid.val);
			offset+=4;
			outcode(mid.entry,te.op,fas.entry,te.entry);
		}else if(fas.type==type["int"]&&te.type==type["void"]){
			int i=0,j=0;
		    tree->name=arg1->name;
		    while(arg1->next[i]){
			    tree->next[i]=arg1->next[i];	
			    i++;
		   }
		   
			mid.op=bo.op;
			mid.val=fas.val;
			mid.type=type["int"];
			mid.entry=fas.entry;
		}
		else{
			mid.type=type["type_error"];
		}
			
	}else{
		treeNode* arg1=new treeNode();
		treeNode* arg2=new treeNode();

		fas=factors(p->next[1],arg1);
		te=term(p->next[2],arg2);
		tree->name=te.op;
		if(fas.type==type["int"]&&te.type==type["int"]){
			mid.val=counter(fas.val,te.val,te.op);
			mid.type=type["int"];
			mid.entry=newtemp(type["int"],offset,mid.val);
			offset+=4;
			arg1->pre=tree;
		    arg2->pre=tree;
		    tree->next[0]=arg1;
		    tree->next[1]=arg2;
			outcode(mid.entry,te.op,fas.entry,te.entry);
		
		}else if(fas.type==type["int"]&&te.type==type["void"]){
			mid.val=fas.val;
			mid.type=type["int"];
			mid.entry=fas.entry;
			int i=0;
		    tree->name=arg1->name;
		    while(arg1->next[i]){
			    tree->next[i]=arg1->next[i];	
			    i++;
		   }
		}
		else{
			mid.type=type["type_error"];
		}
		if(p->use==13){//term -> + factors term
			mid.op="+";
		}else if(p->use==14){//term -> - factors term
			mid.op="-";
		}
	}

	return mid;	
}
int getNumber(string number,int base){//base 表示进制 
	int num=0;
	int power=1;
	if(base==10||base==8){
		for(int i=number.length()-1;i>=0;i--){
			num+=(number[i]-'0')*power;
			power*=base;
		}	
	}else if(base==16){
		for(int i=number.length()-1;i>1;i--){//因为前两位为表示16进制的0x 
			if(number[i]>='A'&&number[i]<='F'){
				num+=(number[i]-'F'+16)*power;
				
			}else{
				num+=(number[i]-'0')*power;
			}
			
			power*=base;
		}	
	} 
	
	return num;
}
midnode number(normalNode * p){
	midnode mid;
	mid.type=type["int"];
	
	if(p->next[0]->type=="digits"){
		mid.val=getNumber(p->next[0]->name,10);	
	}else if(p->next[0]->type=="digits8"){
		mid.val=getNumber(p->next[0]->name,8);		
	}else if(p->next[0]->type=="digits16"){
		mid.val=getNumber(p->next[0]->name,16);		
	}
	string b;
    stringstream ss;  
    ss<<mid.val;
    ss>>b;
    symbolNode str;
    str.name=b;
    str.val=mid.val;
    str.pos=offset;
    str.type="null";//表示常数的变量 
    symbolTable.push_back(str);
	offset+=4;
	top++;
	mid.entry=symbolTable.size()-1;
	return mid;
}
midnode bitwiseOperators(normalNode * p){//位运算符 
	midnode mid;
	if(p->next[0]->name=="&"){
		mid.op="&";
	}else if(p->next[0]->name=="|"){
		mid.op="|";
	}
	return mid;
} 
void check(symbolNode* s,stack<int>* sta,int num){
	int i;
	int c=num;

	queue<int>* n=new queue<int>();
	for(i=0;i<symbolTable.size();i++){
		if(symbolTable[i].name==s->name&&symbolTable[i].type=="func")
			break;
	}
	for(int j=topS[topS.size()-1];j<i;j++){
		if(symbolTable[j].param==1){
			int x;
			x=sta->top();
			n->push(x);			
			outcode(x,"param");
			sta->pop();
			if(symbolTable[j].type==symbolTable[x].type){
				num=num-1;
			}
				
			else{
				if(num==0)
					ErrorProc(s->useRows[s->useRows.size()-1],s->name,"传入参数数目少于定义参数数目");
				else
					ErrorProc(symbolTable[x].useRows[symbolTable[x].useRows.size()-1],symbolTable[x].name,"传入参数类型不匹配"); 
			}
		}
		else{
			break;
		}
	}
	if(num>0)
		ErrorProc(s->useRows[s->useRows.size()-1],s->name,"传入参数数目少于定义参数数目");
	symbolNode numb;
	numb.name=c;
	numb.type="int";
	numb.val=c;
	symbolTable.push_back(numb);
	outcode(findid(s->name),"call",findid(numb.name));
} 
midnode AssignmentFunction(normalNode * p,treeNode * tree){//AssignmentFunction -> id Assignment_or_FunctionCall 
	midnode mid,AF;
	treeNode * id=new treeNode();
	treeNode * af=new treeNode();
	int i=0;
	while(tree->next[i]) i++;
	tree->next[i]=af;
	af->pre=tree;
	af->next[0]=id;
	id->pre=af;
	symbolNode * find =lookup(p->next[0]->name);
	int j = findid(find->name);
	AF=Assignment_or_FunctionCall(p->next[1],af);
	if(AF.type==type["func"]&&find->type=="func"){//函数调用，并且有定义 
		af->name="\"func:"+p->next[0]->name+"\"";
		mid.entry=j;
		mid.stack=AF.stack;
		mid.num=AF.num;
		addTable(find->name,p->next[0]->row);
		check(find,mid.stack,mid.num);//检查与定义的时候的参数类型，数目是否一致
	}else if(AF.type==type["func"]&&find->type!="func"){
		af->name="\"func:"+p->next[0]->name+"\"";
		ErrorProc(p->next[0]->row,p->next[0]->name,"函数未定义"); 
		mid.type=type["type_error"]; 
	}else if(j==-1){
		af->name="=";
		id->name=p->next[0]->name;
		mid.type=type["null"];
		ErrorProc(p->next[0]->row,p->next[0]->name,"变量未定义"); 
	}else if(AF.type==type[symbolTable[j].type]){//find->type]){
		af->name="=";
		id->name=p->next[0]->name;

		mid.type=type["void"];
		if(AF.type==type["int"]||AF.type==type["bool"]){
			mid.val=AF.val;
			mid.entry=AF.entry;
			outcode(j,"=",AF.entry);
			addTable(p->next[0]->name,p->next[0]->row);
		}else if(AF.type==type["int_array"]||AF.type==type["bool_array"]){
			mid.val=AF.val;
			mid.entry=AF.entry;
			addTable(p->next[0]->name,p->next[0]->row);
			outcode(j,"[]=",AF.offset,AF.entry);
		}
	}
	else if(AF.type!=type[find->type]){
		mid.type=type["type_error"]; 
		ErrorProc(p->next[0]->row,p->next[0]->name,"赋值类型不一致"); 
	}
	
	return mid;
}
midnode Assignment_or_FunctionCall(normalNode* p,treeNode* tree){//赋值或者函数调用 
	midnode mid,Array,RV,P;
	if(p->use==33){//Assignment_or_FunctionCall  -> array = rightValue 
		Array=ARray(p->next[0],tree);
		RV=rightValue(p->next[2],tree);
		if(Array.type==type["void"]&&RV.type!=type["type_error"]){
			mid.type=RV.type;
			mid.val=RV.val;
			mid.entry=RV.entry;
		}else if(Array.type==type["array"]&&RV.type!=type["type_error"]){
			mid.type=type[searchType(RV.type)+"_array"];
			mid.val=RV.val;
			mid.entry=RV.entry;
			mid.offset=Array.entry;
		}else{
			mid.type=type["type_error"]; 
		}
	}else if(p->use==34){//Assignment_or_FunctionCall  -> ( ParametersList ) ;
		P=ParametersList(p->next[1],tree);
		mid.stack=P.stack;
		mid.type=type["func"];
		mid.num=P.num;
	}
	return mid;
}
midnode rightValue(normalNode * p, treeNode* tree){//右值
	midnode mid,ex,bo;
	if(p->use==35){//rightValue -> expression  
		ex=expression(p->next[0],tree);
		mid.val=ex.val;
		mid.entry=ex.entry;
		mid.type=ex.type;
		mid.turelist=ex.turelist;
		mid.falselist=ex.falselist;
	} else if(p->use==36){//rightValue -> Boolean 
		bo=Boolean(p->next[0]);
		mid.type=type["bool"];
		mid.entry=bo.entry;
		mid.val=bo.val;
		mid.turelist=ex.turelist;
		mid.falselist=ex.falselist;		
	}
	return mid;	
}
int* makelist(int n){
	int list[Max];
	list[0]=n;
	list[1]=-1;
	return list;
}
midnode Boolean(normalNode * p){
	midnode mid;
	if(p->use==37){//Boolean -> true
		mid.entry=newtemp(type["bool"],offset,1);
		offset+=1;
		int s=newtemp(type["bool"],offset,1);
		offset+=1;
		symbolTable[symbolTable.size()-1].name="1";
		outcode(mid.entry,"=",s);
		mid.turelist=makelist(nextquad);
		int postion=-1;
		outcode(nextquad,"goto") ;
	}else if(p->use==38){//Boolean -> false
		mid.entry=newtemp(type["bool"],offset,0);
		offset+=1;
		int s=newtemp(type["bool"],offset,0);
		offset+=1;
		symbolTable[symbolTable.size()-1].name="0";
		outcode(mid.entry,"=",s); 
		mid.falselist=makelist(nextquad);
		int postion=-1;
		outcode(nextquad,"goto") ;
	}
	return mid;
}
midnode ParametersList(normalNode * p,treeNode * tree){//参数列表 ParametersList -> Parameter parameterClosure
	midnode mid, Pa,Pc;
	Pa=Parameter(p->next[0],tree,0);
	Pc=parameterClosure(p->next[1],tree,1);
	mid.num=Pc.num+1;
	mid.stack=Pc.stack;
	mid.stack->push(Pa.entry);
	mid.type=type["void"];

	return mid;
}
midnode Parameter(normalNode * p,treeNode *tree,int numB){
	midnode mid;
    treeNode * n=new treeNode();
    int i=0;
    while(tree->next[i]) i++;
	if(p->use==42){//Parameter -> id
		symbolNode * s=lookup(p->next[0]->name);
		n->name=p->next[0]->name;
		tree->next[numB]=n;
		n->pre=tree;
		if(s->type!="null"){
			mid.entry=findid(s->name);
			mid.type=type["void"];
			addTable(p->next[0]->name,p->next[0]->row);
		}else{
			ErrorProc(p->next[0]->row,p->next[0]->name,"变量未定义"); 
			mid.type=type["null"]; 
		}	
	}else if(p->use==43){//Parameter -> number
		midnode num=number(p->next[0]);
		mid.val=num.val;
		string b;
        stringstream ss;  
        ss<<tempBase;
        ss>>b;
		n->name=b;
		tree->next[numB]=n;
		n->pre=tree;
		mid.type=type["void"];
		mid.entry=num.entry;
	}else if(p->use==44){//Parameter -> Boolean
		midnode bo=Boolean(p->next[0]);
		mid.val=bo.val;
		n->name=p->next[0]->next[0]->name;
		tree->next[numB]=n;
		n->pre=tree;
		mid.type=type["void"];
		mid.entry=bo.entry;
	}
	return mid;
}
void backpatch(int* a,int num){
	int i=0;
	while(*a!=-1) {
		codes[(*a)].result=num;
		a++;
	}

}
int* merge(int* n1,int* n2){
	int list[Max];
	int i=0,j=0,pos=0;
	while(n1[i]!=-1){
		list[pos]=n1[i];
		pos++;
		i++;
	}
	while(n2[j]!=-1){
		list[pos]=n2[j];
		pos++;
		j++;
	}
	list[pos]=-1;
	return list;
}
int* merge(int* n1,int* n2,int* n3){
	int list[Max];
	int i=0,j=0,k=0,pos=0;
	while(n1[i]!=-1){
		list[pos]=n1[i];
		pos++;
		i++;
	}
	while(n2[j]!=-1){
		list[pos]=n2[j];
		pos++;
		j++;
	}	
	while(n3[k]!=-1){
		list[pos]=n3[k];
		pos++;
		k++;
	}
	list[pos]=-1;
	return list;
}
midnode parameterClosure(normalNode* p,treeNode* tree,int numB){//参数闭包
	midnode mid,Pa,Pc;
	if(p->use==40){//parameterClosure -> , Parameter parameterClosure
		Pa=Parameter(p->next[1],tree,numB);
		Pc=parameterClosure(p->next[2],tree,numB+1);
		mid.type=type["void"];
		mid.stack=Pc.stack;
		mid.stack->push(Pa.entry);
		mid.num=Pc.num+1; 
	}else if(p->use==41){//parameterClosure -> ε
		mid.num=0;
		stack<int>* s=new stack<int>();
		mid.stack=s;
		mid.type=type["void"];	
	}
	return mid;	
}
midnode whileLoop(normalNode* p,treeNode * tree){//whileLoop -> while ( M relation_or_logicalExpression ) { M Program M} ;
	midnode mid,rl,pro,M1,M2,M3;
	treeNode * wh=new treeNode();
	treeNode * E=new treeNode();
	treeNode * S=new treeNode();
	wh->next[0]=E;
	wh->next[1]=S;
	E->pre=wh;
	S->pre=wh;
    wh->name="while";
    S->name="while_body";
    int i=0;
    while(tree->next[i]) i++;
    tree->next[i]=wh;
    
	M1=M(p->next[2]);
	rl=relation_or_logicalExpression(p->next[3],E);
	M2=M(p->next[6]);
	topS.push_back(symbolTable.size());//表示新子符号表 
	pro=program(p->next[7],S);
	M3=M(p->next[8]);
	for(int i=symbolTable.size()-1;i>=topS[topS.size()-1];i--){//把子表的局部变量出栈 
		symbolTable[i].type="null";
	} 
	if(rl.type==type["bool"])
		mid.type=pro.type;
	else
		mid.type=type["type_error"];
	backpatch(pro.nextlist,M1.quad);
	backpatch(rl.turelist,M2.quad);
	backpatch(rl.falselist,M3.quad+1);
	mid.nextlist=rl.falselist;
	outcode(M1.quad,"goto");
	return mid;
}
midnode M(normalNode* p){
	midnode mid;
	mid.quad=nextquad;
	return mid;
}

midnode relation_or_logicalExpression(normalNode* p,treeNode* tree){
	midnode mid;
	if(p->use==45){//relation_or_logicalExpression -> ! rightValue
		treeNode * rig=new treeNode();
		tree->name="!";
		rig->pre=tree;
		tree->next[1]=rig;
		midnode rv=rightValue(p->next[1],rig);
		if(rv.type!=type["type_error"])
			mid.type=type["bool"];
		else
			mid.type=type["type_error"];
		mid.turelist=rv.falselist;
		mid.falselist=rv.turelist;
	}else if(p->use==46){//relation_or_logicalExpression -> rightValue relation_or_logicalOperator M rightValue
		midnode rv1,ro,rv2,m;
		treeNode * arg1=new treeNode();
		treeNode * arg2=new treeNode();

		rv1=rightValue(p->next[0],arg1);
		ro=relation_or_logicalOperator(p->next[1]);
		m=M(p->next[2]);
		rv2=rightValue(p->next[3],arg2);
		tree->name=ro.op;
		tree->next[0]=arg1->next[0];
		tree->next[1]=arg2->next[0];
		arg1->pre=tree;
		arg2->pre=tree;		
		if(rv1.type==type["int"]&&rv2.type==type["int"]&&(ro.op==">"||ro.op=="<"||ro.op=="=="||ro.op==">="||ro.op=="<="||ro.op=="<>")){
				mid.type=type["bool"];
				int x[Max],y[Max];
				x[0]=nextquad;
				x[1]=-1;
				y[0]=nextquad+1;
				y[1]=-1;
				mid.turelist=x;//makelist(nextquad);
				mid.falselist=y;//makelist(nextquad+1);
				*mid.falselist=nextquad+1;
				outcode(-1,"if"+ro.op+"goto",rv1.entry,rv2.entry);//待回填 
				outcode(-1,"goto");
		}else if(ro.op=="&&"){
			mid.type=type["bool"];
			backpatch(rv1.turelist,m.quad);
			mid.turelist=rv2.turelist;
			mid.falselist=merge(rv1.falselist,rv2.falselist);
		}else if(ro.op=="||"){
			mid.type=type["bool"];
			backpatch(rv1.falselist,m.quad);
			mid.falselist=rv2.falselist;
			mid.turelist=merge(rv1.turelist,rv2.turelist);
		}
	}
	return mid;
}
midnode relation_or_logicalOperator(normalNode* p){
	midnode mid,oper;
	if(p->use==47){//relation_or_logicalOperator -> logicalOperators
		oper=logicalOperators(p->next[0]);
		mid.op=oper.op;
	}else if(p->use==48){//relation_or_logicalOperator -> relationalOperators 
		oper=relationalOperators(p->next[0]);
		mid.op=oper.op;
	}
	return mid;
}
midnode relationalOperators(normalNode* p){
	midnode mid;
	mid.op=p->next[0]->type;
	return mid;
}
midnode logicalOperators(normalNode* p){
	midnode mid;
	mid.op=p->next[0]->type;
	return mid;
}
midnode ConditionalStatement(normalNode* p,treeNode* tree){// if ( relation_or_logicalExpression ) { M Program } ; otherwiseStatement
	midnode mid,rl,m,pro,other;
	treeNode* ifelse=new treeNode();
	treeNode* E=new treeNode();
	treeNode* S1=new treeNode();
	treeNode* S2=new treeNode();
	int i=0;
	while(tree->next[i]) i++;
	tree->next[i]=ifelse;
	ifelse->next[0]=E;
	ifelse->next[1]=S1;
	S1->name="if_body";

	ifelse->pre=tree;
	rl=relation_or_logicalExpression(p->next[2],E); 
	m=M(p->next[5]);
	topS.push_back(symbolTable.size());//表示新子符号表 
	pro=program(p->next[6],S1);
	for(int i=symbolTable.size()-1;i>=topS[topS.size()-1];i--){//把子表的局部变量出栈 
		symbolTable[i].type="null";
	} 
	other=otherwiseStatement(p->next[9],S2);
	backpatch(rl.turelist,m.quad);
	backpatch(rl.falselist,other.quad);
	if(other.op=="else"){//如果有else
		ifelse->name="\"if-then-else\"";
		ifelse->next[2]=S2;
		S2->name="else_body";
		//outcode(-1,"goto");
	}else{
		ifelse->name="\"if-then\"";
	}
	mid.nextlist=merge(pro.nextlist,other.nextlist);
	mid.type=type["void"];
	
	return mid;
		
}
midnode otherwiseStatement(normalNode * p,treeNode* tree){
	midnode mid,m, pro;
	if(p->use==61){//otherwiseStatement -> else { M Program } ;
		m=M(p->next[2]);
		pro=program(p->next[3],tree);
		mid.type=type["void"];
		mid.quad=m.quad;
		mid.nextlist=pro.nextlist;
		mid.op="else";
	}else if(p->use==62){//otherwiseStatement -> ε
		mid.quad=nextquad;
		mid.nextlist=makelist(nextquad);
		mid.type=type["void"];
	}
	return mid;
}
void gen_graphic(treeNode* root){//画图 
   freopen("graph.dot","w",stdout);
    cout<<"digraph G{"<<endl;
    cout<<"node[shape =record, charset= \"UTF-8\", fontname=\"FangSong\", fontsize=14]"<<endl;
	int ncount=1;
	queue< treeNode* >  queue; 
	queue.push(root);
	root->number=ncount; 
	cout<<"node"<<ncount<<"[label="<<root->name<<"]"<<endl;
	while(!queue.empty()){
		treeNode* temp=queue.front();
		queue.pop();
		int i=0;
		while(temp->next[i]){
			queue.push(temp->next[i]);
			ncount++;
			int n=temp->number;
			temp->next[i]->number=ncount;
			string child=temp->next[i]->name;
			if(temp->next[i]->name=="=") child="\"=\"";
			if(temp->next[i]->name=="==") child="\"==\"";
			if(temp->next[i]->name=="+") child="\"\\+\"";
			if(temp->next[i]->name=="-") child="\"\\-\"";
			if(temp->next[i]->name=="*") child="\"*\"";
			if(temp->next[i]->name=="/") child="\"/\"";
			if(temp->next[i]->name=="%") child="\"%\"";
			if(temp->next[i]->name=="&") child="\"&\"";
			if(temp->next[i]->name=="|") child="\"|\"";
			if(temp->next[i]->name=="&&") child="\"&&\"";
			if(temp->next[i]->name=="||") child="\"||\"";
			if(temp->next[i]->name==">") child="\"\\>\"";
			if(temp->next[i]->name=="<") child="\"\\<\"";
			if(temp->next[i]->name==">=") child="\">=\"";
			if(temp->next[i]->name=="<=") child="\"<=\"";
			if(temp->next[i]->name=="<>") child="\"<>\"";
			if(temp->next[i]->name=="[]") child="\"[]\"";
			cout<<"node"<<ncount<<"[label="<<child<<"]"<<endl;
			cout<<"node"<<n<<"->node"<<ncount<<endl;
			i++;
		}
	}
    cout<<"}"<<endl;
  fclose(stdout);
   system("dot -Tpng graph.dot -o sample.png");
}
int main(int argc, char** argv) {
	normalNode* r=SynAnalysis();
	initType();
	topS.push_back(0);
	rnext->name="program";
	rnext->pre= Troot;
	Troot->next[0]=rnext;
	cout<<"###################语义分析#######################"<<endl; 
	midnode m=program(r,rnext); 
/*	cout<<"###################符号表########################"<<endl; 
	cout<<"名称\t类型\t定义行\t使用行\t维数\t存储地址"<<endl;
	for(int i=0;i<symbolTable.size();i++){//符号表 
		cout<<symbolTable[i].name<<"\t"<<symbolTable[i].type<<"\t"<<symbolTable[i].defineRow<<"\t";
		for(int j=0;j<symbolTable[i].useRows.size();j++){
			cout<<symbolTable[i].useRows[j]<<",";
		}
		cout<<"\t"<<symbolTable[i].dimension<<"\t"<<symbolTable[i].pos<<endl;
	}*/
	cout<<"###################中间代码#######################"<<endl;
	for(int i=0;i<codes.size();i++){
		if(codes[i].op=="goto")
			cout<<codes[i].op<<" "<<codes[i].result<<endl;
		else if(codes[i].op=="param"||codes[i].op=="get"||codes[i].op=="put")
			cout<<codes[i].op<<" "<<symbolTable[codes[i].result].name<<endl;
		else if(codes[i].op=="call")
			cout<<codes[i].op<<" "<<symbolTable[codes[i].result].name<<" "<<symbolTable[codes[i].arg1].val<<endl;
		else if(codes[i].op=="if==goto"||codes[i].op=="if>goto"||codes[i].op=="if<goto"||codes[i].op=="if>=goto"||
		codes[i].op=="if<=goto"||codes[i].op=="if<>goto") 
			cout<<codes[i].op<<" "<<symbolTable[codes[i].arg1].name<<" "<<symbolTable[codes[i].arg2].name<<" "<<codes[i].result<<endl; 
		else if(codes[i].op=="=")
			cout<<codes[i].op<<" "<<symbolTable[codes[i].arg1].name<<" "<<symbolTable[codes[i].result].name<<endl; 
		else
			cout<<codes[i].op<<" "<<symbolTable[codes[i].arg1].name<<" "<<symbolTable[codes[i].arg2].name<<" "<<symbolTable[codes[i].result].name<<endl; 

	}
	//绘制抽象语法树
	if(m.type!=type["type_error"]) 
		gen_graphic(rnext);
	return 0;
}
