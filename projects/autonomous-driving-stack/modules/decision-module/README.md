# Decision Module

`decision-module`은 K-City 경로 처리와 Gateway CAN 라우팅 엔트리포인트를 제공한다.

## 디렉터리
- `config.ini`: 설정 파일 심볼릭 링크(`../../config/decision-module.ini`)
- `include/`: 공개 헤더 래퍼
- `source_kcity/`: K-City 공개 엔트리포인트 소스
- `source_gateway/`: Gateway 공개 엔트리포인트 소스
- `build.sh`: K-City 빌드 스크립트(`bulid.sh` 래퍼)
- `bulid.sh`: 기존 K-City 빌드 스크립트(호환 유지)
- `cangateway.sh`: Gateway 빌드 스크립트

## 공통 의존성
- 설정 파서 구현: `../common/src/ConfigParser.cpp`
- 공통 사양/CRC: `../common/src/CanSpecConfigLoader.cpp`, `../common/src/CrcProvider*.cpp`

## 비공개 구현
- `../common/src/private/decision-module/MainModule_impl.cpp`
- `../common/src/private/decision-module/Communicator_impl.cpp`
- `../common/src/private/decision-module/PathManager_impl.cpp`
- `../common/src/private/decision-module/CANGateway_impl.cpp`
- `../common/src/private/decision-module/CRC_impl.cpp`

## 빌드
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/decision-module
./build.sh
./cangateway.sh
USE_PRIVATE_IMPL=1 ./build.sh
USE_PRIVATE_IMPL=1 ./cangateway.sh
```
