using CLIENT_wpf.VAL;

namespace CLIENT_wpf.MODEL
{
    public class STATE : _MOTHER
    {
        private bool _Connection = false;
        public bool Connection
        {
            get => _Connection;
            set
            {
                _Connection = value;
                OnPropertyChanged(nameof(Connection));
            }
        }

        private int _State = CONSTANTS.STATE_BAD;
        public int State
        {
            get => _State;
            set
            {
                _State = value;
                OnPropertyChanged(nameof(State));
            }
        }

        private int _Emo = -1;
        public int Emo
        {
            get => _Emo;
            set
            {
                _Emo = value;
                OnPropertyChanged(nameof(Emo));
            }
        }

    }

}
