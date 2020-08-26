
using CLIENT_wpf.VAL;
using System;
using System.Collections.Generic;
using System.Windows;

namespace CLIENT_wpf.VIEW
{
    /// <summary>
    /// MakingWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MakingWindow : Window
    {
        private class item
        {
            public string name { get; set; }
            public string val { get; set; }
        }
        
        public DataGetEventHandler DataSendEvent;
        List<item> Items;
        
        public MakingWindow()
        {
            InitializeComponent();
            add_items();
            item_setting();
            
        }

        private void Click_No(object sender, RoutedEventArgs e)
        {
            Window.GetWindow(this).Close();
        }

        private void Click_Yes(object sender, RoutedEventArgs e)
        {
            if (!TB_TITLE.Text.Equals(""))
            {
                String opt = get_reuslt();
                String temp = "@," + TB_TITLE.Text + "," + TB_PASSWD.Text + ",@," + opt + "\n";
                //Console.WriteLine(temp);
                DataSendEvent(temp);

                Window.GetWindow(this).Close();
            }
            else
            {
                MessageBox.Show("제목을 입력하세요","주의");
            }
        }
        
        public void Recv_From_Parent(string param)
        {
            Console.WriteLine("Set Action value called");   
        }

        #region 콤보박스 추가/이벤트
        void item_setting()
        {
            A1.ItemsSource = Items;
            A2.ItemsSource = Items;
            A3.ItemsSource = Items;
            A4.ItemsSource = Items;
            A5.ItemsSource = Items;
            A6.ItemsSource = Items;
            A7.ItemsSource = Items;

            item i = Items[0];

            A1.SelectedItem = i;
            A2.SelectedItem = i;
            A3.SelectedItem = i;
            A4.SelectedItem = i;
            A5.SelectedItem = i;
            A6.SelectedItem = i;
            A7.SelectedItem = i;
        }
        void add_items()
        {
            Items = new List<item>() {
                new item() { name = "없음", val = "0" },
                new item() { name = "강퇴", val = "1" },
                new item() { name = "필터링", val = "2" },
                new item() { name = "블랙", val = "3" },
                new item() { name = "스티커", val = "4" },


                new item() { name = "붉은 얼굴", val = "7" },
                new item() { name = "푸른 얼굴", val = "8" },
                new item() { name = "빅 헤드", val = "9" },
                new item() { name = "모자이크", val = "10" },
                new item() { name = "사륜안", val = "11" },
                new item() { name = "180도 회전", val = "12" },
                new item() { name = "빅 아이", val = "13" },
                new item() { name = "ㅃㅏ직", val = "14" },
                new item() { name = "눙물", val = "15" },
                new item() { name = "Rose", val = "16" },
            };
        }
        String get_reuslt()
        {
            string result = "";
            if (A1.IsEnabled == true)
            {
                result = result + (string)A1.SelectedValue+",";

            }else { result = result + "0,"; }
            if (A2.IsEnabled == true)
            {
                result = result + (string)A2.SelectedValue + ",";

            }
            else { result = result + "0,"; }
            if (A3.IsEnabled == true)
            {
                result = result + (string)A3.SelectedValue + ",";

            }
            else { result = result + "0,"; }
            if (A4.IsEnabled == true)
            {
                result = result + (string)A4.SelectedValue + ",";

            }
            else { result = result + "0,"; }
            if (A5.IsEnabled == true)
            {
                result = result + (string)A5.SelectedValue + ",";

            }
            else { result = result + "0,"; }
            if (A6.IsEnabled == true)
            {
                result = result + (string)A6.SelectedValue + ",";

            }
            else { result = result + "0,"; }
            if (A7.IsEnabled == true)
            {
                result = result + (string)A7.SelectedValue + "";

            }
            else { result = result + "0"; }

            Console.WriteLine(result);


            return result;
            
        }
        #endregion

        #region 체크박스 활성/비활성화
        private void E1_Checked(object sender, RoutedEventArgs e)
        {
            if (E1.IsChecked == true) { A1.IsEnabled = true; }
            else { A1.IsEnabled = false; }
        }
        private void E2_Checked(object sender, RoutedEventArgs e)
        {
            if (E2.IsChecked == true) { A2.IsEnabled = true; }
            else { A2.IsEnabled = false; }
        }
        private void E3_Checked(object sender, RoutedEventArgs e)
        {
            if (E3.IsChecked == true) { A3.IsEnabled = true; }
            else { A3.IsEnabled = false; }
        }
        private void E4_Checked(object sender, RoutedEventArgs e)
        {
            if (E4.IsChecked == true) { A4.IsEnabled = true; }
            else { A4.IsEnabled = false; }
        }
        private void E5_Checked(object sender, RoutedEventArgs e)
        {
            if (E5.IsChecked == true) { A5.IsEnabled = true; }
            else { A5.IsEnabled = false; }
        }
        private void E6_Checked(object sender, RoutedEventArgs e)
        {
            if (E6.IsChecked == true) { A6.IsEnabled = true; }
            else { A6.IsEnabled = false; }
        }
        private void E7_Checked(object sender, RoutedEventArgs e)
        {
            if (E7.IsChecked == true) { A7.IsEnabled = true; }
            else { A7.IsEnabled = false; }
        }
        #endregion
    }
}
