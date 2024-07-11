
1. cyclonedds-master 폴더를 루트에 복사

1. cyclonedds-master.tar.tar.gz 루트에 복사
  -> tar -zxvf ./cyclonedds-master.tar.gz

2. /hs_cmake/usr 하위 폴더에 있는 3개 폴더를 /usr 하위에 덮어쓰기 (bin, lib, shar)

3. cd /home/root/cyclonedds-master/build 경로에서 make 
 -> cmake 오류발생시
   : export CMAKE_ROOT=/usr/share/cmake-3.16

 -> 날짜 오류 발생시
   : date -s '2023-09-23 12:34:56' 등으로 시간 수정

 -> root@s32g274akat:~/cyclonedds-master/build# make
     make: /usr/bin/cmake: Permission denied
     make: *** [Makefile:1506: cmake_check_build_system] Error 127
   : cd /usr/bin
   : chmod +x cmake

4.  /home/root/cyclonedds-master/build/bin/cyclonedds.xml 이며, <ports> 에 ether-type decimal 로 입력
 -> : echo $CYCLONEDDS_URI
     위 명령에 대해서 파일경로 응답이 안오면
 -> export CYCLONEDDS_URI=file:///home/root/cyclonedds-master/build/bin/cyclonedds.xml

 -> cyclonedds-master/src/core/ddsi/src/ddsi_raweth.c 의 177번째 줄 vtag = vlan_id; 로 작성

5. examples 폴더에 katech.tar 압축해제

6. build 폴더에서 rm -rf ./* 해서 폴더 전체 삭제

7. cmake -DBUILD_EXAMPLES=ON ..

8. make

9. build/bin/KatechPong 실행 후 build/bin/KatechPing 
 -> 정해진 시간만 돌다가 끄려면 KatechPing 숫자 하면 숫자 시간만큼만 돌다가 꺼짐