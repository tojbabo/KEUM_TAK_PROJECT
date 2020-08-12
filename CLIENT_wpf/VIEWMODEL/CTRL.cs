using CLIENT_wpf.MODEL;
using CLIENT_wpf.VAL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.VIEWMODEL
{
    public static class CTRL
    {
        #region property
        public static _DATA DATA { get; private set; }
        public static void initProperty()
        {
            DATA = _DATA.CORE;
            DATA.MyState = new STATE();
            DATA.YourState = new STATE();
        }
        #endregion

        #region function
        public static void ConnectionStateChange(int num)
        {
            // 연결 안됐을떄
            if (num == CONSTANTS.STATE_DISCONNECT)
            {
                DATA.R = 1;
                DATA.Y = 2;
                DATA.G = 2;
                DATA.IsConnect = false;
            }
            // 연결 중
            else if (num == CONSTANTS.STATE_TRYCONNECT)
            {
                DATA.R = 2;
                DATA.Y = 1;
                DATA.G = 2;

            }
            // 연결 됨
            else if (num == CONSTANTS.STATE_CONNECTING)
            {
                DATA.R = 2;
                DATA.Y = 2;
                DATA.G = 1;
                DATA.IsConnect = true;
            }

        }

        public static void ConnectionStateChecking(int num, STATE s)
        {
            // 연결 안됐을떄
            if (num == CONSTANTS.STATE_BAD)
            {
                s.State = CONSTANTS.STATE_BAD;
            }
            // 연결 중
            else if (num == CONSTANTS.STATE_SOSO)
            {
                s.State = CONSTANTS.STATE_SOSO;

            }
            // 연결 됨
            else if (num == CONSTANTS.STATE_GOOD)
            {
                s.State = CONSTANTS.STATE_GOOD;
            }
            else if (num == CONSTANTS.STATE_DISCONNECT)
            {
                s.Connection = false;
            }
            else if(num == CONSTANTS.STATE_CONNECTING)
            {
                s.Connection = true;
                s.State = CONSTANTS.STATE_BAD;
            }

        }

        #endregion
    }
}
