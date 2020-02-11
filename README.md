# KEUM_TAK_PROJECT
프로젝트 속성 - 구성 속성 - 디버깅 - 환경 <PATH=C:\Temp\opencv\build\x64\vc15\bin;%PATH%> 내용을 추가해 주어야 한다.
# - 0211
WPF 환경에서 dll 연동
1. sscanf의 부재점을 단순히 인덱싱 하여 substring하는 방식으로 해결
   sprintf는 그냥 문자열 합치기로 하면 될 것 같음.
2. 스레드 생성까지 무리 없이 해결 - 인자 넘겨주는 것은 아직 못함
3. 촬영한 영상을 전송하기 위해 처리 과정이 필요(OPENCV),
   but, C# 자체가 문법이 달라서 진행에 무리가 존재 --> C++ dll을 만들어서 참조하기로 결정
   dll을 만들어서 참조는 성공. (프로젝트\bin\release / debug에 dll 파일 추가)

NEXT - 이미지 처리하는 함수를 dll에 담아야 함. 
		이미지를 주고 socket에 보낼 수 있도록 변환시킨 데이터를 리턴할지, 혹은
		socket까지 넘겨주어서 전송시킬 수 있도록 할 지 결정해야함.
#
# - 0210
WPF 환경 구축 - MFC에서 바꿀 예정 
1. 기본적인 TCP 연결 확인
2. opencv 이미지 촬영 상태 확인

NEXT - 기본적인 문법자체가 조금 달라서 수정 필요 (sscanf / sprintf 등)
		서버 쪽과 통신에 있어서 제어 메시지를 조금 수정해야함
#
# - 0114
MFC 환경 구축 (Opencv / WInsock 등)
1. 기본적인 TCP 연결 상태 확인
2. opencv 이미지 촬영 상태 확인
3. udp 영상 통신 확인 전송은 잘 됨
#