using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;
using CLIENT_wpf.FUNC;
using CLIENT_wpf.VIEWMODEL;
using CLIENT_wpf.VAL;
using CLIENT_wpf.VIEW;

namespace CLIENT_wpf.VIEW
{

    public delegate void DataGetEventHandler(string item);
    public delegate void DataPushEventHandler(string value);

    public partial class StartWindow : Window
    {
        #region 멤버 변수
        Thread Msg_recv_thread;
        Socket sock;

        PasswdWindow PW;
        MakingWindow MW;
        ChattingWindow CW;
        DetailWindow DW;

        int Current_room_number = -1;
        public DataPushEventHandler DataSendEvent;


        #endregion

        #region 윈도우 관련작업

        public StartWindow()
        { 
            InitializeComponent();
            UTILITY.START();
            CTRL.initProperty();

            UTILITY.Read_File();

            this.DataContext = CTRL.DATA;
            CTRL.DATA.List = new List<ROOM>();
            CTRL.DATA.Title = "대기 방";

            CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
            if (CTRL.DATA.AutoConnect) Init_window();

            
        }
        private bool Init_window(string msg="")
        {

            CTRL.ConnectionStateChange(CONSTANTS.STATE_TRYCONNECT);

            sock = PROTOCOL.CREATE_SOCKET(CTRL.DATA.IP, CTRL.DATA.PORT, CONSTANTS.TCP, CONSTANTS.CONNECT);
            if (sock == null)
            {
                Console.WriteLine("연결이 성공적으로 작동하지 않았습니다.");
                Console.WriteLine($"---IP[{CTRL.DATA.IP}]/PORT[{CTRL.DATA.PORT}]/OPT[{CONSTANTS.TCP}/{CONSTANTS.CONNECT}]---\n");
                CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
                return false;
            }

            Msg_recv_thread = new Thread(THREAD_MSG_RECV);
            Msg_recv_thread.Start();

            if (msg != "")
            {
                byte[] buf = Encoding.Default.GetBytes(msg);
                sock.Send(buf);

            }

            CTRL.ConnectionStateChange(CONSTANTS.STATE_CONNECTING);

            RequestRoomList();

            return true;
        }
        
        // 종료 시 처리할 작업들
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _Window_Close(); 
        }
        private void _Window_Close()
        {
            CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
            if (sock != null)
            {
                sock.Shutdown(SocketShutdown.Both);
            }
            UTILITY.Release_thread(Msg_recv_thread);

        }

        #endregion

        #region UI - 이벤트

        private void Click_Join(object sender, RoutedEventArgs e)
        {
            if (ListView.SelectedItem != null)
            {
                int id = ((ROOM)ListView.SelectedItem).id;
                byte[] buf = Encoding.Default.GetBytes($"^,{id}\n");
                sock.Send(buf);
            }
            else
            {
                Console.WriteLine("★☆★☆★☆DEBUG MODE☆★☆★☆★");
                CW = new ChattingWindow();
                CW.Show();
                this.Close();
            }
        }

        private void Click_MakingRoom(object sender, RoutedEventArgs e)
        {
            MW = new MakingWindow();
            this.DataSendEvent += new DataPushEventHandler(MW.Recv_From_Parent);
            MW.DataSendEvent += new DataGetEventHandler(this.Recv_From_MakingWindow);

            MW.Show();
        }

        private void Click_F5(object sender, RoutedEventArgs e)
        {
            if (CTRL.DATA.IsConnect == true) RequestRoomList();
        }

        private void RequestRoomList()
        {
            CTRL.DATA.List = new List<ROOM>();
            Thread.Sleep(200);
            byte[] buf = Encoding.Default.GetBytes("!request\n");
            sock.Send(buf);
        }

        private void Click_Setting(object sender, RoutedEventArgs e)
        {
            if (Grid_Setting.Visibility == Visibility.Hidden)
                Grid_Setting.Visibility = Visibility.Visible;
            else
                Grid_Setting.Visibility = Visibility.Hidden;
        }

        private void Click_CONNECT(object sender, RoutedEventArgs e)
        {

            Grid_Setting.Visibility = Visibility.Hidden;

            if (CTRL.DATA.IsConnect ==false)
            {
                if (Init_window())
                {
                    CTRL.ConnectionStateChange(CONSTANTS.STATE_CONNECTING);
                    return;
                }
            }
            else
            {
                if (sock != null)
                {
                    sock.Shutdown(SocketShutdown.Both);
                    sock.Close();
                }
                UTILITY.Release_thread(Msg_recv_thread);
                CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
            }
        }

