#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

typedef struct{
    char* formula;//产生式
    char  right[100][100]; //产生式右部
    int   rightNum;	//产生式右部个数
    char  left;	//产生式左部
    int   hasNull; //是否有ε产生式
}grammarElement;
grammarElement  gramOldSet[200];//原始文法的产生式集


typedef struct {
    char *base;
    char *top;
    int  size;
}Stack;
Stack analyzeStack;
/*变量定义*/
char terSymbol[200];    //终结符号
char non_ter[200];      //非终结符号
char allSymbol[400];    //所有符号
char firstSET[100][100];//各产生式右部的FIRST集
char followSET[100][100];//各产生式左部的FOLLOW集
char* M[200][200];         //分析表
int N[200][200];
char* input;
int terSymbolLength;
int non_terLength;
int allSymbolLength;
int gramOldSetNum;

void toxy(int x, int y){    //将光标移动到X,Y坐标处
    COORD pos = { x , y };
    HANDLE Out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(Out, pos);
}

/*初始化栈*/
void initStack(){
    analyzeStack.base=(char*)malloc(50*sizeof(char));
    analyzeStack.size=50;
    analyzeStack.top=analyzeStack.base;
}

/*入栈操作*/
int push(char a){
    if(analyzeStack.top-analyzeStack.base>analyzeStack.size){
        return 0;
    }
    *analyzeStack.top=a;
    analyzeStack.top++;
    return 1;
}

/*出栈操作*/
char pop(){
    if(analyzeStack.top==analyzeStack.base){
        return 0;
    }
    analyzeStack.top--;
    char ch=*analyzeStack.top;
    return ch;
}

char getTop(){
    return *(analyzeStack.top-1);
}

//遍历栈
void stackTraverse(){
    char *temp=analyzeStack.base;
    while(temp!=analyzeStack.top){
        printf("%c",*temp);
        temp++;
    }
}

void destroyStack(){
    analyzeStack.base=NULL;
    analyzeStack.top=NULL;
    analyzeStack.size=0;
}

int  getAllSymbol(){
    int i;
    int index=0;
    for(i=0;i<5;i++){
        for(int j=0;j<strlen(gramOldSet[i].formula);j++){
            char ch=gramOldSet[i].formula[j];
            if(ch!='-'&&ch!='>'&&ch!='|'&&ch!='$'){
                int flag=0;
                for(int k=0;k<index;k++){
                    if(allSymbol[k]==ch){
                        flag=-1;
                    }
                }
                if(flag==0){
                    allSymbol[index]=ch;
                    index++;
                }
            }
        }
    }
    return index;
}

int getTerSymbol(int index){
    int n=0;
    for(int i=0;i<index;i++){
        char ch=allSymbol[i];
        if(ch<65||ch>90){
            terSymbol[n]=ch;
            n++;
        }
    }
    terSymbol[n]='#';
    n++;
    return n;
}

int getNonTerSymbol(int index){
    int n=0;
    for(int i=0;i<index;i++){
        char ch=allSymbol[i];
        if(ch>=65&&ch<=90){
            non_ter[n]=ch;
            n++;
        }
    }
    return n;
}

int isTerSymbol(char ch){
    for(int i=0;i<terSymbolLength;i++){
        if(ch==terSymbol[i]){
            return 1;
        }
    }
    return 0;
}

int isNonTerSymbol(char ch){
    for(int i=0;i<non_terLength;i++){
        if(ch==non_ter[i]){
            return 1;
        }
    }
    return 0;
}

int isNull(char ch){
    if(ch=='$'){
        return 1;
    }
    return 0;
}

int getNonTerPoint(char ch){
    for(int h=0;h<gramOldSetNum;h++){
        if(gramOldSet[h].left==ch){
            return h;
        }
    }
}

int getTerPoint(char ch){
    for(int h=0;h<strlen(terSymbol);h++){
        if(ch==terSymbol[h]){
            return h;
        }
    }
}

