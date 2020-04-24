using System;
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
using System.Drawing;
using System.IO;

namespace CLIENT_wpf
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : System.Windows.Window
    {
        const int CONNECT = 1;
        const int TCP = 1;
        const int UDP = 2;
        const int BUF_SZ = 1024;
        const int BUF_LEN = 65540;
        const int PACK_SIZE = 4096;
        const int ENCODE_QUALITY = 80;
        const int frameWidth = 320;
        const int frameHeight = 240;



        String SERV_IP = "192.168.0.48";
        int SERV_PORT = 9000;

        int ID;
        int PORT;

        VideoCapture cap;
        WriteableBitmap wb;

        Socket sock = null;


        Thread T_img_send;
        Thread T_img_recv;
        Thread T_msg_recv;

        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void dll_IMG_SEND_THREAD(String serv_ip, int serv_port);                               // [old] 이미지 전송 스레드
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void dll_IMG_RECV_THREAD(String serv_ip, int serv_port);                               // [old] 이미지 수신 스레드
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void Hi();                                                                             // 버전 체크 및 DLL 연결 확인
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static int dll_Get_Socket(String serv_ip, int serv_port, int opt);                            // C++ 용 SOCKET 구하는 함수
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static int DLL_SENDING(int sock, String msg, int str_len);                                    // C++ 소켓으로 메시지 전송
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void DLL_IMG_SEND(byte[] b, int sock);                                                 // [new] 이미지 전송 스레드
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static byte[] DLL_IMG_RECV (int sock);                                                        // [new] 이미지 수신 스레드

        // 최초 실행되는 함수
        public MainWindow()
        {
            InitializeComponent();
        }
        // 최초 실행되는 커스텀 함수 - 아직 기능 미구현
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("0424 - ver1.2");
            Hi();

            int num = dll_Get_Socket(SERV_IP, PORT, 1);




        }
        // 문자열에서 특정 문자 뒤의 숫자를 얻어오는 함수
        private int Token_Get_Num(String Base, String target)
        {
            Console.WriteLine("BASE : " + Base);
            Console.WriteLine("Target : " + target);

            int num;
            num = Base.IndexOf(target);
            String sub_str = Base.Substring(num + 1);
            if (int.TryParse(sub_str, out num))
                return num;

            return -1;

        }
        // 문자열의 특정 문자의 위치를 얻어오는 함수
        private int Token_Get_Index(String Base, String target)
        {
            int index = Base.IndexOf(target);
            if (index != -1)
            {
                Console.WriteLine("BASE : " + Base);
                Console.WriteLine("Target : " + target);
                Console.WriteLine("Total lenth : " + Base.Length);
                Console.WriteLine("Index : " + index);
                Console.WriteLine("Left : " + Base.Substring(0, index));
                Console.WriteLine("Right : " + Base.Substring(index));
            }
            return index;
        }
        // 최초 카메라 셋팅
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
        // 종료 시 처리할 작업들
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {


            if (cap != null && cap.IsOpened())
            {
                cap.Dispose();
            }

            Release_thread(T_img_recv);
            Release_thread(T_img_send);
            Release_thread(T_msg_recv);
        }
        private void Release_thread(Thread t)
        {
            if (t == null)
                return;
            t.Interrupt();
            t.Abort();
        }
        // SHOW 버튼 클릭시 (테스트용)
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            /*
            T_img_send = new Thread(() => TH());
            T_img_send.Start();
            /*
            if (!InitWebCamera())
            {
                //MessageBox.Show("CAM OPEN FALSE");
            }
            else
            {
                //MessageBox.Show("CAM OPEN OK");
            }

            //Cv2.NamedWindow("1", WindowMode.AutoSize);
            DispatcherTimer timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromTicks(10000000);
            timer.Tick += new EventHandler(timer_Tick);
            timer.Start();
            CAPTURE_IMG();*/


        }
        // 테스팅용
        private void TH()
        {
            Console.WriteLine("Thread start");

            WriteableBitmap wb;
            cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
            cap.FrameWidth = frameWidth;
            cap.FrameHeight = frameHeight;
            cap.Open(0);
            Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
            {
                wb = new WriteableBitmap(cap.FrameWidth, cap.FrameHeight, 96, 96, PixelFormats.Bgr24, null);
                image.Source = wb;


                Mat mat = new Mat();

                while (true)
                {
                    if (cap.Read(mat))
                    {
                        Cv2.ImShow("2", mat);

                        WriteableBitmapConverter.ToWriteableBitmap(mat, wb);

                    }
                    int c = Cv2.WaitKey(10);
                    if (c != -1)
                        break;
                }
            }));
            Console.WriteLine("Thread end");
        }
        // CONNECT 버튼 클릭시
        private void CLICK_CONNECT(object sender, RoutedEventArgs e)
        {
            SERV_IP = TBX_IP.Text;
            int.TryParse(TBX_PORT.Text, out SERV_PORT);

            sock = CREATE_SOCKET(SERV_IP, SERV_PORT, TCP, CONNECT);
            Console.WriteLine(sock);

            if (sock != null)
            {
                //MessageBox.Show("CONNECT FALSE");
            }
            else
            {
                //MessageBox.Show("CONNECT OK");
            }
            String name = "^" + TBX_NAME.Text + "\n";
            byte[] buf = new byte[BUF_SZ];

            buf = Encoding.ASCII.GetBytes(name);
            sock.Send(buf);
            Console.WriteLine("send name data");

            T_msg_recv = new Thread(THREAD_MSG_RECV);
            T_msg_recv.Start();

        }
        // 윈도우 상에 이미지 출력 관련 함수
        private void CAPTURE_IMG()
        {
            Mat mat = new Mat();
            OpenCvSharp.Size size = new OpenCvSharp.Size(640, 480);
            Mat dst = new Mat();

            while (true)
            {
                if (cap.Read(mat))
                {


                    Cv2.Resize(mat, dst, size);

                    var b = new byte[mat.Channels() * mat.Cols * mat.Rows];
                    System.Runtime.InteropServices.Marshal.Copy(mat.Data, b, 0, mat.Channels() * mat.Cols * mat.Rows);




                    //testhree(b);
                    Cv2.ImShow("2", mat);

                    WriteableBitmapConverter.ToWriteableBitmap(mat, wb);

                }

                int c = Cv2.WaitKey(10);
                if (c != -1)
                    break;

            }
        }
        // 소켓을 생성하는 함수 
        private Socket CREATE_SOCKET(String ip, int port, int type, int opt)
        {
            // type : TCP(1) / UDP(2)
            // opt : 1 - 연결 UDP / 그 외: 일반 UDP
            Socket temp_sock;
            if (type == TCP)
            {
                temp_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                try
                {
                    temp_sock.Connect(ip, port);
                    Console.WriteLine("TCP 연결 성공");
                    return temp_sock;
                }
                catch
                {
                    Console.WriteLine("TCP 연결 실패");
                    return null;
                }

            }
            else if (type == UDP)
            {
                temp_sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                if (opt == 1)
                {
                    try
                    {
                        sock.Connect(ip, port);
                        Console.WriteLine("UDP 연결 성공");
                        return temp_sock;
                    }
                    catch
                    {
                        Console.WriteLine("UDP 연결 실패");
                        return null;
                    }
                }
                else
                {
                    Console.WriteLine("UDP 소켓 생성 성공");
                    return temp_sock;
                }
            }
            else
                Console.WriteLine("잘못된 소켓 생성");

            return null;
        }
        // 메시지 수신 쓰레드
        private void THREAD_MSG_RECV()
        {
            byte[] buf = new byte[BUF_SZ];
            int len;
            String data = "";
            String temp;
            while (true)
            {
                Console.WriteLine("메시지 수신중");
                len = sock.Receive(buf);
                temp = Encoding.UTF8.GetString(buf, 0, len);
                Console.WriteLine("서버로 부터 수신한 메시지 : " + temp);

                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                 {
                     TBX_MESSAGE.AppendText(temp);
                 }));
                data += temp;
                while (true)
                {
                    int num = Token_Get_Index(data, "\n");                                                  // \n으로 메시지 구분
                    if (num == -1)                                                                          // \n이 없는 경우 -> 메시지를 완전히 수신 안한 경우 다음 메시지에 추가 됨
                    {
                        break;
                    }
                    else if (num == data.Length - 1)                                                        // \n이 문자열 맨 마지막인 경우 -> 메시지 정상 수신
                    {
                        MSG_CHECKING(data);
                        data = "";
                        break;
                    }
                    else                                                                                    // \n이 문자열 사이에 존재 -> 메시지 동시에 수신
                    {
                        temp = data.Substring(0, num);                                                      // \n기준으로 왼쪽 : 현재 메시지로 판단
                        data = data.Substring(num + 1);                                                       // \n기준으로 오른쪽 : 다음 수신 메시지로 판단
                        MSG_CHECKING(temp);
                    }
                }
            }
        }
        // 수신한 메시지를 프로토콜에 맞게 확인 후 처리하는 함수
        private void MSG_CHECKING(String data)
        {
            if (data[0] == '#')
            {
                Console.WriteLine("# - 내 아이디 와 포트 할당받음 !!");
                var token = data.Split(',');
                ID = Token_Get_Num(token[0], ":");
                PORT = Token_Get_Num(token[1], ":");

                Console.WriteLine("ID : " + ID + "\nPORT :" + PORT);

                int temp = dll_Get_Socket(SERV_IP, PORT, 1);

                T_img_send = new Thread(() => THREAD_IMG_SEND(temp, SERV_IP, PORT));
                T_img_send.Start();


                //                T_img_send = new Thread(() => dll_IMG_SEND_THREAD(SERV_IP, PORT));
                //               T_img_send.Start();
                //SEND 스레드
            }
            else if (data[0] == '$')
            {
                Console.WriteLine("$ - 다른 유저의 아이디와 포트 할당받음 !!");
                var token = data.Split(',');
                int other_id = Token_Get_Num(token[0], ":");
                int other_port = Token_Get_Num(token[1], ":");
                byte[] buf = new byte[BUF_SZ];


                data = "$" + other_id.ToString();
                buf = Encoding.ASCII.GetBytes(data);
                sock.Send(buf);
                Console.WriteLine("send to server : " + data + "\n");
                //PORT 요청 작업

            }
            else if (data[0] == '@')
            {
                Console.WriteLine("@ - Thead를 생성할 PORT를 할당 받음 !!");
                int target_port = Token_Get_Num(data, ":");
                Console.WriteLine("New port is : " + target_port);

                int temp = dll_Get_Socket(SERV_IP, target_port, 1);
                T_img_recv = new Thread(() => THREAD_IMG_RECV(temp));
                T_img_recv.Start();
                
                
                
                //RECV 스레드
                //T_img_recv = new Thread(() => dll_IMG_RECV_THREAD(SERV_IP, target_port));
                //T_img_recv.Start();
            }
        }
        // 서버로 메시지 보내는 함수
        private void BTN_MSG_SEND_Click(object sender, RoutedEventArgs e)
        {
            TBX_INPUT.Focus();
            byte[] buf = new byte[BUF_SZ];

            buf = Encoding.ASCII.GetBytes(TBX_INPUT.Text + "\n");
            sock.Send(buf);
            TBX_INPUT.Text = "";

        }
        // 엔터로 메시지 전송
        private void TBX_INPUT_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                BTN_MSG_SEND_Click(this, e);
            }
        }

        // 이미지 전송 함수 - UI에 출력 되는 버전
        private void THREAD_IMG_SEND(int t_ImgSend_Sock, String ip, int port)
        {
           
            int size;

            cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
            cap.FrameWidth = frameWidth;
            cap.FrameHeight = frameHeight;
            cap.Open(0);
            WriteableBitmap wb;
            Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
            {
                wb = new WriteableBitmap(cap.FrameWidth, cap.FrameHeight, 96, 96, PixelFormats.Bgr24, null);
                image.Source = wb;

                Mat mat = new Mat();
                while (true)
                {
                    if (cap.Read(mat))
                    {
                        //Cv2.ImShow("1", mat);

                        size = mat.Channels() * mat.Cols * mat.Rows;
                        var b = new byte[size];
                        System.Runtime.InteropServices.Marshal.Copy(mat.Data, b, 0, size);

                        DLL_IMG_SEND(b,t_ImgSend_Sock);

                        WriteableBitmapConverter.ToWriteableBitmap(mat, wb);

                    }
                    int c = Cv2.WaitKey(10);
                    if (c != -1)
                        break;
                }
            }));
        }

        // 이미지 수신 함수 - UI에 출력 되는 버전
        private void THREAD_IMG_RECV(int t_ImgRecv_Sock)
        {
            Console.WriteLine("recv thread start!!");
            int num = DLL_SENDING(t_ImgRecv_Sock, "hi", "hi".Length);
            
            /*
            /// send trigger를 한번 보내야함
            int num = 1 ;
            Console.WriteLine("TIRGGER RESULT : " + num.ToString());
            int size;

            WriteableBitmap wb;
            Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
            {
                wb = new WriteableBitmap(frameWidth, frameHeight, 96, 96, PixelFormats.Bgr24, null);
                image2.Source = wb;

                Mat mat = new Mat();

                while (true)
                {
                    if (cap.Read(mat))
                    {
                        //Cv2.ImShow("1", mat);

                        size = mat.Channels() * mat.Cols * mat.Rows;
                        var b = new byte[size];
                        System.Runtime.InteropServices.Marshal.Copy(mat.Data, b, 0, size);

                        testhree(b, t_ImgSend_Sock);

                        WriteableBitmapConverter.ToWriteableBitmap(mat, wb);

                    }
                    int c = Cv2.WaitKey(10);
                    if (c != -1)
                        break;
                }
            }));*/
        }


    }
}

  