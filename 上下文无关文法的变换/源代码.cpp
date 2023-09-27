#include<iostream>
#include<algorithm>
#include<set>
#include<map>
using namespace std;

//定义文法
struct Grammar{
    set<char> nonterminal;                  //非终结符，是一个char类型的集合
    set<char> terminal;                     //终结符，是一个char类型的集合
    map<char,set<string> > production;      //生成式，是从char到一个string集合的映射
    char start;                             //初始非终结符
};

void input(Grammar &G){
    string input_str;
    while(cin>>input_str){
        if(input_str[0]=='N'){                  //处理所有终结符
            for(int i=1;i<input_str.size();i++){
                char item=input_str[i];
                if(item!='='&&item!='{'&&item!=','&&item!='}'){
                    G.nonterminal.insert(item);
                }
            }
        }else if(input_str[0]=='T'){            //处理所有非终结符
            for(int i=1;i<input_str.size();i++){
                char item=input_str[i];
                if(item!='='&&item!='{'&&item!=','&&item!='}'){
                    G.terminal.insert(item);
                }
            }
        }else if(input_str[0]=='P'){            //处理所有生成式
            while(cin>>input_str){
                if(input_str[0]=='S'&&input_str[1]=='='){       //如果读到“S=”，说明读到初始非终结符
                    G.start=input_str[2];
                    return;
                }
                char left=input_str[0];
                string right="";
                for(int i=4;i<input_str.size();i++){
                    if(input_str[i]=='|'){
                        G.production[left].insert(right);
                        right="";
                    }else{
                        right=right+input_str[i];
                    }
                }
                G.production[left].insert(right);
            }
        }
    }
}

void output(Grammar G1){
    cout<<"N={";
    set<char>::iterator p=G1.nonterminal.begin();
    for(int i=0;i<G1.nonterminal.size();p++,i++){
        if(i==G1.nonterminal.size()-1){
            cout<<*p<<"}\n";
        }else{
            cout<<*p<<",";
        }
    }
    cout<<"T={";
    p=G1.terminal.begin();
    for(int i=0;i<G1.terminal.size();p++,i++){
        if(i==G1.terminal.size()-1){
            cout<<*p<<"}\n";
        }else{
            cout<<*p<<",";
        }
    }
    cout<<"P:\n";
    p=G1.nonterminal.begin();
    for(int i=0;i<G1.nonterminal.size();p++,i++){
        set<string> prud=G1.production[*p];
        if(prud.size()>0){
            cout<<"\t"<<*p<<"-->";
            set<string>::iterator p1=prud.begin();
            for(int j=0;j<prud.size();p1++,j++){
                if(j==prud.size()-1){
                    cout<<*p1<<"\n";
                }else{
                    cout<<*p1<<"|";
                }
            }
        }
    }
    cout<<"S="<<G1.start<<endl;
}

Grammar algorithm1(Grammar G,set<char> terminal){       //一般情况下，terminal字段为G.terminal，用于消除epsilon产生式时为{N}
    set<char> N0,N,NT;
    do{
        N0=N;
        set_union(N0.begin(),N0.end(),terminal.begin(),terminal.end(),inserter(NT,NT.begin()));
        set<char>::iterator p=G.nonterminal.begin();    //*p是G中的非终结符，即生成式中的left
        for(int i=0;i<G.nonterminal.size();p++,i++){    //遍历所有非终结符
            if(N.count(*p)) continue;                  //left已在N中，跳过本轮循环
            set<string> prud=G.production[*p];
            set<string>::iterator p1=prud.begin();
            for(int j=0;j<prud.size();p1++,j++){        //遍历每个终结符的所有生成式
                int flag=0;
                string right=*p1;                       //生成式右端字符串
                for(auto item:right){
                    if(!NT.count(item)){                //如果right有字符不在NT中，即right不在NT的克林闭包中
                        flag=1;
                        break;
                    }
                }
                if(!flag){                              //反之，即right在NT的克林闭包中
                    N.insert(*p);                      //将*p（即left）加入N
                    break;                              //left已加入N，可以跳过本轮循环，直接分析下一个非终结符
                }
            }
        }
    }while(N0!=N);
    Grammar G1;                                         //返回文法G1，G1消除了无法推导出非终结符的终结符
    G1.nonterminal=N;
    G1.terminal=G.terminal;
    set<char>::iterator p=G1.nonterminal.begin();
    for(int i=0;i<G1.nonterminal.size();p++,i++){       //删除G1中含有以被消除的终结符的生成式
        for(set<string>::iterator p1=G.production[*p].begin();p1!=G.production[*p].end();p1++){
            int flag=0;
            for(auto item:*p1){
                if(item!='N'&&!G1.nonterminal.count(item)&&!G1.terminal.count(item)){     //*p1(right)中的某一位不在G1的非终结符或终结符表中出现,且不为空
                    flag=1;
                    break;
                }
            }
            if(!flag){
                G1.production[*p].insert(*p1);
            }
        }
    }
    G1.start=G.start;
    return G1;
}