/*初始化产生式*/
void init(){
    gramOldSet[0].formula="E->TG";
    gramOldSet[1].formula="G->+TG|$";
    gramOldSet[2].formula="T->FH";
    gramOldSet[3].formula="H->*FH|$";
    gramOldSet[4].formula="F->(E)|i";
    gramOldSetNum=5;
    allSymbolLength=getAllSymbol();
    terSymbolLength=getTerSymbol(allSymbolLength);
    non_terLength=getNonTerSymbol(allSymbolLength);
    //input="(i+(i*i))#";
    input="(i+*(i*i))#";
}

/*对产生式进行处理*/
void prepare(){
    for(int i=0;i<gramOldSetNum;i++){
        int formulaLen=strlen(gramOldSet[i].formula);
        int mark1=0,mark2=-1;
        gramOldSet[i].rightNum=0;
        gramOldSet[i].hasNull=0;
        for(int j=0;j<formulaLen;j++){
            //扫描到->则将->前的符号确定为产生式的左部
            if(gramOldSet[i].formula[j]=='-'&&gramOldSet[i].formula[j+1]=='>'){
                gramOldSet[i].left=gramOldSet[i].formula[j-1];
                j++;
                mark1=j+1;
                mark2++;
                continue;
            }
            //扫描到或运算符 '|' 跳过
            if(gramOldSet[i].formula[j]=='|'){
                for(int temp=mark1;temp<j;temp++){
                    gramOldSet[i].right[mark2][temp-mark1]=gramOldSet[i].formula[temp];
                }
                mark1=j+1;
                mark2++;//找到一个右部产生式
                continue;
            }
            //扫描到空
            if(gramOldSet[i].formula[j]=='$'){
                gramOldSet[i].hasNull=1;
                break;
            }
        }
        for(int temp=mark1;temp<formulaLen;temp++){
            gramOldSet[i].right[mark2][temp-mark1]=gramOldSet[i].formula[temp];
        }
        gramOldSet[i].rightNum=mark2+1;
    }
}

