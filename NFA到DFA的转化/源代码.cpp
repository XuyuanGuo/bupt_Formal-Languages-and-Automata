#include<bits/stdc++.h>
using namespace std;

//1.	参考数据结构和函数接口
//1.1数据结构
const int maxiNum = 100;	//nfa的状态数量最大值，也即小状态的数量，假设为10
const int maxNum = 1000;//2 ^ maxiNum的值，dfa中元素最多有多少个
int nfaStateNum;//nfa的状态数量，初始为0
int dfaStateNum;//dfa的状态数量，初始为0
int nfaDeduce0[maxiNum][maxiNum];	//二维数组，其中nfaDeduce[i]是一个一维数组，存储在NFA中通过状态qi输入0能到达的状态
int nfaDeduce1[maxiNum][maxiNum];	//二维数组，其中nfaDeduce[i]是一个一维数组，存储在NFA中通过状态qi输入1能到达的状态
int nfaStateCondition[maxiNum];//记录nfa状态是否为首尾，0是一般正常，1是首节点，2是尾节点
typedef struct dfaStateNode {
	int l = 0;
	int isStart=0;
	int isFinal=0;
	int nfaNode[maxiNum]={0};	//存储一个dfa状态中包含的nfa状态，以-1结尾
}dfaStateNode;
dfaStateNode dfaState[maxNum];//数组形式存储所有dfa状态
int dfaDeduce0[maxNum];//存储DFA中qi输入0时能到达的状态
int dfaDeduce1[maxNum];	//存储DFA中qi输入1时能到达的状态
int ifDfaDeduce[maxNum];//存储dfa中状态qi是否已经进行处理
queue<dfaStateNode> q;//存储DFA状态的队列
queue<int>qnum;
int dfsState[maxNum];

//1.2函数接口
//1.2.1输入以及预处理
void init();//初始化函数

//1.2.2生成dfa状态及转移表
int find(dfaStateNode node);//判断一个dfaStateNode在DFA状态中是否出现过，若无则返回-1，若空返回-2，否则返回其下标
void handle(int handleNum);//处理dfa状态的第几个
void createDfa();//完善所有的dfa状态和dfa的deduce数组

//1.2.3删除不可达元素
void dfs(int a);//记录一遍dfs，经过为1，否则为0；在后续输出中为0则不输出


//1.2.4输出
void printdfa();



//2.	输入以及预处理
void init() {
	string str;
	getline(cin, str);
	dfaStateNode node0,node1;
	while (getline(cin, str)) {//初始化nfadeduce0和nfadeduce1，nfastatecondition，nfastatenum
		if (str == "")
			continue;
		str += '\0';
		int i = 0;//读入到字符串第i位
		int j = 0;//dfa状态中已有j个小元素
		if (str[0] == '(') {//判断是不是起始和终止节点
			i++;
			if (str[i] == 's') {
				nfaStateCondition[nfaStateNum] = 1;//此节点为首节点
			}
			else if (str[i] == 'e') {
				nfaStateCondition[nfaStateNum] = 2;//此节点W为尾结点
			}
			else {
				cout << "error1\n";
			}
			i = 3;
		}
		if (str[i] == '[') {//读入第一个部分
			if (nfaStateNum != str[i + 2] - '0')//后续可能修改成两位数及以上
				cout << "error2\n";
			i += 5;
		}
		else
			cout << "error3\n";

		if (str[i] == '[') {//读入第二部分
			i++;
			while (str[i + 1] <= '9' && str[i + 1] >= '0') {
				nfaDeduce0[nfaStateNum][j] = str[i + 1]-'0';
				node0.nfaNode[j]= str[i + 1] - '0';
				i += 3;
				j++;
			}
			node0.l = j;
			nfaDeduce0[nfaStateNum][j] = -1;
			node0.nfaNode[j] = -1;
			i++;
		}
		else if (str[i] == 'N') {
			nfaDeduce0[nfaStateNum][j] = -1;
			node0.nfaNode[j] = -1;
			i += 2;
		}
		else
			cout << "error4\n";
		j = 0;
		if (str[i] == '[') {//读入第三部分
			i++;
			while (str[i + 1] <= '9' && str[i + 1] >= '0') {
				nfaDeduce1[nfaStateNum][j] = str[i + 1] - '0';
				node1.nfaNode[j] = str[i + 1] - '0';
				i += 3;
				j++;
			}
			node1.l = j;
			nfaDeduce1[nfaStateNum][j] = -1;
			node1.nfaNode[j] = -1;
			i++;
		}
		else if (str[i] == 'N') {
			nfaDeduce1[nfaStateNum][j] = -1;
			node1.nfaNode[j] = -1;
			i += 2;
		}
		else
			cout << "error5\n";
		//q.push(node0);//两个dfa节点入队列
		//q.push(node1);

		nfaStateNum++;
	}
	//初始化dfastate和dfastatenum
	for (int i = 0; i < nfaStateNum; i++) {
		dfaState[i].nfaNode[0] = i;
		dfaState[i].nfaNode[1] = -1;
		dfaState[i].l = 1;
		if (nfaStateCondition[i] == 1)
			dfaState[i].isStart = 1;
		else if (nfaStateCondition[i] == 2)
			dfaState[i].isFinal = 1;
	}
	dfaStateNum = nfaStateNum;

	return;
}


