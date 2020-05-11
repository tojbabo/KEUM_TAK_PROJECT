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
    public partial class StartWindow : Window
    {
        VAL val = new VAL();
        List<DATA> listData;
        Thread Msg_recv_thread;
        Socket sock;

        public StartWindow()
        {
            InitializeComponent();
            add_listview();
        }

        // 종료 시 처리할 작업들
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            UTILITY.Release_thread(Msg_recv_thread);
            sock.Close();
        }


        private void Click_Join(object sender, RoutedEventArgs e)
        {

            ChattingWindow CW = new ChattingWindow();
            Window.GetWindow(this).Close();
            CW.Show();
        }

        private void Click_MakingRoom(object sender, RoutedEventArgs e)
        {
            MakingWindow MW = new MakingWindow();
            MW.Show();
        }

        private void add_listview()
        {
            listData = new List<DATA>();
            listData.Add(new DATA() { id=1, title="[임시]첫번째 생성해본 방", man=1});

            ListView.ItemsSource = listData;
            ListView.Items.Refresh();
            
            
        }
        private class DATA
        {
            public int id { get; set; }
            public string title { get; set; }
            public int man { get; set; }

        }

        private void Click_DEBUG(object sender, RoutedEventArgs e)
        {
            sock = PROTOCOL.CREATE_SOCKET(val.SERV_IP, val.SERV_PORT, VAL.TCP, VAL.CONNECT);

            Msg_recv_thread = new Thread(THREAD_MSG_RECV);
            Msg_recv_thread.Start();

        }
        void THREAD_MSG_RECV()
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
                        data = "";
                        break;
                    }
                    else                                                                                    // \n이 문자열 사이에 존재 -> 메시지 동시에 수신
                    {
                        temp = data.Substring(0, num);                                                      // \n기준으로 왼쪽 : 현재 메시지로 판단
                        data = data.Substring(num + 1);                                                       // \n기준으로 오른쪽 : 다음 수신 메시지로 판단
                    }
                }
            }
        }
    }
}
