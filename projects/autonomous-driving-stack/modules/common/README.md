# Common Module

`common`은 모든 모듈에서 공유하는 설정 로딩/CRC/신호 코덱 계층이다.

## 디렉터리
- `config/`: 공통 CAN 사양 파일(`can_spec.ini`)
- `include/`: `CanSpecProvider`, `CrcProvider`, `SignalCodec` 공개 인터페이스
- `include/modules/shared/`: 모듈 공통 헤더
- `src/`: 공통 구현
- `src/private/`: 비공개 구현 위치 안내

## 공통 구현 파일
- `src/ConfigParser.cpp`: 모듈 공용 설정 파서 구현(중복 제거)
- `src/CanSpecConfigLoader.cpp`: CAN 스펙 키-값 로더
- `src/CrcProviderPublic.cpp`, `src/CrcProviderPrivate.cpp`: CRC provider 구현

## 공통 헤더 파일
- `include/modules/shared/ControlModuleCommon.h`
- `include/modules/shared/ModuleInterfaceCommon.h`
- `include/modules/shared/communicator_cd.h`
- `include/modules/shared/config_parser.h`

## 설정 파일 우선순위
1. `AUTODRIVE_CAN_SPEC`
2. `modules/common/config/can_spec.ini`
