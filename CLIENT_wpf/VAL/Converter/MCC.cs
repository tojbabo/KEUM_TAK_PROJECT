using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace CLIENT_wpf.VAL.Converter
{
    // Main Connection Converter : 주 연결 관리
    public class MCC : IValueConverter
        
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) 
        {
            var val = value as bool?;
            if (val != null)
            {
                bool flag = (bool)value;
                if (!flag) return "연  결  시  도";
                else return "연  결  해  제";
            }

            var val2 = value as int?;
            if (val2 == 1)
            {
                return 1;
            }
            else if(val2 == 2)
            {
                return 0.2;
            }


            throw new NotImplementedException();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
