#pragma once

#include"_HEADER.h"

#define EVENT_DEFAULT -1		/* 없는 값 */
#define EVENT_KICK 1			/* 강제 퇴장 */
#define EVENT_CHATFILTER 2		/* 채팅 필터 */
#define EVENT_BLKOUT 3			/* 화면 안보임 */

std::string ChatFilter(int StringLength);
