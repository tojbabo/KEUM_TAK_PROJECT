using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Windows;

namespace CLIENT_wpf.VIEWMODEL
{
    public class _MOTHER : INotifyPropertyChanged
    {
        #region object life-cycle
        /*protected _MOTHER() { }
        private static readonly object instanceLock = new object();
        private _MOTHER instance;
        public  _MOTHER CORE
        {
            get
            {
                lock (instanceLock)
                {
                    if(instance == null)
                    {
                        instance = new _MOTHER();
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
            CORE = new _MOTHER();
        }*/
        #endregion

        #region property change
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyname)
        {
            Application.Current.Dispatcher.BeginInvoke((Action)(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyname));
            }));

        }
        #endregion
    }
}
