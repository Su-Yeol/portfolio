# Private Implementation Guide

## 1. 목적
- 공개 저장소에는 인터페이스와 연결 코드만 유지한다.
- 계산/판단 알고리즘, CRC 연산, CAN 프레임 상수/비트필드 처리 로직은 `modules/common/src/private/` 하위 구현 파일로 분리한다.
- `remote-control`, `common` 모듈은 이 문서의 비공개화 대상에서 제외한다.

## 2. 빌드/연결 방법
1. 비공개 구현 파일 위치를 준비한다.
- `modules/common/src/private/aeb-control/*_impl.cpp`
- `modules/common/src/private/control-module/*_impl.cpp`
- `modules/common/src/private/decision-module/*_impl.cpp`
2. 비공개 구현 포함 빌드 시 `USE_PRIVATE_IMPL=1`을 지정한다.

```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/aeb-control
USE_PRIVATE_IMPL=1 ./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/control-module
USE_PRIVATE_IMPL=1 ./build.sh

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/decision-module
USE_PRIVATE_IMPL=1 ./build.sh
USE_PRIVATE_IMPL=1 ./cangateway.sh
```

3. 비공개 구현 파일이 없으면 공개 엔트리포인트는 명확한 오류 메시지를 출력하고 종료한다.

## 3. Private API 목록

| Module | Public file/function | Private impl file/function | I/O | Description |
|---|---|---|---|---|
| aeb-control | `aeb-control/src/AEBControl_S32G_v2.cpp::main` | `common/src/private/aeb-control/AEBControl_S32G_v2_impl.cpp::main` | GPS/CAN 입력, AEB CAN FD 출력 | AEB 메인 제어 루프 |
| aeb-control | `aeb-control/src/AEBControl_S32G_v2.cpp::{AEBComControl,AEBStopControl}` | `common/src/private/aeb-control/AEBControl_S32G_v2_impl.cpp::{AEBComControl,AEBStopControl}` | 차량 상태 입력, 제동/경보 프레임 출력 | AEB 판단/프레임 생성 |
| aeb-control | `aeb-control/src/AEBControl_S32G_v2.cpp::ComputeAebFrameCrc` | `common/src/private/aeb-control/AEBControl_S32G_v2_impl.cpp::ComputeAebFrameCrc` | 프레임 바이트 입력, CRC 출력 | CRC 계산 |
| aeb-control | `aeb-control/src/Communicator.cpp::{VehicleReceiver,GPSReceiver,MCUSender,Key}` | `common/src/private/aeb-control/Communicator_impl.cpp::{VehicleReceiver,GPSReceiver,MCUSender,Key}` | CAN/UDP 소켓 I/O | 차량/통신 처리 |
| aeb-control | `aeb-control/src/Communicator.cpp::CANClass::*` | `common/src/private/aeb-control/Communicator_impl.cpp::CANClass::*` | CAN 소켓 입출력 | CAN 입출력 구현 |
| control-module | `control-module/src/ControlModule.cpp::main` | `common/src/private/control-module/ControlModule_impl.cpp::main` | GPS/경로/차량 입력, 제어 출력 | 제어 메인 루프 |
| control-module | `control-module/src/Controller.cpp::ControlLogic::{SCC,PurePursuit,GetCurvature,GetLateraldeviation,GetRelativeYawAngle}` | `common/src/private/control-module/Controller_impl.cpp::ControlLogic::{SCC,PurePursuit,GetCurvature,GetLateraldeviation,GetRelativeYawAngle}` | 차량/경로 입력, 조향/가감속 출력 | 종/횡방향 제어 계산 |
| control-module | `control-module/src/PathManager.cpp::PathConvert::*` | `common/src/private/control-module/PathManager_impl.cpp::PathConvert::*` | 글로벌 경로/GPS 입력, 로컬 경로 출력 | 경로 변환/밀도 계산 |
| control-module | `control-module/src/Communicator.cpp::{VehicleReceiver,IbeoReceiver,GPSParser,PathReceiver,MCUSender,ViewerSender}` | `common/src/private/control-module/Communicator_impl.cpp::{VehicleReceiver,IbeoReceiver,GPSParser,PathReceiver,MCUSender,ViewerSender}` | CAN/UDP/TCP I/O | 센서/차량 데이터 변환 및 송수신 |
| control-module | `control-module/src/Communicator.cpp::CANClass::*` | `common/src/private/control-module/Communicator_impl.cpp::CANClass::*` | CAN 소켓 입출력 | CAN 프레임 처리 |
| decision-module | `decision-module/source_kcity/MainModule.cpp::main` | `common/src/private/decision-module/MainModule_impl.cpp::main` | 센서/경로 입력, 의사결정 출력 | K-City 메인 루프 |
| decision-module | `decision-module/source_kcity/PathManager.cpp::PathConverter::{GenerateLocalPath,PedestrianDistance,IbeoPedestrianDistance,PathHeadingAngle}` | `common/src/private/decision-module/PathManager_impl.cpp::PathConverter::{GenerateLocalPath,PedestrianDistance,IbeoPedestrianDistance,PathHeadingAngle}` | GPS/객체/경로 입력, 거리/판단 값 출력 | 객체-경로 거리 및 경로 판단 |
| decision-module | `decision-module/source_kcity/Communicator.cpp::{VehicleReceiver,MobileyeReceiver,IbeoReceiver,RadarReceiver,GPSParser,PathReceiver,MCUSender,ViewerSender}` | `common/src/private/decision-module/Communicator_impl.cpp::{VehicleReceiver,MobileyeReceiver,IbeoReceiver,RadarReceiver,GPSParser,PathReceiver,MCUSender,ViewerSender}` | CAN/UDP/TCP I/O | 센서 융합 데이터 파싱/송신 |
| decision-module | `decision-module/source_gateway/CANGateway.cpp::main` | `common/src/private/decision-module/CANGateway_impl.cpp::main` | 게이트웨이 CAN/UDP 입력, 차량 CAN 출력 | 게이트웨이 메인 루프 |
| decision-module | `decision-module/source_gateway/CANGateway.cpp::{CCANtoMDPS,MDPStoCCAN,ECANtoDRV,DRVtoECAN}` | `common/src/private/decision-module/CANGateway_impl.cpp::{CCANtoMDPS,MDPStoCCAN,ECANtoDRV,DRVtoECAN}` | CAN 버스 입력/출력 | 게이트웨이 라우팅/보정 |
| decision-module | `decision-module/source_gateway/CANGateway.cpp::ComputeGatewayCrc` | `common/src/private/decision-module/CANGateway_impl.cpp::ComputeGatewayCrc` | 프레임 바이트 입력, CRC 출력 | 게이트웨이 CRC 계산 |
| decision-module | `decision-module/source_gateway/CRC.cpp::main` | `common/src/private/decision-module/CRC_impl.cpp::main` | 테스트 페이로드 입력, CRC 출력 | CRC 검증 실행 |

