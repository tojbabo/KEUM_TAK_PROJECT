using CLIENT_wpf.VAL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Contexts;
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

namespace CLIENT_wpf.VIEW
{
    /// <summary>
    /// DetailWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class DetailWindow : Window
    {
        public DataGetEventHandler DataSendEvent;
        
        public DetailWindow(ROOM target)
        {
            InitializeComponent();
            this.DataContext = target;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            DataSendEvent(TBX_ID.Text);
            Window.GetWindow(this).Close();
        }
        public void Recv_From_Parent(string param)
        {
            Console.WriteLine("Set Action value called");
        }
    }
}
