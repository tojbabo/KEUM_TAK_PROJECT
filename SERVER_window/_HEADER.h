#pragma once

#ifndef OJJJ
#define OJJJ

#define VERSION "1.3.0"
#define COMMENT "����"

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

// ^ : Ŭ���̾�Ʈ�� �̸� ����
// $ : ���ο� ���ӿ� ���� ��ȣ - ���ο� ���� / ���� ����� Ŭ���̾�Ʈ�� ���� ��û
// # : �ڽ��� ���̵�, ��Ʈ�� �ο��ϴ� ��ȣ
// <%s> inet_ntoa(adr.sin_addr) - IP�ּ� , <%d> ntohs(adr.sin_port) - PORT ��ȣ
#endif