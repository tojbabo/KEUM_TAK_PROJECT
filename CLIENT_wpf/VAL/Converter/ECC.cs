using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace CLIENT_wpf.VAL.Converter
{
    public class ECC : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var val = (int)value;

            switch (val)
            {
                case 0:
                    return "없음";
                case 1:
                    return "강퇴";
                case 2:
                    return "필터링";
                case 3:
                    return "블랙";
                case 4:
                    return "스티커";
                case 5:
                    return "";
                case 6:
                    return "";
                case 7:
                    return "붉은 얼굴";
                case 8:
                    return "푸른 얼굴";
                case 9:
                    return "빅 헤드";
                case 10:
                    return "모자이크";
                case 11:
                    return "사륜안";
                case 12:
                    return "180도 회전";
                case 13:
                    return "빅 아이";
                case 14:
                    return "ㅃㅏ직";
                case 15:
                    return "눙물";
                case 16:
                    return "sweet guy";
                case 19:
                    break;
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