        private void ListView_DoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (ListView.SelectedItem != null)
            {
                int id = ((ROOM)ListView.SelectedItem).id;

                DW = new DetailWindow((ROOM)ListView.SelectedItem);
                this.DataSendEvent += new DataPushEventHandler(DW.Recv_From_Parent);
                DW.DataSendEvent += new DataGetEventHandler(this.Recv_From_DetailWindow);
                DW.Show();

                //byte[] buf = Encoding.Default.GetBytes($"^,{id}\n");
                //sock.Send(buf);
            }
        } 

        #endregion

        #region 페이지 스레드
        private void THREAD_MSG_RECV()
        {
            byte[] buf = new byte[CONSTANTS.BUF_SZ];
            int len;
            String data = "";
            String temp;
            while ( CTRL.DATA.IsConnect == true)
            {
                len = sock.Receive(buf);
                if (len == 0 || len == -1)
                {
                    Console.WriteLine($"서버로 부터 연결이 끊어졌습니다.");
                    CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
                    break;
                }
                else
                {
                    temp = Encoding.Default.GetString(buf, 0, len);
                    data += temp;
                    while (true)
                    {
                        int num = UTILITY.Token_Get_Index(data, "\n");                                          // \n으로 메시지 구분
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
                            data = data.Substring(num + 1);                                                     // \n기준으로 오른쪽 : 다음 수신 메시지로 판단
                            MSG_CHECKING(temp);                                                                 // \n기준으로 오른쪽 : 다음 수신 메시지로 판단
                        }
                    }
                }
            }
            Console.WriteLine($"[StartWindow] 메시지 스레드 종료");
        }

        private void MSG_CHECKING(String data)
        {
            Console.WriteLine("☆[StartWindow]" + data);

            if (data == null)
                return;

            // !,roomID,roomTITLE,roomPERSON - 방 리스트 수신
            else if (data[0] == '!')
            {
                var token = data.Split(',');

                CTRL.DATA.List.Add(new ROOM()
                {
                    id = Convert.ToInt32(token[1]),
                    title = token[2],
                    man = token[3],
                    EAgry = Convert.ToInt32(token[4]),
                    EDigu = Convert.ToInt32(token[5]),
                    EFear = Convert.ToInt32(token[6]),
                    EHapy = Convert.ToInt32(token[7]),
                    ENtur = Convert.ToInt32(token[8]),
                    ESad = Convert.ToInt32(token[9]),
                    ESprs = Convert.ToInt32(token[10]),
                });
            }

            // @,roomID - 해당 방의 비밀번호 요청
            else if (data[0] == '@')
            {
                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    var token = data.Split(',');
                    int.TryParse(token[1], out int target_id);
                    String target_title = token[2];

                    PW = new PasswdWindow(target_title);
                    this.DataSendEvent += new DataPushEventHandler(PW.Recv_From_Parent);
                    PW.DataSendEvent += new DataGetEventHandler(this.Recv_From_PasswdWindow);
                    PW.Show();
                }));
            }

            // #,roomTITLE,roomPORT - 방에 접근하시오
            else if (data[0] == '#')
            {
                Console.WriteLine("방에 입장 하겠음다.");
                var token = data.Split(',');


                ROOM r = new ROOM(){
                    id = Convert.ToInt32(token[1]),
                    title = token[2],
                    man = token[3],
                    EAgry = Convert.ToInt32(token[4]),
                    EDigu = Convert.ToInt32(token[5]),
                    EFear = Convert.ToInt32(token[6]),
                    EHapy = Convert.ToInt32(token[7]),
                    ENtur = Convert.ToInt32(token[8]),
                    ESad = Convert.ToInt32(token[9]),
                    ESprs = Convert.ToInt32(token[10]),
                };

                /*
                int.TryParse(token[1], out Current_room_number);
                String title = token[2];
                String PORT = token[3];
                */

                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    _Window_Close();
                    //CW = new ChattingWindow(PORT, title,(ROOM)ListView.SelectedItem);
                    CW = new ChattingWindow(r);
                    this.DataSendEvent += new DataPushEventHandler(CW.Recv_From_Parent);
                    CW.DataSendEvent += new DataGetEventHandler(this.Recv_From_ChattingWindow);
                    CW.Show();
                    this.Hide();
                }));
            }

            // $,Message1,Message2 - 서버로부터 제어 메시지 수신
            else if (data[0] == '$')
            {
                var token = data.Split(',');
                MessageBox.Show(token[1] + token[2]);
            }
        }

        #endregion

        #region 자식 프로세스 관련
        private void Recv_From_MakingWindow(string item)
        {
            if (sock == null)
            {
                MessageBox.Show("서버에 먼저 연결하세요","Error");
            }
            else
            {
                byte[] buf = Encoding.Default.GetBytes(item);
                sock.Send(buf);
            }
        }

        private void Recv_From_PasswdWindow(string passwd)
        {
            int id = ((ROOM)ListView.SelectedItem).id;
            byte[] buf = Encoding.Default.GetBytes("#," + id + "," + passwd + "\n");
            Console.WriteLine("★" + "#," + id + "," + passwd + "\n");

            sock.Send(buf);
        }

        private void Recv_From_ChattingWindow(String a)
        {
            //다시 연결
            this.Show();
            String msg = "";
            if (!a.Equals(""))
            {
                var token = a.Split(',');
                MessageBox.Show(token[0],token[1],MessageBoxButton.OK,MessageBoxImage.Warning);
            }
            if (Current_room_number != -1 && CTRL.DATA.AutoConnect)
            {
                msg = $"$,{Current_room_number}";
                Current_room_number = -1;
            }
            CTRL.DATA.Title = "대기 방";

            if (CTRL.DATA.AutoConnect) Init_window(msg);
            else CTRL.ConnectionStateChange(CONSTANTS.STATE_DISCONNECT);
        }

        private void Recv_From_DetailWindow(string id)
        {
            byte[] buf = Encoding.Default.GetBytes($"^,{id}\n");
            sock.Send(buf);
        }
        #endregion
    }
}