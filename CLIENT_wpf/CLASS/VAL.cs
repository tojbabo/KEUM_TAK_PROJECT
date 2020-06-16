using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.CLASS
{
    public class VAL
    {
        public const int CONNECT = 1;
        public const int TCP = 1;
        public const int UDP = 2;
        public const int BUF_SZ = 1024;
        public const int BUF_LEN = 65540;
        public const int PACK_SIZE = 4096;
        public const int ENCODE_QUALITY = 80;
        public const int frameWidth = 320;
        public const int frameHeight = 240;
        public const double FRAME_INTERVAL = (1000 / 24);

        public String SERV_IP = "192.168.63.155";
        public int SERV_PORT = 9000;
    }
}
