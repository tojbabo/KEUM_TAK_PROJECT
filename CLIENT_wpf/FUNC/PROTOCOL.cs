using CLIENT_wpf.CLASS;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CLIENT_wpf.FUNC
{
    class PROTOCOL
    {

        static public Socket CREATE_SOCKET(String ip, int port, int type, int opt)
        {
            // type : TCP(1) / UDP(2)
            // opt : 1 - 연결 UDP / 그 외: 일반 UDP
            Socket temp_sock;
            if (type == VAL.TCP)
            {
                temp_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
                //temp_sock.ReceiveTimeout = 2000;
                try
                {
                    temp_sock.Connect(ip, port);
                    Console.WriteLine("TCP 연결 성공");
                    return temp_sock;
                }
                catch
                {
                    Console.WriteLine("TCP 연결 실패");
                    return null;
                }

            }
            else if (type == VAL.UDP)
            {
                temp_sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                //temp_sock.ReceiveTimeout = 2000;
                if (opt == 1)
                {
                    try
                    {
                        temp_sock.Connect(ip, port);
                        Console.WriteLine("UDP 연결 성공");
                        return temp_sock;
                    }
                    catch
                    {
                        Console.WriteLine("UDP 연결 실패");
                        return null;
                    }
                }
                else
                {
                    Console.WriteLine("UDP 소켓 생성 성공");
                    return temp_sock;
                }
            }
            else
                Console.WriteLine("잘못된 소켓 생성");

            return null;
        }
    }
}
