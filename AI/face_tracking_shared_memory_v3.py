import numpy as np
import cv2
import tflearn
import tensorflow as tf
import operator
from tflearn.data_preprocessing import ImagePreprocessing
from collections import deque
import mmap
import os
import warnings


warnings.filterwarnings(action='ignore')
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
tf.logging.set_verbosity(tf.logging.ERROR)


class EmotionRecognition:
    def __init__(self, key):
        # Create emotion queue of last 'x' emotions to smooth the output
        self.emotion_queue = deque(maxlen=10)
        self.n = 5
        self.model = None
        self.memory_key = key
        print("Control memory name : ", self.memory_key)

    def smooth_emotions(self, prediction):
        emotions = ["Angry", "Disgust", "Fear", "Happy", "Sad", "Surprise", "Neutral"]
        emotion_values = {'Angry': 0.0, 'Disgust': 0.0, 'Fear': 0.0, 'Happy': 0.0, 'Sad': 0.0, 'Surprise': 0.0,
                          'Neutral': 0.0}

        emotion_probability, emotion_index = max((val, idx) for (idx, val) in enumerate(prediction[0]))
        emotion = emotions[emotion_index]

        # Append the new emotion and if the max length is reached pop the oldest value out
        self.emotion_queue.appendleft((emotion_probability, emotion))

        # Iterate through each emotion in the queue and create an average of the emotions
        for pair in self.emotion_queue:
            emotion_values[pair[1]] += pair[0]

        # Select the current emotion based on the one that has the highest value
        average_emotion = max(emotion_values.items(), key=operator.itemgetter(1))[0]

        return average_emotion

    def process_image(self, roi_gray):
        image_scaled = np.array(cv2.resize(roi_gray, (48, 48)), dtype=float)
        image_processed = image_scaled.flatten()
        image_processed = image_processed.reshape([-1, 48, 48, 1])

        # prediction = self.model.predict(image_processed)
        prediction = self.model.predict(image_processed)
        print("prediction = ", prediction)
        emotion = self.smooth_emotions(prediction)

        emotions = ["Angry", "Disgust", "Fear", "Happy", "Sad", "Surprise", "Neutral"]
        emotion_index = emotions.index(emotion)

        return emotion_index

    def run(self):
        # Real-time pre-processing of the image data
        img_prep = ImagePreprocessing()
        img_prep.add_featurewise_zero_center()
        img_prep.add_featurewise_stdnorm()
        # Real-time data augmentation
        img_aug = tflearn.ImageAugmentation()
        img_aug.add_random_flip_leftright()
        # img_aug.add_random_crop([48, 48], padding=8)
        print("Making Pre-processing model ok / Waiting Making ResNet...")
        # Building Residual Network
        net = tflearn.input_data(shape=[None, 48, 48, 1], data_preprocessing=img_prep, data_augmentation=img_aug)
        net = tflearn.conv_2d(net, nb_filter=16, filter_size=3, regularizer='L2', weight_decay=0.0001)
        net = tflearn.residual_block(net, self.n, 16)
        net = tflearn.residual_block(net, 1, 32, downsample=True)
        net = tflearn.residual_block(net, self.n - 1, 32)
        net = tflearn.residual_block(net, 1, 64, downsample=True)
        net = tflearn.residual_block(net, self.n - 1, 64)
        net = tflearn.batch_normalization(net)
        net = tflearn.activation(net, 'relu')
        net = tflearn.global_avg_pool(net)
        print("Making ResNet ok / Waiting Load Model...")
        # Regression
        net = tflearn.fully_connected(net, 7, activation='softmax')
        # mom = tflearn.Momentum(learning_rate=0.1, lr_decay=0.0001, decay_step=32000, staircase=True, momentum=0.9)
        adam = tflearn.Adam(learning_rate=0.01, beta1=0.99, beta2=0.999,
                            epsilon=1e-08, use_locking=False, name='Adam')
        net = tflearn.regression(net, optimizer=adam,
                                 loss='categorical_crossentropy')

        # checkpoint_path = 'models/model_resnet_emotion'
        self.model = tflearn.DNN(net, checkpoint_path='model_adam',
                            max_checkpoints=10, tensorboard_verbose=0,
                            clip_gradients=0.)
        self.model.load('model_adam/model.tfl')

        print("Model Load ok / Waiting Connect...")
        face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

        # Shared Memory
        shm_list = []
        shm_control = mmap.mmap(0, 230410, self.memory_key)
        print("connect control shared memory ok")
        print("Start!! Please Wait New Client's Connect")

        while True:
            shm_control.seek(0)
            # print("shm_control = ", shm_control.read())
            # print(shm_control[0:4], shm_control[0], shm_control[0:1])
            if shm_control[0:1] == b'0':
                # print("control shared memory = 0")
                pass
            elif shm_control[0:1] == b'1':
                # 0 으로 trigger 바꾸고 감정 값 추가
                # print("control shared memory = 1")
                emotion_list = []
                for shm_in_for in shm_list:
                    shm_in_for.seek(0)
                    # Decode binary to numpy.ndarray
                    nparr = np.frombuffer(shm_in_for.read(), np.int8)
                    img = cv2.imdecode(nparr, cv2.IMREAD_UNCHANGED)
                    if img is None:
                        # 이미지 없을 때 예외처리
                        emotion_list.append(8)
                        continue
                    else:
                        img = cv2.flip(img, 1)
                        # print("img shape = ", img.shape)
                        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
                        faces = face_cascade.detectMultiScale(gray, 1.3, 5)
                        # print("faces = ", faces)
                        if faces is ():
                            emotion_list.append(7)
                        else:
                            for (x, y, w, h) in faces:
                                # 얼굴부분 자름
                                roi_gray = gray[y:y + h, x:x + w]
                                # 감정인식 결과값 숫자로 반환
                                emotion_list.append(self.process_image(roi_gray))
                # result 양식 : 0, <1-result>, <2-result>~
                write_result = "0,"
                for one in emotion_list:
                    # 마지막 결과값일때 ~ 추가
                    if one is emotion_list[-1]:
                        write_result += str(one) + ",-"
                    else:
                        write_result += str(one) + ","
                write_result += "-"
                # print("shmlist", shm_list)
                # print("emotion_list = ", emotion_list)
                # print("write_result = ", write_result)
                # change trigger to 0
                shm_control.seek(0)
                shm_control.write(bytes(write_result, 'UTF-8'))
            elif shm_control[0:1] == b'2':
                # 0 으로 바꾸고 append shm mmap
                # name 양식 = "I00_0"
                shm_name = shm_control[2:7].decode("utf-8")
                print("new connect !!! name = ", shm_name)
                # shm_list.append(mmap.mmap(0, 240 * 320 * 3, shm_name))
                shm_list.append(mmap.mmap(0, 230410, shm_name))
                # 0 으로 바꾸지 말고 다른걸로
                shm_control.write(bytes('1', 'UTF-8'))
            elif shm_control[0:1] == b'3':
                shm_control.seek(0)
                shm_control.write(bytes('0,-', 'UTF-8'))
            elif shm_control[0:1] == b'9':
                return
            else:
                # print("Trigger is not 0~3")
                pass


if __name__ == "__main__":
    emotion_recognition = EmotionRecognition(sys.argv[1])
    emotion_recognition.run()
    print("end")
