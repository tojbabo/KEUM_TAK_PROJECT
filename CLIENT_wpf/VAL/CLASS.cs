using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.VAL
{
/*
0 : 화난 이모티콘 스티커
1 : 역겹 이모티콘 스티커
2 : 공포 이모티콘 스티커
3 : 행복 이모티콘 스티커
4 : 슬픔 이모티콘 스티커
5 : 놀람 이모티콘 스티커
6 : 무표정 이모티콘 스티커

7 : 빨간얼굴
8 : 파란얼굴
9 : 큰 얼굴
10 : 모자이크
11 : 빨간눈
12 : 180도 회전
13 : 큰 눈
14 : 빠직 스티커
15 : 눈물 스티커
16 : 사실은 오래전부터 당신같은 남자를 기다려왔다우
*/
    public class ROOM
    {
        public int id { get; set; }
        public string title { get; set; }
        public string man { get; set; }

        public int EAgry { get; set; } 
        public int EDigu { get; set; } 
        public int EFear { get; set; } 
        public int EHapy { get; set; }  
        public int ENtur { get; set; } 
        public int ESad { get; set; }
        public int ESprs { get; set; }

        public int[] EMO = new int[7];


        public void Show()
        {
            Console.WriteLine($"------- Room data ---------------------");
            Console.WriteLine($"------- ID             : {id}");
            Console.WriteLine($"------- TITLE          : {title}");
            Console.WriteLine($"------- man            : {man}");
            Console.WriteLine($"------- Angry Event    : {EAgry}");
            Console.WriteLine($"------- Disgust Event  : {EDigu}");
            Console.WriteLine($"------- Fear Event     : {EFear}");
            Console.WriteLine($"------- Happy Event    : {EHapy}");
            Console.WriteLine($"------- Sad Event      : {ESad}");
            Console.WriteLine($"------- Surprise Event : {ESprs}");
            Console.WriteLine($"------- Neture Event   : {ENtur}");
            Console.WriteLine($"---------------------------------------");
        }
        public void InitializeEmo()
        {
            EMO[0] = EAgry;
            EMO[1] = EDigu;
            EMO[2] = EFear;
            EMO[3] = EHapy;
            EMO[4] = ESad;
            EMO[5] = ESprs;
            EMO[6] = ENtur;
        }
    }
}
