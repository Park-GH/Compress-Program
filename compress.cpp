#include <iostream>
#include <fstream> 
#include <queue>
#include <list>
#include <string>
#include <vector>
using namespace std;

//허프만코드 구현을 위한 트리
class node {
public:

	node(int freq = 0, char sym = '0')
	{
		left = list<node>(); right = list<node>();
		frequency = freq; symbol = sym;
	}

	int frequency;
	char symbol;

	//자손은 포인터 대신 list로 가리키도록 설정
	list<node> left;
	list<node> right;
};

struct comp_freq {
	bool operator()(const node& p, const node& q) {
		return p.frequency > q.frequency;
	}
};

//symbol과 허프만코드 관계 저장
class code {
public:
	char sym;
	string huffcode;
};
struct comp_length {
	bool operator()(const code& p, const code& q) {
		return p.huffcode.length() > q.huffcode.length();
	}
};


void search(const node& root, string code_cur, priority_queue<code, vector<code>, comp_length>& pq2);
string return_code(vector<code>& code_vec, char ch);

int main() {
	
	int testcase;
	cout << "입력파일 이름은 testcase 순서대로 testcase0.txt, testcase1.txt, testcase2.txt, ... 의 형식이 되어야 합니다." << endl;
	cout << "예를들면 3을 입력시 testcase0.txt, testcase1.txt, testcase2.txt 3개의 파일에 대하여 압축을 진행합니다." << endl;
	cout << "testcase 수를 입력하세요 : ";
	cin >> testcase;
	for (int casenum = 0; casenum < testcase; casenum++) {

		/*input file 입력 후 문자의 빈도수 저장*/

		int ascii[128] = { 0 };
		char ch;
		ifstream read("testcase"+ to_string(casenum) + ".txt");

		while ((ch = read.get()) != EOF) {
			ascii[ch]++;
		}

		/*priority queue에 입력된 문자와 빈도수 입력*/
		priority_queue<node, vector<node>, comp_freq> pq;
		int count = 0;
		for (int i = 0; i < 128; i++) {
			if (ascii[i]) {
				count++;
				node item(ascii[i], (char)i);
				pq.push(item);
			}
		}

		//priority queue를 허프만 코드 트리로 변환
		while (pq.size() >= 2) {
			node p = pq.top();
			pq.pop();
			node q = pq.top();
			pq.pop();
			node r;
			r.left.push_back(p);
			r.right.push_back(q);
			r.frequency = p.frequency + q.frequency;
			pq.push(r);
		}

		//허프만코드와 심볼의 정보를 담는 벡터 및 priority queue 생성
		//priority queue에서 이후 코드 정보를 벡터로 저장 후 linear search를 하기 때문에, 
		//빈도수가 높을 수록 빠르게 찾는게 시간복잡도가 유리하므로 priority queue에서 코드 길이가 짧은 순서로 저장
		priority_queue<code, vector<code>, comp_length> pq2;

		/*inorder 탐색 후 벡터에 저장*/
		node root = pq.top();

		//문자가 1개인 경우 고려
		if (count == 1)
			search(root, "0", pq2);
		else
		search(root, "", pq2);
		
		/*hfb와 hft 출력파일 객체 생성*/
		ofstream writehfb("testcase"+ to_string(casenum) + ".hfb", ios::out | ios::binary);
		ofstream writehft("testcase" + to_string(casenum) + ".hft");

		/*
			Header Encoding
			가장 첫번째줄 마지막 바이트의 유효 비트 수
			두번째줄 문자 갯수

			그 이후로

			첫 바이트 문자
			두번째 바이트 문자길이
			세번째 바이트 문자의 허프만코드(길이8이상마다 바이트 새로 부여)
		*/

		//priority queue에서 pop한 정보를 벡터에 집어넣기 위해 생성
		vector<code> code_vec;

		int index = 0;
		char tempch = '0';
		writehfb.write((const char*)&tempch, 1);	//마지막 바이트의 유효 비트 수 저장을 위한 첫 바이트 할당
		writehfb.write((const char*)&count, 1);	//문자의 갯수


		/*
			priority queue에 저장된 허프만코드 정보를 벡터에 저장과 동시에 hft, hfb 입력
		*/
		while (!pq2.empty()) {
			if (index != 0)
				writehft.write(", ", 2);	//hft 작성(콤마) 

			code_vec.push_back(pq2.top());	//priority queue의 허프만코드 정보 벡터에 push
			pq2.pop();

			writehfb.write((const char*)&code_vec[index].sym, 1);	// 코드의 심볼을 hfb에 저장

			int codelength = code_vec[index].huffcode.length();		// 코드의 길이를 hfb에 저장 
			writehfb.write((const char*)&codelength, 1);

			/*
				허프만 코드를 hfb에 저장
				코드를 1바이트 자료형에 담은 후 hfb에 저장
				만약 길이가 8을 넘는 경우 기존 바이트를 저장 및 새로 바이트 부여 후 저장
				example) 1111000101의 허프만코드면, 첫번째 바이트에 11110001 두번째 바이트에 00000001로 나누어 저장,
						바이트의 유효한 비트는 앞서 저장된 코드의 길이를 통해 알 수 있다.
						길이가 10인 경우 첫번쨰 바이트의 8bit(11110000), 두번째 바이트의 lsb로부터 2bit(01)까지가 허프만코드의 정보가 됨을 알 수 있고 이 부분을 디코더에서 스트링으로 결합한다.
			*/
			char buf = 0;
			int msb = 0;
			int effbit = 0;
			for (int i = 0; i < codelength; i++) {
				if (i % 8 == 0 && i > 0) {
					writehfb.write((const char*)&buf, 1);	//1바이트의 공간을 전부 채운 경우 write
					buf = 0;
				}
				buf = (buf << 1) + code_vec[index].huffcode[i] - '0';
			}
			writehfb.write((const char*)&buf, 1);

			// hft파일 작성, 아스키코드 10은 CR을 나타내고 hft파일에서 개행으로 구분이 안되기 때문에, CR로 따로 표시하도록 작성
			if (code_vec[index].sym == 10)
				writehft << "CR(" << ascii[code_vec[index].sym] << "):" << code_vec[index].huffcode;
			else
				writehft << code_vec[index].sym << "(" << ascii[code_vec[index].sym] << "):" << code_vec[index].huffcode;

			index++;
		}


		writehft << "\n";	//hft의 정보와 바이너리의 간격

		//end of encoding header

		//input.txt를 변환을 위해 파일포인터를 처음부터 다시 읽음
		read.clear();
		read.seekg(0, ios::beg);

		unsigned char buf = 0;
		int msb = 0;

		/*
			순차적으로 input.txt의 문자를 입력받아서 처리
			1바이트에 8bit를 채운 경우 hfb파일에 입력 후 새로 바이트 할당
			아래 while문 반복시 마지막 바이트에 유효bit가 있는 경우는 hfb파일에 입력 안된다.
		*/
		while ((ch = read.get()) != EOF) {

			string buf_code = return_code(code_vec, ch);	//문자를 입력시 코드를 반환
			writehft << buf_code;	//코드를 hft에 write
			int codelength = buf_code.length();	//코드의 길이

			for (int i = 0; i < codelength; i++) {
				buf = (buf << 1) + buf_code[i] - '0';
				msb++;

				//1바이트를 전부 채운 경우 hfb에 입력 후 새로 바이트 할당
				if (msb == 8) {
					msb = 0;
					writehfb.write((const char*)&buf, 1);
					buf = 0;
				}
			}

		}

		//마지막 바이트의 유효 bit 정보 저장
		char effectbit = (char)msb;

		//마지막 바이트에 유효 bit가 있는 경우, 버퍼에 입력후 hfb에 write (검사를 안하면 의미없는 0바이트가 입력됨)
		if (msb != 0 && msb != 8) {
			while (msb != 8) {
				buf = buf << 1;
				msb++;
			}
			writehfb.write((const char*)&buf, 1);
		}

		//인코딩 헤더 첫바이트에 마지막 바이트의 유효 비트 수 입력
		writehfb.seekp(0, ios::beg);
		writehfb.write((const char*)&effectbit, 1);
		cout << "testcase"<<casenum<<" compress complete!" << endl;
	}
}

void search(const node& root, string code_cur, priority_queue<code, vector<code>, comp_length>& pq2) {
	//inorder 탐색을 통해 트리의 leaf인 경우, 허프만 코드를 심볼과 같이 priority queue에 push
	if (!root.left.empty())
		search(root.left.front(), code_cur + "0", pq2);
	if (root.left.empty() && root.right.empty()) {
		code item;
		item.sym = root.symbol;
		item.huffcode = code_cur;
		pq2.push(item);
	}
	if (!root.right.empty())
		search(root.right.front(), code_cur + "1", pq2);
}


string return_code(vector<code>& code_vec, char ch) {
	//문자를 입력으로 주었을때 벡터에서 해당 문자에 해당하는 허프만코드를 탐색 후 리턴
	for (code curcode : code_vec)
		if (curcode.sym == ch)
			return curcode.huffcode;

	//벡터에 없는 경우 Error 리턴
	cout << "error" << endl;
	return "Error";	
}