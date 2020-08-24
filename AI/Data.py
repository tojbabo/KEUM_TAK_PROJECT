import numpy as np
import os
import csv
from numpy import genfromtxt


class Data:
    def __init__(self):
        self.path = 'C:/Users/yts02/PycharmProjects/Project/venv/data/'
        self.file = self.path + 'icml_face_data.csv'

    def data_set_split(self):
        if os.path.isfile(self.file):
            f = open(self.file, 'r')
        else:
            return

        if os.path.isfile(self.path + 'train_set.csv'):
            return

        rr = csv.reader(f)
        else_set = []
        f1 = open(self.path + 'train_set.csv', 'w', newline='')
        wr1 = csv.writer(f1)
        f2 = open(self.path + 'validation_set.csv', 'w', newline='')
        wr2 = csv.writer(f2)
        f3 = open(self.path + 'test_set.csv', 'w', newline='')
        wr3 = csv.writer(f3)
        for row in rr:
            if row[1] == 'Training':
                wr1.writerow([row[2], row[0]])
            elif row[1] == 'PrivateTest':
                wr2.writerow([row[2], row[0]])
            elif row[1] == 'PublicTest':
                wr3.writerow([row[2], row[0]])
            else:
                else_set.append([row[0], row[2]])

    def data_train_target_split(self):
        if not os.path.isfile(self.path + 'train_set.csv'):
            return

        if os.path.isfile(self.path + 'train_data.csv'):
            return

        f1 = open(self.path + 'train_set.csv', 'r', newline='')
        wr1 = csv.reader(f1)
        f2 = open(self.path + 'validation_set.csv', 'r', newline='')
        wr2 = csv.reader(f2)
        f3 = open(self.path + 'test_set.csv', 'r', newline='')
        wr3 = csv.reader(f3)

        x_train = []
        y_train = []
        x_valid = []
        y_valid = []
        x_test = []
        y_test = []
        for row in wr1:
            x_train.append(row[0])
            y_train.append(row[1])
        for row in wr2:
            x_valid.append(row[0])
            y_valid.append(row[1])
        for row in wr3:
            x_test.append(row[0])
            y_test.append(row[1])

        f1.close()
        f2.close()
        f3.close()

        f1 = open(self.path + 'train_data.csv', 'w', newline='')
        wr1 = csv.writer(f1)
        f2 = open(self.path + 'train_label.csv', 'w', newline='')
        wr2 = csv.writer(f2)
        f3 = open(self.path + 'validation_data.csv', 'w', newline='')
        wr3 = csv.writer(f3)
        f4 = open(self.path + 'validation_label.csv', 'w', newline='')
        wr4 = csv.writer(f4)
        f5 = open(self.path + 'test_data.csv', 'w', newline='')
        wr5 = csv.writer(f5)
        f6 = open(self.path + 'test_label.csv', 'w', newline='')
        wr6 = csv.writer(f6)

        for i in range(0, len(x_train)):
            wr1.writerow(x_train[i])
            wr2.writerow(y_train[i])

        for i in range(0, len(x_valid)):
            wr3.writerow(x_valid[i])
            wr4.writerow(y_valid[i])

        for i in range(0, len(x_test)):
            wr5.writerow(x_test[i])
            wr6.writerow(y_test[i])

    def read_data(self):
        x_train = np.asarray(genfromtxt('data/train_data.csv', delimiter=' ',  skip_header=1,  dtype=float))
        # print("train X = ", X)
        y_train = np.asarray(genfromtxt('data/train_label.csv', delimiter=',', skip_header=1, dtype=int))
        # print("train Y = ", Y)
        x_valid = np.asarray(genfromtxt('data/validation_data.csv', delimiter=' ',  skip_header=1,  dtype=float))
        # print("train X = ", X)
        y_valid = np.asarray(genfromtxt('data/validation_label.csv', delimiter=',', skip_header=1, dtype=int))
        # print("train Y = ", Y)
        x_test = np.asarray(genfromtxt('data/test_data.csv', delimiter=' ',  skip_header=1,  dtype=float))
        # print("test X = ", X_test)
        y_test = np.asarray(genfromtxt('data/test_label.csv', delimiter=',', skip_header=1, dtype=int))
        # print("test Y = ", Y_test)

        return x_train, y_train, x_valid, y_valid, x_test, y_test


if __name__ == '__main__':
    data = Data()
    data.data_set_split()
    data.data_train_target_split()
    x_train, y_train, x_valid, y_valid, x_test, y_test = data.read_data()
    print(x_train)
    print(y_train)
