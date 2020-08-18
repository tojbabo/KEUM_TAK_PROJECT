using System;
using System.Windows;

namespace CLIENT_wpf.VIEW
{
    /// <summary>
    /// PasswdWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class PasswdWindow : Window
    {
        public DataGetEventHandler DataSendEvent;

        public PasswdWindow(string title="DEBUG")
        {
            InitializeComponent();
            _Window.Title = title;
        }       
        
        private void Click_OK(object sender, RoutedEventArgs e)
        {
            String temp = TB_Passwd.Text;
            Console.WriteLine(temp);
            DataSendEvent(temp);

            Window.GetWindow(this).Close();
        }
        
        public void Recv_From_Parent(string param)
        {
            Console.WriteLine("Set Action value called");
        }
    }
}
