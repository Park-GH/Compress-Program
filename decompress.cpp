#include <iostream>
#include <fstream> 
#include <queue>
#include <string>
#include <unordered_map>
using namespace std;

//symbol과 허프만코드 정보 저장
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
	cout << "입력파일 이름은 testcase 순서대로 testcase0.hfb, testcase1.hfb, testcase2.hfb, ... 의 형식이 되어야 합니다." << endl;
	cout << "예를들면 3을 입력시 testcase0.hfb, testcase1.hfb, testcase2.hfb 3개의 파일에 대하여 압축해제를 진행합니다." << endl;
	cout << "testcase 수를 입력하세요 : ";
	cin >> testcase;
	for (int casenum = 0; casenum < testcase; casenum++) {

		ifstream read_file("testcase" + to_string(casenum) + ".hfb", ios::in | ios::binary);
		/*
		encoding header의 정보
		첫 바이트는 마지막 바이트의 유효 bit수
		두번째 바이트 문자 갯수
		세번째 바이트부터 문자별 정보가
		문자(1바이트), 문자의 길이(1바이트), 허프만코드(1바이트이상)으로 정보 담김

		세번째 바이트부터 문자 갯수만큼 반복하여 각 문자에 대한 정보를code 객체에 저장과 동시에 빠른 탐색을 위해 unordered map에 key를 code로 value를 symbol로 입력
		문자의 길이가 8을 넘는 경우, 해당 문자에대한 코드를 1바이트넘게 새로 부여받음.
		*/

		unordered_map<string, char> um;
		/*
			마지막에 의미없이 파일을 입력받는 경우를 제외시키기 위해 파일의 끝을 변수 save에 저장
			파일포인터를 끝으로 셋팅 후 마지막 바이트를 저장 후 다시 처음으로 되돌림.
		*/
		read_file.seekg(0, ios::end);
		int save = (int)read_file.tellg();
		read_file.seekg(0, ios::beg);

		//마지막 바이트의 유효bit와, 코드의 갯수 읽음
		char effectbit;
		read_file.read(&effectbit, 1);
		char size;
		read_file.read(&size, 1);

		/*문자의 정보를 객체 code에 저장*/

		//문자 갯수만큼 반복
		for (int i = 0; i < (int)size; i++) {
			char symbol;
			char length;
			char huffcode;

			read_file.read(&symbol, 1);
			read_file.read(&length, 1);
			read_file.read(&huffcode, 1);

			/*
			입력받은 huffcode에서 허프만코드 길이만큼 string으로 변환 후 unorderedmap에 코드와 심볼을 넣는다.
			만약 길이가 8이 넘으면 새로운 바이트를 입력받고 나머지 코드 길이만큼 잘라서 붙인다.
			1바이트의 허프만코드의 lsb부터 허프만코드의 스트링에 저장
			*/
			string huff_ = "";
			string huff_buf = "";
			for (int j = 0; j < length; j++) {
				//길이가 8을 넘는 경우, 버퍼에 저장후 새로 코드 읽음
				if (j % 8 == 0 && j > 0) {
					read_file.read(&huffcode, 1);
					huff_buf = huff_buf + huff_;
					huff_ = "";
				}
				//huffcode의 lsb를 string huff_에 저장
				if (huffcode & 1) {
					huff_ = "1" + huff_;
				}
				else
					huff_ = "0" + huff_;
				huffcode = huffcode >> 1;
			}

			//huff_buf의 뒤로 huff_를 이어붙인다.
			huff_buf = huff_buf + huff_;

			//unoredered map에 insert
			um.insert({ huff_buf,symbol });
		}

		ofstream write_out("testcase" + to_string(casenum) + ".hfd");

		/*
			변환된 binary를 문자로 변환

			먼저 입력된 바이트를 string으로 변환
			string을 길이를 늘리면서 읽은 후 unordered map에 일치하는 값이 존재시 문자로 write
			그 후 일치한 string을 잘라낸다.
			매칭이 안되는 경우 다음 바이트를 읽은 후에 이어붙인다.
			만약 마지막 바이트인 경우 인코딩 헤더의 처음에 받은 정보인 유효bit만큼만 남은 string에 이어붙인 후 탐색한다.
			그 후 불필요한 정보를 입력받지 않기 위해 종료
		*/
		string buffer = "";		//바이트를 입력받은 후 string으로 변환
		string buffer2 = "";	//key값을 찾는 string을 저장하는 buffer, 문자로 변환하고 남은 bit string(buffer3)에 buffer를 이어 붙인다.
		string buffer3 = "";	//unordered map의 key값으로 쓰이는 buffer, buffer2를 잘라내서 사용 및 문자 변환후 남은 bit string 저장 

		while (!read_file.eof()) {
			//입력파일로부터 buf에 1바이트 불러옴 
			char buf;
			read_file.read(&buf, 1);

			for (int j = 0; j < 8; j++) {
				if (buf & 1)
					buffer = "1" + buffer;
				else
					buffer = "0" + buffer;
				buf = buf >> 1;
			}

			//파일의 끝인 경우, effectbit만큼만 할당
			//유효 bit가 8인 경우도 인코딩시 유효bit가 0으로 저장되기 때문에, 유효bit가 0이 아닌 경우 실행
			if (read_file.tellg() == save && effectbit != 0)
				buffer = buffer.substr(0, effectbit);

			buffer2 = buffer3 + buffer;
			buffer = "";

			//buffer2에서 허프만코드와 매칭되는 문자를 찾은 후 출력파일에 write, 매칭되는 문자가 없을때 while문 탈출
			//buffer3에 buffer2를 쪼개어서 매칭될때까지 찾고 남은 bit열은 다시 buffer3로 저장되있음 
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

			//마지막 바이트 도달시 탈출
			if (read_file.tellg() == save) {
				cout << "testcase"<<casenum<<" decompress complete!" << endl;
				break;
			}
		}
	}
}