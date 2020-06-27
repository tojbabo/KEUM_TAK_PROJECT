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
using System.Diagnostics;
using System.Windows.Markup;

namespace CLIENT_wpf
{

    public partial class ChattingWindow : System.Windows.Window
    {
        public DataGetEventHandler DataSendEvent;

        // 디버깅용 - 보낼지 말지
        bool isSend = true;
        // 디버깅용 - UI에 그릴지 말지
        bool isNew = true;
        // 디버기용 - 연결 체크 여부 확인
        bool isConnect = false;

        bool isRun = true;

        int ID;
        int PORT;

        int EMO_my;
        int EMO_you;

        string IP = "127.0.0.1";

        VideoCapture cap;
        WriteableBitmap wb;

        EMOTICON EMO;

        Socket sock = null;

        Thread T_img_send;
        Thread T_img_recv;
        Thread T_msg_recv;

        VAL val = new VAL();

        #region 윈도우 관련

        public ChattingWindow()
        {
            InitializeComponent();
            _Window.Title = "Debug";
            EMO = new EMOTICON();
            //forDEBUG.Visibility = Visibility.Collapsed;
        }

        public ChattingWindow(String ip,String port, String name="DEBUG")
        { 
            InitializeComponent();
            _Window.Title = name;
            int.TryParse(port, out PORT);
            TBX_PORT.Text = port; 
            TBX_IP.Text = ip;
            IP = ip;
            forDEBUG.Visibility = Visibility.Collapsed;
            EMO_my = -1;
            EMO_you = -1;
            EMO = new EMOTICON();

            Connect_to_Server();
        }

        // 종료 시 처리할 작업들
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _Window_Closing();   
        }
        public void _Window_Closing()
        {


            isRun = false;

            // Console.WriteLine("리시브 종료 시작");
            UTILITY.Release_thread(T_img_recv);
            //Console.WriteLine("리시브 종료");
            //UTILITY.Release_thread(T_img_send); 
            //Console.WriteLine("샌드 종료");
            UTILITY.Release_thread(T_msg_recv);
            //Console.WriteLine("메시지 종료");

            if (sock != null)
            {
                sock.Shutdown(SocketShutdown.Both);
                sock.Close();
                T_msg_recv.Join();
            }

            if (T_img_recv != null)
            {
                bool a = T_img_recv.IsAlive;
                Console.WriteLine("T_Img_recv is : " + a);
            }
            bool b = T_msg_recv.IsAlive;
            Console.WriteLine("T_msg_recv is : " + b);

            DataSendEvent("");
        }

        #endregion

        #region 통신 관련
        private bool Connect_to_Server()
        {
            if (isConnect)
            {
                sock.Close();
                _Window_Closing();

                BTN_CONNECT.Content = "CONNECT";
                UTILITY.bool_change(isConnect);
                return true;
            }
            else
            {
                try
                {
                    int.TryParse(TBX_PORT.Text, out val.SERV_PORT);


                    sock = PROTOCOL.CREATE_SOCKET(IP, val.SERV_PORT, VAL.TCP, VAL.CONNECT);
                    if (sock == null)
                    {
                        Console.WriteLine("msg recv 소켓 생성에 에러 발생");
                        return false;
                    }
                    String name = "^" + TBX_NAME.Text + "\n";
                    byte[] buf = new byte[VAL.BUF_SZ];

                    buf = Encoding.ASCII.GetBytes(name);
                    sock.Send(buf);

                    T_msg_recv = new Thread(THREAD_MSG_RECV);
                    T_msg_recv.Start();

                    BTN_CONNECT.Content = "DISCONNECT";
                    UTILITY.bool_change(isConnect);
                    return true;

                }
                catch (Exception e)
                {
                    Console.WriteLine("연결 처리 중 에러 발생");
                    return false;
                }
            }
            

        }

        public void Recv_From_Parent(string param)
        {
            Console.WriteLine("Set Action value called");
        }

        #endregion

        #region UI 이벤트
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
        {


            /* 
            StartWindow SW = new StartWindow();
            System.Windows.Window.GetWindow(this).Close();
            SW.Show();*/

            /*
            Console.WriteLine("ChattingWindow -> StartWindow");
            DataSendEvent("");
            System.Windows.Window.GetWindow(this).Close();*/


            // 서버로부터 내 얼굴 받아보기
            byte[] buf = new byte[VAL.BUF_SZ];

            String data = "$" + ID.ToString();
            buf = Encoding.ASCII.GetBytes(data);
            sock.Send(buf);

        }
        // CONNECT 버튼 클릭시
        private void CLICK_CONNECT(object sender, RoutedEventArgs e)
        {
            val.SERV_IP = TBX_IP.Text;
            Connect_to_Server();
        }

