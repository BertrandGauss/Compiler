#include <iostream>
#include <cstring> 
#include <fstream>
#include<windows.h> 
#include<cctype>
#include <map> 
#include <vector> 
using namespace std;

const int MAX=100;  
const int Max=100000;

int row=0,col=0;//当前扫描指针指向的位置，row指行，col指列 
int totalrow=0;//总共的行数 
string ScanBuffer[MAX];//扫描缓冲区，定义为字符串数组
char c;//当前读到的字符 
string result;//定义字符串，保存从源程序中连续读取的字符串

int total=0;//总共字符数
int letters=0;//标识符数目
int digits=0;//数字个数 
int opers=0; //运算符个数
int Delimiters=0;//分界符个数 
int keywords=0;//关键词个数
int id=0;//不同标识符的个数 
 
map<string,int> keyword;//关键字表 
map<string,int> Delimiter;//分隔符表
map<string,int> oper;//运算符表 
map<string,int> mark;//标记每个符号存在符号表的第几位 

ofstream outfile("output.txt");//词法分析结果保存在文件中 
ofstream outputfile("symbol_table.csv");

struct symbol_table{//符号表 
	string name;//名字 
	vector<int> line;//使用行 
};
symbol_table symbolTable[Max];

symbol_table find_symbol(string name){//通过表示符的名字检索 
	int number=mark[name];
	return symbolTable[number]; 
} 

void insert_tabel(string name,int line) {//将新检索到的标识符加入符号表 
	if(mark[name]){
		symbolTable[mark[name]].line.push_back(line);
	}
	else{
		id++; 
		mark[name]=id;
		symbolTable[id].name=name;
		symbolTable[id].line.push_back(line);
	
	}
}

void save_symbol_table(){//保存符号表为csv文件方便之后使用 
	outputfile<<"序号"<<','<<"名字"<<','<<"使用行"<< endl;
	for(int i=1;i<=id;i++){
		string lines;
		outputfile<<i<<','<<symbolTable[i].name<<','<<symbolTable[i].line[0];
		for(int j=1;j<symbolTable[i].line.size();j++){
			outputfile<<' '<<symbolTable[i].line[j];
		}
		outputfile<<endl;
	}
}

void initKeyword(){//初始化关键字表 
keyword["int"]=1;
keyword["if"]=2;
keyword["while"]=3;
keyword["get"]=4;
keyword["put"]=5;
keyword["else"]=6;
keyword["bool"]=7;
keyword["true"]=8;
keyword["false"]=9; 
} 

void initDelimiter(){//初始化分界符表 
Delimiter[","]=10;
Delimiter["("]=11;
Delimiter[")"]=12;
Delimiter["{"]=13;
Delimiter["}"]=14;
Delimiter["["]=15;
Delimiter["]"]=16;
Delimiter[";"]=17;
} 

void initOperate(){//初始化运算符表 
oper["+"]=18;
oper["-"]=19;
oper["="]=20;
oper["=="]=21;
oper[">"]=22;
oper["<"]=23;
oper["<>"]=24;
oper[">="]=25;
oper["<="]=26;
oper["&&"]=27;
oper["||"]=28;
oper["!"]=29;
oper["&"]=30;
oper["|"]=31;
oper["*"]=32;
oper["/"]=33;
oper["%"]=34;
} 

void getChar(){//从缓冲区去读取 
	c=ScanBuffer[row][col];
	col++;
}

void jump(){//跳过空格、制表符 ，换行， 
	 while(c=='\n'){
		row++;
		col=0;
		c=ScanBuffer[row][col];
		col++;
	} 
	while(c==' '||c=='\t') {
		c=ScanBuffer[row][col];
		col++;
	}
}

void init(){//初始化 
	initKeyword();
	initDelimiter();
	initOperate();
}

void ErrorProc(string str){//错误处理，str为错误类型 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);//字体为红色 
	cout<<"[Error]"<<row+1<<" row"<<','<<col<<" col "<<result<<"为"<<str<<endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);//字体为白色 
} 

void back(){//回退一格 
	col--;
	c=ScanBuffer[row][col];
}

