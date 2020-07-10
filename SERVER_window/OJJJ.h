#pragma once
#include<WinSock2.h>
#include <opencv2/opencv.hpp>
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
//#include <thread>
//#include <string.h>
#define FRAME_HEIGHT 320
#define FRAME_WIDTH 480
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

#define BUF_LEN 65540 // Larger than maximum UDP packet size
#define BUF_SIZE 1024

//#define IP "127.0.0.1"
#define IP "0.0.0.0"
//#define IP "220.149.128.87"
#define PORT 9000

#define MAXIMUM_USER 4

char* cutting_str(char* source, int strlen) {
	char temp[20];
	for (int i = 0; i < strlen; i++) {
		if (source[i] == '\n') {
			strncpy(temp, source, i);
			temp[std::strlen(temp)] = 0;
			return temp;
		}
	}

	return NULL;

}
char* get_value(char* source, int source_len, const char* target, int target_len) {
	char* temp = strstr(source, target);
	return cutting_str(temp,source_len) + target_len;
}
