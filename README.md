# KEUM_TAK_PROJECT
프로젝트 속성 - 구성 속성 - 디버깅 - 환경 <PATH=C:\Temp\opencv\build\x64\vc15\bin;%PATH%> 내용을 추가해 주어야 한다.
# -0224
UI 개선
1. 채팅 임시 구현
2. 이미지 사이즈 640 X 480 --> 320 X 240 으로 조정

NEXT - 다른 유저의 이미지를 받게 될 경우, 윈도우 위에 출력해야 하는데 dll(c++)과 wpf(c#) 간에 어떻게 주고받을지 알아내야함
#
# - 0218
통신간에 발생하는 문제점 개선
1. tcp 통신간에 버퍼가 끊기는 문제점 개선
   ( 데이터의 말단에  '\n'으로 정의하여 말단을 만나기 전까진 데이터를 수신했다고 생각하지 않도록 함)
2. 서버 측에선 아직 미구현 

NEXT - RECV Thread 구현
#
# - 0214
SEND THREAD 완벽구현
1. DLL 내에서 thread생성하여 send하는 기능 구현
2. IP와 PORT를 넘겨주어 thread내에서 socket을 생성하여 전송하도록 함.

NEXT - RECV thread 구현
#
# - 0213
dll을 thread로 구동
1. c++로 작성하여 dll로 만든 모듈을 thread로 구동함을 확인
2. 인자 넘겨주는 것 또한 정상적으로 동작됨을 확인

NEXT - send thread 구현 -> socket을 만들어서 인자로 넘겨줄지, socket생성 과정부터 thread에서 처리할지 결정해야함.
#
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