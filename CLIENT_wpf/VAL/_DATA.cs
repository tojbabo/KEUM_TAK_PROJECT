using CLIENT_wpf.MODEL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.VAL
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

        #region Property_Option
        
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


        private string _NAME = "USER";
        public string NAME
        {
            get => _NAME;
            set
            {
                _NAME = value;
                OnPropertyChanged(nameof(NAME));
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

        #region Property_Data
        private List<ROOM> _List;
        public List<ROOM> List
        {
            get => _List;
            set
            {
                _List = value;
                OnPropertyChanged(nameof(List));
            }
        }

        private string _Title = "DEBUG";
        public string Title {
            get => _Title;
            set
            {
                _Title = value;
                OnPropertyChanged(nameof(Title));

            }
        }

        private bool _IsConnect = false;
        public bool IsConnect
        {
            get => _IsConnect;
            set
            {
                _IsConnect = value;
                OnPropertyChanged(nameof(IsConnect));
            }
        }

        private int _R = 2;
        public int R
        {
            get => _R;
            set
            {
                _R = value;
                OnPropertyChanged(nameof(R));
            }
        }
        private int _Y = 2;
        public int Y
        {
            get => _Y;
            set
            {
                _Y = value;
                OnPropertyChanged(nameof(Y));
            }
        }
        private int _G = 2;
        public int G
        {
            get => _G;
            set
            {
                _G = value;
                OnPropertyChanged(nameof(G));
            }
        }

        private STATE _MyState;
        public STATE MyState
        {
            get => _MyState;
            set
            {
                _MyState = value;
                OnPropertyChanged(nameof(MyState));
            }
        }

        private STATE _YourState;
        public STATE YourState
        {
            get => _YourState;
            set
            {
                _YourState = value;
                OnPropertyChanged(nameof(YourState));
            }
        }

        private int _TestEmo;
        public int TestEmo
        {
            get => _TestEmo;
            set
            {
                _TestEmo = value;
                OnPropertyChanged(nameof(TestEmo));
            }
        }

        #endregion
    }
}
