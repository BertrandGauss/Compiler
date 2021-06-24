#include <iostream>
#include <cstring> 
#include <fstream>
#include<windows.h> 
#include<cctype>
#include <map> 
#include <stack>
#include <vector> 
#include <stdio.h> 
#include <set>
#include <queue>
#include "experiment1.h"

using namespace std;

const int infer = 201;
const int emptynull = 202;
const int grammerend = 203;
const int synch =204;
const int Max=10000;
const int Max_Proc=500;
const int Max_Length=700;
const int Max_Ter=100;
const int Max_NonTer=100;
 

extern map<string,int> keyword;//关键词 
extern map<string,int> Delimiter;//分割符 
extern map<string,int> oper;//运算符 
int proNum = 0; //文法数目 

int proc[Max_Proc][Max_Length];//文法，proc[i][j]
vector<int> empty;//可以推出空的非终结符 
vector<int> emptyRecu;//防止递归 
vector<int> followRecu;//在求follow时防止递归 
vector<int> connectFirst;//用于连接first和follow集，以便构造follow集 
map<string,int> id_digits;//数字和标识符 
map<string,int> specialMap;//特殊符号 
vector<pair<string,int> > nonTer;//非终结符的集合 
vector<pair<string,int> > Ter;//终结符集合  
vector<int>  first[Max_Length]; //first集 
vector<int>  follow[Max_Length];//follow集 

map<int,int> firstVisit; //标记该非终结符的first是否求过 
map<int,int> followVisit;//标记该非终结符的follow是否求过 
int Table [Max_NonTer][Max_Ter][Max_Length]; //带同步信号的LL(1)分析表 


struct normalNode{//分析树节点 
	string type;
	int number;//该节点的标号 
	normalNode* pre;//该节点的父节点 
	normalNode* bro;//该节点的右边的兄弟 
	normalNode* next[Max_Length];//该节点的子节点集合 
};
normalNode* t= new normalNode();//语法分析树的根节点 

void initid_digits(){//初始化数字标识符表 
	id_digits["id"]=35;
	id_digits["digits16"]=36;
	id_digits["digits8"]=37;
	id_digits["digits"]=38;
} 
void initspecialMap(){//初始化特殊符号表
	specialMap["->"]=infer;
	specialMap["ε"]=emptynull;
	specialMap["$"]=grammerend;
	specialMap["synch"] =synch;	
} 
 
int dynamicNonTer(string word)  //动态生成非终结符的标号 
{  
    int i = 0;  
    int dynamicNum;  
    for(i=0; i<nonTer.size(); i++)  {  
        if(word==nonTer[i].first)  
            return nonTer[i].second;  
    }  
    if(i == nonTer.size())  {  
        if(i == 0){  
            dynamicNum = GRAMMAR_BASE;  
            nonTer.push_back(make_pair(word,dynamicNum));  
        }  
        else{  
            dynamicNum = nonTer[nonTer.size()-1].second + 1;  
            nonTer.push_back(make_pair(word,dynamicNum));  
        }  
    }  
    return dynamicNum;  
}  
string searchName(int id){//通过标号获取符号的名称 
	for(int i=0;i<Ter.size();i++){
		if(Ter[i].second==id){
			return Ter[i].first;
		}
	} 
	for(int i=0;i<nonTer.size();i++){
		if(nonTer[i].second==id){
			return nonTer[i].first;
		}
	} 
	if(id==infer)
		return "->";
	if(id==emptynull)
		return "ε";
	if(id==grammerend)
		return "$";
	else return "";
	/*if(id==synch)
		return "synch";*/
}
void initGrammer(){//初始化文法 
	init();
	initid_digits();
	initspecialMap();
	FILE* infile;
	int line = 1;  //
    int count = 0; 	
	infile = fopen("wenfa.txt","r");
	if(!infile){  
        cout<<"文法打开失败\n";  
        return;  
    }  
    char ch = fgetc(infile);  
    while(ch!=EOF){   
    	string result=""; 
        while(ch == ' ' || ch == '\t')  
            ch = fgetc(infile);  
        while(ch!=' ' && ch!= '\n' && ch!=EOF){  
            result  += ch;  
            ch = fgetc(infile);  
        }  
        while(ch == ' ' || ch == '\t')  
            ch = fgetc(infile);  
		if(keyword[result]||Delimiter[result]||oper[result]||id_digits[result]) {
		  	int num=0;
		  	if(keyword[result]){
				num= keyword[result];	
			}	
			else if(Delimiter[result]){
				num= Delimiter[result];
			}
			else if(oper[result]){
				num= oper[result];
			}
			else if(id_digits[result]){
				num= id_digits[result];
			}
			count++;
			proc[line][count]=num;	
			int flag=0 ;
		  	for(int i=0; i<Ter.size(); i++) { //得到终结符集合 
        		if(result==Ter[i].first) {  
            		flag= Ter[i].second;  
        		}  
   			}  
   			if(!flag){
   				Ter.push_back(make_pair(result,num));				
			} 

		}
		else if(!specialMap[result]){
			int dynamicNum= dynamicNonTer(result); 			
			count++;
			proc[line][count]=dynamicNum;
		}
		else{			
			count++;
			proc[line][count]=specialMap[result];
		}
        if(ch == '\n') { //文法换行 
            count = 0;    
            line++;  
            ch = fgetc(infile); 
			proNum++; 
        }  
    }
    
    cout<<"*************************************文法********************************"<<endl;
	for(int i=1;i<=proNum;i++){
    	for(int j=1;j<Max_Length;j++){
    		if(proc[i][j])  
                cout<<searchName(proc[i][j])<<' ';  
      		else 
                break;  
		}
		cout<<endl;
	}
	cout<<"************************************终结符*****************************"<<endl;  
    for(int i=0; i<Ter.size(); i++)  
        cout<<Ter[i].first<<' ';
    cout<<endl; 
    cout<<"************************************非终结符*****************************"<<endl;  
    for(int i=0; i<nonTer.size(); i++)  
        cout<<nonTer[i].first<<' '; 
    cout<<endl;  	
}
bool is_Terminal(int id){//是否为终结符 
	for(int i=0;i<Ter.size();i++){
		if(id==Ter[i].second)
			return true;
	} 
	return false;
} 
bool is_in(int id,vector<int>& array){
	int i=0;
	while(array[i]){
		if(array[i]==id) return true;
		i++;
	}
	return false;
}

