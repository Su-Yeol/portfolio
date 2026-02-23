# Autonomous Driving Modules

`autonomous-driving-stack/modules`는 주행 제어, 의사결정, 원격 통신 모듈과 공통 라이브러리 계층을 포함한다.

## 비공개화 범위
- 적용: `aeb-control`, `control-module`, `decision-module`
- 제외: `remote-control`, `common`

## 폴더
- `aeb-control/`: AEB 제어 엔트리포인트
- `control-module/`: 종/횡방향 제어 엔트리포인트
- `decision-module/`: K-City/게이트웨이 엔트리포인트
- `remote-control/`: 원격 통신 유틸리티
- `common/`: 공통 사양/CRC/코덱/헤더

## 공통 헤더 구조
중복 헤더를 아래 4개 공통 헤더로 정리했다.
- `common/include/modules/shared/ControlModuleCommon.h`
- `common/include/modules/shared/ModuleInterfaceCommon.h`
- `common/include/modules/shared/communicator_cd.h`
- `common/include/modules/shared/config_parser.h`

## 설정 구조
설정 원본은 루트 `../config/*.ini`이며, 모듈 내부 `config.ini`는 심볼릭 링크로 연결된다.

## 빌드
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/aeb-control
./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/control-module
./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/decision-module
./build.sh
./cangateway.sh
```

`USE_PRIVATE_IMPL=1`을 지정하면 비공개 구현을 포함해 빌드한다.

## include 정책
- 공개 소스는 `common/include/modules/*` 공용 헤더를 참조한다.
- 비공개 구현(`common/src/private/*_impl.cpp`)은 모듈 헤더를 `modules/<module>/...` 경로로 include 하도록 정리했다.
