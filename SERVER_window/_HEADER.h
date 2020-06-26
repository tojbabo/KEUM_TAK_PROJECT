#pragma once

#ifndef OJJJ
#define OJJJ

#define VERSION "1.3.0"
#define COMMENT "시작"

#include<WinSock2.h>
#include <opencv2/opencv.hpp>
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <thread>
#include <string.h>

#define FRAME_HEIGHT 240
#define FRAME_WIDTH 320
#define FRAME_INTERVAL (1000/24)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

#define BUF_LEN 65540 // Larger than maximum UDP packet size
#define BUF_SIZE 1024

//#define IP "127.0.0.1"
#define IP "0.0.0.0"
//#define IP "220.149.128.87"
//#define PORT 9000

#define MAXIMUM_USER 4

// ^ : 클라이언트의 이름 지정
// $ : 새로운 접속에 대한 신호 - 새로운 접속 / 새로 연결된 클라이언트에 대한 요청
// # : 자신의 아이디, 포트를 부여하는 신호
// <%s> inet_ntoa(adr.sin_addr) - IP주소 , <%d> ntohs(adr.sin_port) - PORT 번호
#endif