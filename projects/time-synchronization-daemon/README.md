# time-synchronization-daemon

TSN/gPTP 시간 동기화 자산을 `module` 기준으로 정리한 프로젝트.

## 구조
- `module/Open-AVB-asi-master`
  - 빌드 엔트리: `CMakeLists.txt`, `Makefile`
  - 문서: `docs/`
  - 실행 스크립트: `scripts/`
  - 에셋: `assets/`
- `module/open-avb-final`
  - 빌드 엔트리: `CMakeLists.txt`, `Makefile`, `Android.mk`
  - 문서: `docs/`
  - 실행 스크립트: `scripts/`
  - 에셋: `assets/`
- `module/gptp-master`
  - 빌드 엔트리: `CMakeLists.txt`
  - 설정: `config/gptp_cfg.ini`
  - 문서: `docs/`
  - 스크립트: `scripts/`
- `module/linuxptp`
  - 빌드 엔트리: `makefile`
  - 소스/헤더: 루트 유지
  - 문서: `docs/`, `docs/man/`
  - 빌드 산출물: `artifacts/obj`, `artifacts/dep`, `artifacts/bin`
- `artifacts/tar/`
  - 보관용 압축 자산

## 빠른 검증 예시
```bash
cd /home/suyeol/KATECH/projects/time-synchronization-daemon/module/gptp-master
cmake -S . -B /tmp/katech-gptp-config-check
```

## 전제조건
- `cmake` 3.x
- `gcc/g++`
