using OpenCvSharp;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CLIENT_wpf.FUNC
{
    public class EMOTICON
    {
        // 이모티콘 7 개

        static Mat[] emoticon = new Mat[7];

        public EMOTICON()
        {
            // 이모티콘 read   
            string dir = "__resource/";
            emoticon[0] = new Mat(dir + "Angry.PNG", ImreadModes.Unchanged);
            emoticon[1] = new Mat(dir + "Disgust.PNG", ImreadModes.Unchanged);
            emoticon[2] = new Mat(dir + "Fear.PNG", ImreadModes.Unchanged);
            emoticon[3] = new Mat(dir + "Happy.PNG", ImreadModes.Unchanged);
            emoticon[4] = new Mat(dir + "Sad.PNG", ImreadModes.Unchanged);
            emoticon[5] = new Mat(dir + "Surprise.PNG", ImreadModes.Unchanged);
            emoticon[6] = new Mat(dir + "Neutral.PNG", ImreadModes.Unchanged);
            /*
            try
            {
                // test image
                string file = dir+"lena.png";
                Mat img = new Mat(file, ImreadModes.Unchanged);

                for (int i = 0; i < 7; i++)
                {
                    img = image_conversion(img, i);     // 이모티콘 추가하여 반환
                    Cv2.ImShow("EMOTI", emoticon[i]);
                    Cv2.ImShow("img", img);
                    Cv2.WaitKey(0);
                    Cv2.DestroyAllWindows();
                }

            }
            catch (Exception e) { Console.WriteLine(e.Message); }
            
        */
        }

        public Mat image_conversion(Mat img, int num)
        {
            int size = 60;              // 이모티콘 사이즈
            int x_pos = 320-60;               // 이모티콘이 들어갈 position
            int y_pos = 240-60;                 // 
            Mat tmp = new Mat();
            if (num < 7 && num >= 0)
            {
                Cv2.Resize(emoticon[num], tmp, new Size(size, size));       // 이모티콘 사이즈 변환


                for (int x = x_pos; x < x_pos + size; x++)    // 이모티콘이 들어갈 x 좌표 loop
                {

                    for (int y = y_pos; y < y_pos + size; y++) // y loop
                    {
                        var pt = img.At<Vec3b>(y, x);                                       // 3 channel
                        var ptr2 = tmp.At<Vec3b>(y - y_pos, x - x_pos);    // 이모티콘의 3 channel

                        // 이미지 대체
                        pt.Item0 = (byte)ptr2.Item0;
                        pt.Item1 = (byte)ptr2.Item1;
                        pt.Item2 = (byte)ptr2.Item2;

                        img.Set<Vec3b>(y, x, pt);
                    }
                }
            }
            return img;
            
            // 반환
        }
    }
}
