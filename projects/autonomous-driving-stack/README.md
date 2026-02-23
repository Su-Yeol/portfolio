# autonomous-driving-stack

자율주행 제어 프로젝트의 공개/비공개 분리 구조를 포함한 루트 저장소다.

## 디렉터리
- `modules/`: 실행 모듈(`aeb-control`, `control-module`, `decision-module`, `remote-control`)과 공통 코드(`common`)
- `config/`: 모듈 공용 설정 원본(`aeb-control.ini`, `control-module.ini`, `decision-module.ini`)
- `logs/`: 런타임 로그 디렉터리
- `scripts/`: 루트 빌드 유틸리티
- `artifacts/`: 루트 스크립트 실행 시 생성되는 산출물 디렉터리

## 설정 파일 정책
- 설정 원본은 `config/*.ini`만 사용한다.
- 모듈 내부 설정 경로는 아래 심볼릭 링크로 연결된다.
  - `modules/aeb-control/conf/config.ini -> ../../../config/aeb-control.ini`
  - `modules/control-module/conf/config.ini -> ../../../config/control-module.ini`
  - `modules/decision-module/config.ini -> ../../config/decision-module.ini`

## 빌드

### 전제조건
- `g++`(C++17), `bash`, `make`(모듈별 스크립트 내부 사용 가능)
- Linux 환경(소켓/CAN 헤더 포함)

### 모듈 단위 빌드
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/aeb-control
./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/control-module
./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/decision-module
./build.sh
./cangateway.sh
```

### 루트 스크립트 빌드
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack
./scripts/build_decision_kcity.sh
./scripts/build_decision_gateway.sh
```

## 비공개 구현 연동
- `USE_PRIVATE_IMPL=1`로 빌드하면 `modules/common/src/private/<module>/*_impl.cpp`를 함께 링크한다.
- 세부 매핑은 `modules/common/src/private/README.md`를 참고한다.

## 최소 재현 체크
```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/aeb-control
USE_PRIVATE_IMPL=1 ./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/control-module
USE_PRIVATE_IMPL=1 ./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/decision-module
USE_PRIVATE_IMPL=1 ./build.sh
```
