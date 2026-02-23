# Control/lib/common

`Control/lib/common`은 Control 모듈의 CAN 사양/신호 디코딩 공통 계층이다.
구조는 `autonomous-driving-stack/modules/common`과 동일한 형태로 정리했다.

## 구조
- `config/can_spec.ini`: 런타임 CAN 사양 설정
- `include/CanSpecProvider.h`: CAN spec provider 인터페이스
- `include/SignalCodec.h`: 비트/바이트 디코딩 유틸리티
- `src/CanSpecConfigLoader.cpp`: INI 로더 구현
- `src/private/README.md`: 로컬 전용 확장 위치

## 로딩 우선순위
1. 환경변수 `KATECH_CONTROL_CAN_SPEC`
2. `./lib/common/config/can_spec.ini`
3. `../lib/common/config/can_spec.ini`
4. `./Control/lib/common/config/can_spec.ini`
