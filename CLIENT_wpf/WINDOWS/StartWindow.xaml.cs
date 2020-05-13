using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Threading;
using CLIENT_wpf.CLASS;
using CLIENT_wpf.FUNC;
using CLIENT_wpf.WINDOWS;

namespace CLIENT_wpf.WINDOWS
{

    public delegate void DataGetEventHandler(string item);
    public delegate void DataPushEventHandler(string value);

    public partial class StartWindow : Window
    {
        VAL val = new VAL();
        List<DATA> listData;
        Thread Msg_recv_thread;
        Socket sock;
        public DataPushEventHandler DataSendEvent;

        private class DATA
        {
            public int id { get; set; }
            public string title { get; set; }
            public string man { get; set; }

        }

        public StartWindow()
        {
            InitializeComponent();
            UTILITY.START();
            Init_window();
        }
        private void Init_window()
        {
            sock = PROTOCOL.CREATE_SOCKET(val.SERV_IP, val.SERV_PORT, VAL.TCP, VAL.CONNECT);
            if (sock == null)
            {
                Console.WriteLine("서버가 열려있지 않습니다. 혹은 연결되지 않습니다.");
                Console.WriteLine("서버를 확인한 후 수동으로 연결하세요");
                return;
            }

            BTN_CONNECT.IsEnabled = false;

            listData = new List<DATA>();
            Msg_recv_thread = new Thread(THREAD_MSG_RECV);
            Msg_recv_thread.Start();
        }
        // 종료 시 처리할 작업들
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _Window_Close();
        }
        private void _Window_Close()
        {
            Console.WriteLine("나 종료된당 히힛");
            UTILITY.Release_thread(Msg_recv_thread);
            sock.Close();
            BTN_CONNECT.IsEnabled = true;
        }

        #region UI - 이벤트
        private void Click_Join(object sender, RoutedEventArgs e)
        {
            int id = ((DATA)ListView.SelectedItem).id;

            PasswdWindow PW = new PasswdWindow();
            this.DataSendEvent += new DataPushEventHandler(PW.Recv_From_Parent);
            PW.DataSendEvent += new DataGetEventHandler(this.Recv_From_Child_PasswdData);
            PW.Show();



            /* ChattingWindow CW = new ChattingWindow();
             Window.GetWindow(this).Close();
             CW.Show();*/
        }

        private void Click_MakingRoom(object sender, RoutedEventArgs e)
        {
            MakingWindow MW = new MakingWindow();
            this.DataSendEvent += new DataPushEventHandler(MW.Recv_From_Parent);
            MW.DataSendEvent += new DataGetEventHandler(this.Recv_From_Child_SendMakingData);

            MW.Show();
        }

        private void Click_F5(object sender, RoutedEventArgs e)
        {
            listData = new List<DATA>();
            byte[] buf = new byte[VAL.BUF_SZ];

            buf = Encoding.ASCII.GetBytes("!request\n");
            sock.Send(buf);
        }

        private void Click_CONNECT(object sender, RoutedEventArgs e)
        {
            Init_window();
        }

        private void ListView_DoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            int id = ((DATA)ListView.SelectedItem).id;

            PasswdWindow PW = new PasswdWindow();
            this.DataSendEvent += new DataPushEventHandler(PW.Recv_From_Parent);
            PW.DataSendEvent += new DataGetEventHandler(this.Recv_From_Child_PasswdData);
            PW.Show();
        }

        #endregion

        #region 페이지 스레드
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
                        MSG_CHECKING(temp);                                                     // \n기준으로 오른쪽 : 다음 수신 메시지로 판단
                    }
                }
            }
        }
        private void MSG_CHECKING(String data)
        {
            if (data[0] == '!')
            {
                Console.WriteLine("! - 방 목록 받아옴!!");

                var token = data.Split(',');

                int.TryParse(token[1], out int ID);
                String TITLE = token[2];
                String PERSON = token[3];

                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {
                    listData.Add(new DATA() { id = ID, title = TITLE, man = PERSON });

                    ListView.ItemsSource = listData;
                    ListView.Items.Refresh();

                }));
            }
            else if (data[0] == '@')
            {
                //비밀 번호 요청
                //Console.WriteLine("@ - Thead를 생성할 PORT를 할당 받음 !!");

            }
            else if (data[0] == '#')
            {
                Console.WriteLine("# - 방에 입장하라는 명령 !!");
                // 방 접근하세요 #,title,port
                var token = data.Split(',');
                String title = token[1];
                String PORT = token[2];
                Dispatcher.Invoke(DispatcherPriority.Normal, new Action(delegate
                {

                    ChattingWindow CW = new ChattingWindow(PORT,title);
                    this.DataSendEvent += new DataPushEventHandler(CW.Recv_From_Parent);
                    CW.DataSendEvent += new DataGetEventHandler(this.Recv_From_Child_ReStart);
                    CW.Show();
                    this.Hide();
                    //_Window_Close();
                    //this.Close();
                    Console.WriteLine("StartWindow -> ChattingWindow");
                }));
            }
            else if (data[0] == '$')
            {
                Console.WriteLine("$ - 서버로 부터 메시지 !!");
                // 방 접근하세요 #,메시지종류,메시지내용

                var token = data.Split(',');
                MessageBox.Show(token[1] + token[2]);
            }
        }


        #endregion

        #region 자식 프로세스 관련
        private void Recv_From_Child_SendMakingData(string item)
        {
            Console.WriteLine("data is : " + item);
            byte[] buf = new byte[VAL.BUF_SZ];


            buf = Encoding.ASCII.GetBytes(item);
            sock.Send(buf);
        }

        private void Recv_From_Child_PasswdData(string passwd)
        {
            int id = ((DATA)ListView.SelectedItem).id;
            byte[] buf = new byte[VAL.BUF_SZ];
            buf = Encoding.ASCII.GetBytes("#," + id + "," + passwd + "\n");
            sock.Send(buf);
        }

        private void Recv_From_Child_ReStart(String a)
        {
            this.Show();
            _Window_Close();
            //다시 연결
            Init_window();

        }
        #endregion

        
    }
}
