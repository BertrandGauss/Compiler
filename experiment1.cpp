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

int row=0,col=0;//��ǰɨ��ָ��ָ���λ�ã�rowָ�У�colָ�� 
int totalrow=0;//�ܹ������� 
string ScanBuffer[MAX];//ɨ�軺����������Ϊ�ַ�������
char c;//��ǰ�������ַ� 
string result;//�����ַ����������Դ������������ȡ���ַ���

int total=0;//�ܹ��ַ���
int letters=0;//��ʶ����Ŀ
int digits=0;//���ָ��� 
int opers=0; //���������
int Delimiters=0;//�ֽ������ 
int keywords=0;//�ؼ��ʸ���
int id=0;//��ͬ��ʶ���ĸ��� 
 
map<string,int> keyword;//�ؼ��ֱ� 
map<string,int> Delimiter;//�ָ�����
map<string,int> oper;//������� 
map<string,int> mark;//���ÿ�����Ŵ��ڷ��ű�ĵڼ�λ 

ofstream outfile("output.txt");//�ʷ���������������ļ��� 
ofstream outputfile("symbol_table.csv");

struct symbol_table{//���ű� 
	string name;//���� 
	vector<int> line;//ʹ���� 
};
symbol_table symbolTable[Max];

symbol_table find_symbol(string name){//ͨ����ʾ�������ּ��� 
	int number=mark[name];
	return symbolTable[number]; 
} 

void insert_tabel(string name,int line) {//���¼������ı�ʶ��������ű� 
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

void save_symbol_table(){//������ű�Ϊcsv�ļ�����֮��ʹ�� 
	outputfile<<"���"<<','<<"����"<<','<<"ʹ����"<< endl;
	for(int i=1;i<=id;i++){
		string lines;
		outputfile<<i<<','<<symbolTable[i].name<<','<<symbolTable[i].line[0];
		for(int j=1;j<symbolTable[i].line.size();j++){
			outputfile<<' '<<symbolTable[i].line[j];
		}
		outputfile<<endl;
	}
}

void initKeyword(){//��ʼ���ؼ��ֱ� 
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

void initDelimiter(){//��ʼ���ֽ���� 
Delimiter[","]=10;
Delimiter["("]=11;
Delimiter[")"]=12;
Delimiter["{"]=13;
Delimiter["}"]=14;
Delimiter["["]=15;
Delimiter["]"]=16;
Delimiter[";"]=17;
} 

void initOperate(){//��ʼ��������� 
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

void getChar(){//�ӻ�����ȥ��ȡ 
	c=ScanBuffer[row][col];
	col++;
}

void jump(){//�����ո��Ʊ�� �����У� 
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

void init(){//��ʼ�� 
	initKeyword();
	initDelimiter();
	initOperate();
}

void ErrorProc(string str){//������strΪ�������� 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);//����Ϊ��ɫ 
	cout<<"[Error]"<<row+1<<" row"<<','<<col<<" col "<<result<<"Ϊ"<<str<<endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);//����Ϊ��ɫ 
} 

void back(){//����һ�� 
	col--;
	c=ScanBuffer[row][col];
}

void LexicalAnalyzer(){//�ʷ�������
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
				if(keyword[result]){//Ϊ�ؼ��� 
					cout<<'<'<<result<<" , "<<keyword[result]<<'>'<<endl;
					outfile<<'<'<<result<<" , "<<keyword[result]<<'>'<<endl;
					keywords++;	
				}
				else{//Ϊ��ʶ�� 
					cout<<'<'<<result<<" , 35>"<<endl;
					outfile<<'<'<<result<<" , 35>"<<endl;
					letters++;
					insert_tabel(result,row+1); 
						
				}
			}
			else{//����
				 
				while(!(Delimiter[ch]||oper[ch]||c==' '||c=='\n'||c=='\t')){				
					result=result+c;
					getChar(); 
				}
				back(); 
				ErrorProc("�Ƿ���ʶ��");
				 
			}
			back(); 
			
		}
		else if(c=='0'){
			result=c;
			getChar();
			if(c=='x'or c=='X'){//ʮ���������� 
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
					ErrorProc("�Ƿ�����");
				}
				else{
					cout<<'<'<<result<<" , 36>"<<endl; 
					outfile<<'<'<<result<<" , 36>"<<endl;
					digits++;
				} 
				
			}
			else{//�˽������� 
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
					ErrorProc("�Ƿ�����");
				}
				else{
					cout<<'<'<<result<<" , 37>"<<endl; 
					outfile<<'<'<<result<<" , 37>"<<endl;
					digits++;
				} 
			}
			back();
				
		} 
		else if(isdigit(c)){//ʮ�������� 
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
				ErrorProc("�Ƿ�����");			
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
			ErrorProc("�Ƿ��ַ�");
		}
		total++;
	}
	cout<<"������"<<totalrow+1<<endl;
	cout<<"��ʶ��������"<<letters<<" �ؼ��ʸ�����"<<keywords;
	cout<<" ���ָ�����"<<digits<<" �ֽ��������"<<Delimiters<<" �����������"<<opers<<endl;
	cout<<"�ܹ��ַ�����Ϊ��"<<total<<endl;
}
int main(int argc, char** argv) {
	int rows=0;
	//char c;
    //string result;//�����ַ����������Դ������������ȡ���ַ���
    string file,outfile;//Ҫ���дʷ��������ļ��� 
    cout<<"��������Ҫ���дʷ��������ļ������֣�"<<endl; 
    cin>>file; 
    fstream infile;
	
    infile.open(file.data());//���ļ����������ļ���������
    init();
    
    while(infile.peek()!=EOF){//���ļ����뻺���� 
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