void getFirstSet(){
    while (1){
        int refresh=0;//用于判定此次循环是否对firstSet进行更新
        for(int i=0;i<gramOldSetNum;i++){
            for(int j=0;j<gramOldSet[i].rightNum;j++){
                char ch=gramOldSet[i].right[j][0];
                /*若X是非终结符，且有产生式X->a...则把a加入FIRST(X)中*/
                if(isTerSymbol(ch)){
                    int flag=0,firstSetLength=strlen(firstSET[i]);
                    for(int k=0;k<firstSetLength;k++){
                        if(ch==firstSET[i][k]){
                            flag=1;
                        }
                    }
                    if(flag==0){
                        firstSET[i][firstSetLength]=ch;
                        refresh=1;
                    }
                }
                /*若X是非终结符，且有产生式X->$则把$加入FIRST(X)中*/
                else if(isNull(ch)){
                    int flag=0,firstSetLength=strlen(firstSET[i]);
                    for(int k=0;k<firstSetLength;k++){
                        if(ch==firstSET[i][k]){
                            flag=1;
                        }
                    }
                    if(flag==0){
                        firstSET[i][firstSetLength]=ch;
                        refresh=1;
                    }
                }
                /*若X->Y...是一个产生式且Y是非终结符，则把FIRST(Y)中的所有非$-元素都加到FIRST(Y)中*/
                else if(isNonTerSymbol(ch)){
                    for(int h=0;h<gramOldSetNum;h++){
                        if(gramOldSet[h].left==ch){
                            for(int t=0;t<strlen(firstSET[h]);t++){
                                if(firstSET[h][t]!='$'){
                                    int flag=0,firstSetLength=strlen(firstSET[i]);
                                    for(int k=0;k<firstSetLength;k++){
                                        if(firstSET[h][t]==firstSET[i][k]){
                                            flag=1;
                                        }
                                    }
                                    if(flag==0){
                                        firstSET[i][firstSetLength]=firstSET[h][t];
                                        refresh=1;
                                    }
                                }
                            }
                        }
                    }
                    /*若X->Y1Y2..Yk是一个产生式，Y1,...,Yi-1都是非终结符*/
                    int rightLength=strlen(gramOldSet[i].right[j]);
                    int nonTerNum=0;
                    for(int k=0;k<rightLength;k++){
                        char chh=gramOldSet[i].right[j][k];
                        if(isNonTerSymbol(chh)){
                            int index=0;
                            for(int h=0;h<gramOldSetNum;h++){
                                if(gramOldSet[h].left==chh){
                                    index=h;
                                }
                            }
                            if(gramOldSet[index].hasNull){
                                nonTerNum++;
                                continue;
                            } else{
                                break;
                            }
                        }
                    }
                    /*若所有的FISRT(Yj)均含有$,则把$加到FIRST(X)中*/
                    if(nonTerNum==rightLength){
                        int flag=0,firstSetLength=strlen(firstSET[i]);
                        for(int k=0;k<firstSetLength;k++){
                            if(firstSET[i][k]=='$'){
                                flag=1;
                            }
                        }
                        if(flag==0){
                            firstSET[i][firstSetLength]='$';
                            refresh=1;
                        }
                    }
                    /*对于任何j,1<=j<=i-1,FIRST(Yj)都含有$,则把FIRST(Yi)中的所有非$-元素都加入FIRST(X)中*/
                    else if(nonTerNum>1){
                        for(int f=0;f<nonTerNum;f++){
                            char chhh=gramOldSet[i].right[j][f];
                            for(int h=0;h<gramOldSetNum;h++){
                                if(gramOldSet[h].left==chhh){
                                    for(int t=0;t<strlen(firstSET[h]);t++){
                                        if(firstSET[h][t]!='$'){
                                            int flag=0,firstSetLength=strlen(firstSET[i]);
                                            for(int k=0;k<firstSetLength;k++){
                                                if(firstSET[h][t]==firstSET[i][k]){
                                                    flag=1;
                                                }
                                            }
                                            if(flag==0){
                                                firstSET[i][firstSetLength]=firstSET[h][t];
                                                refresh=1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(refresh==0){
            break;
        }
    }
}

void getFollowSet(){
    /*对于文法的开始符号，置#于其FOLLOW集中*/
    followSET[0][0]='#';
    while (1){
        int refresh=0;//用于判定此次循环是否对followSet进行更新
        for(int i=0;i<gramOldSetNum;i++){
            for(int j=0;j<gramOldSet[i].rightNum;j++){
                int rightLength=strlen(gramOldSet[i].right[j]);
                for(int k=0;k<rightLength;k++){
                    char ch1=gramOldSet[i].right[j][k];
                    if((k+1<rightLength)&&isNonTerSymbol(ch1)){
                        char ch2=gramOldSet[i].right[j][k+1];
                        if(isTerSymbol(ch2)){
                            int index=0;
                            for(int h=0;h<gramOldSetNum;h++){
                                if(gramOldSet[h].left==ch1){
                                    index=h;
                                }
                            }
                            int flag=0,followSetLength=strlen(followSET[index]);
                            for(int h=0;h<followSetLength;h++){
                                if(followSET[index][h]==ch2){
                                    flag=1;
                                }
                            }
                            if(flag==0){
                                followSET[index][followSetLength]=ch2;
                                refresh=1;
                            }
                        }else if(isNonTerSymbol(ch2)){
                            int index1=0,index2=0;
                            for(int h=0;h<gramOldSetNum;h++){
                                if(gramOldSet[h].left==ch1){
                                    index1=h;
                                }
                                if(gramOldSet[h].left==ch2){
                                    index2=h;
                                }
                            }
                            for(int t=0;t<strlen(firstSET[index2]);t++){
                                if(firstSET[index2][t]!='$'){
                                    int flag=0,followSetLength=strlen(followSET[index1]);
                                    for(int h=0;h<followSetLength;h++){
                                        if(followSET[index1][h]==firstSET[index2][t]){
                                            flag=1;
                                        }
                                    }
                                    if(flag==0){
                                        followSET[index1][followSetLength]=firstSET[index2][t];
                                        refresh=1;
                                    }
                                }
                            }
                        }
                    }
                    if(k==rightLength-2&&isNonTerSymbol(ch1)){
                        char ch2=gramOldSet[i].right[j][k+1];
                        int index1=0,index2=0;
                        for(int h=0;h<gramOldSetNum;h++){
                            if(gramOldSet[h].left==ch1){
                                index1=h;
                            }
                            if(gramOldSet[h].left==ch2){
                                index2=h;
                            }
                        }
                        if(gramOldSet[index2].hasNull){
                            int followLength=strlen(followSET[i]);
                            for(int h=0;h<followLength;h++){
                                int flag=0,length=strlen(followSET[index1]);
                                for(int g=0;g<length;g++){
                                    if(followSET[index1][g]==followSET[i][h]){
                                        flag=1;
                                    }
                                }
                                if(flag==0){
                                    followSET[index1][length]=followSET[i][h];
                                    refresh=1;
                                }
                            }
                        }
                    }
                    if(k==rightLength-1&&isNonTerSymbol(ch1)){
                        int index=0;
                        for(int h=0;h<gramOldSetNum;h++){
                            if(gramOldSet[h].left==ch1){
                                index=h;
                            }
                        }
                        int followLength=strlen(followSET[i]);
                        for(int h=0;h<followLength;h++){
                            int flag=0,length=strlen(followSET[index]);
                            for(int g=0;g<length;g++){
                                if(followSET[index][g]==followSET[i][h]){
                                    flag=1;
                                }
                            }
                            if(flag==0){
                                followSET[index][length]=followSET[i][h];
                                refresh=1;
                            }
                        }
                    }
                }
            }
        }
        if(refresh==0){
            break;
        }
    }
}

/*构建分析表*/
void creatMSet(){
    for(int i=0;i<strlen(non_ter);i++){
        for(int j=0;j<strlen(terSymbol);j++){
           M[i][j]="error";
           N[i][j]=-1;
        }
    }
    for(int i=0;i<gramOldSetNum;i++){
        for(int j=0;j<strlen(terSymbol);j++){
            for(int k=0;k<strlen(firstSET[i]);k++){
                if(terSymbol[j]==firstSET[i][k]){
                    N[i][j]=1;
                    for(int h=0;h<gramOldSet[i].rightNum;h++){
                        char  ch=gramOldSet[i].right[h][0];
                        if(isTerSymbol(ch)){
                            if(ch==terSymbol[j]){
                                M[i][j]=gramOldSet[i].right[h];
                            }
                        } else if(isNonTerSymbol(ch)){
                            int mark=getNonTerPoint(ch);
                            for(int t=0;t<strlen(firstSET[mark]);t++){
                                if(firstSET[mark][t]==terSymbol[j]){
                                    M[i][j]=gramOldSet[i].right[h];
                                }
                            }
                        }
                    }
                }
            }
            for(int k=0;k<strlen(followSET[i]);k++){
                if(terSymbol[j]==followSET[i][k]){
                    N[i][j]=3;
                    M[i][j]="synch";
                }
            }
        }
        if(gramOldSet[i].hasNull){
            for(int j=0;j<strlen(terSymbol);j++){
                for(int k=0;k<strlen(followSET[i]);k++){
                    if(terSymbol[j]==followSET[i][k]){
                        M[i][j]="$";
                        N[i][j]=2;
                    }
                }
            }
        }
    }
}

void showBasicInfo(){
    /*for(int i=0;i<gramOldSetNum;i++){
        printf("%s\t%c\t%d\t%d\t",gramOldSet[i].formula,gramOldSet[i].left,gramOldSet[i].rightNum,gramOldSet[i].hasNull);
        if(gramOldSet[i].rightNum>=2){
            for (int j = 0; j < gramOldSet[i].rightNum; ++j) {
                printf("%s\t",gramOldSet[i].right[j]);
            }
        }
        printf("\n");
    }*/

    printf("\tFirst\tFollow\n");
    for (int k = 0; k < 5; k++) {
        printf("%c\t%s\t%s\n",gramOldSet[k].left,firstSET[k],followSET[k]);
    }
    printf("\t");
    for(int i=0;i<strlen(terSymbol);i++){
        printf("%c\t",terSymbol[i]);
    }
    printf("\n");
    for(int i=0;i<strlen(non_ter);i++){
        printf("%c\t",gramOldSet[i].left);
        for(int j=0;j<strlen(terSymbol);j++){
            int t=N[i][j];
            if(t>0&&t!=3){
                printf("%c->%s\t",gramOldSet[i].left,M[i][j]);
            } else if(t==3){
               printf("%s\t",M[i][j]);
            } else{
                printf("error\t");
            }
        }
        printf("\n");
    }
}

void analyzeInput(){
    initStack();
    push('#');
    push(gramOldSet[0].left);
    int flag=1,step=0,index=0,line=20,error=0;
    char* generate=" ";
    char left=' ';
    toxy(0,line);
    printf("步骤");
    toxy(15,line);
    printf("符号栈");
    toxy(30,line);
    printf("输入串");
    toxy(45,line);
    printf("所用产生式");
    line++;
    while (flag!=-2){
        toxy(0,line+step);
        printf("%d",step);
        toxy(15,line+step);
        stackTraverse();
        toxy(29,line+step);
        for(int i=index;i<strlen(input);i++){
           printf("%c",input[i]);
        }
        toxy(44,line+step);
        if(left==' '){
            printf(" ");
        }else if(left=='e'){
            printf("%s",generate);
        }else{
            printf("%c->%s",left,generate);
        }
        char ch=getTop();
        if(isTerSymbol(ch)&&ch!='#'){
            if(ch==input[index]){
                index++;
                step++;
                pop();
                left=' ';
            } else{
                pop();
                step++;
                left='e';
                generate="终结符不匹配，栈顶终结符弹出";
                error=1;
            }
        } else if(ch=='#'){
            if(input[index]=='#'){
                flag=-2;
            } else{
                pop();
                step++;
                left='e';
                generate="终结符不匹配，栈顶终结符弹出";
                error=1;
            }
        } else if(isNonTerSymbol(ch)){
            int mark=getNonTerPoint(ch);
            char c=input[index];

            if(N[mark][getTerPoint(c)]==1) {
                left = pop();
                char *right = M[mark][getTerPoint(c)];
                for (int h = strlen(right) - 1; h >= 0; h--) {
                    push(right[h]);
                }
                step++;
                generate = M[mark][getTerPoint(c)];
            }else if(N[mark][getTerPoint(c)]==2){
                left=pop();
                step++;
                generate=M[mark][getTerPoint(c)];
            }else if(N[mark][getTerPoint(c)]==-1){
                index++;
                step++;
                left='e';
                generate="错误，跳过当前错误输入字符";
                error=1;
            } else if(N[mark][getTerPoint(c)]==3){
                step++;
                left='e';
                pop();
                generate="错误，栈顶非终结符弹出";
                error=1;
            }
        }
    }
    if(error==1){
        printf("\n输入语句有误请修改!!!");
    } else if(error==0){
        printf("\n输入正确!!!");
    }
    destroyStack();
}

int main() {
    init();
    prepare();
    getFirstSet();
    getFollowSet();
    creatMSet();
    showBasicInfo();
    analyzeInput();
    return 0;
}