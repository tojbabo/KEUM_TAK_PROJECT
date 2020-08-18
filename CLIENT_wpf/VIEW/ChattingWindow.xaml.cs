using System;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using OpenCvSharp;
using OpenCvSharp.Extensions;
using System.Net.Sockets;
using System.Windows.Threading;
using System.Threading;
using CLIENT_wpf.FUNC;
using CLIENT_wpf.VIEWMODEL;
using CLIENT_wpf.VAL;
using System.Drawing;
using System.Windows.Shapes;
using System.Windows.Automation.Peers;

namespace CLIENT_wpf.VIEW
{

    public partial class ChattingWindow : System.Windows.Window
    {
        #region 멤버 변수
        public DataGetEventHandler DataSendEvent;
        string MsgToParents="";

        ROOM Room;

        bool isRun = true;

        int ID;
        int PORT;

        int EMO_my;
        int EMO_you;

        VideoCapture cap;

        EMOTICON EMO;

        Socket sock = null;

        Thread T_msg_recv;
        #endregion

        #region 윈도우 관련

        public ChattingWindow()
        {
            InitializeComponent();
            CTRL.initProperty();

            this.DataContext = CTRL.DATA;
            EMO = new EMOTICON();
            EMO_my = -1;
            EMO_you = -1;
            CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
        }

        public ChattingWindow(String port, String name = "DEBUG", ROOM room = null)
        {
            InitializeComponent();
            forDEBUG.Visibility = Visibility.Collapsed;
            CTRL.DATA.Title = name;
            TBX_PORT.Text = port;

            if (room != null)
            {
                Room = room;
                Room.Show();
            }

            this.DataContext = CTRL.DATA;
            EMO = new EMOTICON();
            EMO_my = -1;
            EMO_you = -1;
            CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
            Connect_to_Server();
        }

        public ChattingWindow(ROOM room)
        {
            InitializeComponent();
            forDEBUG.Visibility = Visibility.Collapsed;
            
            if (room != null)
            {
                Room = room;
                //Room.Show();
            }

            CTRL.DATA.Title = Room.title;
            TBX_PORT.Text = Room.man;

            this.DataContext = CTRL.DATA;
            EMO = new EMOTICON();
            EMO_my = -1;
            EMO_you = -1;
            CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
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
            UTILITY.Release_thread(T_msg_recv);
            if (sock != null)
            {
                sock.Shutdown(SocketShutdown.Both);
                sock.Close();
                T_msg_recv.Join();
            }
            if (DataSendEvent != null)
            {
                DataSendEvent(MsgToParents);
            }
            else
            {
                StartWindow SW = new StartWindow();
                SW.Show();
            }
        }

        #endregion

