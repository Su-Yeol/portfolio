#!/bin/bash

# 입력 값을 변수에 저장
input=$1

# 기본 경로 설정
base_path="./conf/Section_2/2_Common_SiteSyncSync_State_Machine"

# 파일 이름에 입력 값을 추가하여 경로 생성
file_name="02_02_A_${input}.ini"

# cd 명령어로 디렉토리 이동
cd ../../../

# 지정된 경로의 파일을 실행
./daemon_cl_log pfe0 -F "${base_path}/${file_name}"

