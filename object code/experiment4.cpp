#include <iostream>
#include <set>
#include "stdio.h"
#include <vector>
#include <cstring> 
#include <sstream>
#include "experiment2.h"
#include "experiment3.h"
#include <algorithm>
using namespace std; 
const int Max=500;
const int UNVISITED=10000000;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
extern map<string,int> keyword;//关键词 
extern map<string,int> Delimiter;//分割符 
extern map<string,int> oper;//运算符 
extern vector<symbolNode> symbolTable;
extern vector<code> codes;//中间代码 
pair<string,vector<int> > reg[32];//mips的0-32号寄存器
vector<int> baseblock;// 基本块 的入口 
bool markReg[32];//标记寄存器是否被使用 

int blockNum=1;//基本块数目 

struct mips{
	string op;
	string arg1;
	string arg2;
	string result;
	mips operator=(mips mi){
		mips m;
		m.op=mi.op;
		m.result=mi.result;
		m.arg1=mi.arg1;
		m.arg2=mi.arg2;
		//cout<<"ff"<<m.arg1<<endl; 
		return m;
	} 
};
vector<mips> mcodes;
void initreg(){//初始化寄存器
	 
	reg[0].first="$0";
	reg[1].first="$at";
	reg[2].first="$v0";
	reg[3].first="$v1";
	for(int i=0;i<=3;i++){
		string b;
    	stringstream ss;
		ss<<i;
    	ss>>b;
    	reg[i+2].first="$a"+b;
	}
	for(int i=0;i<=7;i++){
		string b;
    	stringstream ss;
		ss<<i;
    	ss>>b;
    	reg[i+8].first="$t"+b;
    	//cout<<reg[i+8].first<<endl;
	}
	for(int i=0;i<=7;i++){
		string b;
    	stringstream ss;
		ss<<i;
    	ss>>b;
    	reg[i+16].first="$s"+b;
	}		
	reg[24].first="$t8";
	reg[25].first="$t9";
	reg[26].first="$k0";
	reg[27].first="$k1";
	reg[28].first="$gp";
	reg[29].first="$sp";
	reg[30].first="$fp";
	reg[31].first="$ra";
} 
void slice(vector<code> c){//划分基本块
	for(int i=1;i<c.size();i++){
		if(c[i].op=="goto"||codes[i].op=="if==goto"||codes[i].op=="if>goto"||codes[i].op=="if<goto"||codes[i].op=="if>=goto"||
		codes[i].op=="if<=goto"||codes[i].op=="if<>goto"){//有条件跳转和无条件跳转 
			baseblock.push_back(c[i].result);
			baseblock.push_back(i+1);
			blockNum+=2;
		}
//		if(c[i].op=="call"){//函数调用 
//			
//		}
	} 
	
} 