        #region 통신 관련
        private bool Connect_to_Server()
        {
            CTRL.ConnectionStateChange(CONSTANTS.STATE_TRYCONNECT);
            if (CTRL.DATA.IsConnect)
            {
                sock.Close();
                _Window_Closing();

                CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
                return true;
            }
            else
            {
                int.TryParse(TBX_PORT.Text, out PORT);

                sock = PROTOCOL.CREATE_SOCKET(CTRL.DATA.IP, PORT, CONSTANTS.TCP, CONSTANTS.CONNECT);
                if (sock == null)
                {
                    Console.WriteLine("연결이 성공적으로 작동하지 않았습니다. - ChattingWindow/Connect_to_Server()");
                    Console.WriteLine($"---IP[{CTRL.DATA.IP}]/PORT[{PORT}]/OPT[{CONSTANTS.TCP}/{CONSTANTS.CONNECT}]---\n");
                    CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
                    return false;
                }
                String name = "^" + CTRL.DATA.NAME + "\n";
                byte[] buf = new byte[CONSTANTS.BUF_SZ];

                buf = Encoding.Default.GetBytes(name);
                sock.Send(buf);

                T_msg_recv = new Thread(THREAD_MSG_RECV);
                T_msg_recv.Start();

                CTRL.ConnectionStateChange(CONSTANTS.STATE_CONNECTING);
                return true;
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
        }

        private void Button_Click_test(object sender,RoutedEventArgs e)
        {
            byte[] buf = new byte[CONSTANTS.BUF_SZ];

            String data = "$" + ID.ToString();
            buf = Encoding.Default.GetBytes(data);
            sock.Send(buf);
        }

        private void CLICK_CONNECT(object sender, RoutedEventArgs e)
        {
            Connect_to_Server();
        }
        // --------------------------------------------------------------------- 여기까지 디버그용
        private void BTN_EXIT_Click(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("ChattingWindow -> StartWindow");
            System.Windows.Window.GetWindow(this).Close();
        }


        // 서버로 메시지 보내는 함수
        private void BTN_MSG_SEND_Click(object sender, RoutedEventArgs e)
        {
            TBX_INPUT.Focus();
            byte[] buf = new byte[CONSTANTS.BUF_SZ];

            buf = Encoding.Default.GetBytes(TBX_INPUT.Text + "\n");
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
            byte[] buf = new byte[CONSTANTS.BUF_SZ];
            int len;
            String data = "";
            String temp;
            while (isRun)
            {
                len = sock.Receive(buf);
                if (len == 0 || len == -1)
                {
                    Console.WriteLine("[ChattingWindow] 서버로 부터 연결이 끊겼습니다. ");
                    CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
                    _Window_Closing();
                }
                else
                {
                    temp = Encoding.Default.GetString(buf, 0, len);
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
            Console.WriteLine("[ChattingWindow]메시지 스레드 종료");
        }
        // 수신한 메시지를 프로토콜에 맞게 확인 후 처리하는 함수
        private void MSG_CHECKING(String data)
        {
            Console.Write("☆[ChattingWindow] "+ data);
            
            // 내 아이디와 포트 할당 #,ID,PORT
            if (data[0] == '#')
            {
                // 내 아이디와 포트 할당
                var token = data.Split(',');
                ID = UTILITY.Token_Get_Num(token[0], ":");
                PORT = UTILITY.Token_Get_Num(token[1], ":");

                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    Console.WriteLine($"{Thread.CurrentThread.ManagedThreadId}");
                    int temp = DLL.dll_Get_Socket(CTRL.DATA.IP, PORT, CONSTANTS.TCP);
                    if (temp != -1)
                    {
                        Thread T_img_send = new Thread(() => THREAD_IMG_SEND(temp));
                        T_img_send.Start();
                    }
                    else
                    {
                        Console.WriteLine($"소켓 생성에 문제가 발생 스레드 생성 중단.");
                    }
                }));
            }
            
            // 다른 유저 아이디와 포트 할당 $,ID,PORT
            else if (data[0] == '$')
            {
                // 다른 유저 아이디 와 포트 할당
                var token = data.Split(',');
                int other_id = UTILITY.Token_Get_Num(token[0], ":");
                int other_port = UTILITY.Token_Get_Num(token[1], ":");
                byte[] buf = new byte[CONSTANTS.BUF_SZ];


                data = "$" + other_id.ToString();
                buf = Encoding.Default.GetBytes(data);
                sock.Send(buf);
                //PORT 요청 작업
            }
            
            // Thread 연결할 포트 할당 @,PORT
            else if (data[0] == '@')
            {
                // @Thread 연결할 port 할당 받음
                int target_port = UTILITY.Token_Get_Num(data, ":");

                Console.WriteLine($"{Thread.CurrentThread.ManagedThreadId}");
                int temp = DLL.dll_Get_Socket(CTRL.DATA.IP, target_port, CONSTANTS.TCP);
                if (temp != -1)
                {
                    Thread T_img_recv = new Thread(() => THREAD_IMG_RECV(temp));
                    T_img_recv.Start();
                }
                else
                {
                    Console.WriteLine($"소켓 생성에 문제가 발생 스레드 생성 중단.");
                }

            }
            
            // 감정 인식결과 수신 &,ID,EMOTION
            else if (data[0] == '&')
            {
                var token = data.Split(',');
                int.TryParse(token[1], out int id);
                int.TryParse(token[2], out int emotion);


                if (id == ID)
                {
                    Console.WriteLine("My emotion : " + emotion);
                    EMO_my = emotion;
                }
                else
                {
                    Console.WriteLine("your emotion : " + emotion);
                    EMO_you = emotion;
                }
            }
            
            // [시스템 메시지]-강제퇴장 
            else if (data[0] == '*')
            {
                var token = data.Split(',');
                int.TryParse(token[1], out int Event);
                if (Event == 1)
                {
                    MsgToParents = "[룰 위반]퇴장 당했습니다. ,웃지마";
                    Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                    {
                        this.Hide();
                        System.Windows.Window.GetWindow(this).Close();
                    }));
                }
            }
            
            // 일반 채팅
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
            Console.WriteLine("[ChattingWindow]샌드 스레드 실행");
            
            int size;

