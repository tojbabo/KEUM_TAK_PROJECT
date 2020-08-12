
using CLIENT_wpf.VAL;
using CLIENT_wpf.VIEWMODEL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CLIENT_wpf.FUNC
{
    class UTILITY
    {

        public static void START()
        {
            Console.WriteLine("0807 - ver1.6 - ");
            DLL.Hi();
        }
        // 문자열에서 특정 문자 뒤의 숫자를 얻어오는 함수
        public static int Token_Get_Num(String Base, String target)
        {
            int num = Base.IndexOf(target) + target.Length;
            String sub_str = Base.Substring(num);
            int ber = sub_str.IndexOf("\n");

            if (ber != -1) sub_str = sub_str.Substring(0,ber);

            if (int.TryParse(sub_str, out num))
                return num;

            return -1;
        }

        public static string Token_Get_Str(String Base, String target)
        {
            int num = Base.IndexOf(target) + target.Length;
            String sub_str = Base.Substring(num);
            int ber = sub_str.IndexOf("\n");

            if (ber != -1) sub_str = sub_str.Substring(0, ber-1);



            return sub_str;
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

            Console.WriteLine("스레드 강제 종료");
        }

        public static void Read_File()
        {
            string path = "config.txt";
            try
            {
                string File_string = System.IO.File.ReadAllText(path);
                CTRL.DATA.PORT = Token_Get_Num(File_string, "port:");
                CTRL.DATA.IP = Token_Get_Str(File_string, "ip:");
                CTRL.DATA.Fps = Token_Get_Num(File_string, "fps:");
                CTRL.DATA.NAME = Token_Get_Str(File_string, "name:");
                CTRL.DATA.AutoConnect = (Token_Get_Num(File_string, "auto_connect:") == 1) ? true : false;

                Console.WriteLine($"----------------- setting [{path}] -----------------");
                Console.WriteLine($"--- user name     is : {CTRL.DATA.NAME}");
                Console.WriteLine($"--- server ip     is : {CTRL.DATA.IP}");
                Console.WriteLine($"--- server port   is : {CTRL.DATA.PORT}");
                Console.WriteLine($"--- frame per sec is : {CTRL.DATA.Fps}");
                Console.WriteLine($"--- auto connect  is : {CTRL.DATA.AutoConnect}");
                Console.WriteLine("--------------------------------------------------------");
            }
            catch (Exception e)
            {
                Console.WriteLine("파일 읽기 실패. config.txt 파일 필요. 기본 정보 대체 ");

                Console.WriteLine($"----------------- setting  [기본정보] -----------------");
                Console.WriteLine($"--- user name     is : {CTRL.DATA.NAME}");
                Console.WriteLine($"--- server ip     is : {CTRL.DATA.IP}");
                Console.WriteLine($"--- server port   is : {CTRL.DATA.PORT}");
                Console.WriteLine($"--- frame per sec is : {CTRL.DATA.Fps}");
                Console.WriteLine($"--- auto connect  is : {CTRL.DATA.AutoConnect}");
                Console.WriteLine("--------------------------------------------------------");
            }

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
