import numpy as np
from data_utils import *
#from conv import *
from fast_conv import *
#from batch_norm_conv import *
from fast_batch_norm_conv import *
from relu import *
from optimize_method import *

class basic_block:
    def __init__(self, x, filter_size, weight_scale=1e-2, conv_size=3, stride1=1, pad1=1, stride2=1, pad2=1, batch_size=100):

        # convolution(x, w, b, stride=1, pad=0)
        # batch_norm_conv(x, gamma, beta, mode="train", eps=1e-5, momentum=0.9)
        # relu(x)
        
        self.x = x
        self.conv_size = conv_size
        self.batch_size = batch_size
        self.filter_size = filter_size
        self.stride1 = stride1

        self.N, self.C, H_0, W_0 = x.shape
        H_1 = int(1 + (H_0 + 2 * pad1 - conv_size) / stride1)
        W_1 = int(1 + (W_0 + 2 * pad1 - conv_size) / stride1)
        self.H_2 = int(1 + (H_1 + 2 * pad2 - conv_size) / stride2)
        self.W_2 = int(1 + (W_1 + 2 * pad2 - conv_size) / stride2)

        self.w1 = np.random.normal(loc=0, scale=weight_scale, size=(filter_size, self.C, self.conv_size, self.conv_size))
        self.b1 = np.random.normal(loc=0, scale=weight_scale, size=(filter_size))
        self.w2 = np.random.normal(loc=0, scale=weight_scale, size=(filter_size, filter_size, self.conv_size, self.conv_size))
        self.b2 = np.random.normal(loc=0, scale=weight_scale, size=(filter_size))
        self.gamma1 = np.ones(shape=(filter_size))
        self.beta1 = np.zeros(shape=(filter_size))
        self.gamma2 = np.ones(shape=(filter_size))
        self.beta2 = np.zeros(shape=(filter_size))
        # self.gamma1 = np.ones(shape=(filter_size*H_1*W_1))
        # self.beta1 = np.zeros(shape=(filter_size*H_1*W_1))
        # self.gamma2 = np.ones(shape=(filter_size*self.H_2*self.W_2))
        # self.beta2 = np.zeros(shape=(filter_size*self.H_2*self.W_2))

        dummy1 = np.zeros(shape=(self.batch_size, filter_size, H_1, W_1))
        dummy2 = np.zeros(shape=(self.batch_size, filter_size, self.H_2, self.W_2))

        self.conv1 = convolution(self.x, self.w1, self.b1, stride1, pad1)
        self.bn1 = batch_norm_conv(dummy1, self.gamma1, self.beta1)
        self.relu1 = relu(dummy1)

        self.conv2 = convolution(dummy1, self.w2, self.b2, stride2, pad2)
        self.bn2 = batch_norm_conv(dummy2, self.gamma2, self.beta2)
        self.relu2 = relu(dummy2)

        self.config_conv1 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=(filter_size, self.C, self.conv_size, self.conv_size)), 'b_velocity': np.zeros(shape=(filter_size))}
        self.config_bn1 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=filter_size), 'b_velocity': np.zeros(shape=filter_size)}
        self.config_conv2 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=(filter_size, filter_size, self.conv_size, self.conv_size)), 'b_velocity': np.zeros(shape=(filter_size))}
        self.config_bn2 = {'momentum': 0.9, 'w_velocity': np.zeros(shape=filter_size), 'b_velocity': np.zeros(shape=filter_size)}

        if (stride1 == 2):
            self.w_stride = np.random.normal(loc=0, scale=weight_scale, size=(filter_size, self.C, self.conv_size, self.conv_size))
            self.b_stride = np.zeros(filter_size)
            self.gamma_stride = np.ones(shape=(filter_size))
            self.beta_stride = np.zeros(shape=(filter_size))
            self.conv_stride = convolution(self.x, self.w_stride, self.b_stride, 2, 1)
            self.bn_stride = batch_norm_conv(dummy2, self.gamma_stride, self.beta_stride)
            self.config_conv_stride = {'momentum': 0.9, 'w_velocity': np.zeros(shape=(filter_size, self.C, self.conv_size, self.conv_size)), 'b_velocity': np.zeros(shape=(filter_size))}
            self.config_bn_stride = {'momentum': 0.9, 'w_velocity': np.zeros(shape=filter_size), 'b_velocity': np.zeros(shape=filter_size)}



    def basic_block_forward(self, x, mode="train"):
        self.x = x
        _, _C, _, _ = x.shape
        # print("    ----- layer 1-1: convolution - forward")
        # print("    ", x.shape)
        conv1_forward = self.conv1.conv_forward_pass(self.x)
        

        # print("    ----- layer 1-2: batch_norm - forward")
        # print("    ", conv1_forward.shape)
        bn1_forward = self.bn1.batchnorm_conv_forward_pass(conv1_forward, mode=mode)

        # print("    ----- layer 1-3: relu - forward")
        # print("    ", bn1_forward.shape)
        relu1_forward = self.relu1.relu_forward_pass(bn1_forward)


        # print("    ----- layer 2-1: convolution - forward")
        # print("    ", relu1_forward.shape)
        conv2_forward = self.conv2.conv_forward_pass(relu1_forward)

        # print("    ----- layer 2-2: batch_norm - forward")
        # print("    ", conv2_forward.shape)
        bn2_forward = self.bn2.batchnorm_conv_forward_pass(conv2_forward, mode=mode)


        # print(bn2_forward.shape)
        temp_n, temp_c, temp_h, temp_w = bn2_forward.shape
        out = np.zeros(shape=(temp_n, temp_c, temp_h, temp_w))

        if (self.stride1 == 2):
            conv_stride_out = self.conv_stride.conv_forward_pass(self.x)
            bn_stride_out = self.bn_stride.batchnorm_conv_forward_pass(conv_stride_out)
            for n in range(self.N):
                for c in range(self.C):
                    for h in range(self.H_2):
                        for w in range(self.W_2):
                            out[n, c, h, w] = bn_stride_out[n, c, h, w] + bn2_forward[n, c, h, w]

        else:
            for n in range(self.N):
                for c in range(self.C):
                    for h in range(self.H_2):
                        for w in range(self.W_2):
                            out[n, c, h, w] = self.x[n, c, h, w] + bn2_forward[n, c, h, w]

        # print("    ----- Residual operation: Y = F(x)+x")
        # print("    ", out.shape)
        relu2_forward = self.relu2.relu_forward_pass(out)
        # print("    ----- layer 2-3: relu - forward")
        # print("    ", relu2_forward.shape)
        # print(conv1_forward[0][0][0][0])
        # print(bn1_forward[0][0][0][0])
        # print(relu1_forward[0][0][0][0])
        # print(conv2_forward[0][0][0][0])
        # print(bn2_forward[0][0][0][0])
        # print(out[0][0][0][0])
        # print(relu2_forward[0][0][0][0])


        return relu2_forward

    def basic_block_backward(self, dout):
        gradient_dict = {}
        #print("    ----- layer 2-3: relu - backward")
        #print("    ", dout)
        relu2_backward = self.relu2.relu_backward_pass(dout)
        #print("    ----- layer 2-2: bn - backward")
        #print("    ", relu2_backward)
        bn2_backward, dgamma2, dbeta2 = self.bn2.batchnorm_conv_backward_pass(relu2_backward)
        #print("    ----- layer 2-1: conv - backward")
        #print("    ", bn2_backward)
        conv2_backward, dw2, db2 = self.conv2.conv_backward_pass(bn2_backward)

        #print("    ----- layer 1-3: relu - backward")
        #print("    ", conv2_backward)
        relu1_backward = self.relu1.relu_backward_pass(conv2_backward)
        #print("    ----- layer 1-2: bn - backward")
        #print("    ", relu1_backward)
        bn1_backward, dgamma1, dbeta1 = self.bn1.batchnorm_conv_backward_pass(relu1_backward)
        #print("    ----- layer 1-1: conv - backward")
        #print("    ", bn1_backward)
        conv1_backward, dw1, db1 = self.conv1.conv_backward_pass(bn1_backward)

        temp_n, temp_c, temp_h, temp_w = conv1_backward.shape
        if (self.stride1 == 2):
            # temp = np.zeros(shape=(temp_n, temp_c, temp_h, temp_w))
            # for n in range(temp_n):
            #     for c in range(temp_c):
            #         for h in range(int(temp_h/2)):
            #             for w in range(int(temp_w/2)):
            #                 temp[n, c, h, w] = relu2_backward[n, c, h, w]

            # for n in range(self.N):
            #     for c in range(self.C):
            #         for h in range(self.H_2):
            #             for w in range(self.W_2):
            #                 conv1_backward[n, c, h, w] = conv1_backward[n, c, h, w] + temp[n, c, h, w]

            bn_stride_backward, dgamma_stride, dbeta_stride = self.bn_stride.batchnorm_conv_backward_pass(relu2_backward)
            conv_stride_backward, dw_stride, db_stride = self.conv_stride.conv_backward_pass(bn_stride_backward)
            for n in range(self.N):
                for c in range(self.C):
                    for h in range(self.H_2):
                        for w in range(self.W_2):
                            conv1_backward[n, c, h, w] = conv1_backward[n, c, h, w] + conv_stride_backward[n, c, h, w]
            gradient_dict['dw_stride'] = dw_stride
            gradient_dict['db_stride'] = db_stride
            gradient_dict['dgamma_stride'] = dgamma_stride
            gradient_dict['dbeta_stride'] = dbeta_stride
                            
        else:
            for n in range(self.N):
                for c in range(self.C):
                    for h in range(self.H_2):
                        for w in range(self.W_2):
                            conv1_backward[n, c, h, w] = conv1_backward[n, c, h, w] + relu2_backward[n, c, h, w]

        gradient_dict['dw1'] = dw1
        gradient_dict['db1'] = db1
        gradient_dict['dw2'] = dw2
        gradient_dict['db2'] = db2
        gradient_dict['dgamma1'] = dgamma1
        gradient_dict['dbeta1'] = dbeta1
        gradient_dict['dgamma2'] = dgamma2
        gradient_dict['dbeta2'] = dbeta2

        #print(conv1_backward)

        return conv1_backward, gradient_dict

    def optimize_basic_block(self, grad_dict, learning_rate):

        # optim_method(w, dw, b, db, layer, learning_rate = 1e-4, config=None)

        # config = {
        #   - momentum
        #   - w_velocity
        #   - b_velocity
        # }
        
        # grad_dict = {
        #   'dw1': dw1
        #   'db1': db1
        #   'dw2': dw2
        #   'db2': db2
        #   'dgamma1': dgamma1
        #   'dbeta1': dbeta1
        #   'dgamma2': dgamma2
        #   'dbeta2': dbeta2
        # }

        dw1 = grad_dict['dw1']
        db1 = grad_dict['db1']
        dgamma1 = grad_dict['dgamma1']
        dbeta1 = grad_dict['dbeta1']
        dw2 = grad_dict['dw2']
        db2 = grad_dict['db2']
        dgamma2 = grad_dict['dgamma2']
        dbeta2 = grad_dict['dbeta2']

        if (self.stride1 == 2):
            dw_stride = grad_dict['dw_stride']
            db_stride = grad_dict['db_stride']
            dgamma_stride = grad_dict['dgamma_stride']
            dbeta_stride = grad_dict['dbeta_stride']
            
            # self.w_stride, self.b_stride, config_temp = sgd_momentum(self.w_stride, dw_stride, self.b_stride, db_stride, "conv", learning_rate=learning_rate, config=self.config_conv_stride)
            # self.config_conv_stride['w_velocity'] = config_temp['w_velocity']
            # self.config_conv_stride['b_velocity'] = config_temp['b_velocity']
            # self.gamma_stride, self.beta_stride, config_temp = sgd_momentum(self.gamma_stride, dgamma_stride, self.beta_stride, dbeta_stride, "batch_norm_conv", learning_rate=learning_rate, config=self.config_bn_stride)
            # self.config_bn_stride['w_velocity'] = config_temp['w_velocity']
            # self.config_bn_stride['b_velocity'] = config_temp['b_velocity']
            
            self.w_stride, self.b_stride, config_temp = sgd(self.w_stride, dw_stride, self.b_stride, db_stride, "conv", learning_rate=learning_rate, config=self.config_conv_stride)
            self.gamma_stride, self.beta_stride, config_temp = sgd(self.gamma_stride, dgamma_stride, self.beta_stride, dbeta_stride, "batch_norm_conv", learning_rate=learning_rate, config=self.config_bn_stride)

            self.conv_stride.weight = self.w_stride
            self.conv_stride.bias = self.b_stride
            self.bn_stride.gamma = self.gamma_stride
            self.bn_stride.beta = self.beta_stride

        # self.w1, self.b1, config_temp = sgd_momentum(self.w1, dw1, self.b1, db1, "conv", learning_rate=learning_rate, config=self.config_conv1)
        # self.config_conv1['w_velocity'] = config_temp['w_velocity']
        # self.config_conv1['b_velocity'] = config_temp['b_velocity']

        # self.gamma1, self.beta1, config_temp = sgd_momentum(self.gamma1, dgamma1, self.beta1, dbeta1, "batch_norm_conv", learning_rate=learning_rate, config=self.config_bn1)
        # self.config_bn1['w_velocity'] = config_temp['w_velocity']
        # self.config_bn1['b_velocity'] = config_temp['b_velocity']

        # self.w2, self.b2, config_temp = sgd_momentum(self.w2, dw2, self.b2, db2, "conv", learning_rate=learning_rate, config=self.config_conv2)
        # self.config_conv2['w_velocity'] = config_temp['w_velocity']
        # self.config_conv2['b_velocity'] = config_temp['b_velocity']

        # self.gamma2, self.beta2, config_temp = sgd_momentum(self.gamma2, dgamma2, self.beta2, dbeta2, "batch_norm_conv", learning_rate=learning_rate, config=self.config_bn2)
        # self.config_bn2['w_velocity'] = config_temp['w_velocity']
        # self.config_bn2['b_velocity'] = config_temp['b_velocity']

        self.w1, self.b1, config_temp = sgd(self.w1, dw1, self.b1, db1, "conv", learning_rate=learning_rate, config=self.config_conv1)
        self.gamma1, self.beta1, config_temp = sgd(self.gamma1, dgamma1, self.beta1, dbeta1, "batch_norm_conv", learning_rate=learning_rate, config=self.config_bn1)
        self.w2, self.b2, config_temp = sgd(self.w2, dw2, self.b2, db2, "conv", learning_rate=learning_rate, config=self.config_conv2)
        self.gamma2, self.beta2, config_temp = sgd(self.gamma2, dgamma2, self.beta2, dbeta2, "batch_norm_conv", learning_rate=learning_rate, config=self.config_bn2)

        self.conv1.weight = self.w1
        self.conv1.bias = self.b1
        self.bn1.gamma = self.gamma1
        self.bn1.beta = self.beta1

        self.conv2.weight = self.w2
        self.conv2.bias = self.b2
        self.bn2.gamma = self.gamma2
        self.bn2.beta = self.beta2

        # print(self.w1)
        # print(self.b1)
        # print(self.gamma1)
        # print(self.beta1)


