# Remote Control Module

`remote-control`은 UDP 기반 원격 제어 데이터 수신/브리징 테스트에 사용하는 유틸리티 모듈이다.

## 디렉터리
- `include/remote.h`: CAN/UDP 래퍼 클래스 선언
- `src/remoteS32G.cpp`: S32G 측 수신 루프
- `src/remotePC.cpp`: PC 측 수신 루프
- `output/`: 빌드 바이너리 저장 경로

## 컴파일 예시

```bash
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/remote-control
mkdir -p output
g++ -std=c++17 -g src/remoteS32G.cpp -Iinclude -o output/remoteS32G
g++ -std=c++17 -g src/remotePC.cpp -Iinclude -o output/remotePC
```

## 실행
- `output/remoteS32G`
- `output/remotePC`

실행 전 IP/포트는 소스 상수 또는 실행 환경에 맞게 조정한다.