        // 서버로 메시지 보내는 함수
        private void BTN_MSG_SEND_Click(object sender, RoutedEventArgs e)
        {
            TBX_INPUT.Focus();
            byte[] buf = new byte[VAL.BUF_SZ];

            buf = Encoding.ASCII.GetBytes(TBX_INPUT.Text + "\n");
            sock.Send(buf);

            Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
            {
                TBX_INPUT.Text = "";
            }));

        }
        // 엔터로 메시지 전송
        private void TBX_INPUT_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                BTN_MSG_SEND_Click(this, e);
            }
        }
        #endregion

        #region 페이지 쓰레드
        // 메시지 수신 쓰레드
        private void THREAD_MSG_RECV()
        {
            byte[] buf = new byte[VAL.BUF_SZ];
            int len;
            String data = "";
            String temp;
            while (isRun)
            {
                len = sock.Receive(buf);
                temp = Encoding.UTF8.GetString(buf, 0, len);
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
            Console.WriteLine("메시지 스레드 종료");
        }
        // 수신한 메시지를 프로토콜에 맞게 확인 후 처리하는 함수
        private void MSG_CHECKING(String data)
        {
            Console.WriteLine("☆[ChattingWindow]"+ data);
            if (data[0] == '#')
            {
                // 내 아이디와 포트 할당
                var token = data.Split(',');
                ID = UTILITY.Token_Get_Num(token[0], ":");
                PORT = UTILITY.Token_Get_Num(token[1], ":");
                if (isSend)
                {
                    if (isNew)
                    {
                        try
                        {
                            Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                            {
                                int temp = DLL.dll_Get_Socket(IP, PORT, 1);
                                T_img_send = new Thread(() => THREAD_IMG_SEND(temp));
                                T_img_send.Start();
                            })); 
                        }
                        catch(Exception e)
                        {
                            Console.WriteLine("에러 발생 ChattigWindow - dll get socket");
                            MSG_CHECKING(data);
                        }
                        
                    }
                    else
                    {
                        T_img_send = new Thread(() => DLL.dll_IMG_SEND_THREAD(IP, PORT));
                        T_img_send.Start();
                        //SEND 스레드
                    }
                }
            
            
            }
            else if (data[0] == '$')
            {
                // 다른 유저 아이디 와 포트 할당
                var token = data.Split(',');
                int other_id = UTILITY.Token_Get_Num(token[0], ":");
                int other_port = UTILITY.Token_Get_Num(token[1], ":");
                byte[] buf = new byte[VAL.BUF_SZ];


                data = "$" + other_id.ToString();
                buf = Encoding.ASCII.GetBytes(data);
                sock.Send(buf);
                //PORT 요청 작업
            }
            else if (data[0] == '@')
            {
                // @Thread 연결할 port 할당 받음
                int target_port = UTILITY.Token_Get_Num(data, ":");

                if (isNew)
                {
                    int temp = DLL.dll_Get_Socket(IP, target_port, 1);
                    T_img_recv = new Thread(() => THREAD_IMG_RECV(temp));
                    T_img_recv.Start();
                }

                else
                {
                    //RECV 스레드
                    T_img_recv = new Thread(() => DLL.dll_IMG_RECV_THREAD(IP, target_port));
                    T_img_recv.Start();
                }
            }
            else if (data[0] == '&')
            {
                var token = data.Split(',');
                int.TryParse (token[1], out int id);
                int.TryParse(token[2], out int emotion);



                if (id == ID)
                {
                    EMO_my = emotion;
                }
                else ////////////////////////////////////////////////////////////////// 상대 아이디로 들어왔을때 처리 할것
                {
                    EMO_you = emotion;
                }
                

            }
            else 
            {   // 일반 메시지
                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    TBX_MESSAGE.AppendText(data);
                }));
            }
        }
        // 이미지 전송 함수 - UI에 출력 되는 버전
        private void THREAD_IMG_SEND(int t_ImgSend_Sock)
        {
            Console.WriteLine("[ChattingWindow]샌드 스레드 실행됨");
            int size;

            cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
            cap.FrameWidth = VAL.frameWidth;
            cap.FrameHeight = VAL.frameHeight;
            cap.Open(0);
            /**/
            Mat mat = new Mat();
            while (isRun)
            {
                cap.Read(mat);
                Cv2.Resize(mat, mat, new OpenCvSharp.Size(VAL.frameWidth, VAL.frameHeight));
                
                size = mat.Channels() * mat.Cols * mat.Rows;

                var b = new byte[size];
                System.Runtime.InteropServices.Marshal.Copy(mat.Data, b, 0, size);

                DLL.DLL_IMG_SEND(b, t_ImgSend_Sock);



                if (EMO_my != -1)
                {
                    //Console.WriteLine("my emotion is : " + EMO_my);
                    mat = EMO.image_conversion(mat, EMO_my);
                }


                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    try
                    {
                        WriteableBitmap wb = new WriteableBitmap(mat.Cols, mat.Rows, 96, 96, PixelFormats.Bgr24, null);
                        image.Source = wb;
                        WriteableBitmapConverter.ToWriteableBitmap(mat, wb);
                    }
                    catch (Exception e) { }
                
                    //Cv2.ImShow("dfdf", mat);
                }));
                int c = Cv2.WaitKey(1000/24);
                if (c != -1)
                    break;
            }
            Console.WriteLine("샌드 스레드 종료");
            DLL.dll_Free_Socket(t_ImgSend_Sock);
            if (cap != null && cap.IsOpened())
            {
                cap.Dispose();
            }
        }
        // 이미지 수신 함수 - UI에 출력 되는 버전
        private void THREAD_IMG_RECV(int t_ImgRecv_Sock)
        {
            Console.WriteLine("[ChattingWindow]리시브 스레드 실행됨");
            int num = DLL.DLL_SENDING(t_ImgRecv_Sock, "send trigger", "send trigger".Length);
            Mat mat;
            IntPtr ptr;

            while (isRun)
            {
                ptr = DLL.DLL_IMG_RECV(t_ImgRecv_Sock);
                if (ptr != null)
                {



                    mat = new Mat(ptr);
                    //Cv2.ImShow("qweqwe", mat);
                    if(EMO_you != -1)
                    {
                        //Console.WriteLine("your emotion is : " + EMO_you);
                        mat = EMO.image_conversion(mat, EMO_you);
                    }


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

                    int c = Cv2.WaitKey(1000 / 30);
                    if (c != -1)
                        break;
                }
                else
                    Console.WriteLine("is null~~!");
            }
            Console.WriteLine("리시브 스레드 종료");
        }
        #endregion

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

       
    }
}
  