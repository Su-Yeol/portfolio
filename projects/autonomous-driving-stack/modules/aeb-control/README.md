# AEB Control Module

`aeb-control`은 차량 상태 기반 AEB CAN FD 제어 프레임 송신 엔트리포인트를 제공한다.

## 디렉터리
- `conf/config.ini`: 설정 파일 심볼릭 링크(`../../../config/aeb-control.ini`)
- `include/`: 공개 헤더 래퍼
- `src/AEBControl_S32G_v2.cpp`: 공개 엔트리포인트
- `src/Communicator.cpp`: 공개 통신 연결 단위
- `build.sh`: 빌드 스크립트

## 공통 의존성
- 설정 파서 구현: `../common/src/ConfigParser.cpp`
- 공통 사양/CRC: `../common/src/CanSpecConfigLoader.cpp`, `../common/src/CrcProvider*.cpp`

## 비공개 구현
- `../common/src/private/aeb-control/AEBControl_S32G_v2_impl.cpp`
- `../common/src/private/aeb-control/Communicator_impl.cpp`

## 빌드
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/aeb-control
./build.sh
USE_PRIVATE_IMPL=1 ./build.sh
```
