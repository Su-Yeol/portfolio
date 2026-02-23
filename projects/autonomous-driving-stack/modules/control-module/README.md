# Control Module

`control-module`은 경로/차량 상태를 입력으로 조향과 가감속 명령을 생성하는 엔트리포인트를 제공한다.

## 디렉터리
- `conf/config.ini`: 설정 파일 심볼릭 링크(`../../../config/control-module.ini`)
- `include/`: 공개 헤더 래퍼
- `src/ControlModule.cpp`: 공개 엔트리포인트
- `src/Communicator.cpp`: 공개 통신 연결 단위
- `src/PathManager.cpp`: 공개 경로 연결 단위
- `src/Controller.cpp`: 공개 제어 연결 단위
- `build.sh`: 빌드 스크립트

## 공통 의존성
- 설정 파서 구현: `../common/src/ConfigParser.cpp`
- 공통 사양/CRC: `../common/src/CanSpecConfigLoader.cpp`, `../common/src/CrcProvider*.cpp`

## 비공개 구현
- `../common/src/private/control-module/ControlModule_impl.cpp`
- `../common/src/private/control-module/Communicator_impl.cpp`
- `../common/src/private/control-module/Controller_impl.cpp`
- `../common/src/private/control-module/PathManager_impl.cpp`

## 빌드
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/control-module
./build.sh
USE_PRIVATE_IMPL=1 ./build.sh
```
