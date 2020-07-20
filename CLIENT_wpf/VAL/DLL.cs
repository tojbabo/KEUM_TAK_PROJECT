using System;
using System.Runtime.InteropServices;


namespace CLIENT_wpf.CLASS
{
    public class DLL
    {
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void Hi();                                                                             // 버전 체크 및 DLL 연결 확인
        //[DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        //extern public static void dll_IMG_SEND_THREAD(String serv_ip, int serv_port);                               // [old] 이미지 전송 스레드
        //[DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        //extern public static void dll_IMG_RECV_THREAD(String serv_ip, int serv_port);                               // [old] 이미지 수신 스레드
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static int dll_Get_Socket(String serv_ip, int serv_port, int opt);                            // C++ 용 SOCKET 구하는 함수
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void dll_Free_Socket(int sock);                                                        // C++ 용 SOCKET 해제하는 함수
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static int DLL_SENDING(int sock, String msg, int str_len);                                    // C++ 소켓으로 메시지 전송
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static bool DLL_IMG_SEND(byte[] b, int sock);                                                 // [new] 이미지 전송 스레드
        [DllImport("forDDL.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static IntPtr DLL_IMG_RECV(int sock);                                                         // [new] 이미지 수신 스레드
    }
}
