using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Documents;
using CLIENT_wpf.WINDOWS;

namespace CLIENT_wpf.WINDOWS
{
    /// <summary>
    /// StartWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class StartWindow : Window
    {
        List<DATA> listData;
        public StartWindow()
        {
            InitializeComponent();
            add_listview();
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

    }
}