//3.	生成dfa状态及转移表
int find(dfaStateNode node) {
	if (node.l == 0)
		return -2;
	for (int i = 0; i < dfaStateNum; i++) {
		if (dfaState[i].l != node.l)
			continue;
		int ok = 1;
		for (int j = 0; j < node.l; j++) {
			if (dfaState[i].nfaNode[j] != node.nfaNode[j])
				ok = 0;
		}
		if (ok == 1)
			return i;
	}
	return -1;
}

void handle(int handleNum) {
	//先初始化处理的这一个dfa状态需要的元素

	dfaStateNode node00, node11;//node0的汇总为node00，node1的汇总为node11

	//初始化node00
	for (int j = 0; j < dfaState[handleNum].l; j++) {///dfa状态的第handlenum个 的第j个元素
		dfaStateNode node0;
		for (int k = 0; nfaDeduce0[dfaState[handleNum].nfaNode[j]][k] != -1; k++) {//它的deduce的第k个元素
			if (nfaStateCondition[nfaDeduce0[dfaState[handleNum].nfaNode[j]][k]] == 1) {
				node0.isStart = 1;
			}
			else if (nfaStateCondition[nfaDeduce0[dfaState[handleNum].nfaNode[j]][k]] == 2) {
				node0.isFinal = 1;
			}
			node0.nfaNode[k] = nfaDeduce0[dfaState[handleNum].nfaNode[j]][k];
			node0.l++;
		}
		//if (node0.isStart == 1)
		//	node00.isStart = 1;
		if (node0.isFinal == 1)
			node00.isFinal = 1;
		for (int i = 0, j = 0; i < node0.l; ) {
			if (j < node00.l) {
				if (node00.nfaNode[j] < node0.nfaNode[i]) {
					j++;
				}
				else if (node00.nfaNode[j] > node0.nfaNode[i]) {
					for (int k = node00.l; k >= j; k--) {
						node00.nfaNode[k + 1] = node00.nfaNode[k];
					}
					node00.nfaNode[j] = node0.nfaNode[i];
					node00.l++;
					node00.nfaNode[node00.l] = -1;
				}
				else {
					i++;
					j++;
				}
			}
			else if (j == node00.l) {

				node00.nfaNode[j] = node0.nfaNode[i];
				node00.l++;
				node00.nfaNode[node00.l] = -1;
				sort(node00.nfaNode, node00.nfaNode + node00.l);
			}
			else {
				cout << "error\n";
			}
		}
	}
	//cout << "node00初始化完成\n";
	//初始化node11
	for (int j = 0; j < dfaState[handleNum].l; j++) {///dfa状态的第handlenum个 的第j个元素
		dfaStateNode  node1;
		for (int k = 0; nfaDeduce1[dfaState[handleNum].nfaNode[j]][k] != -1; k++) {//它的deduce的第k个元素
			if (nfaStateCondition[nfaDeduce1[dfaState[handleNum].nfaNode[j]][k]] == 1) {
				node1.isStart = 1;
			}
			else if (nfaStateCondition[nfaDeduce1[dfaState[handleNum].nfaNode[j]][k]] == 2) {
				node1.isFinal = 1;
			}
			node1.nfaNode[k] = nfaDeduce1[dfaState[handleNum].nfaNode[j]][k];
			node1.l++;
		}
		//if (node1.isStart == 1)
		//	node11.isStart = 1;
		if (node1.isFinal == 1)
			node11.isFinal = 1;
		for (int i = 0, j = 0; i < node1.l; ) {
			if (j < node11.l) {
				if (node11.nfaNode[j] < node1.nfaNode[i]) {
					j++;
				}
				else if (node11.nfaNode[j] > node1.nfaNode[i]) {
					for (int k = node11.l; k >= j; k--) {
						node11.nfaNode[k + 1] = node11.nfaNode[k];
					}
					node11.nfaNode[j] = node1.nfaNode[i];
					node11.l++;
					node11.nfaNode[node11.l] = -1;
				}
				else {
					i++;
					j++;
				}
			}
			else if (j == node11.l) {

				node11.nfaNode[j] = node1.nfaNode[i];
				node11.l++;
				node11.nfaNode[node11.l] = -1;
				sort(node11.nfaNode, node11.nfaNode + node11.l);
			}
			else {
				cout << "error\n";
			}
		}
	}
	//cout << "node11初始化完成\n";

	int n0, n1;
	n0 = find(node00);
	n1 = find(node11);
	if (0 < n0 && n0 < nfaStateNum) {
		if (ifDfaDeduce[handleNum] == 0) {
			q.push(node00);
			qnum.push(n0);
		}
	}
	if (n0 == -1) {
		q.push(node00);
		qnum.push(dfaStateNum);
		dfaState[dfaStateNum] = node00;
		n0 = dfaStateNum;
		dfaStateNum++;
	}
	if (0 < n1 && n1 < nfaStateNum) {
		if (ifDfaDeduce[handleNum] == 0) {
			q.push(node11);
			qnum.push(n1);
		}
	}
	if (n1 == -1) {
		q.push(node11);
		qnum.push(dfaStateNum);
		dfaState[dfaStateNum] = node11;
		n1 = dfaStateNum;
		dfaStateNum++;
	}
	ifDfaDeduce[handleNum] = 1;
	dfaDeduce0[handleNum] = n0;
	dfaDeduce1[handleNum] = n1;

}

