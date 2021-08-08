#include <iostream>
#include <fstream> 
#include <queue>
#include <list>
#include <string>
#include <vector>
using namespace std;

//�������ڵ� ������ ���� Ʈ��
class node {
public:

	node(int freq = 0, char sym = '0')
	{
		left = list<node>(); right = list<node>();
		frequency = freq; symbol = sym;
	}

	int frequency;
	char symbol;

	//�ڼ��� ������ ��� list�� ����Ű���� ����
	list<node> left;
	list<node> right;
};

struct comp_freq {
	bool operator()(const node& p, const node& q) {
		return p.frequency > q.frequency;
	}
};

//symbol�� �������ڵ� ���� ����
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
	cout << "�Է����� �̸��� testcase ������� testcase0.txt, testcase1.txt, testcase2.txt, ... �� ������ �Ǿ�� �մϴ�." << endl;
	cout << "������� 3�� �Է½� testcase0.txt, testcase1.txt, testcase2.txt 3���� ���Ͽ� ���Ͽ� ������ �����մϴ�." << endl;
	cout << "testcase ���� �Է��ϼ��� : ";
	cin >> testcase;
	for (int casenum = 0; casenum < testcase; casenum++) {

		/*input file �Է� �� ������ �󵵼� ����*/

		int ascii[128] = { 0 };
		char ch;
		ifstream read("testcase"+ to_string(casenum) + ".txt");

		while ((ch = read.get()) != EOF) {
			ascii[ch]++;
		}

		/*priority queue�� �Էµ� ���ڿ� �󵵼� �Է�*/
		priority_queue<node, vector<node>, comp_freq> pq;
		int count = 0;
		for (int i = 0; i < 128; i++) {
			if (ascii[i]) {
				count++;
				node item(ascii[i], (char)i);
				pq.push(item);
			}
		}

		//priority queue�� ������ �ڵ� Ʈ���� ��ȯ
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

		//�������ڵ�� �ɺ��� ������ ��� ���� �� priority queue ����
		//priority queue���� ���� �ڵ� ������ ���ͷ� ���� �� linear search�� �ϱ� ������, 
		//�󵵼��� ���� ���� ������ ã�°� �ð����⵵�� �����ϹǷ� priority queue���� �ڵ� ���̰� ª�� ������ ����
		priority_queue<code, vector<code>, comp_length> pq2;

		/*inorder Ž�� �� ���Ϳ� ����*/
		node root = pq.top();

		//���ڰ� 1���� ��� ���
		if (count == 1)
			search(root, "0", pq2);
		else
		search(root, "", pq2);
		
		/*hfb�� hft ������� ��ü ����*/
		ofstream writehfb("testcase"+ to_string(casenum) + ".hfb", ios::out | ios::binary);
		ofstream writehft("testcase" + to_string(casenum) + ".hft");

		/*
			Header Encoding
			���� ù��°�� ������ ����Ʈ�� ��ȿ ��Ʈ ��
			�ι�°�� ���� ����

			�� ���ķ�

			ù ����Ʈ ����
			�ι�° ����Ʈ ���ڱ���
			����° ����Ʈ ������ �������ڵ�(����8�̻󸶴� ����Ʈ ���� �ο�)
		*/

		//priority queue���� pop�� ������ ���Ϳ� ����ֱ� ���� ����
		vector<code> code_vec;

		int index = 0;
		char tempch = '0';
		writehfb.write((const char*)&tempch, 1);	//������ ����Ʈ�� ��ȿ ��Ʈ �� ������ ���� ù ����Ʈ �Ҵ�
		writehfb.write((const char*)&count, 1);	//������ ����


		/*
			priority queue�� ����� �������ڵ� ������ ���Ϳ� ����� ���ÿ� hft, hfb �Է�
		*/
		while (!pq2.empty()) {
			if (index != 0)
				writehft.write(", ", 2);	//hft �ۼ�(�޸�) 

			code_vec.push_back(pq2.top());	//priority queue�� �������ڵ� ���� ���Ϳ� push
			pq2.pop();

			writehfb.write((const char*)&code_vec[index].sym, 1);	// �ڵ��� �ɺ��� hfb�� ����

			int codelength = code_vec[index].huffcode.length();		// �ڵ��� ���̸� hfb�� ���� 
			writehfb.write((const char*)&codelength, 1);

			/*
				������ �ڵ带 hfb�� ����
				�ڵ带 1����Ʈ �ڷ����� ���� �� hfb�� ����
				���� ���̰� 8�� �Ѵ� ��� ���� ����Ʈ�� ���� �� ���� ����Ʈ �ο� �� ����
				example) 1111000101�� �������ڵ��, ù��° ����Ʈ�� 11110001 �ι�° ����Ʈ�� 00000001�� ������ ����,
						����Ʈ�� ��ȿ�� ��Ʈ�� �ռ� ����� �ڵ��� ���̸� ���� �� �� �ִ�.
						���̰� 10�� ��� ù���� ����Ʈ�� 8bit(11110000), �ι�° ����Ʈ�� lsb�κ��� 2bit(01)������ �������ڵ��� ������ ���� �� �� �ְ� �� �κ��� ���ڴ����� ��Ʈ������ �����Ѵ�.
			*/
			char buf = 0;
			int msb = 0;
			int effbit = 0;
			for (int i = 0; i < codelength; i++) {
				if (i % 8 == 0 && i > 0) {
					writehfb.write((const char*)&buf, 1);	//1����Ʈ�� ������ ���� ä�� ��� write
					buf = 0;
				}
				buf = (buf << 1) + code_vec[index].huffcode[i] - '0';
			}
			writehfb.write((const char*)&buf, 1);

			// hft���� �ۼ�, �ƽ�Ű�ڵ� 10�� CR�� ��Ÿ���� hft���Ͽ��� �������� ������ �ȵǱ� ������, CR�� ���� ǥ���ϵ��� �ۼ�
			if (code_vec[index].sym == 10)
				writehft << "CR(" << ascii[code_vec[index].sym] << "):" << code_vec[index].huffcode;
			else
				writehft << code_vec[index].sym << "(" << ascii[code_vec[index].sym] << "):" << code_vec[index].huffcode;

			index++;
		}


		writehft << "\n";	//hft�� ������ ���̳ʸ��� ����

		//end of encoding header

		//input.txt�� ��ȯ�� ���� ���������͸� ó������ �ٽ� ����
		read.clear();
		read.seekg(0, ios::beg);

		unsigned char buf = 0;
		int msb = 0;

		/*
			���������� input.txt�� ���ڸ� �Է¹޾Ƽ� ó��
			1����Ʈ�� 8bit�� ä�� ��� hfb���Ͽ� �Է� �� ���� ����Ʈ �Ҵ�
			�Ʒ� while�� �ݺ��� ������ ����Ʈ�� ��ȿbit�� �ִ� ���� hfb���Ͽ� �Է� �ȵȴ�.
		*/
		while ((ch = read.get()) != EOF) {

			string buf_code = return_code(code_vec, ch);	//���ڸ� �Է½� �ڵ带 ��ȯ
			writehft << buf_code;	//�ڵ带 hft�� write
			int codelength = buf_code.length();	//�ڵ��� ����

			for (int i = 0; i < codelength; i++) {
				buf = (buf << 1) + buf_code[i] - '0';
				msb++;

				//1����Ʈ�� ���� ä�� ��� hfb�� �Է� �� ���� ����Ʈ �Ҵ�
				if (msb == 8) {
					msb = 0;
					writehfb.write((const char*)&buf, 1);
					buf = 0;
				}
			}

		}

		//������ ����Ʈ�� ��ȿ bit ���� ����
		char effectbit = (char)msb;

		//������ ����Ʈ�� ��ȿ bit�� �ִ� ���, ���ۿ� �Է��� hfb�� write (�˻縦 ���ϸ� �ǹ̾��� 0����Ʈ�� �Էµ�)
		if (msb != 0 && msb != 8) {
			while (msb != 8) {
				buf = buf << 1;
				msb++;
			}
			writehfb.write((const char*)&buf, 1);
		}

		//���ڵ� ��� ù����Ʈ�� ������ ����Ʈ�� ��ȿ ��Ʈ �� �Է�
		writehfb.seekp(0, ios::beg);
		writehfb.write((const char*)&effectbit, 1);
		cout << "testcase"<<casenum<<" compress complete!" << endl;
	}
}

void search(const node& root, string code_cur, priority_queue<code, vector<code>, comp_length>& pq2) {
	//inorder Ž���� ���� Ʈ���� leaf�� ���, ������ �ڵ带 �ɺ��� ���� priority queue�� push
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
	//���ڸ� �Է����� �־����� ���Ϳ��� �ش� ���ڿ� �ش��ϴ� �������ڵ带 Ž�� �� ����
	for (code curcode : code_vec)
		if (curcode.sym == ch)
			return curcode.huffcode;

	//���Ϳ� ���� ��� Error ����
	cout << "error" << endl;
	return "Error";	
}