#include <iostream>
#include <fstream> 
#include <queue>
#include <string>
#include <unordered_map>
using namespace std;

//symbol�� �������ڵ� ���� ����
class code {
public:
	code(char symbol, string huff) {
		sym = symbol;
		huffcode = huff;
	}
	char sym;
	string huffcode;
};


int main() {
	int testcase;
	cout << "�Է����� �̸��� testcase ������� testcase0.hfb, testcase1.hfb, testcase2.hfb, ... �� ������ �Ǿ�� �մϴ�." << endl;
	cout << "������� 3�� �Է½� testcase0.hfb, testcase1.hfb, testcase2.hfb 3���� ���Ͽ� ���Ͽ� ���������� �����մϴ�." << endl;
	cout << "testcase ���� �Է��ϼ��� : ";
	cin >> testcase;
	for (int casenum = 0; casenum < testcase; casenum++) {

		ifstream read_file("testcase" + to_string(casenum) + ".hfb", ios::in | ios::binary);
		/*
		encoding header�� ����
		ù ����Ʈ�� ������ ����Ʈ�� ��ȿ bit��
		�ι�° ����Ʈ ���� ����
		����° ����Ʈ���� ���ں� ������
		����(1����Ʈ), ������ ����(1����Ʈ), �������ڵ�(1����Ʈ�̻�)���� ���� ���

		����° ����Ʈ���� ���� ������ŭ �ݺ��Ͽ� �� ���ڿ� ���� ������code ��ü�� ����� ���ÿ� ���� Ž���� ���� unordered map�� key�� code�� value�� symbol�� �Է�
		������ ���̰� 8�� �Ѵ� ���, �ش� ���ڿ����� �ڵ带 1����Ʈ�Ѱ� ���� �ο�����.
		*/

		unordered_map<string, char> um;
		/*
			�������� �ǹ̾��� ������ �Է¹޴� ��츦 ���ܽ�Ű�� ���� ������ ���� ���� save�� ����
			���������͸� ������ ���� �� ������ ����Ʈ�� ���� �� �ٽ� ó������ �ǵ���.
		*/
		read_file.seekg(0, ios::end);
		int save = (int)read_file.tellg();
		read_file.seekg(0, ios::beg);

		//������ ����Ʈ�� ��ȿbit��, �ڵ��� ���� ����
		char effectbit;
		read_file.read(&effectbit, 1);
		char size;
		read_file.read(&size, 1);

		/*������ ������ ��ü code�� ����*/

		//���� ������ŭ �ݺ�
		for (int i = 0; i < (int)size; i++) {
			char symbol;
			char length;
			char huffcode;

			read_file.read(&symbol, 1);
			read_file.read(&length, 1);
			read_file.read(&huffcode, 1);

			/*
			�Է¹��� huffcode���� �������ڵ� ���̸�ŭ string���� ��ȯ �� unorderedmap�� �ڵ�� �ɺ��� �ִ´�.
			���� ���̰� 8�� ������ ���ο� ����Ʈ�� �Է¹ް� ������ �ڵ� ���̸�ŭ �߶� ���δ�.
			1����Ʈ�� �������ڵ��� lsb���� �������ڵ��� ��Ʈ���� ����
			*/
			string huff_ = "";
			string huff_buf = "";
			for (int j = 0; j < length; j++) {
				//���̰� 8�� �Ѵ� ���, ���ۿ� ������ ���� �ڵ� ����
				if (j % 8 == 0 && j > 0) {
					read_file.read(&huffcode, 1);
					huff_buf = huff_buf + huff_;
					huff_ = "";
				}
				//huffcode�� lsb�� string huff_�� ����
				if (huffcode & 1) {
					huff_ = "1" + huff_;
				}
				else
					huff_ = "0" + huff_;
				huffcode = huffcode >> 1;
			}

			//huff_buf�� �ڷ� huff_�� �̾���δ�.
			huff_buf = huff_buf + huff_;

			//unoredered map�� insert
			um.insert({ huff_buf,symbol });
		}

		ofstream write_out("testcase" + to_string(casenum) + ".hfd");

		/*
			��ȯ�� binary�� ���ڷ� ��ȯ

			���� �Էµ� ����Ʈ�� string���� ��ȯ
			string�� ���̸� �ø��鼭 ���� �� unordered map�� ��ġ�ϴ� ���� ����� ���ڷ� write
			�� �� ��ġ�� string�� �߶󳽴�.
			��Ī�� �ȵǴ� ��� ���� ����Ʈ�� ���� �Ŀ� �̾���δ�.
			���� ������ ����Ʈ�� ��� ���ڵ� ����� ó���� ���� ������ ��ȿbit��ŭ�� ���� string�� �̾���� �� Ž���Ѵ�.
			�� �� ���ʿ��� ������ �Է¹��� �ʱ� ���� ����
		*/
		string buffer = "";		//����Ʈ�� �Է¹��� �� string���� ��ȯ
		string buffer2 = "";	//key���� ã�� string�� �����ϴ� buffer, ���ڷ� ��ȯ�ϰ� ���� bit string(buffer3)�� buffer�� �̾� ���δ�.
		string buffer3 = "";	//unordered map�� key������ ���̴� buffer, buffer2�� �߶󳻼� ��� �� ���� ��ȯ�� ���� bit string ���� 

		while (!read_file.eof()) {
			//�Է����Ϸκ��� buf�� 1����Ʈ �ҷ��� 
			char buf;
			read_file.read(&buf, 1);

			for (int j = 0; j < 8; j++) {
				if (buf & 1)
					buffer = "1" + buffer;
				else
					buffer = "0" + buffer;
				buf = buf >> 1;
			}

			//������ ���� ���, effectbit��ŭ�� �Ҵ�
			//��ȿ bit�� 8�� ��쵵 ���ڵ��� ��ȿbit�� 0���� ����Ǳ� ������, ��ȿbit�� 0�� �ƴ� ��� ����
			if (read_file.tellg() == save && effectbit != 0)
				buffer = buffer.substr(0, effectbit);

			buffer2 = buffer3 + buffer;
			buffer = "";

			//buffer2���� �������ڵ�� ��Ī�Ǵ� ���ڸ� ã�� �� ������Ͽ� write, ��Ī�Ǵ� ���ڰ� ������ while�� Ż��
			//buffer3�� buffer2�� �ɰ�� ��Ī�ɶ����� ã�� ���� bit���� �ٽ� buffer3�� ��������� 
			int t = 0, k = 0;
			while ((t + k) < (int)buffer2.length()) {
				k++;
				buffer3 = buffer2.substr(t, k);
				if (um.find(buffer3) != um.end()) {
					write_out << um.find(buffer3)->second;
					t += k;
					k = -1;
				}
			}

			//������ ����Ʈ ���޽� Ż��
			if (read_file.tellg() == save) {
				cout << "testcase"<<casenum<<" decompress complete!" << endl;
				break;
			}
		}
	}
}