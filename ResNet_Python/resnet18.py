import numpy as np
#from conv import convolution
from fast_conv import convolution
#from batch_norm_conv import batch_norm_conv
from fast_batch_norm_conv import batch_norm_conv
from relu import relu
from basic_block import basic_block
from global_average_pooling import GAP
from full_connected import full_connected
from softmax_loss import softmax_loss
from optimize_method import *

class resnet18_cifar10:
    def __init__(self, X_train, y_train, X_val, y_val, learning_rate, weight_scale=5e-2, batch_size=100, val_size=25):

        # convolution(x, w, b, stride=1, pad=0)
        # batch_norm_conv(x, gamma, beta, mode="train", eps=1e-5, momentum=0.9)
        # relu(x)
        # basic_block(x, conv_size, weight_scale=1e-2, feature_size=3, stride1=1, pad1=1, stride2=1, pad2=1, mode="train")
        # full_connected(x, w, b)
        # softmax_loss(x, y)

        self.X_train = X_train      # (N, C, H, W)
        self.y_train = y_train      # (N,)
        self.X_val = X_val          # (N_2, C, H, W)
        self.y_val = y_val          # (N_2,)
        self.learning_rate = learning_rate

        stride = 2
        pad = 1
        conv_size = 3
        filter_size_1 = 16
        filter_size_2 = 32
        filter_size_3 = 64
        class_size = 10
        self.batch_size = batch_size
        self.val_size = val_size
        self.resnet_grad_dict = {}
        self.val_best_acc = 0.0
        self.best_model = None

        N, C, H_0, W_0 = X_train.shape
        self.N = N
        self.N_2, _, _, _ = X_val.shape
        H_1 = int(1 + (H_0 + 2 * pad - conv_size) / stride)
        W_1 = int(1 + (W_0 + 2 * pad - conv_size) / stride)
        H_2 = int(1 + (H_1 + 2 * pad - conv_size) / stride)
        W_2 = int(1 + (W_1 + 2 * pad - conv_size) / stride)

        self.w_conv1 = np.random.normal(loc=0, scale=weight_scale, size=(filter_size_1, C, conv_size, conv_size))
        self.b_conv1 = np.random.normal(loc=0, scale=weight_scale, size=(filter_size_1,))
        self.w_bn1 = np.ones(shape=(filter_size_1))
        self.b_bn1 = np.zeros(shape=(filter_size_1))
        # self.w_bn1 = np.ones(shape=(filter_size_1*H_0*W_0))
        # self.b_bn1 = np.zeros(shape=(filter_size_1*H_0*W_0))

        self.w_fc12 = np.random.normal(loc=0, scale=weight_scale, size=(class_size, filter_size_3))
        self.b_fc12 = np.zeros(shape=(class_size,))

        dummy_in = np.zeros(shape=(self.batch_size, C, H_0, W_0))
        dummy_x1 = np.zeros(shape=(self.batch_size, filter_size_1, H_0, W_0))
        dummy_x2 = np.zeros(shape=(self.batch_size, filter_size_2, H_1, W_1))
        dummy_x3 = np.zeros(shape=(self.batch_size, filter_size_3, H_2, W_2))
        dummy_x4 = np.zeros(shape=(self.batch_size, filter_size_3, 1, 1))
        dummy_x5 = np.zeros(shape=(self.batch_size, class_size))
        dummy_y = np.zeros(shape=(self.batch_size,))

        self.config_conv1 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=(filter_size_1, C, conv_size, conv_size)), 'b_velocity': np.zeros(shape=(filter_size_1,))}
        self.config_bn1 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=(filter_size_1,)), 'b_velocity': np.zeros(shape=(filter_size_1,))}
        self.config_fc12 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=(class_size, filter_size_3)), 'b_velocity': np.zeros(shape=(filter_size_3,))}

        self.conv_1 = convolution(dummy_in, self.w_conv1, self.b_conv1, stride=1, pad=1)                                                       # out: (N, F_0, H_0, W_0)
        self.batch_norm_conv1 = batch_norm_conv(dummy_x1, self.w_bn1, self.b_bn1)
        self.relu_1 = relu(dummy_x1)
        
        self.basic_block_2 = basic_block(dummy_x1, filter_size=filter_size_1, batch_size=batch_size)
        self.basic_block_3 = basic_block(dummy_x1, filter_size=filter_size_1, batch_size=batch_size)
        self.basic_block_4 = basic_block(dummy_x1, filter_size=filter_size_2, stride1=stride, pad1=pad, batch_size=batch_size)                 # out: (N, F_1, H_1, W_1)

        self.basic_block_5 = basic_block(dummy_x2, filter_size=filter_size_2, batch_size=batch_size)                                           
        self.basic_block_6 = basic_block(dummy_x2, filter_size=filter_size_2, batch_size=batch_size)
        self.basic_block_7 = basic_block(dummy_x2, filter_size=filter_size_3, stride1=stride, pad1=pad, batch_size=batch_size)                 # out: (N, F_2, H_2, W_2)

        self.basic_block_8 = basic_block(dummy_x3, filter_size=filter_size_3, batch_size=batch_size)                                           
        self.basic_block_9 = basic_block(dummy_x3, filter_size=filter_size_3, batch_size=batch_size)
        self.basic_block_10 = basic_block(dummy_x3, filter_size=filter_size_3, batch_size=batch_size)

        self.global_average_pooling_11 = GAP(dummy_x3)                                                                                         # out: (N, 1)
        self.full_connected_12 = full_connected(dummy_x4, self.w_fc12, self.b_fc12)                                                            # out: (N, class_size)
        self.softmax_loss_13 = softmax_loss(dummy_x5, dummy_y)                                                                                 # loss: (scaler)

    def resnet18_forward(self, batch_X, batch_y=None, mode="train"):             # batch_X: (batch_size, C, H, W), batch_y:(batch_size,)
        # print("input")
        # print(batch_X.shape)
        # print("layer 1: convolution - forward")
        conv_1_forward = self.conv_1.conv_forward_pass(batch_X)       # out: (N, F_0, H_0, W_0)
        # print(conv_1_forward.shape)
        batch_norm_1_forward = self.batch_norm_conv1.batchnorm_conv_forward_pass(conv_1_forward)
        # print(batch_norm_1_forward.shape)
        relu_1_forward = self.relu_1.relu_forward_pass(batch_norm_1_forward)
        # print("layer 2: basic_block - forward")
        # print(relu_1_forward.shape)
        block_2_forward = self.basic_block_2.basic_block_forward(relu_1_forward, mode=mode)
        # print("layer 3: basic_block - forward")
        # print(block_2_forward.shape)
        block_3_forward = self.basic_block_3.basic_block_forward(block_2_forward, mode=mode)
        # print("layer 4: basic_block - forward")
        # print(block_3_forward.shape)
        block_4_forward = self.basic_block_4.basic_block_forward(block_3_forward, mode=mode)                       # out: (N, F_1, H_1, W_1)

        # print("layer 5: basic_block - forward")
        # print(block_4_forward.shape)
        block_5_forward = self.basic_block_5.basic_block_forward(block_4_forward, mode=mode)                       
        # print("layer 6: basic_block - forward")
        # print(block_5_forward.shape)
        block_6_forward = self.basic_block_6.basic_block_forward(block_5_forward, mode=mode)
        # print("layer 7: basic_block - forward")
        # print(block_6_forward.shape)
        block_7_forward = self.basic_block_7.basic_block_forward(block_6_forward, mode=mode)                       # out: (N, F_2, H_2, W_2)

        # print("layer 8: basic_block - forward")
        # print(block_7_forward.shape)
        block_8_forward = self.basic_block_8.basic_block_forward(block_7_forward, mode=mode)                       
        # print("layer 9: basic_block - forward")
        # print(block_8_forward.shape)
        block_9_forward = self.basic_block_9.basic_block_forward(block_8_forward, mode=mode)
        # print("layer 10: basic_block - forward")
        # print(block_9_forward.shape)
        block_10_forward = self.basic_block_10.basic_block_forward(block_9_forward, mode=mode)

        # print("layer 11: global_average_pooling - forward")
        # print(block_10_forward.shape)
        gap_11_forward = self.global_average_pooling_11.GAP_forward_pass(block_10_forward)                            # out: (N, 1)
        # print("layer 12: full_connected - forward")
        # print(gap_11_forward.shape)
        fc_12_forward = self.full_connected_12.full_connected_forward(gap_11_forward)                              # out: (N, class_size)
        #print(fc_12_forward)
        if (mode == 'train'):
            # print("layer 13: softmax - loss")
            # print(fc_12_forward.shape)
            loss = self.softmax_loss_13.softmax_loss_forward(fc_12_forward, batch_y)                               # loss: (scaler)
            print(self.softmax_loss_13.out)
            return loss
        elif (mode == 'test'):
            return fc_12_forward

    def resnet18_backward(self):
        #print("layer 13: softmax - backward")
        softmax_13_backward = self.softmax_loss_13.softmax_loss_backward()
        #print(softmax_13_backward.shape)
        #print("layer 12: full_connected - backward")
        fc_12_backward, dw_fc_12, db_fc_12 = self.full_connected_12.full_connected_backward(softmax_13_backward)
        #print(fc_12_backward.shape)
        #print("layer 11: global_average_pooling - backward")
        gap_11_backward = self.global_average_pooling_11.GAP_backward_pass(fc_12_backward)

        #print(gap_11_backward)
        #print("layer 10: basic_block - backward")
        block_10_backward, block_10_grad_dict = self.basic_block_10.basic_block_backward(gap_11_backward)
        #print(block_10_backward.shape)
        #print("layer 9: basic_block - backward")
        block_9_backward, block_9_grad_dict = self.basic_block_9.basic_block_backward(block_10_backward)
        #print(block_9_backward.shape)
        #print("layer 8: basic_block - backward")
        block_8_backward, block_8_grad_dict = self.basic_block_8.basic_block_backward(block_9_backward)

        #rint(block_8_backward.shape)
        #print("layer 7: basic_block - backward")
        block_7_backward, block_7_grad_dict = self.basic_block_7.basic_block_backward(block_8_backward)
        #print(block_7_backward.shape)
        #print("layer 6: basic_block - backward")
        block_6_backward, block_6_grad_dict = self.basic_block_6.basic_block_backward(block_7_backward)
        #print(block_6_backward.shape)
        #print("layer 5: basic_block - backward")
        block_5_backward, block_5_grad_dict = self.basic_block_5.basic_block_backward(block_6_backward)

        #print(block_5_backward.shape)
        #print("layer 4: basic_block - backward")
        block_4_backward, block_4_grad_dict = self.basic_block_4.basic_block_backward(block_5_backward)
        #print(block_4_backward.shape)
        #print("layer 3: basic_block - backward")
        block_3_backward, block_3_grad_dict = self.basic_block_3.basic_block_backward(block_4_backward)
        #print(block_3_backward.shape)
        #print("layer 2: basic_block - backward")
        block_2_backward, block_2_grad_dict = self.basic_block_2.basic_block_backward(block_3_backward)
    
        #print(block_2_backward.shape)
        #print("layer 1: convolution - backward")
        relu_1_backward = self.relu_1.relu_backward_pass(block_2_backward)
        bn_1_backward, dw_bn_1, db_bn_1 = self.batch_norm_conv1.batchnorm_conv_backward_pass(relu_1_backward)
        dscores, dw_conv_1, db_conv_1 = self.conv_1.conv_backward_pass(bn_1_backward)

        #print(dscores.shape)

        self.resnet_grad_dict['dw_fc_12'] = dw_fc_12
        self.resnet_grad_dict['db_fc_12'] = db_fc_12
        self.resnet_grad_dict['dw_bn_1'] = dw_bn_1
        self.resnet_grad_dict['db_bn_1'] = db_bn_1
        self.resnet_grad_dict['dw_conv_1'] = dw_conv_1
        self.resnet_grad_dict['db_conv_1'] = db_conv_1
        self.resnet_grad_dict['block_10_grad_dict'] = block_10_grad_dict
        self.resnet_grad_dict['block_9_grad_dict'] = block_9_grad_dict
        self.resnet_grad_dict['block_8_grad_dict'] = block_8_grad_dict
        self.resnet_grad_dict['block_7_grad_dict'] = block_7_grad_dict
        self.resnet_grad_dict['block_6_grad_dict'] = block_6_grad_dict
        self.resnet_grad_dict['block_5_grad_dict'] = block_5_grad_dict
        self.resnet_grad_dict['block_4_grad_dict'] = block_4_grad_dict
        self.resnet_grad_dict['block_3_grad_dict'] = block_3_grad_dict
        self.resnet_grad_dict['block_2_grad_dict'] = block_2_grad_dict

        return dscores

    def _step(self, grad_dict):
        dw_fc_12 = grad_dict['dw_fc_12']
        db_fc_12 = grad_dict['db_fc_12']
        dw_bn_1 = self.resnet_grad_dict['dw_bn_1']
        db_bn_1 = self.resnet_grad_dict['db_bn_1']
        dw_conv_1 = grad_dict['dw_conv_1']
        db_conv_1 = grad_dict['db_conv_1']
        block_10_grad_dict = grad_dict['block_10_grad_dict']
        block_9_grad_dict = grad_dict['block_9_grad_dict']
        block_8_grad_dict = grad_dict['block_8_grad_dict']
        block_7_grad_dict = grad_dict['block_7_grad_dict']
        block_6_grad_dict = grad_dict['block_6_grad_dict']
        block_5_grad_dict = grad_dict['block_5_grad_dict']
        block_4_grad_dict = grad_dict['block_4_grad_dict']
        block_3_grad_dict = grad_dict['block_3_grad_dict']
        block_2_grad_dict = grad_dict['block_2_grad_dict']


        # self.w_fc12, self.b_fc12, config_temp3 = sgd_momentum(self.w_fc12, dw_fc_12, self.b_fc12, db_fc_12, "full_connected", learning_rate=self.learning_rate, config=self.config_fc12)
        # self.config_fc12['w_velocity'] = config_temp3['w_velocity']
        # self.config_fc12['b_velocity'] = config_temp3['b_velocity']        
        self.w_fc12, self.b_fc12, config_temp2 = sgd(self.w_fc12, dw_fc_12, self.b_fc12, db_fc_12, "full_connected", learning_rate=self.learning_rate, config=self.config_fc12)
        self.basic_block_10.optimize_basic_block(block_10_grad_dict, self.learning_rate)
        self.basic_block_9.optimize_basic_block(block_9_grad_dict, self.learning_rate)
        self.basic_block_8.optimize_basic_block(block_8_grad_dict, self.learning_rate)
        self.basic_block_7.optimize_basic_block(block_7_grad_dict, self.learning_rate)
        self.basic_block_6.optimize_basic_block(block_6_grad_dict, self.learning_rate)
        self.basic_block_5.optimize_basic_block(block_5_grad_dict, self.learning_rate)
        self.basic_block_4.optimize_basic_block(block_4_grad_dict, self.learning_rate)
        self.basic_block_3.optimize_basic_block(block_3_grad_dict, self.learning_rate)
        self.basic_block_2.optimize_basic_block(block_2_grad_dict, self.learning_rate)
        
        # self.w_conv1, self.b_conv1, config_temp = sgd_momentum(self.w_conv1, dw_conv_1, self.b_conv1, db_conv_1, "conv", learning_rate=self.learning_rate, config=self.config_conv1)
        # self.config_conv1['w_velocity'] = config_temp['w_velocity']
        # self.config_conv1['b_velocity'] = config_temp['b_velocity']
        self.w_conv1, self.b_conv1, config_temp = sgd(self.w_conv1, dw_conv_1, self.b_conv1, db_conv_1, "conv", learning_rate=self.learning_rate, config=self.config_conv1)

        # self.w_bn1, self.b_bn1, config_temp2 = sgd_momentum(self.w_bn1, dw_bn_1, self.b_bn1, db_bn_1, "batch_norm_conv", learning_rate=self.learning_rate, config=self.config_bn1)
        # self.config_bn1['w_velocity'] = config_temp2['w_velocity']
        # self.config_bn1['b_velocity'] = config_temp2['b_velocity']
        self.w_bn1, self.b_bn1, config_temp2 = sgd(self.w_bn1, dw_bn_1, self.b_bn1, db_bn_1, "batch_norm_conv", learning_rate=self.learning_rate, config=self.config_bn1)

        self.conv_1.weight = self.w_conv1
        self.conv_1.bias = self.b_conv1
        self.batch_norm_conv1.gamma = self.w_bn1
        self.batch_norm_conv1.beta = self.b_bn1
        self.full_connected_12.w = self.w_fc12
        self.full_connected_12.b = self.b_fc12

    def check_accuracy(self, scores, batch_y):
        N, M = scores.shape
        correct_pred = 0
        for n in range(N):
            temp = -1
            for m in range(M):
                if (scores[n, m] > temp):
                    y_pred = m
                    temp = scores[n, m]
            if (y_pred == batch_y[n]):
                correct_pred = correct_pred + 1
        accuracy = correct_pred / N
        # print(scores)
        # print(batch_y)
        # print(accuracy)
        return accuracy

    def construct_batch(self):
        N, C, H, W = self.X_train.shape
        X_train_temp = np.zeros(shape=(self.batch_size, C, H, W))
        y_train_temp = np.zeros(shape=(self.batch_size,))
        X_val_temp = np.zeros(shape=(self.batch_size, C, H, W))
        y_val_temp = np.zeros(shape=(self.batch_size,))
        X_train_batch = {}
        y_train_batch = {}
        X_val_batch = {}
        y_val_batch = {}

        # for n in range(int(self.N / self.batch_size)):
        #     for e in range(self.batch_size):
        #         y_train_temp[e] = self.y_train[n*100+e]
        #         for c in range(C):
        #             for h in range(H):
        #                 for w in range(W):
        #                     X_train_temp[e, c, h, w] = self.X_train[n*100+e, c, h, w]
        #     X_train_batch['batch_%d' % (n)] = X_train_temp
        #     y_train_batch['batch_%d' % (n)] = y_train_temp
        #     print("Constructing train_batch %d" % (n))

        # for n in range(int(self.N_2 / self.batch_size)):
        #     for e in range(self.batch_size):
        #         y_val_temp[e] = self.y_val[n*100+e]
        #         for c in range(C):
        #             for h in range(H):
        #                 for w in range(W):
        #                     X_val_temp[e, c, h, w] = self.X_val[n*100+e, c, h, w]
        #     X_val_batch['batch_%d' % (n)] = X_val_temp
        #     y_val_batch['batch_%d' % (n)] = y_val_temp
        #     print("Constructing test_batch %d" % (n))

        for n in range(int(self.N / self.batch_size)):
            X_train_batch['batch_%d' % (n)] = self.X_train[n*self.batch_size:(n+1)*self.batch_size, :, :,:]
            y_train_batch['batch_%d' % (n)] = self.y_train[n*self.batch_size:(n+1)*self.batch_size]
            print("Constructing train_batch %d" % (n))
        for n in range(int(self.N_2 / self.batch_size)):
            X_val_batch['batch_%d' % (n)] = self.X_val[n*self.batch_size:(n+1)*self.batch_size, :, :,:]
            y_val_batch['batch_%d' % (n)] = self.y_val[n*self.batch_size:(n+1)*self.batch_size]
            print("Constructing test_batch %d" % (n))

        # print('Validate Data Slicing....')
        # print("X_train: %r" % (self.X_train[207, 2, 1, 1] == X_train_batch['batch_2'][7, 2, 1, 1]))
        # print("y_train: %r" % (self.y_train[133] == y_train_batch['batch_1'][33]))
        # print("X_train: %r" % (self.X_val[88, 1, 6, 17] == X_val_batch['batch_0'][88, 1, 6, 17]))
        # print("y_train: %r" % (self.y_val[4] == y_val_batch['batch_0'][4]))

        return X_train_batch, y_train_batch, X_val_batch, y_val_batch

    def train(self, epochs=10):
        iter_per_epoch = int(self.N / self.batch_size)                          # Total number of data = iterations_per_epoch * batch_size
        total_iters = int(epochs * (self.N / self.batch_size))
        epoch_count = 0
        X_train_batch, y_train_batch, X_val_batch, y_val_batch = self.construct_batch()
        print("Start training....")
        loss = 0
        for iter in range(total_iters):
            batch_num = iter % iter_per_epoch
            if (batch_num == 0):
                epoch_count = epoch_count + 1
                train_acc = 0
                val_acc = 0
                for batch in range(iter_per_epoch):
                    train_scores = self.resnet18_forward(X_train_batch['batch_%d' % (batch)], batch_y=None, mode="test")
                    train_acc = train_acc + self.check_accuracy(train_scores, y_train_batch['batch_%d' % (batch)])
                for batch in range(int(self.val_size/self.batch_size)):
                    val_scores = self.resnet18_forward(X_val_batch['batch_%d' % (batch)], batch_y=None, mode="test")
                    val_acc = val_acc + self.check_accuracy(val_scores, y_val_batch['batch_%d' % (batch)])
                train_acc = train_acc / (iter_per_epoch)
                val_acc = val_acc / (self.val_size/self.batch_size)
                loss = loss / (iter_per_epoch)
                print('(Epoch %d / %d) train acc: %f val acc: %f loss: %f' % (epoch_count, epochs, train_acc, val_acc, loss))
                # print('(Iteration %d / %d) loss: %f' % (batch_num+1, iter_per_epoch, loss))

                # if (val_acc > self.val_best_acc):
                #     self.best_model = self.resnet_grad_dict
                loss = 0
                loss = loss + self.resnet18_forward(X_train_batch['batch_%d' % (batch_num)], y_train_batch['batch_%d' % (batch_num)], mode="train")
                _ = self.resnet18_backward()
                self._step(self.resnet_grad_dict)
                
            else:
                loss = loss + self.resnet18_forward(X_train_batch['batch_%d' % (batch_num)], y_train_batch['batch_%d' % (batch_num)], mode="train")
                _ = self.resnet18_backward()
                self._step(self.resnet_grad_dict)
                #print('(Iteration %d / %d) loss: %f' % (batch_num+1, iter_per_epoch, loss))
