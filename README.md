# Compress-Program
허프만 코드를 이용한 압축 및 압축 해제 프로그램

텍스트 파일을 압축하는 프로그램입니다.
고려사항은 아스키코드에 한해서 작동 가능합니다. 텍스트파일에 유니코드가 들어가면 안됩니다.

Compress.exe가 압축프로그램 Decompress.exe가 압축해제 프로그램입니다.

과제물의 실행 방법은

각 실행파일은 이름의 입력을 따로 지정하는게 아닌 디폴트로 셋팅되어있고, 처음에 테스트케이스의 개수를 입력 받습니다.
Compress.exe의 경우
테스트케이스 이름의 형식은, compress.exe의 경우 순서대로 testcase0.txt, testcase1.txt, testcase2.txt, …으로 “testcase” + 번호 + “.txt”의 형태로 파일을 입력으로 받으며, 
과제의 경우는 테스트케이스가 5개이기 때문에 5를 입력시 testcase0.txt ~ testcase4.txt의 5개 파일로부터 인코딩을 거쳐 hfb 파일과, 그에 따른 정보를 담고 사람이 볼 수 있도록 한 hft파일을 생성합니다.

Decompress.exe의 경우 compress.exe와 같이 “testcase” + 번호 + “.hfb”의 형태로 파일을 입력받으며, 5를 입력시 testcase0.hfb ~ testcase4.hfb 파일을 입력으로 하여 testcase0.hfd~testcase4.hfd 파일을 출력으로 디코딩 합니다.

testcase3과 testcase4의 경우, 실험을 위해 높은 용량의 텍스트로서 수행되었기 때문에 인코딩과 디코딩시 다소 시간이 걸릴 수 있습니다.
