using CLIENT_wpf.CLASS;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CLIENT_wpf.FUNC
{
    class UTILITY
    {

        public static void START()
        {
            Console.WriteLine("0513 - ver1.5.1 - ");
            DLL.Hi();
        }
        // 문자열에서 특정 문자 뒤의 숫자를 얻어오는 함수
        public static int Token_Get_Num(String Base, String target)
        {
            /*
            Console.WriteLine("BASE : " + Base);
            Console.WriteLine("Target : " + target);*/

            int num;
            num = Base.IndexOf(target);
            String sub_str = Base.Substring(num + 1);
            if (int.TryParse(sub_str, out num))
                return num;

            return -1;

        }

        // 문자열의 특정 문자의 위치를 얻어오는 함수
        public static int Token_Get_Index(String Base, String target)
        {
            int index = Base.IndexOf(target);
            if (index != -1)
            {/*
                Console.WriteLine("BASE : " + Base);
                Console.WriteLine("Target : " + target);
                Console.WriteLine("Total lenth : " + Base.Length);
                Console.WriteLine("Index : " + index);
                Console.WriteLine("Left : " + Base.Substring(0, index));
                Console.WriteLine("Right : " + Base.Substring(index));*/
            }
            return index;
        }

        // 쓰레드 종료
        public static void Release_thread  (Thread t)
        {
            if (t == null)
                return;
                //Console.WriteLine("인터럽트 전");
            //t.Interrupt();
            t.Abort();
        }

        public static bool bool_change(bool data)
        {
            if (data)
                return false;
            else
                return true;
        }

    }
}
