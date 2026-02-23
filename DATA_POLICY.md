# DATA POLICY

## Data Classification
- Public: 구조 문서, 실행 가이드, 빌드 진입점
- Internal: 실험 설정값, 테스트 시나리오 상세
- Restricted: 주행 로그 원본, 위치/센서 원시 데이터, 인증정보, 소유권 미확인 자산

## Prohibited for Public Push
- 개인/장비 식별 가능 데이터
- 위치 추적 가능 주행 로그 원본
- 인증정보(토큰, 키, 계정정보)
- 소유권 미확인 회사 코드/산출물

## Repository Rules
- 로그/아티팩트/출력 바이너리는 기본 Git 추적 제외
- 민감 항목은 `archive/private-local/`로 격리
- 보관 파일 `archive/umsd-package/UMSD.tar`는 배포/분석 대상이 아님
- `projects/adaptive-autosar/**/ara-api`, `projects/adaptive-autosar/**/yocto-layers`는 자리표시자 README만 공개
- `projects/autonomous-driving-stack/modules/common/src/private/**`는 비공개 구현으로 유지하고 Git 추적 제외
- NATS 연결 설정은 공개본에서 샘플 값(`CHANGE_ME_*`)만 사용
- Adaptive AUTOSAR SoC 자산은 `projects/adaptive-autosar/{axon,s32g}` 하위에서만 관리

## Pre-Publish Checklist
1. `.gitignore` 규칙 적용 상태 확인
2. `git status`에서 민감 파일 미추적 여부 확인
3. 라이선스/소유권 검토 완료 확인
4. README/PROJECTS 링크 경로 실존 여부 확인
