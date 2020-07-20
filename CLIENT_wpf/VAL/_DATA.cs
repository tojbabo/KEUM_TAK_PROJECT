using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.VIEWMODEL
{
    public class _DATA : _MOTHER
    {
        #region Life Cycle
        protected _DATA() { }
        private static readonly object instanceLock = new object();
        private static _DATA instance;
        public static _DATA CORE
        {
            get
            {
                lock (instanceLock)
                {
                    if (instance == null)
                    {
                        instance = new _DATA();
                    }
                    return instance;
                }
            }
            private set
            {
                lock (instanceLock)
                {
                    instance = value;
                }
            }
        }
        public void ResetModel()
        {
            CORE = new _DATA();
        }
        #endregion

        #region Property
        
        private string _IP = "127.0.0.1";
        public string IP {
            get => _IP;
            set
            {
                _IP = value;
                OnPropertyChanged(nameof(IP));
            }
        }

        private int _PORT = 9000;
        public int PORT
        {
            get => _PORT;
            set
            {
                _PORT = value;
                OnPropertyChanged(nameof(PORT));
            }
        }

        private int _fps=24;
        public int Fps
        {
            get => _fps;
            set
            {
                _fps = value;
                OnPropertyChanged(nameof(Fps));
            }
        }

        private bool _AutoConnect=false;
        public bool AutoConnect { 
            get => _AutoConnect;
            set
            {
                _AutoConnect = value;
                OnPropertyChanged(nameof(AutoConnect));


            }
        }

        #endregion
    }
}
