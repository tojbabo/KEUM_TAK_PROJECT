# A large portion of this code comes from the tf-learn example page:
# https://github.com/tflearn/tflearn/blob/master/examples/images/residual_network_cifar10.py

import tflearn
import numpy as np
from numpy import genfromtxt
from tflearn.data_preprocessing import ImagePreprocessing


if __name__ == "__main__":
    n = 5

    # Data loading and pre-processing
    X = np.asarray(genfromtxt('data/train_data.csv', delimiter=' ',  skip_header=1,  dtype=float))
    print("train X = ", X)
    Y = np.asarray(genfromtxt('data/train_label.csv', delimiter=',', skip_header=1, dtype=int))
    print("train Y = ", Y)

    X_test = np.asarray(genfromtxt('data/test_data.csv', delimiter=' ',  skip_header=1,  dtype=float))
    print("test X = ", X_test)
    Y_test = np.asarray(genfromtxt('data/test_label.csv', delimiter=',', skip_header=1, dtype=int))
    print("test Y = ", Y_test)

    # 모델 제작 자체에는 predict data 가 필요없음
    # predict_value = np.asarray(genfromtxt('data/test_data.csv', delimiter=' ', skip_header=1, dtype=float))
    # predict_value = predict_value.reshape([-1, 48, 48, 1])

    # Reshape the images into 48x4
    X = X.reshape([-1, 48, 48, 1])
    X_test = X_test.reshape([-1, 48, 48, 1])

    # One hot encode the labels
    # one hot encode = 집합에 하나 = 1, 나머지 = 0 으로 바꾸는 인코딩
    Y = tflearn.data_utils.to_categorical(Y, 7)
    Y_test = tflearn.data_utils.to_categorical(Y_test, 7)

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

    # Building Residual Network
    net = tflearn.input_data(shape=[None, 48, 48, 1], data_preprocessing=img_prep, data_augmentation=img_aug)
    net = tflearn.conv_2d(net, nb_filter=16, filter_size=3, regularizer='L2', weight_decay=0.0001)
    net = tflearn.residual_block(net, n, 16)
    net = tflearn.residual_block(net, 1, 32, downsample=True)
    net = tflearn.residual_block(net, n-1, 32)
    net = tflearn.residual_block(net, 1, 64, downsample=True)
    net = tflearn.residual_block(net, n-1, 64)
    net = tflearn.batch_normalization(net)
    net = tflearn.activation(net, 'relu')
    net = tflearn.global_avg_pool(net)

    # Regression
    # softmax = 출력값을 0~1 사이로 정규화
    net = tflearn.fully_connected(net, 7, activation='softmax')
    # momentum = local minimum 을 피하기 위해, 경사하강법 속도 증가를 위해 사용하는 기법
    mom = tflearn.Momentum(learning_rate=0.1, lr_decay=0.0001, decay_step=32000, staircase=True, momentum=0.9)
    # loss = 최적화에 사용하는 값
    net = tflearn.regression(net, optimizer=mom,
                             loss='categorical_crossentropy')
    print("Regression OK!")
    # Training
    model = tflearn.DNN(net, checkpoint_path='new_model/',
                        max_checkpoints=20, tensorboard_verbose=0,
                        clip_gradients=0.)
    print("Training OK! Start Save Model")

    model.load('new_model/model.tfl')
    print("Model Load OK!")

    model.fit(X, Y, n_epoch=150, snapshot_epoch=False, snapshot_step=500,
              show_metric=True, batch_size=128, shuffle=True, run_id='resnet_emotion')

    print("Model Fit Ok!")

    score = model.evaluate(X_test, Y_test)
    print('Test accuarcy: ', score)

    model.save('new_model/model.tfl')
    print("Model Save OK!")
    # prediction = model.predict(predict_value)
    # print(prediction)
