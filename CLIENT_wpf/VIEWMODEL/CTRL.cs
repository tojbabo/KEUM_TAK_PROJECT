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
        public static _DATA DATA { get; private set; }
        public static void initProperty()
        {
            DATA = _DATA.CORE;
        }
    }
}