bool infer_emptynull(int id){//是否为空 
    int flag = 0; 
    bool result=true;
	emptyRecu.push_back(id);
	if(is_in(id,empty))
	 	return true;
    for(int j = 1; j<=proNum; j++){  
        if(proc[j][1] == id){  
             int rightLength = 3;  //产生式右边的长度 
                for(rightLength = 3;rightLength<Max_Length; rightLength++){  
                    if(proc[j][rightLength] == 0)  
                        break;   
                }       
            rightLength--;  
            if(rightLength - 2 == 1 && is_in(proc[j][rightLength],empty))// 如果长度为1，并且已经求过    
	  			return true; 
            else if(rightLength -2 == 1 && is_Terminal(proc[j][rightLength]))//如果长度为1，并且是终结符
				return false;  
            else{  //如果长度超过了2
                for(int k=3; k<=rightLength; k++){  
                    if(is_in(proc[j][k],emptyRecu)) 
                       flag = 1;  
                }  
                if(flag)  
                    continue;  
                else{  
                    for(int k=3; k<=rightLength; k++){  
                        result= result&&infer_emptynull(proc[j][k]);  
                        emptyRecu.push_back(proc[j][k]) ; 
                    }  
                }  
            }  
            if(!result)  
                continue;  
            else if(result)  
                return true;  
        }  
    }  
    return false;	
}
void emptynullSet(int emptynull){   
    for(int j = 1; j<=proNum; j++){  
		//cout<<proc[j][3]<<endl;     
        if(proc[j][3] == emptynull&&proc[j][4]==0){//如果右边的第一个是该字符，并且长度只有1 
            empty.push_back(proc[j][1]) ;
            //cout<<empty.size()<<endl;
            emptynullSet(proc[j][1]);  
        }  
    }  
}  
void findFirstSet(int id){ //求first集，传入的参数是在非终结符集合中的序号  
    int k = 0;      
    int currentNum = nonTer[id].second;//非终结符的标号  
    for(int j = 1; j<=proNum;j++){  //遍历全部产生式  
        if(currentNum == proc[j][1]){//找到该非终结符的产生式 
            if(is_Terminal(proc[j][3] )|| proc[j][3] == emptynull){  //当右边的第一个是终结符或者空的时候
            
				first[id].push_back(proc[j][3]);//并入当前非终结符的first集中
            }  
            else if(!is_Terminal(proc[j][3])){ //当右边的第一个是非终结符的时候
                if(proc[j][3] == currentNum)//遇到左递归 
                    continue; 
				while(nonTer[k].second != proc[j][3])  k++; //记录下右边第一个非终结符的位置                      
                if(firstVisit[k] == 0){  //非终结符未访问过 
                    findFirstSet(k);  
                    firstVisit[k] = 1;  
                }  
                for(int i=0;i<first[k].size();i++){
                	if(first[k][i]!=emptynull){
                		first[id].push_back(first[k][i]);// 
					}
				}
                
                int rightLength = 0;  //产生式右部的长度 
                for(rightLength = 3;rightLength<Max_Length; rightLength++){  
                    if(proc[j][rightLength] == 0)  
                        break;
                }                   
                for(int k = 3; k<rightLength; k++){ 
                
                    if(infer_emptynull(proc[j][k]) && k<rightLength -1) { //如果右部的当前字符能推出空并且还不是最后一个字符，就将之后的一个字符并入first集中  
                    if(is_Terminal(proc[j][k+1])){
                    	first[id].push_back(proc[j][k+1]);
                    		break;
					}
							
					
                        int u = 0;  
                        while(nonTer[u].second != proc[j][k+1]) u++;//记录下一个符号的位置  
                        if(firstVisit[u] == 0){  
                            findFirstSet(u);  
                            firstVisit[u] = 1;  
                        }  
                    	for(int i=0;i<first[k].size();i++){
                			if(first[u][i]!=emptynull)
                				first[id].push_back(first[u][i]);
						}  
                    }  
                    else if(infer_emptynull(proc[j][k])&& k == rightLength -1)  //到达最后一个字符，并且产生式右部都能推出空,将空值并入First集中     
						first[id].push_back(emptynull); 
                    else  
                        break;
                }  
            }  
        }  
    }  
    firstVisit[id] = 1;//标记为访问过 
}  
void First() { //first   
    emptynullSet(emptynull);//先求出能直接推出空的非终结符集合  
	for(int i=0; i<nonTer.size(); i++) 
        findFirstSet(i);
	for(int i=0; i<nonTer.size(); i++) {//去重 
		set<int> s(first[i].begin(), first[i].end());
    	first[i].assign(s.begin(), s.end());
	}  
    cout<<"************************************First集******************************"<<endl;
    for(int i=0; i<nonTer.size(); i++){  
        cout<<"First<"<<nonTer[i].first<<"> = ";  
        for(int j=0;j<first[i].size(); j++){  
			cout<<searchName(first[i][j])<<" ";
        }  
        cout<<endl;  
    }  
}  
bool is_right(int num,int* pro){//是否在产生式的右边 
	for(int i=3;i<Max_Length;i++){
		if(pro[i]==num)
			return true;
	} 
	return false;
} 
 
