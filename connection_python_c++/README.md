
1. pyinstaller
python 실행파일 제작  
    
2. count_to_10_with_shared_memory.py  
  - shared memory 사용
  - C++ 프로그램이 memory 에 쓰는 내용을 trigger로 다른 동작 실해
  - next = 계속해서 다음 동작 / return = 초기화
  - count_to_10 : 실행파일
  
3. shared_memory_with_python_exe.cpp
  - shared memory 사용
  - python 실행 파일 실행 - 2개의 프로세스로 동작 ( 1. python / 2. c++ )
  - 읽는 내용에 따라 memory 에 쓰는 내용 변경
  
4. visual_opencv_test.cpp
  - c++ opencv test
