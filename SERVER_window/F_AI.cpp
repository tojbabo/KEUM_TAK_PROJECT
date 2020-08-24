#include "_AI.h"

using namespace std;

string ChatFilter(int StringLength) {
	string result;

	srand((unsigned int)time(0));
	int num;
	for (int i = 0; i < StringLength; i++) {
		num = rand();
		switch (num%6)
		{
		case 0:
			result.append("@");
			break;
		case 1:
			result.append("#");
			break;
		case 2:
			result.append("$");
			break;
		case 3:
			result.append("%");
			break;
		case 4:
			result.append("^");
			break;
		case 5:
			result.append("&");
			break;
		default:
			break;
		}
	}
	return result;
}