            cap = VideoCapture.FromCamera(CaptureDevice.Any, 0);
            cap.FrameWidth = CONSTANTS.frameWidth;
            cap.FrameHeight = CONSTANTS.frameHeight;
            cap.Open(0);
            CTRL.ConnectionStateChecking(CONSTANTS.STATE_CONNECTING, CTRL.DATA.MyState);

            Mat mat = new Mat();
            CTRL.ConnectionStateChecking(CONSTANTS.STATE_SOSO, CTRL.DATA.MyState);
            while (isRun)
            {
                cap.Read(mat);
                Cv2.Resize(mat, mat, new OpenCvSharp.Size(CONSTANTS.frameWidth, CONSTANTS.frameHeight));

                size = mat.Channels() * mat.Cols * mat.Rows;

                var b = new byte[size];
                System.Runtime.InteropServices.Marshal.Copy(mat.Data, b, 0, size);

                if (!(DLL.DLL_IMG_SEND(b, t_ImgSend_Sock)))
                    break;

                
                if (EMO_my != -1)
                {
                    //Console.WriteLine("my emotion is : " + EMO_my);
                    mat = EMO.image_conversion(mat, EMO_my);
                    CTRL.ConnectionStateChecking(CONSTANTS.STATE_GOOD, CTRL.DATA.MyState);
                }
                else CTRL.ConnectionStateChecking(CONSTANTS.STATE_SOSO, CTRL.DATA.MyState);
                //mat = EMO.image_conversion(mat, 1);

                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    try
                    {
                        WriteableBitmap wb = new WriteableBitmap(mat.Cols, mat.Rows, 96, 96, PixelFormats.Bgr24, null);
                        image.Source = wb;
                        WriteableBitmapConverter.ToWriteableBitmap(mat, wb);
                    }
                    catch (Exception e) { }
                }));
                //CTRL.ConnectionStateChecking(CONSTANTS.STATE_GOOD, CTRL.DATA.MyState);
                int c = Cv2.WaitKey(1000 / 20);
                if (c != -1)
                    break;
            }

            CTRL.ConnectionStateChecking(CONSTANTS.STATE_DISCONNECT, CTRL.DATA.MyState);
            Console.WriteLine("[ChattingWindow]샌드 스레드 종료");
            if (cap != null && cap.IsOpened())
            {
                cap.Dispose();
            }
            DLL.dll_Free_Socket(t_ImgSend_Sock);
        }
        // 이미지 수신 함수 - UI에 출력 되는 버전
        private void THREAD_IMG_RECV(int t_ImgRecv_Sock)
        {
            Console.WriteLine("[ChattingWindow]리시브 스레드 실행");
            
            Mat mat;
            IntPtr ptr;
            CTRL.ConnectionStateChecking(CONSTANTS.STATE_CONNECTING, CTRL.DATA.YourState);



            CTRL.ConnectionStateChecking(CONSTANTS.STATE_SOSO, CTRL.DATA.YourState);
            while (isRun)
            {

                ptr = DLL.DLL_IMG_RECV(t_ImgRecv_Sock);

                if (ptr == null)
                    continue;

                mat = new Mat(ptr);

                if (mat == null)
                    continue;

                if (mat.Cols != 320 && mat.Rows != 240) { continue; }

                
                //CTRL.ConnectionStateChecking(CONSTANTS.STATE_GOOD, CTRL.DATA.YourState);


                if (EMO_you != -1)
                {
                    mat = EMO.image_conversion(mat, EMO_you);
                    CTRL.ConnectionStateChecking(CONSTANTS.STATE_GOOD, CTRL.DATA.YourState);
                }
                else CTRL.ConnectionStateChecking(CONSTANTS.STATE_SOSO, CTRL.DATA.YourState);

                Dispatcher.BeginInvoke(DispatcherPriority.Normal, new Action(delegate
                {
                    WriteableBitmap wb = new WriteableBitmap(mat.Cols, mat.Rows, 96, 96, PixelFormats.Bgr24, null);
                    image2.Source = wb;
                    WriteableBitmapConverter.ToWriteableBitmap(mat, wb);

                }));

                int c = Cv2.WaitKey(1);
                if (c != -1)
                    break;

            }
            CTRL.ConnectionStateChecking(CONSTANTS.STATE_DISCONNECT, CTRL.DATA.YourState);
            Console.WriteLine("[ChattingWindow]리시브 스레드 종료");

            DLL.dll_Free_Socket(t_ImgRecv_Sock);
        }
        #endregion
    }
}
  