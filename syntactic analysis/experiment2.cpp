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
 

extern map<string,int> keyword;//�ؼ��� 
extern map<string,int> Delimiter;//�ָ�� 
extern map<string,int> oper;//����� 
int proNum = 0; //�ķ���Ŀ 

int proc[Max_Proc][Max_Length];//�ķ���proc[i][j]
vector<int> empty;//�����Ƴ��յķ��ս�� 
vector<int> emptyRecu;//��ֹ�ݹ� 
vector<int> followRecu;//����followʱ��ֹ�ݹ� 
vector<int> connectFirst;//��������first��follow�����Ա㹹��follow�� 
map<string,int> id_digits;//���ֺͱ�ʶ�� 
map<string,int> specialMap;//������� 
vector<pair<string,int> > nonTer;//���ս���ļ��� 
vector<pair<string,int> > Ter;//�ս������  
vector<int>  first[Max_Length]; //first�� 
vector<int>  follow[Max_Length];//follow�� 

map<int,int> firstVisit; //��Ǹ÷��ս����first�Ƿ���� 
map<int,int> followVisit;//��Ǹ÷��ս����follow�Ƿ���� 
int Table [Max_NonTer][Max_Ter][Max_Length]; //��ͬ���źŵ�LL(1)������ 


struct normalNode{//�������ڵ� 
	string type;
	int number;//�ýڵ�ı�� 
	normalNode* pre;//�ýڵ�ĸ��ڵ� 
	normalNode* bro;//�ýڵ���ұߵ��ֵ� 
	normalNode* next[Max_Length];//�ýڵ���ӽڵ㼯�� 
};
normalNode* t= new normalNode();//�﷨�������ĸ��ڵ� 

void initid_digits(){//��ʼ�����ֱ�ʶ���� 
	id_digits["id"]=35;
	id_digits["digits16"]=36;
	id_digits["digits8"]=37;
	id_digits["digits"]=38;
} 
void initspecialMap(){//��ʼ��������ű�
	specialMap["->"]=infer;
	specialMap["��"]=emptynull;
	specialMap["$"]=grammerend;
	specialMap["synch"] =synch;	
} 
 