void connectFirstSet(int *p){  //在求follo集时，把下一个非终结符的first和follow连起来 
    int i = 0;  
    int flag = 0;  
    //如果P的长度为1  
    if(p[1] == -1){  
        if(p[0] == emptynull) 
            connectFirst.push_back(emptynull) ;  
        else {  
            for(i=0; i<nonTer.size(); i++){  
                if(nonTer[i].second == p[0])  {  
                    flag = 1;
					for(int j=0;j<first[i].size();j++)  
                    	connectFirst.push_back(first[i][j]);  
                    break;  
                }  
            }         
            if(flag == 0) {  //是终结符
                for(i=0; i<Ter.size(); i++) {  
                    if(Ter[i].second == p[0]) {  
                        connectFirst.push_back(Ter[i].second);  
                        break;  
                    }  
                }  
            }  
        }  
    }      
    else{  //p长度大于1  
        for(i=0; i<nonTer.size(); i++){  
            if(nonTer[i].second == p[0]){  
                flag = 1;  
                for(int j=0;j<first[i].size();j++){
                	if(first[i][j]!=emptynull)
                		connectFirst.push_back(first[i][j]);	
				}                   	    
                break;  
            }  
        }  
        if(flag == 0){  //是终结符 
            for(i=0; i<Ter.size(); i++){  
                if(Ter[i].second == p[0]){   
                    connectFirst.push_back(Ter[i].second);  
                    break;  
                }  
            }  
        }  
        flag = 0;  
        int length = 0;  
        for(length = 0;; length++){  
            if(p[length] == 0)  
                break;  
        }  
        for(int k=0; k<length; k++){  
            emptyRecu.clear();//初始化，清空之前的值 
  			if(infer_emptynull(p[k])  && k<length -1){//如果右部的当前字符能推出空并且还不是最后一个字符，就将之后的一个字符并入First集中  
                int u = 0;  
                for(u=0; u<nonTer.size(); u++){          
                    if(nonTer[u].second == p[k+1]) {  
                        flag = 1;  
                        for(int j=0;j<first[u].size();j++){
                			if(first[u][j]!=emptynull)
                				connectFirst.push_back(first[u][j]);          				
						}    
                        break;  
                    }  
                }              
                if(flag == 0) {  //终结符 
                    for(u=0; u<Ter.size(); u++){  
                        if(Ter[u].second == p[k+1]){
							connectFirst.push_back(Ter[u].second);  
                            break;  
                        }  
                    }  
                }  
                flag = 0;  
            }  
            else  
                break;  
        }  
    } 
	set<int> s(connectFirst.begin(), connectFirst.end());//去重 
    connectFirst.assign(s.begin(), s.end());  
}  
void findfollowSet(int id){//求follow集，传入的参数是在非终结符集合中的序号  
    int currentNum = nonTer[id].second;//当前的非终结符标号   
    bool result = true;  
    followRecu.push_back(currentNum);  
    if(proc[1][1] == currentNum)//如果当前符号就是开始符号,把特殊符号$加入其Follow集中  
 		follow[id].push_back(grammerend);  
    for(int j = 1; j<=proNum; j++){      
        if(is_right(currentNum,proc[j])){   //该非终结符在产生式的右边 
            int leftNum = proc[j][1];//产生式的左边 
            int leftPos = 0; //非终结符在产生式右边的位置 
            int rightLength = 1;  
            int k = 0;//k为该非终结符在产生式右部的序号  
            int PosArray[Max_Length]; 
			memset(PosArray,-1,sizeof(PosArray));   
            for(leftPos = 0; leftPos < nonTer.size(); leftPos++)  
                if(nonTer[leftPos].second == leftNum)  
                    break;  
            for(rightLength = 1;rightLength<=Max_Length; rightLength++){   
                if(proc[j][rightLength+2] == 0)  
                    break;  
                if(currentNum == proc[j][rightLength+2]){
                	PosArray[k] = rightLength;//把非终结符右边的符号的位置压入 
                	k++;               
				}                   
            }  
            rightLength--; 
			//cout<< rightLength<<endl;
            for(int i=0;i<Max_Length; i++){
                if(PosArray[i] == -1)  
                    break;   
                if(PosArray[i] == rightLength&&proc[j][PosArray[i]+2]!=leftNum){//如果该非终结符在右部产生式的最后，并且和左部符号不同 
                    if(is_in(leftNum,followRecu)){ 
						for(int l=0;l<follow[leftPos].size();l++) 
							follow[id].push_back(follow[leftPos][l]);
						continue; 
                    }  
                    if(followVisit[leftPos] == 0){  
                        findfollowSet(leftPos);  
                        followVisit[leftPos] = 1;  
                    }  
                    for(int l=0;l<follow[leftPos].size();l++) 
						follow[id].push_back(follow[leftPos][l]);  
                }
				else if(PosArray[i] == rightLength&&proc[j][PosArray[i]+2]==leftNum) continue; 
				else{  
                    int n = 0;  
                    result = true;
                    for(n=PosArray[i]+1; n<=rightLength; n++){  
                        emptyRecu.clear(); 
                        result =result&&infer_emptynull(proc[j][n+2]);  
                    }  
                    if(result){  
                        if(is_in(leftNum,followRecu)){ 
							for(int l=0;l<follow[leftPos].size();l++) 
								follow[id].push_back(follow[leftPos][l]);
							continue; 
	                    }  
	                    if(followVisit[leftPos] == 0){  
	                        findfollowSet(leftPos);  
	                        followVisit[leftPos] = 1;  
	                    }  
	                    for(int l=0;l<follow[leftPos].size();l++) 
							follow[id].push_back(follow[leftPos][l]);  
	                } 
                    int temp[Max_Length];  
                    for(int n=PosArray[i]+1; n<=rightLength; n++) {
                    	temp[n-PosArray[i]-1] = proc[j][n+2];
					}                           
                    temp[rightLength-PosArray[i]] = -1;  
                    connectFirst.clear();//初始化  
                    connectFirstSet(temp);  
                   	for(int l=0;l<connectFirst.size();l++){
                   		if(connectFirst[l]!=emptynull)
                   			follow[id].push_back(connectFirst[l]);	 
					   }
                }  
            }  
        }  
    }  
    followVisit[id] = 1; 
	set<int> s(follow[id].begin(), follow[id].end());//去重 
    follow[id].assign(s.begin(), s.end()); 
}                 