void LexicalAnalyzer(){//词法分析器
	//getChar();
	while(!(row==totalrow&&ScanBuffer[row][col]=='\n')){
		getChar(); 
		jump();
		result="";
		string ch;
		ch=c;
		if(isalpha(c)||c=='_'){
			//getChar();
			while(isalpha(c)||isdigit(c)||c=='_'){				
				result=result+c;
				getChar(); 
			}
			ch=c;
			if(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t'){
				if(keyword[result]){//为关键词 
					cout<<'<'<<result<<" , "<<keyword[result]<<'>'<<endl;
					outfile<<'<'<<result<<" , "<<keyword[result]<<'>'<<endl;
					keywords++;	
				}
				else{//为标识符 
					cout<<'<'<<result<<" , 35>"<<endl;
					outfile<<'<'<<result<<" , 35>"<<endl;
					letters++;
					insert_tabel(result,row+1); 
						
				}
			}
			else{//错误
				 
				while(!(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t')){				
					result=result+c;
					getChar(); 
				}
				back(); 
				ErrorProc("非法标识符");
				 
			}
			back(); 
			
		}
		else if(c=='0'){
			result=c;
			getChar();
			if(c=='x'or c=='X'){//十六进制数字 
				result=result+c;
				getChar();
				while(isdigit(c)|c=='A'|c=='B'|c=='C'|c=='D'|c=='E'|c=='F'){
					result=result+c;
					getChar(); 					
				}
				ch=c;
				if(!(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t')){
					while(!(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t')){				
						result=result+c;
						getChar(); 
					}
					back(); 
					ErrorProc("非法数字");
				}
				else{
					cout<<'<'<<result<<" , 36>"<<endl; 
					outfile<<'<'<<result<<" , 36>"<<endl;
					digits++;
				} 
				
			}
			else{//八进制数字 
				while(isdigit(c)&&c-'0'<8&&c-'0'>=0){
					result=result+c;
					getChar(); 
				}
				ch=c;
				if(!(Delimiter[ch]|oper[ch]|c==' ')){
					while(!(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t')){				
						result=result+c;
						getChar(); 
					}
					back(); 
					ErrorProc("非法数字");
				}
				else{
					cout<<'<'<<result<<" , 37>"<<endl; 
					outfile<<'<'<<result<<" , 37>"<<endl;
					digits++;
				} 
			}
			back();
				
		} 
		else if(isdigit(c)){//十进制数字 
			while(isdigit(c)){
				result=result+c;
				getChar(); 		
			}				
			ch=c;
			if(!(Delimiter[ch]|oper[ch]|c==' ')){
				while(!(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t')){				
				result=result+c;
				getChar(); 
				}
				back(); 	
				ErrorProc("非法数字");			
			}
			else{
				cout<<'<'<<result<<" , 38>"<<endl; 
				outfile<<'<'<<result<<" , 38>"<<endl;
				digits++;
			}
			back();
		}
		else if(Delimiter[ch]){
			result=ch;
			cout<<'<'<<result<<" , "<<Delimiter[result]<<'>'<<endl;
			outfile<<'<'<<result<<" , "<<Delimiter[result]<<'>'<<endl;
			Delimiters++;
		}
		else if(c=='>'||c=='<'||c=='='||c=='&'||c=='|'||c=='!'){
			result=c;
			getChar();
			if((c=='>'||c=='<'||c=='='||c=='&'||c=='|'||c=='!')&&oper[c+result]){
				result=result+c;
				
			}
			else{
				back(); 
			}
			cout<<'<'<<result<<" , "<<oper[result]<<'>'<<endl;
			outfile<<'<'<<result<<" , "<<oper[result]<<'>'<<endl;	

			opers++;			
		} 
		else if(c=='+'|c=='-'|c=='*'|c=='/'|c=='%'){
			result=c;
			cout<<'<'<<result<<" , "<<oper[result]<<'>'<<endl;
			outfile<<'<'<<result<<" , "<<oper[result]<<'>'<<endl;
			opers++;
		}
		else {
			result=c;
			ErrorProc("非法字符");
		}
		total++;
	}
	cout<<"行数："<<totalrow+1<<endl;
	cout<<"标识符个数："<<letters<<" 关键词个数："<<keywords;
	cout<<" 数字个数："<<digits<<" 分界符个数："<<Delimiters<<" 运算符个数："<<opers<<endl;
	cout<<"总共字符个数为："<<total<<endl;
}
int main(int argc, char** argv) {
	int rows=0;
	//char c;
    //string result;//定义字符串，保存从源程序中连续读取的字符串
    string file,outfile;//要进行词法分析的文件名 
    cout<<"请输入你要进行词法分析的文件的名字："<<endl; 
    cin>>file; 
    fstream infile;
	
    infile.open(file.data());//将文件流对象与文件连接起来
    init();
    
    while(infile.peek()!=EOF){//把文件读入缓冲区 
			c=infile.get();
			ScanBuffer[rows].push_back(c);
			if(c=='\n') rows++;
	}

	totalrow=--rows;
	LexicalAnalyzer();
	save_symbol_table();
	infile.close();
	outputfile.close();

	return 0;
}
