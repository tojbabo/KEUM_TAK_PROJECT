#pragma once

#include"_HEADER.h"

#define EVENT_DEFAULT -1		/* ���� �� */
#define EVENT_KICK 1			/* ���� ���� */
#define EVENT_CHATFILTER 2		/* ä�� ���� */
#define EVENT_BLKOUT 3			/* ȭ�� �Ⱥ��� */

std::string ChatFilter(int StringLength);
