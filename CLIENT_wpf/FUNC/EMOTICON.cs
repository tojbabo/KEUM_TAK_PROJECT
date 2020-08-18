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
        CascadeClassifier faceCascade;

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
            String filenameFaceCascade = dir+"haarcascade_frontalface_alt.xml";

            faceCascade = new CascadeClassifier();
            if (!faceCascade.Load(filenameFaceCascade))
            {
                Console.WriteLine("error");
                return;
            }
        }

        public Mat image_conversion(Mat img, int num)
        {
            int size = 60;              // 이모티콘 사이즈
            int x_pos = 540;               // 이모티콘이 들어갈 position
            int y_pos = 400;                 // 

            // 이미지 얼굴 검출 실패 시
            if (num == 7)
            {
                string str = "Finding faces in images...";
                Cv2.PutText(img, str, new Point(10, 30), HersheyFonts.HersheyComplex, 0.7, Scalar.Black, 2, LineTypes.AntiAlias);
                return img;
            }


            // detect
            Rect[] faces = faceCascade.DetectMultiScale(img);
            int x, y, w, h;
            if (faces.Length == 0) { } //Console.WriteLine("No Face In Images");
            else
            {
                x = faces[0].Location.X;
                y = faces[0].Location.Y;
                w = faces[0].BottomRight.X - x;
                h = faces[0].BottomRight.Y - y;
               /*
                foreach (var item in faces)
                {
                    Cv2.Rectangle(img, item, Scalar.Red); // add rectangle to the image
                }*/

                Mat tmp = new Mat();
                Cv2.Resize(emoticon[num], tmp, new Size(size, size));       // 이모티콘 사이즈 변환

                Mat cut = img.SubMat(new Rect(x, y, w, h));
                Cv2.Resize(cut, cut, new Size((int)(cut.Width * 1.2), (int)(cut.Height * 1.2)));

                Mat emoti_tmp = new Mat();
                Cv2.Resize(emoticon[num], emoti_tmp, new Size(cut.Width, cut.Height));

                int w_new = cut.Width;
                int h_new = cut.Height;
                int x_new = x + w / 2 - w_new / 2;
                int y_new = y + h / 2 - h_new / 2;

                // int x_new = x + w/2;
                // int y_new = y - h/2;


                for (int i = x_new; i < x_new + w_new; i++)
                {
                    for (int j = y_new; j < y_new + h_new; j++)
                    {
                        var pt = img.At<Vec3b>(j, i);
                        var ptr2 = emoti_tmp.At<Vec3b>(j - y_new, i - x_new);

                        // 이미지 대체
                        if ((byte)ptr2.Item0 != 0) pt.Item0 = (byte)ptr2.Item0;
                        if ((byte)ptr2.Item1 != 0) pt.Item1 = (byte)ptr2.Item1;
                        if ((byte)ptr2.Item2 != 0) pt.Item2 = (byte)ptr2.Item2;

                        img.Set<Vec3b>(j, i, pt);
                    }
                }
            }

            return img;
        }
    }
}
