import os
import pickle
import numpy as np

class data_utils:
    def __init__(self, data_path, num_training = 49000, num_validation = 1000, num_test = 1000):
        self.data_path = data_path
        self.num_training = num_training
        self.num_validation = num_validation
        self.num_test = num_test

    def NormalizeData(self, data):
        return (data - np.min(data)) / (np.max(data) - np.min(data))

    def load_cifar_10_batch(self, filename):
        with open(filename, 'rb') as fo:
            data = pickle.load(fo, encoding='latin1')
            #data = pickle.load(fo)
            X = data['data']
            Y = data['labels']
            X = X.reshape(10000, 3, 32, 32).astype('float32')
            Y = np.asarray(Y)
        return X, Y

    def load_cifar_10(self):
        X = []
        Y = []
        for i in range(1, 6):
            filename = os.path.join(self.data_path, 'data_batch_%d' % (i, ))
            Xx, Yy = self.load_cifar_10_batch(filename)
            X.append(Xx)
            Y.append(Yy)
        X_train = np.concatenate(X)
        # mean_image = np.mean(X_train, axis=0)
        # X_train -= mean_image
        X_train = self.NormalizeData(X_train)
        Y_train = np.concatenate(Y)
        X_test, Y_test = self.load_cifar_10_batch(os.path.join(self.data_path, 'test_batch'))
        # X_test -= mean_image
        X_test = self.NormalizeData(X_test)
        return X_train, Y_train, X_test, Y_test
    
    def data_preprocessing(self):
        X_train, Y_train, X_test, Y_test = self.load_cifar_10()
        X_valid = X_train[(range(self.num_training, self.num_training + self.num_validation))]
        Y_valid = Y_train[(range(self.num_training, self.num_training + self.num_validation))]
        X_train = X_train[(range(self.num_training))]
        Y_train = Y_train[(range(self.num_training))]
        X_test = X_test[(range(self.num_test))]
        Y_test =  Y_test[(range(self.num_test))]

        dict = {
            'X_train': X_train, 'Y_train': Y_train,
            'X_valid': X_valid, 'Y_valid': Y_valid,
            'X_test': X_test, 'Y_test': Y_test
        }
        return dict


if __name__ == '__main__':
    dataset = data_utils('./dataset_cifar-10')
    data = dataset.data_preprocessing()
    for key, value in data.items():
        print('%s:' % key, value.shape)