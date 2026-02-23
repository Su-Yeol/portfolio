# SECURITY POLICY

## Reporting a Vulnerability
- 공개 이슈에 민감 내용을 직접 게시하지 마십시오.
- 보안 이슈는 비공개 채널(이메일 또는 private issue)로 제보하십시오.
- 재현 절차, 영향 범위, 임시 완화책을 함께 제공해 주세요.

## Supported Scope
- 이 저장소는 포트폴리오 목적의 재구성 저장소입니다.
- 아래 항목은 보안 리뷰 대상에서 기본 제외됩니다.
  - 대용량 실험 로그
  - 회사/기관 소유 가능 바이너리 산출물
  - 보관 전용 archive 자산

## Security Controls in This Repo
- `.gitignore`로 로그/아티팩트/빌드 캐시/민감 파일 추적 차단
- `archive/private-local`에 인증서/키/토큰 관련 파일 격리
- `archive/umsd-package/UMSD.tar` 분석/실행 금지
- `projects/adaptive-autosar/**/ara-api`, `projects/adaptive-autosar/**/yocto-layers`는 자리표시자 README만 공개
- `projects/autonomous-driving-stack/modules/common/src/private/**`는 비공개 구현 경로로 유지하고 Git 추적에서 제외
- NATS 연결 정보는 공개본에서 `CHANGE_ME_*` 샘플 값으로 유지하고, 원본은 `archive/private-local/prepublish/`에 백업
- AXON/S32G 공개 경로는 `projects/adaptive-autosar/` 하위로 통합해 추적 범위를 명확화
- `projects/autonomous-driving-stack`는 `logs/`, `artifacts/`를 기본 비추적 경로로 운용

## Disclosure Policy
- 라이선스/소유권 검토 전까지 문서 중심 공개를 기본으로 적용합니다.
- 회사 소유 코드가 확인되면 즉시 private 전환 또는 공개 범위 축소를 수행합니다.
