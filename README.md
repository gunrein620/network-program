# network-program

### 1-1 실행순서 #####################

tar -xvzf unpv13e.tar.gz // 압축푸는거
[1]cd unpv13e // 작업디렉토리 이동
[2]./configure // 컨파일 준비
[3]cd lib // 컨파일 순서조정위해 이동
[4]make // 컨파일 자동순서 조정 
[5]cd intro // 컴파일 하기위해 이동
[6] 컴파일하기 ( 실행파일 생성)
  gcc -o p11_cli -I../lib daytimetcpcli.c ../libunp.a 
  gcc -o p11_srv -I../lib daytimetcpsrv.c ../libunp.a
[7] 서버실행 
  sudo ./p11_srv
[8] 새 터미널창 열고 확인하기 
  ./p11_cli 127.0.0.1


#### 1-2 실습 ########################

[1] 복사파일 만들기 
cp daytimetcpcli.c p12_cli.c
cp daytimetcpsrv.c p12_srv.c

[2] #include "unp.h" 지우기.
- unp.h의 #include문 + macro정의에서
    - 최착의 `#endif` 반드시 제외하고
    - 모두 복사 붙인다.

[3] 컴파일
gcc -o p12_cli -I../lib p12_cli.c ../libunp.a
gcc -o p12_srv -I../lib p12_srv.c ../libunp.a

[4] 서버실행
sudo ./p12_srv

[5] 클라이언트 실행
./p12_cli 127.0.0.1
