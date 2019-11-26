#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

typedef struct{
    char* formula;//����ʽ
    char  right[100][100]; //����ʽ�Ҳ�
    int   rightNum;	//����ʽ�Ҳ�����
    char  left;	//����ʽ��
    int   hasNull; //�Ƿ��ЦŲ���ʽ
}grammarElement;
grammarElement  gramOldSet[200];//ԭʼ�ķ��Ĳ���ʽ��


typedef struct {
    char *base;
    char *top;
    int  size;
}Stack;
Stack analyzeStack;
/*��������*/
char terSymbol[200];    //�ս����
char non_ter[200];      //���ս����
char allSymbol[400];    //���з���
char firstSET[100][100];//������ʽ�Ҳ���FIRST��
char followSET[100][100];//������ʽ�󲿵�FOLLOW��
char* M[200][200];         //������
int N[200][200];
char* input;
int terSymbolLength;
int non_terLength;
int allSymbolLength;
int gramOldSetNum;

void toxy(int x, int y){    //������ƶ���X,Y���괦
    COORD pos = { x , y };
    HANDLE Out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(Out, pos);
}

/*��ʼ��ջ*/
void initStack(){
    analyzeStack.base=(char*)malloc(50*sizeof(char));
    analyzeStack.size=50;
    analyzeStack.top=analyzeStack.base;
}

/*��ջ����*/
int push(char a){
    if(analyzeStack.top-analyzeStack.base>analyzeStack.size){
        return 0;
    }
    *analyzeStack.top=a;
    analyzeStack.top++;
    return 1;
}

/*��ջ����*/
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

//����ջ
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

/*��ʼ������ʽ*/
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

/*�Բ���ʽ���д���*/
void prepare(){
    for(int i=0;i<gramOldSetNum;i++){
        int formulaLen=strlen(gramOldSet[i].formula);
        int mark1=0,mark2=-1;
        gramOldSet[i].rightNum=0;
        gramOldSet[i].hasNull=0;
        for(int j=0;j<formulaLen;j++){
            //ɨ�赽->��->ǰ�ķ���ȷ��Ϊ����ʽ����
            if(gramOldSet[i].formula[j]=='-'&&gramOldSet[i].formula[j+1]=='>'){
                gramOldSet[i].left=gramOldSet[i].formula[j-1];
                j++;
                mark1=j+1;
                mark2++;
                continue;
            }
            //ɨ�赽������� '|' ����
            if(gramOldSet[i].formula[j]=='|'){
                for(int temp=mark1;temp<j;temp++){
                    gramOldSet[i].right[mark2][temp-mark1]=gramOldSet[i].formula[temp];
                }
                mark1=j+1;
                mark2++;//�ҵ�һ���Ҳ�����ʽ
                continue;
            }
            //ɨ�赽��
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
        int refresh=0;//�����ж��˴�ѭ���Ƿ��firstSet���и���
        for(int i=0;i<gramOldSetNum;i++){
            for(int j=0;j<gramOldSet[i].rightNum;j++){
                char ch=gramOldSet[i].right[j][0];
                /*��X�Ƿ��ս�������в���ʽX->a...���a����FIRST(X)��*/
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
                /*��X�Ƿ��ս�������в���ʽX->$���$����FIRST(X)��*/
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
                /*��X->Y...��һ������ʽ��Y�Ƿ��ս�������FIRST(Y)�е����з�$-Ԫ�ض��ӵ�FIRST(Y)��*/
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
                    /*��X->Y1Y2..Yk��һ������ʽ��Y1,...,Yi-1���Ƿ��ս��*/
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
                    /*�����е�FISRT(Yj)������$,���$�ӵ�FIRST(X)��*/
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
                    /*�����κ�j,1<=j<=i-1,FIRST(Yj)������$,���FIRST(Yi)�е����з�$-Ԫ�ض�����FIRST(X)��*/
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
    /*�����ķ��Ŀ�ʼ���ţ���#����FOLLOW����*/
    followSET[0][0]='#';
    while (1){
        int refresh=0;//�����ж��˴�ѭ���Ƿ��followSet���и���
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

/*����������*/
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
    printf("����");
    toxy(15,line);
    printf("����ջ");
    toxy(30,line);
    printf("���봮");
    toxy(45,line);
    printf("���ò���ʽ");
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
                generate="�ս����ƥ�䣬ջ���ս������";
                error=1;
            }
        } else if(ch=='#'){
            if(input[index]=='#'){
                flag=-2;
            } else{
                pop();
                step++;
                left='e';
                generate="�ս����ƥ�䣬ջ���ս������";
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
                generate="����������ǰ���������ַ�";
                error=1;
            } else if(N[mark][getTerPoint(c)]==3){
                step++;
                left='e';
                pop();
                generate="����ջ�����ս������";
                error=1;
            }
        }
    }
    if(error==1){
        printf("\n��������������޸�!!!");
    } else if(error==0){
        printf("\n������ȷ!!!");
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