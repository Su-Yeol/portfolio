# tsn-communication-stack

gPTP 시험 시나리오 스크립트와 설정 파일을 섹션별로 관리하는 TSN 검증 프로젝트.

## 구조
- `src/gptp/conf/`: 섹션별 `.ini` 구성
- `src/gptp/sh/`: 섹션별 실행 `.sh` 스크립트
- `tools/get_phc10us`, `tools/get_phc100us`: PHC 확인 유틸리티

## 실행 엔트리
- 예시 스크립트: `src/gptp/sh/Section_1/1_Common_AED_Tests/06_01_01_A.sh`
- 대응 설정: `src/gptp/conf/Section_1/1_Common_AED_Tests/06_01_01_A.ini`

## 최소 재현 체크
```bash
bash -n /home/suyeol/KATECH/projects/tsn-communication-stack/src/gptp/sh/Section_1/1_Common_AED_Tests/06_01_01_A.sh
```

## 전제조건
- `bash`
