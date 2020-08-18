using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.VAL
{
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
            Console.WriteLine($"------- Neture Event   : {ENtur}");
            Console.WriteLine($"------- Sad Event      : {ESad}");
            Console.WriteLine($"------- Surprise Event : {ESprs}");
            Console.WriteLine($"---------------------------------------");
        }
    }
}