void getnextuse(int start,int end){//计算变量的下次引用信息 
	for(int i=end-1;i>=start;i--){
		if(codes[i].op=="put"||codes[i].op=="get"||codes[i].op=="param"){
			codes[i].r.first=symbolTable[codes[i].result].nextuse;
			codes[i].r.second=symbolTable[codes[i].result].act;
			symbolTable[codes[i].result].nextuse=-1;
			symbolTable[codes[i].result].act=false;
		}else if(codes[i].op=="+"||codes[i].op=="-"||codes[i].op=="*"||codes[i].op=="/"||codes[i].op=="%"){
			codes[i].r.first=symbolTable[codes[i].result].nextuse;
			codes[i].r.second=symbolTable[codes[i].result].act;
			symbolTable[codes[i].result].nextuse=-1;
			symbolTable[codes[i].result].act=false;	
			codes[i].a1.first=symbolTable[codes[i].arg1].nextuse;
			codes[i].a1.second=symbolTable[codes[i].arg1].act;
			symbolTable[codes[i].arg1].nextuse=i;
			symbolTable[codes[i].arg1].act=true;
			codes[i].a2.first=symbolTable[codes[i].arg2].nextuse;
			codes[i].a2.second=symbolTable[codes[i].arg2].act;
			symbolTable[codes[i].arg2].nextuse=i;
			symbolTable[codes[i].arg2].act=true;		
		}else if(codes[i].op=="="){
			codes[i].r.first=symbolTable[codes[i].result].nextuse;
			codes[i].r.second=symbolTable[codes[i].result].act;
			symbolTable[codes[i].result].nextuse=-1;
			symbolTable[codes[i].result].act=false;	
			codes[i].a1.first=symbolTable[codes[i].arg1].nextuse;
			codes[i].a1.second=symbolTable[codes[i].arg1].act;
			symbolTable[codes[i].arg1].nextuse=i;
			symbolTable[codes[i].arg1].act=true;
		}else if(codes[i].op=="if==goto"||codes[i].op=="if>goto"||codes[i].op=="if<goto"||codes[i].op=="if>=goto"||
		codes[i].op=="if<=goto"){
			codes[i].a1.first=symbolTable[codes[i].arg1].nextuse;
			codes[i].a1.second=symbolTable[codes[i].arg1].act;
			symbolTable[codes[i].arg1].nextuse=i;
			symbolTable[codes[i].arg1].act=true;
			codes[i].a2.first=symbolTable[codes[i].arg2].nextuse;
			codes[i].a2.second=symbolTable[codes[i].arg2].act;
			symbolTable[codes[i].arg2].nextuse=i;
			symbolTable[codes[i].arg2].act=true;			
		}
	}	
} 
int searchReg(int x){
	for(int i=0;i<32;i++){
		for(int j=0;j<reg[i].second.size();j++){
			if(x==reg[i].second[j])
				return i;
		}
	}
	return -1;
}
mips outmipscode(string op,string rs="null",string rt="null",string rd="null"){
	mips m;
	m.op=op;
	m.result=rs;
	m.arg1=rt;
	m.arg2=rd;
	mcodes.push_back(m);
	return m;
}
int getReg(code c){
	if(c.op=="="){
		int x=searchReg(c.arg1);
		int y=searchReg(c.result); 
		if(y!=-1){
			if(reg[y].second.size()==1)
				return y;
		}else if(x!=-1){
			if(reg[x].second.size()==1&&c.a1.first==-1&&!c.a1.second)
				return x;
		}else 
		if(y!=-1){
			for(int j=0;j<reg[y].second.size();j++){
				if(reg[y].second[j]==c.result){
					reg[y].second[j]=-1;
					break;
				}
					
			}
		}
		for(int i=8;i<25;i++){
			if(!markReg[i]){
				reg[i].second.push_back(c.result);
				markReg[i]=true;
				return i;		
				}
			}
		if(c.r.first!=-1){//有下次引用
			return 24; 
		}
	}else if(c.op=="param"){
		for(int i=4;i<8;i++){
			if(!markReg[i]){
				reg[i].second.push_back(c.result);
				markReg[i]=true;
				return i;		
			}
		}
	}else if(c.op=="if>goto"||c.op=="if<goto"||c.op=="if>=goto"||c.op=="if<=goto"){
		for(int i=8;i<16;i++){//得到用于临时存储的 
			if(!markReg[i]){
				//reg[i].second.push_back(code.result);
				markReg[i]=true;
				return i;		
			}
		}
		//reg[8].second.push_back(code.result);
		return 8;
	}else if(c.op=="+"||c.op=="-"||c.op=="*"||c.op=="/"||c.op=="%"||c.op=="&"||c.op=="|"){
		int x=searchReg(c.result);
		//cout<<symbolTable[c.result].name<<endl;
		if(reg[x].second.size()==1){
			return x;
		}else{
			for(int i=8;i<25;i++){
				if(!markReg[i]){
					reg[i].second.push_back(c.result);
					markReg[i]=true;
					return i;		
				}
			}	
		} 
	}else if(c.op=="get"||c.op=="put"){
		int z=searchReg(c.result);
		if(z!=-1&&reg[z].second.size()==1)
		    return z;
		for(int i=16;i<25;i++){
				if(!markReg[i]){
					reg[i].second.push_back(c.result);
					markReg[i]=true;
					return i;		
				}
			}	
	}
} 
int getTempReg(){
	for(int i=8;i<25;i++){
		if(!markReg[i]){
			//reg[i].second.push_back(c.result);
			markReg[i]=true;
			return i;	
		}
	}
}
int main(int argc, char** argv){
	
	initreg();
	codes=SemAnalysis();

	baseblock.push_back(0);
	
	slice(codes);
	set<int> s(baseblock.begin(),baseblock.end());//去重 
    baseblock.assign(s.begin(), s.end()); 
	sort(baseblock.begin(),baseblock.end());
//	for(int i=0;i<baseblock.size();i++){
//		cout<<baseblock[i]<<endl;
//	}
	int j=1;
	mips m1;
	m1.result="main:";
	//cout<<"main:"<<endl;
	mcodes.push_back(m1);

	getnextuse(0,codes.size()); 

	for(int j=0;j<baseblock.size();j++){
		mips m;
		string b;
    	stringstream ss;
		ss<<j;
		ss>>b;
		if(j!=0){
			m.result="L"+b+":";
			mcodes.push_back(m);
			//cout<<m.result<<endl;	
		}
		if(j==baseblock.size()-1){
			for(int i=baseblock[j];i<codes.size();i++){
				if(codes[i].op=="="){
					mips m;
					int x= getReg(codes[i]);
					m.op="add";
					if(symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("add",reg[x].first,reg[searchReg(codes[i].arg1)].first,"$0");
					}		
					else{
						m=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
					}	
				}
				else if(codes[i].op=="goto"){
					mips m;
					int y;
					for(y=0;y<baseblock.size();y++){
						if(baseblock[y]==codes[i].result)
							break;
					}
					ss<<y;
					ss>>b;
					m=outmipscode("j","L"+b);
					//cout<<m.op<<" "<<m.result<<endl;
				}
				else if(codes[i].op=="+"){
					mips m,m1;
					m.op="add";
					int x= getReg(codes[i]);
					m.result=reg[x].first;
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("add",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg1)].first,symbolTable[codes[i].arg2].name);
					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg2].name);
					}
				}
				else if(codes[i].op=="-"){
					mips m,m1;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("sub",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						if(symbolTable[codes[i].arg2].val>0){
							m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg1)].first,"-"+symbolTable[codes[i].arg2].name);
						}
						else{
							string b;
    						stringstream ss;
							ss<<-symbolTable[codes[i].arg2].val;
							ss>>b;
							m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg1)].first,b);
						}
					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(symbolTable[codes[i].arg2].val>0){
							m=outmipscode("addi",reg[x].first,reg[y].first,"-"+symbolTable[codes[i].arg2].name);
						}
						else{
							string b;
    						stringstream ss;
							ss<<-symbolTable[codes[i].arg2].val;
							ss>>b;
							m=outmipscode("addi",reg[x].first,reg[y].first,b);
						}		
					}
				}else if(codes[i].op=="*"){
					mips m,m1,m2;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("mul",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("mul",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[y].first);

					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						m=outmipscode("mul",reg[x].first,reg[y].first,reg[z].first);
					}
				}else if(codes[i].op=="/"){
					mips m,m1,m2;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("div",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("div",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[y].first);

					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						m=outmipscode("div",reg[x].first,reg[y].first,reg[z].first);
					}
				}else if(codes[i].op=="%"){
					mips m,m1,m2;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("rem",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("rem",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[y].first);

					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						m=outmipscode("rem",reg[x].first,reg[y].first,reg[z].first);
					}
				}else if(codes[i].op=="&"){
					mips m,m1;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("and",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("andi",reg[x].first,reg[searchReg(codes[i].arg1)].first,symbolTable[codes[i].arg2].name);
					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("andi",reg[x].first,"$0",symbolTable[codes[i].arg2].name);
					}
				}else if(codes[i].op=="|"){
					mips m,m1;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("or",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("ori",reg[x].first,reg[searchReg(codes[i].arg1)].first,symbolTable[codes[i].arg2].name);
					}else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("ori",reg[x].first,"$0",symbolTable[codes[i].arg2].name);
					}
				}else if(codes[i].op=="get"){
					mips m1,m2,m3; 
					m1=outmipscode("li","$v0","5");
					m2=outmipscode("syscall");
					int x=getReg(codes[i]); 
					m=outmipscode("add",reg[x].first,"$v0","$0");
				}else if(codes[i].op=="put"){
					mips m1,m2,m3; 
					m1=outmipscode("add","$a0",reg[searchReg(codes[i].result)].first,"$0");
					m2=outmipscode("li","$v0","1");
					m=outmipscode("syscall");
				}else if(codes[i].op=="if==goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
						ss<<z;
						ss>>b;
						m=outmipscode("beq",reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first,"L"+b);				
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
						ss<<x;
						ss>>b;
						m=outmipscode("beq",reg[searchReg(codes[i].arg1)].first,reg[z].first,"L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();y++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
						ss<<x;
						ss>>b;
	
						m=outmipscode("beq",reg[z].first,reg[searchReg(codes[i].arg1)].first,"L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						int y;
						for(y=0;y<baseblock.size();y++){
							if(baseblock[y]==codes[i].result)
								break;
						}	
						ss<<y;
						ss>>b;
						m=outmipscode("beq",reg[x].first,reg[z].first,"L"+b);
					}
				}else if(codes[i].op=="if<goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
						ss<<z;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
						ss<<x;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg2)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
						ss<<x;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[x].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int y;
						for(y=0;y<baseblock.size();y++){
							if(baseblock[y]==codes[i].result)
								break;
						}	
						ss<<y;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					
				}else if(codes[i].op=="if>goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[searchReg(codes[i].arg1)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
						ss<<z;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg1)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
						ss<<x;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[z].first,reg[x].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					
				}else if(codes[i].op=="if<=goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[searchReg(codes[i].arg1)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
						ss<<z;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg1)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();y++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[z].first,reg[x].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}
				}else if(codes[i].op=="if>=goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
						ss<<z;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();y++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg2)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();y++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[x].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}
					
				}
					
				}
		}
		else{
			for(int i=baseblock[j];i<baseblock[j+1];i++){
					if(codes[i].op=="="){
						mips m;
						int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("add",reg[x].first,reg[searchReg(codes[i].arg1)].first,"$0");
					}		
					else{
						m=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
					}	
				}
				else if(codes[i].op=="goto"){
					mips m;
					int y;
					for(y=0;y<baseblock.size();y++){
						if(baseblock[y]==codes[i].result)
							break;
					}
					string b;
    				stringstream ss;
					ss<<y;
					ss>>b;
					m=outmipscode("j","L"+b);
				}
				else if(codes[i].op=="+"){
					mips m,m1;
					m.op="add";
					int x= getReg(codes[i]);
					m.result=reg[x].first;
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("add",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg1)].first,symbolTable[codes[i].arg2].name);
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg2)].first,symbolTable[codes[i].arg1].name);
					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg2].name);
					}
                }
				else if(codes[i].op=="-"){
					mips m,m1;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("sub",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						if(symbolTable[codes[i].arg2].val>0){
							m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg1)].first,"-"+symbolTable[codes[i].arg2].name);
						}
						else{
							string b;
    						stringstream ss;
							ss<<-symbolTable[codes[i].arg2].val;
							ss>>b;
							m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg1)].first,b);
						}
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						if(symbolTable[codes[i].arg1].val>0){
							m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg2)].first,"-"+symbolTable[codes[i].arg1].name);
						}
						else{
							string b;
    						stringstream ss;
							ss<<-symbolTable[codes[i].arg1].val;
							ss>>b;
							m=outmipscode("addi",reg[x].first,reg[searchReg(codes[i].arg2)].first,b);
						}
					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(symbolTable[codes[i].arg2].val>0){
							m=outmipscode("addi",reg[x].first,reg[y].first,"-"+symbolTable[codes[i].arg2].name);
						}
						else{
							string b;
    						stringstream ss;
							ss<<-symbolTable[codes[i].arg2].val;
							ss>>b;
							m=outmipscode("addi",reg[x].first,reg[y].first,b);
						}		
					}
				}else if(codes[i].op=="*"){
					mips m,m1,m2;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("mul",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("mul",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[y].first);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("mul",reg[x].first,reg[searchReg(codes[i].arg2)].first,reg[y].first);

					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						m=outmipscode("mul",reg[x].first,reg[y].first,reg[z].first);
					}
				}else if(codes[i].op=="/"){
					mips m,m1,m2;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("div",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("div",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[y].first);
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("div",reg[x].first,reg[searchReg(codes[i].arg2)].first,reg[y].first);
					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						m=outmipscode("div",reg[x].first,reg[y].first,reg[z].first);
					}
				}else if(codes[i].op=="%"){
					mips m,m1,m2;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("rem",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("rem",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[y].first);
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("rem",reg[x].first,reg[searchReg(codes[i].arg2)].first,reg[y].first);
					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						m=outmipscode("rem",reg[x].first,reg[y].first,reg[z].first);
					}
				}else if(codes[i].op=="&"){
					mips m,m1;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("and",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("andi",reg[x].first,reg[searchReg(codes[i].arg1)].first,symbolTable[codes[i].arg2].name);
					}else if(symbolTable[codes[i].arg1].param!=-1&&symbolTable[codes[i].arg2].param==-1){
						m=outmipscode("andi",reg[x].first,reg[searchReg(codes[i].arg2)].first,symbolTable[codes[i].arg1].name);
					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("andi",reg[x].first,"$0",symbolTable[codes[i].arg2].name);
					}
				}else if(codes[i].op=="|"){
					mips m,m1;
					int x= getReg(codes[i]);
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("or",reg[x].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						m=outmipscode("ori",reg[x].first,reg[searchReg(codes[i].arg1)].first,symbolTable[codes[i].arg2].name);
					}else if(symbolTable[codes[i].arg1].param!=-1&&symbolTable[codes[i].arg2].param==-1){
						m=outmipscode("ori",reg[x].first,reg[searchReg(codes[i].arg2)].first,symbolTable[codes[i].arg1].name);
					}
					else{
						int y=getTempReg();
						m1=outmipscode("addi",reg[y].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						m=outmipscode("ori",reg[x].first,"$0",symbolTable[codes[i].arg2].name);
					}
				}else if(codes[i].op=="get"){
					mips m1,m2,m3; 
					m1=outmipscode("li","$v0","5");
					m2=outmipscode("syscall");
					int x=getReg(codes[i]);
					m=outmipscode("add",reg[x].first,"$v0","$0");
				}else if(codes[i].op=="put"){
					mips m1,m2,m3; 
					m1=outmipscode("add","$a0",reg[searchReg(codes[i].result)].first,"$0");
					m2=outmipscode("li","$v0","1");
					m=outmipscode("syscall");
				}else if(codes[i].op=="if==goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<z;
    	                ss>>b;
						m=outmipscode("beq",reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first,"L"+b);
					}		
					else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<x;
    	                ss>>b;
						m=outmipscode("beq",reg[searchReg(codes[i].arg1)].first,reg[z].first,"L"+b);
					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();y++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<x;
    	                ss>>b;
						m=outmipscode("beq",reg[z].first,reg[searchReg(codes[i].arg1)].first,"L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[y].second.size()==0) markReg[y]=false;
						if(reg[z].second.size()==0) markReg[z]=false;
						int y;
						for(y=0;y<baseblock.size();y++){
							if(baseblock[y]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<y;
    	                ss>>b;
						m=outmipscode("beq",reg[x].first,reg[z].first,"L"+b);
					}
				}else if(codes[i].op=="if<goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<z;
    	                ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<z;
    	                ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg2)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<x;
    	                ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[x].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int y;
						for(y=0;y<baseblock.size();y++){
							if(baseblock[y]==codes[i].result)
								break;
						}	
		                string b;
    	                stringstream ss;
		                ss<<y;
    	                ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					
				}else if(codes[i].op=="if>goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[searchReg(codes[i].arg1)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}
						string b;
    	                stringstream ss;	
						ss<<z;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg1)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int x;
						for(x=0;x<baseblock.size();x++){
							if(baseblock[x]==codes[i].result)
								break;
						}	
						string b;
    	                stringstream ss;
						ss<<x;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}
					    string b;
    	                stringstream ss;	
						ss<<l;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[z].first,reg[x].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
						string b;
    	                stringstream ss;
						ss<<l;
						ss>>b;
						m=outmipscode("bne",reg[y].first,"$0","L"+b);
					}
					
				}else if(codes[i].op=="if<=goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[searchReg(codes[i].arg1)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
						string b;
    	                stringstream ss;
						ss<<z;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg1)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}	
				        string b;
    	                stringstream ss;
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg2)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}
				        string b;
    	                stringstream ss;	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);

					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[z].first,reg[x].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}
		                string b;
    	                stringstream ss;	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}
				}else if(codes[i].op=="if>=goto"){
					mips m1,m,m2,m3;
					int y=getTempReg();
					if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param==-1){
						m1=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[searchReg(codes[i].arg2)].first);
						int z;
						for(z=0;z<baseblock.size();z++){
							if(baseblock[z]==codes[i].result)
								break;
						}	
			            string b;
    	                stringstream ss;
						ss<<z;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
						
					}else if(symbolTable[codes[i].arg2].param!=-1&&symbolTable[codes[i].arg1].param==-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[searchReg(codes[i].arg1)].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}
		                string b;
    	                stringstream ss;	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);

					}else if(symbolTable[codes[i].arg2].param==-1&&symbolTable[codes[i].arg1].param!=-1){
						int z=getTempReg();
						m1=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg1].name);
						if(reg[z].second.size()==0) markReg[y]=false;
						m2=outmipscode("slt",reg[y].first,reg[z].first,reg[searchReg(codes[i].arg2)].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();y++){
							if(baseblock[l]==codes[i].result)
								break;
						}
		                string b;
    	                stringstream ss;	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}
					else{
						int x=getTempReg();
						m1=outmipscode("addi",reg[x].first,"$0",symbolTable[codes[i].arg1].name);
						int z=getTempReg();
						m2=outmipscode("addi",reg[z].first,"$0",symbolTable[codes[i].arg2].name);
						m3=outmipscode("slt",reg[y].first,reg[x].first,reg[z].first);
						if(reg[y].second.size()==0) markReg[y]=false;
						int l;
						for(l=0;l<baseblock.size();l++){
							if(baseblock[l]==codes[i].result)
								break;
						}
		                string b;
    	                stringstream ss;	
						ss<<l;
						ss>>b;
						m=outmipscode("beq",reg[y].first,"$0","L"+b);
					}	
				}
			}
					
		}
		
	}
	cout<<"##############################目标代码############################"<<endl; 
	for(int i=0;i<mcodes.size();i++){
		
		if(mcodes[i].op=="syscall"){
			cout<<mcodes[i].op<<endl; 
		}else if(mcodes[i].op=="j"){
			cout<<mcodes[i].op<<" "<<mcodes[i].result<<endl; 
		}else if(mcodes[i].op=="li"){
			cout<<mcodes[i].op<<" "<<mcodes[i].result<<","<<mcodes[i].arg1<<endl;
		}else if(mcodes[i].op=="add"||mcodes[i].op=="addi"||mcodes[i].op=="sub"||mcodes[i].op=="mul"||mcodes[i].op=="div"||mcodes[i].op=="rem"||mcodes[i].op=="slt"||mcodes[i].op=="beq"||mcodes[i].op=="bne"||
		mcodes[i].op=="andi"||mcodes[i].op=="and"||mcodes[i].op=="or"||mcodes[i].op=="ori"){
			cout<<mcodes[i].op<<" "<<mcodes[i].result<<","<<mcodes[i].arg1<<","<<mcodes[i].arg2<<endl;
		}else{
			cout<<mcodes[i].result<<endl;
		}
	}
	return 0;
}
