using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.VAL
{
    public class CONSTANTS
    {
        public const int CONNECT = 1;
        public const int TCP = 0;
        public const int UDP = 1;
        public const int BUF_SZ = 1024;
        public const int BUF_LEN = 65540;
        public const int PACK_SIZE = 4096;
        public const int ENCODE_QUALITY = 80;
        public const int frameWidth = 320;
        public const int frameHeight = 240;

        public const int STATE_DISCONNECT = 1;
        public const int STATE_TRYCONNECT = 2;
        public const int STATE_CONNECTING = 3;
        public const int STATE_BAD = 4;
        public const int STATE_SOSO = 5;
        public const int STATE_GOOD = 6;

        public const int EVENT_BLKCOUT = 3;
        public const int EVENT_STICKER = 4;
        public const int EVENT_NONE = -1;



    }
}