void Follow(){
	//First();
	
	for(int i=0; i<nonTer.size(); i++)  {  
        followRecu.clear(); 
        findfollowSet(i); 
    } 
	cout<<"************************************Follow集******************************"<<endl;  
    for(int i=0; i<nonTer.size(); i++){  
        cout<<"Follow<"<<nonTer[i].first<<"> = ";  
        for(int j=0;j<follow[i].size(); j++){  
			cout<<searchName(follow[i][j])<<" ";
        }  
        cout<<endl;  
    }   
}
void create_LL1(){//构造LL1分析表 
	vector<int> select[Max_Ter];//辅助构建分析表
	for(int i = 1; i<=proNum; i++){//遍历产生式  
        int leftNum = proc[i][1];// 产生式左部 
        int leftPos = 0;  
        bool result = true;  
        for(leftPos = 0; leftPos < nonTer.size(); leftPos++)  
            if(nonTer[leftPos].second == leftNum)  
                break;  
        int rightLength = 1;  
        for(rightLength = 1;; rightLength++)  
            if(proc[i][rightLength+2] == 0)  
                break;   
        rightLength--; 
		//select[i-1].push_back(1);  
        if(rightLength == 1 && proc[i][rightLength + 2] == emptynull){ //右部推出式的长度为1并且是空,select[i-1] = follow[左边] ,select[i-1] = follow[锟斤拷锟絔 
        	for(int j=0;j<follow[leftPos].size();j++){
        		select[i-1].push_back(follow[leftPos][j]);
			} 
        }          
        else {  // 如果右部不是空的时候,select[i-1] = first[右部全部] 如果右部能够推出空，select[i-1] = first[右部全部] ^ follow[左边]  
            int temp[Max_Length];  
            int n = 0;  
            memset(temp,-1,sizeof(temp));  
            for(n=1; n<=rightLength; n++)  
                temp[n-1] = proc[i][n+2];   
            temp[rightLength] = -1;  
            connectFirst.clear(); 
            connectFirstSet(temp);  
            for(int j=0;j<connectFirst.size();j++){
            	if(connectFirst[j]!=emptynull)
        			select[i-1].push_back(connectFirst[j]);
			}  
            for(n=1; n<=rightLength; n++){  
                emptyRecu.clear();  
                result =result && infer_emptynull(proc[i][n+2]);  
            }  
            if(result) {  //右部能推出空，将follow[左边]并入select[i-1]中 
                for(int j=0;j<follow[leftPos].size();j++){
            		if(follow[leftPos][j]!=emptynull)
        				select[i-1].push_back(follow[leftPos][j]);
				}   
            }  
        }  
    }
	fstream outfile;  
    outfile.open("prediceTableLL(1).tsv",ios::out);  
  
    for(int i=0; i<proNum; i++){  
        int m = 0;  
        for(int t=0; t<nonTer.size(); t++){  
            if(nonTer[t].second == proc[i+1][1]){  
                m = t;  
                break;  
            }  
        }  
        for(int j=0;j<select[i].size(); j++) {  
            for(int k=0; k<Ter.size(); k++){  
                if(Ter[k].second == select[i][j]){  
                    int n = 0;  
                    for(n=1; n<=Max_Length; n++){  
                        Table[m][k][n-1] = proc[i+1][n];  
                        if(proc[i+1][n] == 0)  
                            break; 
                    }  
                    break;  
                }    
            }  
            if(select[i][j]==grammerend){
            	for(int n=1; n<=Max_Length; n++){  
                        Table[m][Ter.size()][n-1] = proc[i+1][n];  
                        if(proc[i+1][n] == 0)  
                            break;  

                    } 	
			}
        }  
        
    }  
    for(int i=0; i<nonTer.size(); i++){
    	for(int j=0;j<Ter.size();j++){
			bool t=false;
			for(int k=0;k<follow[i].size();k++){
				if(follow[i][k]==Ter[j].second){
					t=true;
					break;
				}		
			}
			if(t&&!Table[i][j][0]){//添加同步信号 
				Table[i][j][0]=synch;
			}			
		}
	}
   // outfile<<',';
    for(int i=0;i<Ter.size();i++){
    	outfile<<'\t'<<Ter[i].first;
	}
	outfile<<endl;
	for(int i=0; i<nonTer.size(); i++){
		outfile<<nonTer[i].first;
		for(int j=0;j<Ter.size()+1;j++){
			outfile<<'\t';
			if(select[i][j]){				
				for(int k=0;; k++) {  
                	if(Table[i][j][k] == 0)   
                    	break;  
                	outfile<<searchName(Table[i][j][k]);  
				}			
			}
			if(Table[i][j][0]==synch)
				outfile<<"synch";
		}
		outfile<<endl;
	}   
    
    outfile.close();    
	
}
fstream resultfile("Result.txt");//语法分析过程 
void SynAnalysis_ErrorProc(node* node,string type){//错误处理 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);//字体为红色 
	cout<<"[语法错误]"<<node->row<<" row"<<','<<node->col<<" col "<<type<<endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);//锟斤拷锟斤拷为锟斤拷色 
	resultfile<<"[语法错误]"<<node->row<<" row"<<','<<node->col<<" col "<<type<<endl;
}
void gen_graphic(normalNode* root){//画图 
    freopen("graph.dot","w",stdout);
    cout<<"digraph G{"<<endl;
    cout<<"node[shape =record, charset= \"UTF-8\", fontname=\"FangSong\", fontsize=14]"<<endl;
	int ncount=1;
	queue< normalNode* >  queue; 
	queue.push(root);

	root->number=ncount; 

	cout<<"node"<<ncount<<"[label="<<root->type<<"]"<<endl;
	while(!queue.empty()){
		normalNode* temp=queue.front();
		queue.pop();
		int flag=0;
		for(int i=0;i<Ter.size();i++){
			if(temp->type==Ter[i].first){
				flag=1;
			}
		}
		if(temp->type=="ε") flag=1;
		if(flag)//遇到终结符直接弹出 
			continue;
		for(int i=0;;i++){
			queue.push(temp->next[i]);
			if(!(temp->next[i]->bro)){
				ncount++;
				int n=temp->number;
				temp->next[i]->number=ncount;
			
				string child=temp->next[i]->type;
				if(temp->next[i]->type=="ε") child="null";
				if(temp->next[i]->type=="(") child="\"(\"";
				if(temp->next[i]->type==")" )child="\")\"";
				if(temp->next[i]->type==";") child="\";\"";
				if(temp->next[i]->type==",") child="\",\"";
				if(temp->next[i]->type=="=") child="\"=\"";
				if(temp->next[i]->type=="==") child="\"==\"";
				if(temp->next[i]->type=="+") child="\"+\"";
				if(temp->next[i]->type=="-") child="\"-\"";
				if(temp->next[i]->type=="{") child="\"\\{\"";
				if(temp->next[i]->type=="]") child="\"\\]\"";
				if(temp->next[i]->type=="[") child="\"\\[\"";
				if(temp->next[i]->type=="}") child="\"\\}\"";
				if(temp->next[i]->type=="*") child="\"*\"";
				if(temp->next[i]->type=="/") child="\"/\"";
				if(temp->next[i]->type=="%") child="\"%\"";
				if(temp->next[i]->type=="&") child="\"&\"";
				if(temp->next[i]->type=="|") child="\"|\"";
				if(temp->next[i]->type=="&&") child="\"&&\"";
				if(temp->next[i]->type=="||") child="\"||\"";
				if(temp->next[i]->type==">") child="\"\\>\"";
				if(temp->next[i]->type=="<") child="\"<\"";
				if(temp->next[i]->type==">=") child="\">=\"";
				if(temp->next[i]->type=="<=") child="\"<=\"";
				if(temp->next[i]->type=="<>") child="\"<>\"";
				cout<<"node"<<ncount<<"[label="<<child<<"]"<<endl;
				cout<<"node"<<n<<"->node"<<ncount<<endl;

			break;
			}
			ncount++;
			temp->next[i]->number=ncount;
			string child=temp->next[i]->type;
			if(temp->next[i]->type=="ε") child="null";
			if(temp->next[i]->type=="(") child="\"(\"";
			if(temp->next[i]->type==")" )child="\")\"";
			if(temp->next[i]->type==";") child="\";\"";
			if(temp->next[i]->type==",") child="\",\"";
			if(temp->next[i]->type=="=") child="\"=\"";
			if(temp->next[i]->type=="==") child="\"==\"";
			if(temp->next[i]->type=="+") child="\"+\"";
			if(temp->next[i]->type=="-") child="\"-\"";
			if(temp->next[i]->type=="{") child="\"\\{\"";
			if(temp->next[i]->type=="]") child="\"\\]\"";
			if(temp->next[i]->type=="[") child="\"\\[\"";
			if(temp->next[i]->type=="}") child="\"\\}\"";
			if(temp->next[i]->type=="*") child="\"*\"";
			if(temp->next[i]->type=="/") child="\"/\"";
			if(temp->next[i]->type=="%") child="\"%\"";
			if(temp->next[i]->type=="&") child="\"&\"";
			if(temp->next[i]->type=="|") child="\"|\"";
			if(temp->next[i]->type=="&&") child="\"&&\"";
			if(temp->next[i]->type=="||") child="\"||\"";
			if(temp->next[i]->type==">") child="\"\\>\"";
			if(temp->next[i]->type=="<") child="\"\\<\"";
			if(temp->next[i]->type==">=") child="\">=\"";
			if(temp->next[i]->type=="<=") child="\"<=\"";
			if(temp->next[i]->type=="<>") child="\"<>\"";
	
			cout<<"node"<<ncount<<"[label="<<child<<"]"<<endl;
			int n=temp->number;
			cout<<"node"<<n<<"->node"<<ncount<<endl;			
		}		
	}
    cout<<"}"<<endl;
    fclose(stdout);
    system("dot -Tpng graph.dot -o sample.png");
}
void SynAnalysis(){//语法分析 
	node* root=lex();//词法分析结果 
	node* temp=root;

	int pos=1;
	int nodeLength=0;//词法分析记号流长度 
	int step=1;//分析到第几步 

	int lexResult[Max];
	
	while(temp->next){
		lexResult[nodeLength]=temp->number;
		nodeLength++;
		temp=temp->next;
	} 	
	lexResult[nodeLength]=grammerend;
	nodeLength++;
    vector<int> stack1;//栈
	stack1.push_back(grammerend); 
	stack1.push_back(proc[1][1]);//压入开始符号 
	root=root->next;
	int flag=1;
	
	normalNode* p= new normalNode();
	p->type = searchName(proc[1][1]);
	p->pre = t;
	t->next[0]= p;
	string ErrorType;
	while(pos!=nodeLength){
		//cout<<pos<<" "<<nodeLength<<endl;
		//cout<< searchName(stack1[stack1.size()-1])<<endl;
		if(lexResult[pos]==grammerend &&stack1[stack1.size()-1]==grammerend){
			break;		
		}
		if(root->number==0){
			root->number=grammerend;
			root->type="$";
		}	
		if(stack1[stack1.size()-1]==emptynull){//如果为空 
			stack1.pop_back();
			if(pos<=nodeLength-2){
				while(!p->bro) {p=p->pre;}//叶节点回溯 
				p= p->bro;
			}
		}
		if(root->number==stack1[stack1.size()-1]){
			cout<<"step:"<<step <<endl;
			cout<<"栈:  ";
			resultfile<<"step:"<<step <<endl;
			resultfile<<"栈:  ";
			for(int i=0;i<stack1.size();i++){
				cout<<searchName(stack1[i])<<' ';
				resultfile<<searchName(stack1[i])<<' ';
			}
			cout<<endl;
			resultfile<<endl;
			cout<<"输入："; 
			resultfile<<"输入："; 
			for (int i=pos;i<nodeLength;i++){
				cout<<searchName(lexResult[i])<<' ';
				resultfile<<searchName(lexResult[i])<<' ';
			}
			cout<<endl;
			resultfile<<endl;
			
			cout<<"匹配"<<endl; 
			resultfile<<"匹配"<<endl; 
			
			stack1.pop_back();
			if(pos<=nodeLength-2){
				while(!p->bro) {p=p->pre;}//叶节点回溯 
				p= p->bro;
			}
			
			root=root->next;
			pos=pos+1;
			step++;
			cout<<endl;
			resultfile<<endl;
		}
		else if(!is_Terminal(stack1[stack1.size()-1])){//栈顶是非终结符 

			int m=0,n=0;
			while(nonTer[m].second!=stack1[stack1.size()-1]) m++;
			if(root->number==grammerend) n=Ter.size();
			else while(Ter[n].second!=root->number) n++;

			if(Table[m][n][0]==0){//分析表为空 
				cout<<"step:"<<step <<endl;
				cout<<"栈:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"栈:  ";

				for(int i=0;i<stack1.size();i++){
					cout<<searchName(stack1[i])<<' ';
					resultfile<<searchName(stack1[i])<<' ';
				}
				
				cout<<endl;
				cout<<"输入："; 
				resultfile<<endl;
				resultfile<<"输入："; 

				for (int i=pos;i<nodeLength;i++){
					cout<<searchName(lexResult[i])<<' ';
					resultfile<<searchName(lexResult[i])<<' ';
				}
				cout<<endl;
				resultfile<<endl;
				
				if(root->type=="{"|root->type=="("|root->type=="int"|root->type=="bool"|root->type=="get"|root->type=="put"){//遇到结构开始符号 
					stack1.pop_back();
				}
				else{//错误 
					if(id_digits[root->type])
						ErrorType="丢失运算符"; 
					else 
						ErrorType=nonTer[m].first+"错误"; 
					SynAnalysis_ErrorProc(root,ErrorType);//错误处理 
					root=root->next;
					pos=pos+1;
				}
				step++;
				flag=0;
				 
			}
			else if(Table[m][n][0]==synch) {//遇到同步信号 

				flag=0;
				cout<<"step:"<<step <<endl;
				cout<<"栈:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"栈:  ";
				for(int i=1;i<stack1.size();i++){
					cout<<searchName(stack1[i])<<' ';
					resultfile<<searchName(stack1[i])<<' ';
				}
				cout<<endl;
				cout<<"输入："; 
				resultfile<<endl;
				resultfile<<"输入："; 
				
				for (int i=pos;i<nodeLength;i++){
					cout<<searchName(lexResult[i])<<' ';
					resultfile<<searchName(lexResult[i])<<' ';
				}
				cout<<endl;
				resultfile<<endl;
				if(nonTer[m].first=="factors")
					ErrorType="丢失操作数";
				else 
					ErrorType=nonTer[m].first+"错误"; 
				SynAnalysis_ErrorProc(root,ErrorType);//错误处理 
				
				stack1.pop_back();
				step++;
				
				cout<<endl;
				resultfile<<endl;
			}
			else{

				cout<<"step:"<<step <<endl;
				cout<<"栈:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"栈:  ";
				
				for(int i=0;i<stack1.size();i++){
					cout<<searchName(stack1[i])<<' ';
					resultfile<<searchName(stack1[i])<<' ';
				}
				cout<<endl;
				cout<<"输入："; 
				resultfile<<endl;
				resultfile<<"输入："; 
				
				for (int i=pos;i<nodeLength;i++){
					cout<<searchName(lexResult[i])<<' ';
					resultfile<<searchName(lexResult[i])<<' ';
				}
				
				cout<<endl;
				cout<<"产生式："; 
				
				resultfile<<endl;
				resultfile<<"产生式：";
				 
				stack1.pop_back();
				int len=0;
				while(Table[m][n][len]) len++;
				for(int i=0;i<len;i++){					
					cout<<searchName(Table[m][n][i])<<" ";
					resultfile<<searchName(Table[m][n][i])<<" ";		
				}

				cout<<endl;
				resultfile<<endl;
				for(int i=len-1;i>1;i--){				
					stack1.push_back(Table[m][n][i]);//反向入栈 
					normalNode* ten= new normalNode();
					ten->type = searchName(Table[m][n][i]);
					ten->pre = p;
					p->next[i-2]= ten;
										
				}
				normalNode* te=p->next[0];
				for(int i=1;i<len-2;i++){					
					te->bro=p->next[i];
					te=te->bro;					
				}
				p=p->next[0];
				step++;	
				cout<<endl;					
			}
		}
		else{
				cout<<"step:"<<step <<endl;
				cout<<"栈:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"栈:  ";

			for(int i=0;i<stack1.size();i++){
				cout<<searchName(stack1[i])<<' ';
				resultfile<<searchName(stack1[i])<<' ';
			}
							
			cout<<endl;
			cout<<"输入："; 
			resultfile<<endl;
			resultfile<<"输入："; 

			for (int i=pos;i<nodeLength;i++){
				cout<<searchName(lexResult[i])<<' ';
				resultfile<<searchName(lexResult[i])<<' ';
			}
			cout<<endl;
			resultfile<<endl;
			if(stack1[stack1.size()-1]==Delimiter[")"]) 
				ErrorType="丢失右括号"; 
			else{
				ErrorType="丢失"+searchName(stack1[stack1.size()-1]);
			}
			//pos++;
			flag=0;
			stack1.pop_back();
			SynAnalysis_ErrorProc(root,ErrorType);//语法错误 	
		}
	}
	if(flag==0){
		cout<<"失败"<<endl;
		resultfile<<"失败"<<endl; 
	}
	else{
		cout<<"成功"<<endl;
		resultfile<<"成功"<<endl;
		gen_graphic(t->next[0]);//成功才画语法分析树的图 
	}

}

int main(int argc, char** argv) {
	initGrammer();
	First();
	Follow();
	create_LL1();
	SynAnalysis();
	
	return 0;
}

