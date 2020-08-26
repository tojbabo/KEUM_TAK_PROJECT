using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace CLIENT_wpf.VAL.Converter
{
    public class EMOCC : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var val = (int)value;

            switch (val)
            {
                case -1:
                    return "감정 인식 안됨";
                case 0:
                    return "화남";
                case 1:
                    return "역겨움";
                case 2:
                    return "공포";
                case 3:
                    return "행복";
                case 4:
                    return "슬픔";
                case 5:
                    return "놀람";
                case 6:
                    return "무표정";
                default:
                    return " - ";

            }
            throw new NotImplementedException();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
