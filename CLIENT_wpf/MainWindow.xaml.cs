﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using OpenCvSharp;
using OpenCvSharp.Extensions;
using System.Net.Sockets;
using System.Windows.Threading;
using System.Threading;

using System.Runtime.InteropServices;

namespace CLIENT_wpf
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : System.Windows.Window
    {
        const int BUF_SZ = 1024;
        const int BUF_LEN = 65540;
        const int PACK_SIZE = 4096;
        const int ENCODE_QUALITY = 80;
        const int frameWidth = 640;
        const int frameHeight = 480;
        const String SERV_IP = "127.0.0.1";
        const int SERV_PORT = 9000;

        int ID;
        int PORT;

        VideoCapture cap;
        WriteableBitmap wb;

        Socket sock = null;

        bool loop = false;

        Thread t;
        Thread T_msg_recv;
        bool t_loop = true;

        [DllImport("test.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void test();


        public MainWindow()
        {
            InitializeComponent();
            
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            /*
            String BASE = "#ABC:33,CDD:47";


            var parts = BASE.Split(',');
            Console.WriteLine("배열의 갯수 : "+parts.Length);
            Console.WriteLine(parts[0]+"\n"+parts[1] + "\n" + BASE.Substring(5));

            Console.WriteLine(Tokenized(parts[1],":") +"\n"+ Tokenized(parts[0], ":"));
           */

            test();
            
            /*
            t = new Thread(ok);
            t.Start();
            */
        }

        private int Tokenized(String BASE,String TARGET)
        {
            int index;
            index = BASE.IndexOf(TARGET);
            String sub_str = BASE.Substring(index+1);
            if (int.TryParse(sub_str, out index))
                return index;
            
            Console.WriteLine("Tokenize error - /Base : " + BASE +" /Sub string : "+sub_str+" /Index : "+index);
            return -1;
            
        }

        private bool InitWebCamera()
        {
            try
            {
                cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
                cap.FrameWidth = frameWidth;
                cap.FrameHeight = frameHeight;
                cap.Open(0);
                wb = new WriteableBitmap(cap.FrameWidth, cap.FrameHeight, 96, 96, PixelFormats.Bgr24, null);
                image.Source = wb;

                return true;
            }
            catch
            {
                return false;
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            loop = false;

            
            if (cap!=null&&cap.IsOpened())
            { 
                cap.Dispose();
            }

            Release_thread(t);
            Release_thread(T_msg_recv);   
        }
        private void Release_thread(Thread t)
        {
            if (t == null)
                return;
            t.Interrupt();
            t.Abort();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (!InitWebCamera())
            {
                ////MessageBox.Show("CAM OPEN FALSE");
            }
            else
            {
                //MessageBox.Show("CAM OPEN OK");
            }

            Mat frame = new Mat(); 
            //Cv2.NamedWindow("1", WindowMode.AutoSize);
            loop = true;
            while (loop)
            {
                if (cap.Read(frame))
                {
                    //Cv2.ImShow("1", frame);
                    WriteableBitmapConverter.ToWriteableBitmap(frame, wb);
                    image.Source = wb;
                }

                int c = Cv2.WaitKey(10);
                if (c != -1)
                    break;

            }
        }

        private void CLICK_CONNECT(object sender, RoutedEventArgs e)
        {
            Boolean IS;

            IS = CREATE_SOCKET();
            if (!IS)
            {
                //MessageBox.Show("CONNECT FALSE");
            }
            else
                //MessageBox.Show("CONNECT OK");

            T_msg_recv = new Thread(THREAD_MSG_RECV);
            T_msg_recv.Start();

        }

        private bool CREATE_SOCKET()
        {
            sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
            try
            {
                sock.Connect(SERV_IP, SERV_PORT);
                Console.WriteLine("최초 연결 성공");
                return true;
            }
            catch
            {
                Console.WriteLine("최초 연결 실패");
                return false;
            }
        }

        private void ok()
        {
            for (int i = 0;t_loop ; i++)
            {
                Thread.Sleep(TimeSpan.FromSeconds(1));
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        int k = int.Parse(tx.Text);
                        k++;
                        tx.Text = k.ToString();
                    });
            }

            t_loop = true;
        }

        private void THREAD_MSG_RECV()
        {
            byte[] buf = new byte[BUF_SZ];
            String data;
            int len;
            while (true)
            {
                Console.WriteLine("서버로 부터 기다리는 중");
                len = sock.Receive(buf);
                data = Encoding.UTF8.GetString(buf, 0, len);
                Console.WriteLine(data);
                if (data[0] == '#')
                {
                    Console.WriteLine("# - 내 아이디 와 포트 할당받음 !!");
                    var token = data.Split(',');
                    ID = Tokenized(token[0], ":");
                    PORT = Tokenized(token[1], ":");

                    Console.WriteLine("ID : " + ID + "\nPORT :" + PORT);
                    /*
                    data = "Hi i'm c# Client";
                    buf = Encoding.ASCII.GetBytes(data);
                    len = sock.Send(buf);
                    */
                    //SEND 스레드
                }
                else if (data[0] == '$')
                {
                    Console.WriteLine("$ - 다른 유저의 아이디와 포트 할당받음 !!");
                    var token = data.Split(',');
                    int other_id = Tokenized(token[0], ":");
                    int other_port = Tokenized(token[1], ":");
                    //PORT 요청 작업

                }
                else if (data[0] == '@')
                {
                    int target_port = Tokenized(data, ":");
                    //RECV 스레드
                }
                
                
            }
        }

        private void THREAD_SEND()
        {
            byte[] buf = new byte[BUF_SZ];
            String msg;
            int len;
            
            Socket send_sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            try
            {
                send_sock.Connect(SERV_IP, PORT);
            }
            catch
            {
                Console.WriteLine("연결 실패");
                return;
            }




        }
    }
}

