using CLIENT_wpf.VAL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

    }

}
