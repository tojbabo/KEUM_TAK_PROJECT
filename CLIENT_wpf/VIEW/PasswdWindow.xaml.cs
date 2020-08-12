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
using System.Windows.Shapes;

namespace CLIENT_wpf.WINDOWS
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