Grammar algorithm2(Grammar G){
    set<char> N0,N1;
    N1.insert(G.start);
    do{
        N0=N1;
        for(auto& item:N0){
            if(G.nonterminal.count(item)){
                for(auto& p:G.production[item]){
                    for(auto s:p){
                        N1.insert(s);
                    }
                }
            }
        }
    }while(N0!=N1);
    Grammar G1;
    set_intersection(G.nonterminal.begin(),G.nonterminal.end(),N0.begin(),N0.end(),inserter(G1.nonterminal,G1.nonterminal.begin()));
    set_intersection(G.terminal.begin(),G.terminal.end(),N0.begin(),N0.end(),inserter(G1.terminal,G1.terminal.begin()));
    for(auto& item:N0){
        G1.production[item]=G.production[item];
    }
    G1.start=G.start;
    return G1;
}

Grammar eliminate_epsilon_productions(Grammar G){
    set<char> temp;
    temp.insert('N');
    Grammar Gt=algorithm1(G,temp);                      //Gt为临时文法，Gt.nonterminal存储了G中所有可以推导出epsilon的非终结符
    Grammar G1;
    set<char>::iterator p=G.nonterminal.begin();
    for(int i=0;i<G.nonterminal.size();p++,i++){       //遍历G的所有生成式
        for(set<string>::iterator p1=G.production[*p].begin();p1!=G.production[*p].end();p1++){
            vector<string> new_rights;                     //将产生式A -> B1B2...Bn消去可以推导出epsilon的非终结符，并转化为多个产生式
            new_rights.push_back("");
            for(const auto& item:*p1){
                if(Gt.nonterminal.count(item)){         //若该字符可推导出epsilon，则插入item本身或epsilon
                    vector<string> temp;
                    for(auto& s:new_rights){
                        temp.push_back(s+item);
                    }
                    new_rights.insert(new_rights.end(),temp.begin(),temp.end());
                }else{
                    for(auto& s:new_rights){            //否则插入item本身
                        s+=item;
                    }
                }
            }
            //是否去重
            for(auto& s:new_rights){                    //将产生的多个产生式插入
                if(s!="N"&&s!="")
                    G1.production[*p].insert(s);
            }
        }
    }
    if(Gt.nonterminal.count(G.start)){
        G1.nonterminal=G.nonterminal;
        G1.nonterminal.insert('T');              //插入一个新的起始符
        G1.terminal=G.terminal;
        G1.production['T'].insert("N");          //新起始符可推导出原起始符和epsilon
        string s="";
        G1.production['T'].insert(s.append(1,G.start));
        G1.start='T';
    }else{
        G1.nonterminal=G.nonterminal;
        G1.terminal=G.terminal;
        G1.start=G.start;
    }
    return G1;
}

Grammar eliminate_unit_productions(Grammar G){
    Grammar G1;
    //对每个非终结符A，构造非终结符集合NA，NA包含所有A可推导出的单个非终结符
    map<char,set<char> > N;
    set<char> N0,N1;
    for(auto& item:G.nonterminal){
        N0.clear();
        N1.clear();
        N1.insert(item);
        do{
            N0=N1;
            for(auto& i:N0){
                for(auto& p:G.production[i]){
                    if(p.size()==1&&G.nonterminal.count(p[0])){
                        N1.insert(p[0]);
                    }
                }
            }  
        }while(N0!=N1);
        N[item]=N0;
    }
    //构造新文法，如果B->w且不是单生成式，则对B所在的NA中所有的A，把A->w加入G1.production
    for(auto& item:G.nonterminal){
        for(auto& p:G.production[item]){
            if(p.size()>1||G.terminal.count(p[0])||p[0]=='N'){
                for(auto& i:G.nonterminal){
                    if(N[i].count(item)){
                        G1.production[i].insert(p);
                    }
                }
            }
        }
    }
    G1.nonterminal=G.nonterminal;
    G1.terminal=G.terminal;
    G1.start=G.start;
    return G1;
}

Grammar eliminate_useless_productions(Grammar G){
    Grammar G1=algorithm1(G,G.terminal);
    Grammar G2=algorithm2(G1);
    return G2;
}

int main(){
    Grammar G,G1,G2,G3;
    input(G);
    set<char> temp;
    temp.insert('N');
    G1=eliminate_epsilon_productions(G);
    G2=eliminate_unit_productions(G1);
    G3=eliminate_useless_productions(G2);
    output(G3);
}

/*test:
N={A,B,C,D,S}
T={a,b,c,d}
P:
S-->N|A|a|bA|ccD
A-->N|abB
B-->aA
C-->ddC
D-->ddd
S=S

*/