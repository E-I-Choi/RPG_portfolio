![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.3-black?logo=unrealengine) ![C++](https://img.shields.io/badge/C++-00599C?logo=cplusplus&logoColor=white) ![PlayFab](https://img.shields.io/badge/PlayFab-blue)
이력서 포트폴리오를 위한 리포지토리입니다.

소개 영상 url
https://www.youtube.com/watch?v=VFLvDHEaI58

안녕하세요 개발자 최은일입니다.
이번 프로젝트는 BP_Only 솔로플레이 프로젝트 경험밖에 없는 제가 멀티플레이, c++ 프로젝트 실무 역량을 갖추기 위해 진행한 프로젝트입니다.
전형적인 한국형 MMORPG를 상정하고 제작하였으며 TopDown 시점의 c++/BP 멀티플레이 프로젝트입니다.
DB는 PlayFab을 활용하였고, 서버는 배치파일을 활용해 dedicated 로비·월드를 각각 실행하였습니다.
다만 DB를 활용하는 과정에서 LogIn과 Load까지는 성공하였으나 SDK 호환 문제로 Grant가 실패해 그 이상으로 진행하지 못했습니다.
현재 로비에서 캐릭터 생성하기 직전까지만 구현이 완료된 상태입니다.

이 프로젝트의 핵심은 c++로 설계한 MMORPG 시스템의 뼈대입니다.
첫째로, 자료 구조 최적화를 위해 커스터마이징한 열거형과 구조체와 클래스.
둘째로, 데이터의 무결성이 중요한 MMORPG 특성을 고려한 중앙통제형 네트워크 통신.
특히 중앙통제형 통신 체제에서 주목할 점은 네트워크 통신은 항상 아래 다섯 단계를 통해 이루어지며 허가된 통신을 시작하는 주체는 항상 서버라는 점입니다.

1. 클라이언트 : 데이터 업데이트 요청
2. 치트 검사
3. 임시 런타임 디팟에 Dirty Data 저장
4. 서버 : 클라이언트 함수 호출 ->
5. 클라이언트 : 서버 함수 호출 - >
6. 서버 : 네트워크 통신(DB에 동기화)
