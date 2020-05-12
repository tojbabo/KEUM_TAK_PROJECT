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
using System.Reflection;
using CLIENT_wpf.CLASS;
using CLIENT_wpf.WINDOWS;
using CLIENT_wpf.FUNC;
using System.Collections.Specialized;

namespace CLIENT_wpf
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class ChattingWindow : System.Windows.Window
    {
        public DataGetEventHandler DataSendEvent;

        bool isSend = true;
        bool isNew = true;

        int ID;
        int PORT;

        VideoCapture cap;
        WriteableBitmap wb;

        Socket sock = null;

        Thread T_img_send;
        Thread T_img_recv;
        Thread T_msg_recv;

       

        VAL val = new VAL();

        /* */
        // 최초 실행되는 함수
        public ChattingWindow(String port)
        {
            InitializeComponent();
            int.TryParse(port, out PORT);
            TBX_PORT.Text = port;

        }
        // 최초 실행되는 커스텀 함수 - 아직 기능 미구현
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("0427 - ver1.4");
            DLL.Hi();
            

            int num = DLL.dll_Get_Socket(val.SERV_IP, PORT, 1);

        }

        public void Recv_From_Parent(string param)
        {
            Console.WriteLine("Set Action value called");
        }

        // 최초 카메라 셋팅
        private bool InitWebCamera()
        {
            try
            {
                cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
                cap.FrameWidth = VAL.frameWidth;
                cap.FrameHeight = VAL.frameHeight;
                cap.Open(0);
                wb = new WriteableBitmap(cap.FrameWidth, cap.FrameHeight, 96, 96, PixelFormats.Bgr24, null);

                image.Source = wb;
                image2.Source = wb;

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

            UTILITY.Release_thread(T_img_recv);
            UTILITY.Release_thread(T_img_send);
            UTILITY.Release_thread(T_msg_recv);
        }

        // SHOW 버튼 클릭시 (테스트용)
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            
            
            
            if (isSend == false) { isSend = true; button.Content = "YES SHOW"; }
            else { isSend = false; button.Content = "NO SHOW"; }



        }
        private void Button_Click_change(object sender, RoutedEventArgs e)
        {
            if (isNew) { isNew = false; BTN_CHANGE.Content = "OLD"; }
            else { isNew = true; BTN_CHANGE.Content = "NEW"; }
        }
        private void Button_Click_test(object sender,RoutedEventArgs e)
        {

            byte[] buf = new byte[VAL.BUF_SZ];

            String data = "$" + ID.ToString();
            buf = Encoding.ASCII.GetBytes(data);
            sock.Send(buf);

        }
        private void BTN_EXIT_Click(object sender, RoutedEventArgs e)
        {/*
            StartWindow SW = new StartWindow();
            System.Windows.Window.GetWindow(this).Close();
            SW.Show();*/


            /**/
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
            CAPTURE_IMG();*/

            Console.WriteLine("ChattingWindow -> StartWindow");

            DataSendEvent("");

            System.Windows.Window.GetWindow(this).Close();

        }

        // CONNECT 버튼 클릭시
        private void CLICK_CONNECT(object sender, RoutedEventArgs e)
        {
            val.SERV_IP = TBX_IP.Text;
            int.TryParse(TBX_PORT.Text, out val.SERV_PORT);

            sock = CREATE_SOCKET(val.SERV_IP, val.SERV_PORT, VAL.TCP, VAL.CONNECT);
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
            byte[] buf = new byte[VAL.BUF_SZ];

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
                    //Cv2.ImShow("2", mat);

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
            if (type == VAL.TCP)
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
            else if (type == VAL.UDP)
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
            byte[] buf = new byte[VAL.BUF_SZ];
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
                    int num = UTILITY.Token_Get_Index(data, "\n");                                                  // \n으로 메시지 구분
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
                ID = UTILITY.Token_Get_Num(token[0], ":");
                PORT = UTILITY.Token_Get_Num(token[1], ":");

                Console.WriteLine("ID : " + ID + "\nPORT :" + PORT);

                if (isSend)
                {
                    if (isNew)
                    {
                        int temp = DLL.dll_Get_Socket(val.SERV_IP, PORT, 1);

                        T_img_send = new Thread(() => THREAD_IMG_SEND(temp));
                        T_img_send.Start();
                    }
                    else
                    {
                        T_img_send = new Thread(() => DLL.dll_IMG_SEND_THREAD(val.SERV_IP, PORT));
                        T_img_send.Start();
                        //SEND 스레드
                    }
                }
            }
            else if (data[0] == '$')
            {
                Console.WriteLine("$ - 다른 유저의 아이디와 포트 할당받음 !!");
                var token = data.Split(',');
                int other_id = UTILITY.Token_Get_Num(token[0], ":");
                int other_port = UTILITY.Token_Get_Num(token[1], ":");
                byte[] buf = new byte[VAL.BUF_SZ];


                data = "$" + other_id.ToString();
                buf = Encoding.ASCII.GetBytes(data);
                sock.Send(buf);
                Console.WriteLine("send to server : " + data + "\n");
                //PORT 요청 작업

            }
            else if (data[0] == '@')
            {
                Console.WriteLine("@ - Thead를 생성할 PORT를 할당 받음 !!");
                int target_port = UTILITY.Token_Get_Num(data, ":");
                Console.WriteLine("New port is : " + target_port);

                if (isNew)
                {
                    int temp = DLL.dll_Get_Socket(val.SERV_IP, target_port, 1);
                    T_img_recv = new Thread(() => THREAD_IMG_RECV(temp));
                    T_img_recv.Start();
                }

                else
                {
                    //RECV 스레드
                    T_img_recv = new Thread(() => DLL.dll_IMG_RECV_THREAD(val.SERV_IP, target_port));
                    T_img_recv.Start();
                }
            }
        }
        // 서버로 메시지 보내는 함수
        private void BTN_MSG_SEND_Click(object sender, RoutedEventArgs e)
        {
            TBX_INPUT.Focus();
            byte[] buf = new byte[VAL.BUF_SZ];

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
        private void THREAD_IMG_SEND(int t_ImgSend_Sock)
        {
            int size;

            cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
            cap.FrameWidth = VAL.frameWidth;
            cap.FrameHeight = VAL.frameHeight;
            cap.Open(0);
            /**/
            Mat mat = new Mat();
            while (true)
            {
                cap.Read(mat);
                size = mat.Channels() * mat.Cols * mat.Rows;

                var b = new byte[size];

                System.Runtime.InteropServices.Marshal.Copy(mat.Data, b, 0, size);

                DLL.DLL_IMG_SEND(b, t_ImgSend_Sock);
                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {

                    WriteableBitmap wb = new WriteableBitmap(cap.FrameWidth, cap.FrameHeight, 96, 96, PixelFormats.Bgr24, null);
                    image.Source = wb;
                    WriteableBitmapConverter.ToWriteableBitmap(mat, wb);

                    //Cv2.ImShow("dfdf", mat);
                }));
                int c = Cv2.WaitKey(10);
                if (c != -1)
                    break;
            }
        }
        // 이미지 수신 함수 - UI에 출력 되는 버전
        private void THREAD_IMG_RECV(int t_ImgRecv_Sock)
        {
            Console.WriteLine("recv thread start!!");
            int num = DLL.DLL_SENDING(t_ImgRecv_Sock, "send trigger", "send trigger".Length);
            Mat mat;
            IntPtr ptr;

            while (true)
            {


                ptr = DLL.DLL_IMG_RECV(t_ImgRecv_Sock);
                if (ptr != null)
                {
                    mat = new Mat(ptr);
                    Console.WriteLine("mat cols : " + mat.Cols +"/ mat rows : " + mat.Rows);
                    //Cv2.ImShow("qweqwe", mat);

                    Dispatcher.BeginInvoke(DispatcherPriority.Normal, new Action(delegate
                    {
                        try
                        {
                            WriteableBitmap wb = new WriteableBitmap(mat.Cols, mat.Rows, 96, 96, PixelFormats.Bgr24, null);
                            image2.Source = wb;
                            WriteableBitmapConverter.ToWriteableBitmap(mat, wb);
                        }
                        catch (Exception e) { }
                    }));

                    int c = Cv2.WaitKey(10);
                    if (c != -1)
                        break;
                }
                else
                    Console.WriteLine("is null~~!");
            }
        }  
    }
}
  