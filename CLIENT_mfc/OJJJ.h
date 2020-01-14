#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#define WIDTH 320
#define HEIGHT 240

#define IP "127.0.0.1"
#define PORT 9000

#define BUF_SIZE 1024

#define FRAME_HEIGHT 720
#define FRAME_WIDTH 1280
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 
#define ENCODE_QUALITY 80

using namespace std;