void createDfa() {

	int handleNum = dfaStateNum;//记录处理到dfa状态的第几个了
	handle(0);
	while (q.size() != 0) {
		q.pop();
		handleNum = qnum.front();
		qnum.pop();
		handle(handleNum);
	
		handleNum++;
	}
	//for (int i = 1; i < nfaStateNum; i++) {
	//	handle(i);
	//}

}


//4.删除不可达元素
void dfs(int a) {
	if (dfsState[a] == 1)
		return;
	dfsState[a] = 1;
	if (dfaDeduce0[a] != -2) {
		dfs(dfaDeduce0[a]);
	}
	if (dfaDeduce1[a] != -2) {
		dfs(dfaDeduce1[a]);
	}
	return;
}


//5.输出
void printdfa() {
	cout << "\t\t0\t1\n";
	for (int i = 0; i < dfaStateNum; i++) {
		if (dfsState[i] == 1) {
			if (dfaState[i].isStart == 1) {//可能同时出现s和e
				cout << "(s)q" << i << '\t';

			}
			else if (dfaState[i].isFinal == 1) {
				cout << "(e)q" << i << '\t';
			}
			else {
				cout << "q" << i << "\t";
			}

			if (dfaDeduce0[i] == -2) {
				cout << "N\t";
			}
			else {
				cout << "q" << dfaDeduce0[i] << "\t";
			}

			if (dfaDeduce1[i] == -2) {
				cout << "N\n";
			}
			else {
				cout << "q" << dfaDeduce1[i] << "\n";
			}

		}
	}



}



//6.main函数
int main() {

	init();
	createDfa();
	dfs(0);
	printdfa();


	return 0;
}