if __name__ == "__main__":
    a = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2, 20, 3, 14, 7]
    a = np.asarray(a)
    a = a.reshape(2, 2, 3, 3)
    w_stride = [[[[-0.1607, -0.0445,  0.0089],
          [ 0.1958,  0.2043,  0.1961],
          [-0.0218,  0.1251,  0.2244]],

         [[ 0.1629,  0.1993, -0.0802],
          [-0.0075,  0.1050,  0.0915],
          [-0.0265, -0.0378,  0.0005]]],


        [[[-0.2269,  0.0335,  0.0626],
          [-0.1046, -0.1966, -0.1790],
          [-0.1353, -0.2169,  0.2183]],

         [[-0.1327,  0.0779, -0.0198],
          [ 0.0278, -0.0068, -0.0624],
          [-0.1078, -0.1495, -0.0122]]]]
    w_stride = np.asarray(w_stride)
    wbn_stride = [1., 1.]
    wbn_stride = np.asarray(wbn_stride)
    bbn_stride = [0.,0.]
    bbn_stride = np.asarray(bbn_stride)

    w1 = [[[[ 0.0813,  0.0423,  0.1468],
          [-0.1966,  0.0336, -0.1638],
          [-0.0618, -0.2025, -0.1858]],

         [[-0.1502,  0.1819, -0.1713],
          [ 0.1107,  0.0565, -0.0324],
          [ 0.2060,  0.1422, -0.1262]]],


        [[[ 0.1356,  0.1674, -0.2026],
          [ 0.0703, -0.1597,  0.1602],
          [ 0.1465, -0.1290,  0.1003]],

         [[-0.1198, -0.0320, -0.0599],
          [ 0.1720,  0.1855, -0.1262],
          [-0.1106, -0.1233, -0.0748]]]]
    w1 = np.asarray(w1)
    w2 = [1., 1.]
    w2 = np.asarray(w2)
    b2 = [0.,0.]
    b2 = np.asarray(b2)

    w3 = [[[[ 0.0330,  0.1556, -0.0777],
          [ 0.1677,  0.1004,  0.0293],
          [-0.1732,  0.2013,  0.0025]],

         [[-0.0394, -0.1233,  0.0269],
          [ 0.2194, -0.1306,  0.1525],
          [-0.1809, -0.0947,  0.0769]]],


        [[[-0.1554, -0.1309,  0.1079],
          [-0.0750, -0.2244, -0.0604],
          [ 0.0136,  0.1751,  0.0191]],

         [[-0.0233,  0.0743, -0.1013],
          [-0.1346,  0.0922,  0.2311],
          [ 0.0217, -0.0893,  0.2281]]]]
    w3 = np.asarray(w3)
    w4 = [1., 1.]
    w4 = np.asarray(w4)
    b4 = [0.,0.]
    b4 = np.asarray(b4)

    model = basic_block(a, 2, stride1 = 1, batch_size=2)

    # model.conv_stride.weight = w_stride
    # model.conv_stride.bias = np.zeros(shape=(2))
    # model.bn_stride.gamma = wbn_stride
    # model.bn_stride.beta = bbn_stride

    model.conv1.weight = w1
    model.conv1.bias = np.zeros(shape=(2))
    model.bn1.gamma = w2
    model.bn1.beta = b2
    model.conv2.weight = w3
    model.conv2.bias = np.zeros(shape=(2))
    model.bn2.gamma = w4
    model.bn2.beta = b4
    model.basic_block_forward(a)
