# A large portion of this code comes from the tf-learn example page:
# https://github.com/tflearn/tflearn/blob/master/examples/images/residual_network_cifar10.py

import tflearn
import numpy as np
from tflearn.data_preprocessing import ImagePreprocessing
import Data


class ModelMaker:
    # epochs, learning_rate, minibatchs, LRdecay
    def __init__(self, epochs=200, learning_rate=0.001, minibatches=128, weight_decay='momentum'):
        self.epochs = epochs
        self.learning_rate = learning_rate
        self.minibatches = minibatches
        self.weight_decay = weight_decay

    # Data loading and pre-processing
    def read_data(self):
        d = Data.Data()
        x_train, y_train, x_valid, y_valid, x_test, y_test = d.read_data()

        # Reshape the images into 48x4
        x_train = x_train.reshape([-1, 48, 48, 1])
        x_valid = x_valid.reshape([-1, 48, 48, 1])
        x_test = x_test.reshape([-1, 48, 48, 1])

        # One hot encode the labels
        # one hot encode = 집합에 하나 = 1, 나머지 = 0 으로 바꾸는 인코딩
        y_train = tflearn.data_utils.to_categorical(y_train, 7)
        y_valid = tflearn.data_utils.to_categorical(y_valid, 7)
        y_test = tflearn.data_utils.to_categorical(y_test, 7)

        return x_train, y_train, x_valid, y_valid, x_test, y_test

    def make_model(self, x_train, y_train, x_valid, y_valid, x_test, y_test):
        n = 5
        # Real-time preprocessing of the image data
        img_prep = ImagePreprocessing()
        # 지정된 평균을 가진 모든 샘플의 중심 = 0
        # 지정 x = 모든 표본에 대해 평균이 평가
        img_prep.add_featurewise_zero_center()
        # 지정된 표준편차에 따라 각 표본의 크기를 조정
        # 지정 x = 모든 샘플 데이터에 대하여 평가
        img_prep.add_featurewise_stdnorm()

        # Real-time data augmentation
        img_aug = tflearn.ImageAugmentation()
        img_aug.add_random_flip_leftright()
        # randomly applying gaussian filter / sigma = max Standard deviation
        img_aug.add_random_blur(sigma_max=5.0)
        # randomly rotate by random angle
        img_aug.add_random_rotation(max_angle=20.0)

        print('preprocessing / augmentation class make ok')

        # Building Residual Network
        net = tflearn.input_data(shape=[None, 48, 48, 1], data_preprocessing=img_prep, data_augmentation=img_aug)
        net = tflearn.conv_2d(net, nb_filter=16, filter_size=3, regularizer='L2', weight_decay=0.0001)
        # add residual layer
        net = tflearn.residual_block(net, n, 16)
        # Max pooling layer - (?, 48, 48, 16) -> (?, 24, 24, 32)
        net = tflearn.residual_block(net, 1, 32, downsample=True)
        # add residual layer
        net = tflearn.residual_block(net, n-1, 32)
        # Max pooling layer - (?, 24, 24, 32) -> (?, 12, 12, 64)
        net = tflearn.residual_block(net, 1, 64, downsample=True)
        # add residual layer
        net = tflearn.residual_block(net, n-1, 64)
        net = tflearn.batch_normalization(net)
        net = tflearn.activation(net, 'relu')
        net = tflearn.global_avg_pool(net)
        net = tflearn.dropout(net, 0.5)
        # Regression
        # softmax = 출력값을 0~1 사이로 정규화
        net = tflearn.fully_connected(net, 7, activation='softmax')
        # momentum = local minimum 을 피하기 위해, 경사하강법 속도 증가를 위해 사용하는 기법
        mom = tflearn.Momentum(learning_rate=0.1, lr_decay=0.0001, decay_step=32000, staircase=True, momentum=0.9)
        # rmsprop = tflearn.RMSProp()
        adam = tflearn.Adam(learning_rate=0.01, beta1=0.99, beta2=0.999,
                            epsilon=1e-08, use_locking=False, name='Adam')
        # adagrad = tflearn.AdaGrad()
        # adadelta = tflearn.AdaDelta()
        # loss = 최적화에 사용하는 값
        net = tflearn.regression(net, optimizer=adam,
                                 loss='categorical_crossentropy')
        # Training
        model = tflearn.DNN(net, tensorboard_dir='D:\졸작관련\Logs/', best_checkpoint_path='new_model/',
                            max_checkpoints=20, tensorboard_verbose=3,
                            clip_gradients=0.)
        print("Make Network OK!")

        # model.load('new_model/model.tfl')
        # print("Model Load OK!")

        model.fit(x_train, y_train, n_epoch=200,
                  validation_set=(x_valid, y_valid),
                  snapshot_epoch=False, snapshot_step=500,
                  show_metric=True, batch_size=128, shuffle=True, run_id='resnet_emotion')
        print("Model Fit Ok!")

        model.save('new_model/model.tfl')
        print("Model Save OK!")

        model.load('new_model/model.tfl')
        score = model.evaluate(x_test, y_test)
        print('Test accuarcy: ', score)

        # prediction = model.predict(predict_value)
        # print(prediction)


if __name__ == '__main__':
    # make_csv = Data.Data()
    # make_csv.data_split()
    mm = ModelMaker()
    # Data loading and pre-processing
    x_train, y_train, x_valid, y_valid, x_test, y_test = mm.read_data()
    print('read data ok!')
    mm.make_model(x_train, y_train, x_valid, y_valid, x_test, y_test)
