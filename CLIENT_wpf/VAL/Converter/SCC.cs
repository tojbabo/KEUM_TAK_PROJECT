using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;

namespace CLIENT_wpf.VAL.Converter
{
    // Sub Connection Converter : 서브 연결 관리
    public class SCC : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var val = value as bool?;
            if (val != null)
            {
                bool flag = (bool)value;
                if (!flag) return Visibility.Hidden;
                else return Visibility.Visible;
            }

            var val2 = value as int?;
            if (val2 == CONSTANTS.STATE_BAD)
            {
                return new SolidColorBrush(Colors.Red);
            }
            else if (val2 == CONSTANTS.STATE_SOSO)
            {
                return new SolidColorBrush(Colors.Orange);
            }
            else if(val2 == CONSTANTS.STATE_GOOD)
            {
                return new SolidColorBrush(Colors.Green);
            }

            return null;


            throw new NotImplementedException();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
