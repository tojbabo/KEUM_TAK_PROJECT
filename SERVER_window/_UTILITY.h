#pragma once
#include"_HEADER.h"

SOCKET Initialize( int port, int opt_1_is_TCP_2_is_UDP);
void Memset_(SOCKADDR_IN* adr, const char* ip, int port);
void Error_(const char* err_msg);