int dynamicNonTer(string word)  //��̬���ɷ��ս���ı�� 
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
string searchName(int id){//ͨ����Ż�ȡ���ŵ����� 
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
		return "��";
	if(id==grammerend)
		return "$";
	else return "";
	/*if(id==synch)
		return "synch";*/
}
void initGrammer(){//��ʼ���ķ� 
	init();
	initid_digits();
	initspecialMap();
	FILE* infile;
	int line = 1;  //
    int count = 0; 	
	infile = fopen("wenfa.txt","r");
	if(!infile){  
        cout<<"�ķ���ʧ��\n";  
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
		  	for(int i=0; i<Ter.size(); i++) { //�õ��ս������ 
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
        if(ch == '\n') { //�ķ����� 
            count = 0;    
            line++;  
            ch = fgetc(infile); 
			proNum++; 
        }  
    }
    
    cout<<"*************************************�ķ�********************************"<<endl;
	for(int i=1;i<=proNum;i++){
    	for(int j=1;j<Max_Length;j++){
    		if(proc[i][j])  
                cout<<searchName(proc[i][j])<<' ';  
      		else 
                break;  
		}
		cout<<endl;
	}
	cout<<"************************************�ս��*****************************"<<endl;  
    for(int i=0; i<Ter.size(); i++)  
        cout<<Ter[i].first<<' ';
    cout<<endl; 
    cout<<"************************************���ս��*****************************"<<endl;  
    for(int i=0; i<nonTer.size(); i++)  
        cout<<nonTer[i].first<<' '; 
    cout<<endl;  	
}
bool is_Terminal(int id){//�Ƿ�Ϊ�ս�� 
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

bool infer_emptynull(int id){//�Ƿ�Ϊ�� 
    int flag = 0; 
    bool result=true;
	emptyRecu.push_back(id);
	if(is_in(id,empty))
	 	return true;
    for(int j = 1; j<=proNum; j++){  
        if(proc[j][1] == id){  
             int rightLength = 3;  //����ʽ�ұߵĳ��� 
                for(rightLength = 3;rightLength<Max_Length; rightLength++){  
                    if(proc[j][rightLength] == 0)  
                        break;   
                }       
            rightLength--;  
            if(rightLength - 2 == 1 && is_in(proc[j][rightLength],empty))// �������Ϊ1�������Ѿ����    
	  			return true; 
            else if(rightLength -2 == 1 && is_Terminal(proc[j][rightLength]))//�������Ϊ1���������ս��
				return false;  
            else{  //������ȳ�����2
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
        if(proc[j][3] == emptynull&&proc[j][4]==0){//����ұߵĵ�һ���Ǹ��ַ������ҳ���ֻ��1 
            empty.push_back(proc[j][1]) ;
            //cout<<empty.size()<<endl;
            emptynullSet(proc[j][1]);  
        }  
    }  
}  
void findFirstSet(int id){ //��first��������Ĳ������ڷ��ս�������е����  
    int k = 0;      
    int currentNum = nonTer[id].second;//���ս���ı��  
    for(int j = 1; j<=proNum;j++){  //����ȫ������ʽ  
        if(currentNum == proc[j][1]){//�ҵ��÷��ս���Ĳ���ʽ 
            if(is_Terminal(proc[j][3] )|| proc[j][3] == emptynull){  //���ұߵĵ�һ�����ս�����߿յ�ʱ��
            
				first[id].push_back(proc[j][3]);//���뵱ǰ���ս����first����
            }  
            else if(!is_Terminal(proc[j][3])){ //���ұߵĵ�һ���Ƿ��ս����ʱ��
                if(proc[j][3] == currentNum)//������ݹ� 
                    continue; 
				while(nonTer[k].second != proc[j][3])  k++; //��¼���ұߵ�һ�����ս����λ��                      
                if(firstVisit[k] == 0){  //���ս��δ���ʹ� 
                    findFirstSet(k);  
                    firstVisit[k] = 1;  
                }  
                for(int i=0;i<first[k].size();i++){
                	if(first[k][i]!=emptynull){
                		first[id].push_back(first[k][i]);// 
					}
				}
                
                int rightLength = 0;  //����ʽ�Ҳ��ĳ��� 
                for(rightLength = 3;rightLength<Max_Length; rightLength++){  
                    if(proc[j][rightLength] == 0)  
                        break;
                }                   
                for(int k = 3; k<rightLength; k++){ 
                
                    if(infer_emptynull(proc[j][k]) && k<rightLength -1) { //����Ҳ��ĵ�ǰ�ַ����Ƴ��ղ��һ��������һ���ַ����ͽ�֮���һ���ַ�����first����  
                    if(is_Terminal(proc[j][k+1])){
                    	first[id].push_back(proc[j][k+1]);
                    		break;
					}
							
					
                        int u = 0;  
                        while(nonTer[u].second != proc[j][k+1]) u++;//��¼��һ�����ŵ�λ��  
                        if(firstVisit[u] == 0){  
                            findFirstSet(u);  
                            firstVisit[u] = 1;  
                        }  
                    	for(int i=0;i<first[k].size();i++){
                			if(first[u][i]!=emptynull)
                				first[id].push_back(first[u][i]);
						}  
                    }  
                    else if(infer_emptynull(proc[j][k])&& k == rightLength -1)  //�������һ���ַ������Ҳ���ʽ�Ҳ������Ƴ���,����ֵ����First����     
						first[id].push_back(emptynull); 
                    else  
                        break;
                }  
            }  
        }  
    }  
    firstVisit[id] = 1;//���Ϊ���ʹ� 
}  
void First() { //first   
    emptynullSet(emptynull);//�������ֱ���Ƴ��յķ��ս������  
	for(int i=0; i<nonTer.size(); i++) 
        findFirstSet(i);
	for(int i=0; i<nonTer.size(); i++) {//ȥ�� 
		set<int> s(first[i].begin(), first[i].end());
    	first[i].assign(s.begin(), s.end());
	}  
    cout<<"************************************First��******************************"<<endl;
    for(int i=0; i<nonTer.size(); i++){  
        cout<<"First<"<<nonTer[i].first<<"> = ";  
        for(int j=0;j<first[i].size(); j++){  
			cout<<searchName(first[i][j])<<" ";
        }  
        cout<<endl;  
    }  
}  
bool is_right(int num,int* pro){//�Ƿ��ڲ���ʽ���ұ� 
	for(int i=3;i<Max_Length;i++){
		if(pro[i]==num)
			return true;
	} 
	return false;
} 
 
void connectFirstSet(int *p){  //����follo��ʱ������һ�����ս����first��follow������ 
    int i = 0;  
    int flag = 0;  
    //���P�ĳ���Ϊ1  
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
            if(flag == 0) {  //���ս��
                for(i=0; i<Ter.size(); i++) {  
                    if(Ter[i].second == p[0]) {  
                        connectFirst.push_back(Ter[i].second);  
                        break;  
                    }  
                }  
            }  
        }  
    }      
    else{  //p���ȴ���1  
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
        if(flag == 0){  //���ս�� 
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
            emptyRecu.clear();//��ʼ�������֮ǰ��ֵ 
  			if(infer_emptynull(p[k])  && k<length -1){//����Ҳ��ĵ�ǰ�ַ����Ƴ��ղ��һ��������һ���ַ����ͽ�֮���һ���ַ�����First����  
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
                if(flag == 0) {  //�ս�� 
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
	set<int> s(connectFirst.begin(), connectFirst.end());//ȥ�� 
    connectFirst.assign(s.begin(), s.end());  
}  
void findfollowSet(int id){//��follow��������Ĳ������ڷ��ս�������е����  
    int currentNum = nonTer[id].second;//��ǰ�ķ��ս�����   
    bool result = true;  
    followRecu.push_back(currentNum);  
    if(proc[1][1] == currentNum)//�����ǰ���ž��ǿ�ʼ����,���������$������Follow����  
 		follow[id].push_back(grammerend);  
    for(int j = 1; j<=proNum; j++){      
        if(is_right(currentNum,proc[j])){   //�÷��ս���ڲ���ʽ���ұ� 
            int leftNum = proc[j][1];//����ʽ����� 
            int leftPos = 0; //���ս���ڲ���ʽ�ұߵ�λ�� 
            int rightLength = 1;  
            int k = 0;//kΪ�÷��ս���ڲ���ʽ�Ҳ������  
            int PosArray[Max_Length]; 
			memset(PosArray,-1,sizeof(PosArray));   
            for(leftPos = 0; leftPos < nonTer.size(); leftPos++)  
                if(nonTer[leftPos].second == leftNum)  
                    break;  
            for(rightLength = 1;rightLength<=Max_Length; rightLength++){   
                if(proc[j][rightLength+2] == 0)  
                    break;  
                if(currentNum == proc[j][rightLength+2]){
                	PosArray[k] = rightLength;//�ѷ��ս���ұߵķ��ŵ�λ��ѹ�� 
                	k++;               
				}                   
            }  
            rightLength--; 
			//cout<< rightLength<<endl;
            for(int i=0;i<Max_Length; i++){
                if(PosArray[i] == -1)  
                    break;   
                if(PosArray[i] == rightLength&&proc[j][PosArray[i]+2]!=leftNum){//����÷��ս�����Ҳ�����ʽ����󣬲��Һ��󲿷��Ų�ͬ 
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
                    connectFirst.clear();//��ʼ��  
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
	set<int> s(follow[id].begin(), follow[id].end());//ȥ�� 
    follow[id].assign(s.begin(), s.end()); 
}                 

void Follow(){
	//First();
	
	for(int i=0; i<nonTer.size(); i++)  {  
        followRecu.clear(); 
        findfollowSet(i); 
    } 
	cout<<"************************************Follow��******************************"<<endl;  
    for(int i=0; i<nonTer.size(); i++){  
        cout<<"Follow<"<<nonTer[i].first<<"> = ";  
        for(int j=0;j<follow[i].size(); j++){  
			cout<<searchName(follow[i][j])<<" ";
        }  
        cout<<endl;  
    }   
}
void create_LL1(){//����LL1������ 
	vector<int> select[Max_Ter];//��������������
	for(int i = 1; i<=proNum; i++){//��������ʽ  
        int leftNum = proc[i][1];// ����ʽ�� 
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
        if(rightLength == 1 && proc[i][rightLength + 2] == emptynull){ //�Ҳ��Ƴ�ʽ�ĳ���Ϊ1�����ǿ�,select[i-1] = follow[���] ,select[i-1] = follow[���] 
        	for(int j=0;j<follow[leftPos].size();j++){
        		select[i-1].push_back(follow[leftPos][j]);
			} 
        }          
        else {  // ����Ҳ����ǿյ�ʱ��,select[i-1] = first[�Ҳ�ȫ��] ����Ҳ��ܹ��Ƴ��գ�select[i-1] = first[�Ҳ�ȫ��] ^ follow[���]  
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
            if(result) {  //�Ҳ����Ƴ��գ���follow[���]����select[i-1]�� 
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
			if(t&&!Table[i][j][0]){//���ͬ���ź� 
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
fstream resultfile("Result.txt");//�﷨�������� 
void SynAnalysis_ErrorProc(node* node,string type){//������ 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);//����Ϊ��ɫ 
	cout<<"[�﷨����]"<<node->row<<" row"<<','<<node->col<<" col "<<type<<endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);//����Ϊ��ɫ 
	resultfile<<"[�﷨����]"<<node->row<<" row"<<','<<node->col<<" col "<<type<<endl;
}
void gen_graphic(normalNode* root){//��ͼ 
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
		if(temp->type=="��") flag=1;
		if(flag)//�����ս��ֱ�ӵ��� 
			continue;
		for(int i=0;;i++){
			queue.push(temp->next[i]);
			if(!(temp->next[i]->bro)){
				ncount++;
				int n=temp->number;
				temp->next[i]->number=ncount;
			
				string child=temp->next[i]->type;
				if(temp->next[i]->type=="��") child="null";
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
			if(temp->next[i]->type=="��") child="null";
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
void SynAnalysis(){//�﷨���� 
	node* root=lex();//�ʷ�������� 
	node* temp=root;

	int pos=1;
	int nodeLength=0;//�ʷ������Ǻ������� 
	int step=1;//�������ڼ��� 

	int lexResult[Max];
	
	while(temp->next){
		lexResult[nodeLength]=temp->number;
		nodeLength++;
		temp=temp->next;
	} 	
	lexResult[nodeLength]=grammerend;
	nodeLength++;
    vector<int> stack1;//ջ
	stack1.push_back(grammerend); 
	stack1.push_back(proc[1][1]);//ѹ�뿪ʼ���� 
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
		if(stack1[stack1.size()-1]==emptynull){//���Ϊ�� 
			stack1.pop_back();
			if(pos<=nodeLength-2){
				while(!p->bro) {p=p->pre;}//Ҷ�ڵ���� 
				p= p->bro;
			}
		}
		if(root->number==stack1[stack1.size()-1]){
			cout<<"step:"<<step <<endl;
			cout<<"ջ:  ";
			resultfile<<"step:"<<step <<endl;
			resultfile<<"ջ:  ";
			for(int i=0;i<stack1.size();i++){
				cout<<searchName(stack1[i])<<' ';
				resultfile<<searchName(stack1[i])<<' ';
			}
			cout<<endl;
			resultfile<<endl;
			cout<<"���룺"; 
			resultfile<<"���룺"; 
			for (int i=pos;i<nodeLength;i++){
				cout<<searchName(lexResult[i])<<' ';
				resultfile<<searchName(lexResult[i])<<' ';
			}
			cout<<endl;
			resultfile<<endl;
			
			cout<<"ƥ��"<<endl; 
			resultfile<<"ƥ��"<<endl; 
			
			stack1.pop_back();
			if(pos<=nodeLength-2){
				while(!p->bro) {p=p->pre;}//Ҷ�ڵ���� 
				p= p->bro;
			}
			
			root=root->next;
			pos=pos+1;
			step++;
			cout<<endl;
			resultfile<<endl;
		}
		else if(!is_Terminal(stack1[stack1.size()-1])){//ջ���Ƿ��ս�� 

			int m=0,n=0;
			while(nonTer[m].second!=stack1[stack1.size()-1]) m++;
			if(root->number==grammerend) n=Ter.size();
			else while(Ter[n].second!=root->number) n++;

			if(Table[m][n][0]==0){//������Ϊ�� 
				cout<<"step:"<<step <<endl;
				cout<<"ջ:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"ջ:  ";

				for(int i=0;i<stack1.size();i++){
					cout<<searchName(stack1[i])<<' ';
					resultfile<<searchName(stack1[i])<<' ';
				}
				
				cout<<endl;
				cout<<"���룺"; 
				resultfile<<endl;
				resultfile<<"���룺"; 

				for (int i=pos;i<nodeLength;i++){
					cout<<searchName(lexResult[i])<<' ';
					resultfile<<searchName(lexResult[i])<<' ';
				}
				cout<<endl;
				resultfile<<endl;
				
				if(root->type=="{"|root->type=="("|root->type=="int"|root->type=="bool"|root->type=="get"|root->type=="put"){//�����ṹ��ʼ���� 
					stack1.pop_back();
				}
				else{//���� 
					if(id_digits[root->type])
						ErrorType="��ʧ�����"; 
					else 
						ErrorType=nonTer[m].first+"����"; 
					SynAnalysis_ErrorProc(root,ErrorType);//������ 
					root=root->next;
					pos=pos+1;
				}
				step++;
				flag=0;
				 
			}
			else if(Table[m][n][0]==synch) {//����ͬ���ź� 

				flag=0;
				cout<<"step:"<<step <<endl;
				cout<<"ջ:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"ջ:  ";
				for(int i=1;i<stack1.size();i++){
					cout<<searchName(stack1[i])<<' ';
					resultfile<<searchName(stack1[i])<<' ';
				}
				cout<<endl;
				cout<<"���룺"; 
				resultfile<<endl;
				resultfile<<"���룺"; 
				
				for (int i=pos;i<nodeLength;i++){
					cout<<searchName(lexResult[i])<<' ';
					resultfile<<searchName(lexResult[i])<<' ';
				}
				cout<<endl;
				resultfile<<endl;
				if(nonTer[m].first=="factors")
					ErrorType="��ʧ������";
				else 
					ErrorType=nonTer[m].first+"����"; 
				SynAnalysis_ErrorProc(root,ErrorType);//������ 
				
				stack1.pop_back();
				step++;
				
				cout<<endl;
				resultfile<<endl;
			}
			else{

				cout<<"step:"<<step <<endl;
				cout<<"ջ:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"ջ:  ";
				
				for(int i=0;i<stack1.size();i++){
					cout<<searchName(stack1[i])<<' ';
					resultfile<<searchName(stack1[i])<<' ';
				}
				cout<<endl;
				cout<<"���룺"; 
				resultfile<<endl;
				resultfile<<"���룺"; 
				
				for (int i=pos;i<nodeLength;i++){
					cout<<searchName(lexResult[i])<<' ';
					resultfile<<searchName(lexResult[i])<<' ';
				}
				
				cout<<endl;
				cout<<"����ʽ��"; 
				
				resultfile<<endl;
				resultfile<<"����ʽ��";
				 
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
					stack1.push_back(Table[m][n][i]);//������ջ 
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
				cout<<"ջ:  ";
				resultfile<<"step:"<<step <<endl;
				resultfile<<"ջ:  ";

			for(int i=0;i<stack1.size();i++){
				cout<<searchName(stack1[i])<<' ';
				resultfile<<searchName(stack1[i])<<' ';
			}
							
			cout<<endl;
			cout<<"���룺"; 
			resultfile<<endl;
			resultfile<<"���룺"; 

			for (int i=pos;i<nodeLength;i++){
				cout<<searchName(lexResult[i])<<' ';
				resultfile<<searchName(lexResult[i])<<' ';
			}
			cout<<endl;
			resultfile<<endl;
			if(stack1[stack1.size()-1]==Delimiter[")"]) 
				ErrorType="��ʧ������"; 
			else{
				ErrorType="��ʧ"+searchName(stack1[stack1.size()-1]);
			}
			//pos++;
			flag=0;
			stack1.pop_back();
			SynAnalysis_ErrorProc(root,ErrorType);//�﷨���� 	
		}
	}
	if(flag==0){
		cout<<"ʧ��"<<endl;
		resultfile<<"ʧ��"<<endl; 
	}
	else{
		cout<<"�ɹ�"<<endl;
		resultfile<<"�ɹ�"<<endl;
		gen_graphic(t->next[0]);//�ɹ��Ż��﷨��������ͼ 
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

