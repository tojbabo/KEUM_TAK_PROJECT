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


        VideoCapture cap;
        WriteableBitmap wb;
        const int frameWidth = 640;
        const int frameHeight = 480;
        bool loop = false;
        Socket sock = null;


        Thread t;
        Thread T_msg_recv;
        bool t_loop = true;
        

        public MainWindow()
        {
            InitializeComponent();
            
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("프로세스 시작");
            t = new Thread(ok);
            t.Start();
            
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

            T_msg_recv = new Thread(THREAD_RECV);
            T_msg_recv.Start();

        }

        private bool CREATE_SOCKET()
        {
            sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
            try
            {
                sock.Connect("127.0.0.1", 9000);
                return true;
            }
            catch
            {
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

        private void THREAD_RECV()
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
                if(data[0] == '#')
                {
                    var parts = data.Split(' ');
                    int a = Convert.ToInt32(parts[0]);
                    int b = Convert.ToInt32(parts[1]);

                    Console.WriteLine(a + b);
                  
                    
                    
                    
                }
                
                
            }
        }
    }
}

