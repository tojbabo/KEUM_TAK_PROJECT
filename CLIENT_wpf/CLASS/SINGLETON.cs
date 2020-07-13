using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.CLASS
{
    public class SINGLETON
    {
        private static readonly Lazy<SINGLETON> _instance =
            new Lazy<SINGLETON>(() => new SINGLETON());

        public static SINGLETON instance
        {
            get
            {
                return _instance.Value;
            }
        }
        private SINGLETON() { }

        public int num = 5;


    }
}