## 4. 변경 매핑

| Module | Public file | 이동한 로직 | Private impl file |
|---|---|---|---|
| aeb-control | `aeb-control/src/AEBControl_S32G_v2.cpp` | AEB 상태 전이, 경보/제동 CAN FD 프레임 생성, CRC 계산, CAN ID/마스크 상수 | `common/src/private/aeb-control/AEBControl_S32G_v2_impl.cpp` |
| aeb-control | `aeb-control/src/Communicator.cpp` | CAN/UDP 통신 처리, 차량 데이터 파싱 및 송수신 | `common/src/private/aeb-control/Communicator_impl.cpp` |
| control-module | `control-module/src/ControlModule.cpp` | 제어 루프 스레드 구성 및 실행 흐름 | `common/src/private/control-module/ControlModule_impl.cpp` |
| control-module | `control-module/src/Controller.cpp` | Pure Pursuit/SCC/곡률/편차/상대각 계산 로직 | `common/src/private/control-module/Controller_impl.cpp` |
| control-module | `control-module/src/PathManager.cpp` | 글로벌→로컬 경로 변환, waypoint 선택, 거리 연산 | `common/src/private/control-module/PathManager_impl.cpp` |
| control-module | `control-module/src/Communicator.cpp` | CAN ID/비트필드 해석, 센서/차량 데이터 변환, MCU/Viewer 송신 | `common/src/private/control-module/Communicator_impl.cpp` |
| decision-module | `decision-module/source_kcity/MainModule.cpp` | K-City 메인 루프와 의사결정 흐름 | `common/src/private/decision-module/MainModule_impl.cpp` |
| decision-module | `decision-module/source_kcity/PathManager.cpp` | 보행자/객체 거리 계산 및 경로 판단 로직 | `common/src/private/decision-module/PathManager_impl.cpp` |
| decision-module | `decision-module/source_kcity/Communicator.cpp` | 차량/레이더/라이다/모바일아이 파싱, 경로/MCU/Viewer 송수신 | `common/src/private/decision-module/Communicator_impl.cpp` |
| decision-module | `decision-module/source_gateway/CANGateway.cpp` | 게이트웨이 라우팅, 조향/제동 신호 보정, CRC 생성, CAN 상수 처리 | `common/src/private/decision-module/CANGateway_impl.cpp` |
| decision-module | `decision-module/source_gateway/CRC.cpp` | CRC 테스트 실행 로직 | `common/src/private/decision-module/CRC_impl.cpp` |
