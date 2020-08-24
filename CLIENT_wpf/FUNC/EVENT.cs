using CLIENT_wpf.VAL;
using OpenCvSharp;
using System;

namespace CLIENT_wpf.FUNC
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
    public class EVENT
    {
        // 이모티콘 7 개
        static Mat[] emoticon = new Mat[10];
        CascadeClassifier faceCascade, eyeCascade;

        public EVENT()
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
            emoticon[7] = new Mat(dir + "AngrySticker.PNG", ImreadModes.Unchanged);
            emoticon[8] = new Mat(dir + "TearSticker.PNG", ImreadModes.Unchanged);
            emoticon[9] = new Mat(dir + "Rose.PNG", ImreadModes.Unchanged);

            String filenameFaceCascade = dir + "haarcascade_frontalface_alt.xml";
            String filenameEyeCascade = dir + "haarcascade_eye_tree_eyeglasses.xml";

            faceCascade = new CascadeClassifier();
            eyeCascade = new CascadeClassifier();
            if (!faceCascade.Load(filenameFaceCascade) || !eyeCascade.Load(filenameEyeCascade))
            {
                Console.WriteLine("Cascade Load Error!");
                return;
            }
        }

        public Mat image_conversion(Mat img, int eventnumber)
        {
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

                Mat cut = img.SubMat(new Rect(x, y, w, h));
                Rect[] eyes = eyeCascade.DetectMultiScale(cut);
                if (eyes.Length == 0) Console.WriteLine("No eyes In Image");

                Mat YCrCb = new Mat();
                Cv2.CvtColor(cut, YCrCb, ColorConversionCodes.BGR2YCrCb);
                // 피부색 범위 = Cr : 77~127 /   Cb : 133~173
                Cv2.InRange(YCrCb, new Scalar(0, 133, 77), new Scalar(255, 173, 127), YCrCb);

                Mat result = new Mat();
                // result = 얼굴부분만 color, 나머지 = 0인 image
                Cv2.BitwiseAnd(cut, YCrCb.CvtColor(ColorConversionCodes.GRAY2BGR), result);

                foreach (Rect eye in eyes)
                {
                    for (int i = eye.X; i < eye.X + eye.Width; i++)
                    {
                        for (int j = eye.Y; j < eye.Y + eye.Height; j++)
                        {
                            var pt = result.At<Vec3b>(j, i);
                            var ptr = cut.At<Vec3b>(j, i);

                            // 이미지 대체
                            pt.Item0 = (byte)ptr.Item0;
                            pt.Item1 = (byte)ptr.Item1;
                            pt.Item2 = (byte)ptr.Item2;

                            result.Set<Vec3b>(j, i, pt);
                        }
                    }
                }

                bool Use_Original = false;

                // 0~6 : 이모티콘 스티커
                if (eventnumber >= 0 && eventnumber <= 6)
                {
                    Use_Original = true;
                    Sticker(ref img, eventnumber);
                }
                else if (eventnumber == 7)
                {
                    RedMask(ref result);
                }
                else if (eventnumber == 8)
                {
                    BlueMask(ref result);
                }
                else if (eventnumber == 9)
                {
                    BigHead(ref result, cut.Width, cut.Height, ref x, ref y, ref w, ref h);
                }
                else if (eventnumber == 10)
                {
                    Use_Original = true;
                    Mosaic(ref img, x, y, w, h);
                }
                else if (eventnumber == 11)
                {
                    RedEyes(ref result, ref eyes);
                }
                else if (eventnumber == 12)
                {
                    Rotation(ref result, x, y, w, h);
                }
                else if (eventnumber == 13)
                {
                    Use_Original = true;
                    BigEyes(ref img, eyes, x, y);
                }
                else if (eventnumber == 14)
                {
                    Use_Original = true;
                    AngrySticker(ref img, x, y, w, h);
                }
                else if (eventnumber == 15)
                {
                    Use_Original = true;
                    TearSticker(ref img, eyes, x, y);
                }
                else if (eventnumber == 16)
                {
                    Use_Original = true;
                    FaceSticker(ref img, 9, ref x, ref y, ref w, ref h);
                }
                else
                {
                    Use_Original = true;
                }

                if (Use_Original == true) return img;

                ChangeImage(ref img, ref result, x, y, w, h);
            }

            return img;
        }

        public Mat ChangeImage(ref Mat original, ref Mat change, int x, int y, int w, int h)
        {
            for (int i = x; i < x + w; i++)
            {
                for (int j = y; j < y + h; j++)
                {
                    if (i <= original.Width && j <= original.Height)
                    {
                        var pt = original.At<Vec3b>(j, i);
                        var ptr2 = change.At<Vec3b>(j - y, i - x);

                        // 이미지 대체
                        if ((byte)ptr2.Item0 != 0) pt.Item0 = (byte)ptr2.Item0;
                        if ((byte)ptr2.Item1 != 0) pt.Item1 = (byte)ptr2.Item1;
                        if ((byte)ptr2.Item2 != 0) pt.Item2 = (byte)ptr2.Item2;

                        original.Set<Vec3b>(j, i, pt);
                    }
                }
            }
            return original;
        }

        public void RedMask(ref Mat img)
        {
            for (int i = 0; i < img.Rows; i++)
            {
                for (int j = 0; j < img.Cols; j++)
                {
                    var ptr = img.At<Vec3b>(j, i);

                    // 이미지 대체
                    if ((byte)ptr.Item0 != 0)
                    {
                        ptr.Item0 /= 10;
                        ptr.Item1 /= 10;
                        ptr.Item2 += 50;
                    }

                    img.Set<Vec3b>(j, i, ptr);
                }
            }
        }

        public void BlueMask(ref Mat img)
        {
            for (int i = 0; i < img.Rows; i++)
            {
                for (int j = 0; j < img.Cols; j++)
                {
                    var ptr = img.At<Vec3b>(j, i);

                    // 이미지 대체
                    if ((byte)ptr.Item0 != 0)
                    {
                        ptr.Item0 += 50;
                        ptr.Item1 /= 10;
                        ptr.Item2 /= 10;
                    }

                    img.Set<Vec3b>(j, i, ptr);
                }
            }
        }

        public void BigHead(ref Mat result, int original_width, int original_height, ref int x, ref int y, ref int w, ref int h)
        {
            Cv2.Resize(result, result, new Size((int)(original_width * 2), (int)(original_height * 2)));

            w = result.Width;
            h = result.Height;
            x = x + original_width / 2 - w / 2;
            if (x < 0) x = 0;
            y = y + original_height / 2 - h / 2;
            if (y < 0) y = 0;
        }

        public void FaceSticker(ref Mat img, int num, ref int x, ref int y, ref int w, ref int h)
        {
            Mat cut = img.SubMat(new Rect(x, y, w, h));
            Cv2.Resize(cut, cut, new Size((int)(cut.Width * 1.3), (int)(cut.Height * 1.3)));

            Mat emoti_tmp = new Mat();
            Cv2.Resize(emoticon[num], emoti_tmp, new Size(cut.Width, cut.Height));

            int w_new = cut.Width;
            int h_new = cut.Height;
            int x_new = x + w / 2 - w_new / 2;
            if (x_new < 0) x_new = 0;
            int y_new = y + h / 2 - h_new / 2;
            if (y_new < 0) y_new = 0;

            ChangeImage(ref img, ref emoti_tmp, x_new, y_new, w_new, h_new);
        }

        public void Sticker(ref Mat img, int num)
        {
            int size = 60;
            int x = img.Width - size;
            int y = img.Height - size;

            Mat emo = new Mat();
            Cv2.Resize(emoticon[num], emo, new Size(size, size));

            ChangeImage(ref img, ref emo, x, y, size, size);
        }

        public void Mosaic(ref Mat img, int x, int y, int w, int h)
        {
            Mat cut = img.SubMat(new Rect(x, y, w, h));
            Cv2.Resize(cut, cut, new Size(0, 0), 0.04, 0.04);
            Cv2.Resize(cut, cut, new Size(w, h));

            ChangeImage(ref img, ref cut, x, y, w, h);
        }

        public void RedEyes(ref Mat img, ref Rect[] eyes)
        {
            foreach (Rect eye in eyes)
            {
                int radius = Convert.ToInt32(eye.Width / 2);
                int center_x = eye.X + radius;
                int center_y = eye.Y + radius;
                Cv2.Circle(img, new Point(center_x, center_y), radius, Scalar.Red, -1, LineTypes.AntiAlias);
            }
        }

        public void Rotation(ref Mat img, int x, int y, int w, int h)
        {
            Cv2.Flip(img, img, FlipMode.X);
        }

        public void BigEyes(ref Mat img, Rect[] eyes, int x, int y)
        {
            foreach (Rect eye in eyes)
            {
                int posx = x + eye.X;
                int posy = y + eye.Y;
                Mat bigeyes = img.SubMat(new Rect(posx, posy, eye.Width, eye.Height));
                Cv2.Resize(bigeyes, bigeyes, new Size((int)(eye.Width * 1.7), (int)(eye.Height * 1.7)));

                int w_new = bigeyes.Width;
                int h_new = bigeyes.Height;
                int x_new = posx + eye.Width / 2 - w_new / 2;
                if (x_new < 0) x_new = 0;
                int y_new = posy + eye.Height / 2 - h_new / 2;
                if (y_new < 0) y_new = 0;

                ChangeImage(ref img, ref bigeyes, x_new, y_new, w_new, h_new);
            }
        }

        public void AngrySticker(ref Mat img, int x, int y, int w, int h)
        {
            Mat cut = img.SubMat(new Rect(x, y, w, h));
            Cv2.Resize(cut, cut, new Size((int)(cut.Width * 1.2), (int)(cut.Height * 1.2)));

            Mat right = new Mat();
            Cv2.Resize(emoticon[7], right, new Size((int)(w * 1.2), (int)(h * 1.2)));

            int right_x = x + w / 2;
            if (right_x < 0) right_x = 0;
            int right_y = y - right.Height / 2;
            if (right_y < 0) right_y = 0;

            Mat left = new Mat();
            Cv2.Resize(emoticon[7], left, new Size((int)(w * 0.7), (int)(h * 0.7)));

            int left_x = x - w / 2;
            if (left_x < 0) left_x = 0;
            int left_y = y + 10;
            if (left_y < 0) left_y = 0;

            ChangeImage(ref img, ref right, right_x, right_y, right.Width, right.Height);
            ChangeImage(ref img, ref left, left_x, left_y, left.Width, left.Height);
        }

        public void TearSticker(ref Mat img, Rect[] eyes, int x, int y)
        {
            foreach (Rect eye in eyes)
            {
                int size = Convert.ToInt32(eye.Width * 2);
                int posx = x + eye.X - (int)(eye.Width / 2);
                if (posx < 0) posx = 0;
                int posy = y + eye.Y + (int)(eye.Height / 2);
                if (posy < 0) posy = 0;

                Mat tear = new Mat();
                Cv2.Resize(emoticon[8], tear, new Size(size, size));

                ChangeImage(ref img, ref tear, posx, posy, size, size);
            }
        }
    }
}
