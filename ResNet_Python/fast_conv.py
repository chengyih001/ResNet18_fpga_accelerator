import numpy as np
from testing_functions import *
from testing.im2col_cython import col2im_6d_cython
# from global_average_pooling import *
# from full_connected import *
# from batch_norm_conv import *
# from relu import *
# from softmax_loss import *

class convolution:
    def __init__(self, x, w, b, stride=1, pad=1):

        # x: (N, C, H, W)
        # w: (F, C, HH, WW)
        # b: (F,)
        
        # out: (N, F, H', W')
        #      H' = 1 + (H + 2 * pad - HH) / stride
        #      W' = 1 + (W + 2 * pad - WW) / stride

        # cache: (x, w, b, stride, pad)

        self.x = x
        self.N, self.C, self.H, self.W = x.shape
        self.F, self._, self.HH, self.WW = w.shape
        self.weight = w
        self.bias = b
        self.stride = stride
        self.pad = pad
        self.H_out = 1 + int((self.H + 2 * self.pad - self.HH) / stride)
        self.W_out = 1 + int((self.W + 2 * self.pad - self.WW) / stride)
        self.x_cols = None


    def conv_forward_pass(self, x):
        self.x = x
        N, C, H, W = x.shape
        p = self.pad
        pad = self.pad
        F, HH, WW = self.F, self.HH, self.WW
        stride = self.stride
        x_padded = np.pad(x, ((0, 0), (0, 0), (p, p), (p, p)), mode='constant')

        # Figure out output dimensions
        H += 2 * pad
        W += 2 * pad
        out_h = (H - HH) // stride + 1
        out_w = (W - WW) // stride + 1

        # Perform an im2col operation by picking clever strides
        shape = (C, HH, WW, N, out_h, out_w)
        strides = (H * W, W, 1, C * H * W, stride * W, stride)
        strides = x.itemsize * np.array(strides)
        x_stride = np.lib.stride_tricks.as_strided(x_padded,
                    shape=shape, strides=strides)
        x_cols = np.ascontiguousarray(x_stride)
        x_cols.shape = (C * HH * WW, N * out_h * out_w)
        self.x_cols = x_cols

        # Now all our convolutions are a big matrix multiply
        res = self.weight.reshape(F, -1).dot(x_cols) + self.bias.reshape(-1, 1)
        #res = self.weight.reshape(F, -1).dot(x_cols)

        # Reshape the output
        res.shape = (F, N, out_h, out_w)
        out = res.transpose(1, 0, 2, 3)

        # Be nice and return a contiguous array
        # The old version of conv_forward_fast doesn't do this, so for a fair
        # comparison we won't either
        out = np.ascontiguousarray(out)
        return out

    def conv_backward_pass(self, dout):
        x, w, b = self.x, self.weight, self.bias
        stride, pad = self.stride, self.pad

        N, C, H, W = self.x.shape
        F, _, HH, WW = self.weight.shape
        _, _, out_h, out_w = dout.shape

        db = np.sum(dout, axis=(0, 2, 3))

        dout_reshaped = dout.transpose(1, 0, 2, 3).reshape(F, -1)

        dx_cols = w.reshape(F, -1).T.dot(dout_reshaped)
        dx_cols.shape = (C, HH, WW, N, out_h, out_w)
        dx = col2im_6d_cython(dx_cols, N, C, H, W, HH, WW, pad, stride)


        H += 2 * pad
        W += 2 * pad

        x_padded = np.pad(x, ((0, 0), (0, 0), (pad, pad), (pad, pad)), mode='constant')
        shape = (C, HH, WW, N, out_h, out_w)
        strides = (H * W, W, 1, C * H * W, stride * W, stride)
        strides = x.itemsize * np.array(strides)
        x_stride = np.lib.stride_tricks.as_strided(x_padded,
                    shape=shape, strides=strides)
        x_cols = np.ascontiguousarray(x_stride)
        x_cols.shape = (C * HH * WW, N * out_h * out_w)

        
        dw = dout_reshaped.dot(x_cols.T).reshape(w.shape)


        return dx, dw, db

if __name__ == '__main__':


    # # np.random.seed(231)
    # # x = np.random.randn(4, 3, 5, 5)
    # # w = np.random.randn(2, 3, 3, 3)
    # # b = np.random.randn(2,)
    # # x = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2]
    # # x = np.asarray(x)
    # # x = x.reshape(2, 2, 2, 2)
    # # w = [[[[ 0.2153, -0.1611, -0.0650],
    # #       [-0.2075, -0.2086,  0.2234],
    # #       [ 0.1922,  0.1426,  0.0115]],

    # #      [[-0.0904, -0.1216, -0.0012],
    # #       [-0.1330,  0.0902, -0.1963],
    # #       [ 0.0412,  0.0044, -0.1748]]],


    # #     [[[ 0.0301, -0.0586, -0.1629],
    # #       [-0.1788,  0.0232,  0.0659],
    # #       [-0.0339,  0.0840, -0.0033]],

    # #      [[ 0.1612, -0.2057, -0.2251],
    # #       [-0.1904, -0.2266,  0.2258],
    # #       [-0.1366, -0.1058,  0.1500]]]]
    # # w = np.asarray(w)
    # # b = [0,0]
    # # b = np.asarray(b)


    # np.random.seed(231)
    # x = np.random.randn(4, 3, 5, 5)
    # w = np.random.randn(2, 3, 3, 3)
    # b = np.random.randn(2,)
    # # x = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2]
    # # x = np.asarray(x)
    # # x = x.reshape(1, 1, 4, 4)
    # # w = [[[[ 0.2406, -0.2518, -0.1063],
    # #       [-0.3172,  0.1428,  0.1195],
    # #       [-0.1825, -0.2225,  0.0734]]]]
    # # w = np.asarray(w)
    # # b = [0]
    # # b = np.asarray(b)

    # conv1 = convolution(x, w, b, stride=1, pad=1)
    # dout = np.random.randn(4, 2, 5, 5)
    # conv_param = {'stride': 1, 'pad': 1}

    # # conv1 = convolution(x, w, b, stride=1, pad=1)
    # # dout = np.random.randn(4,2,8,8)
    # # conv_param = {'stride':1, 'pad':1}

    # dx_num = eval_numerical_gradient_array(lambda x: conv_forward_fast(x, w, b, conv_param)[0], x, dout)
    # dw_num = eval_numerical_gradient_array(lambda w: conv_forward_fast(x, w, b, conv_param)[0], w, dout)
    # db_num = eval_numerical_gradient_array(lambda b: conv_forward_fast(x, w, b, conv_param)[0], b, dout)

    # out = conv1.conv_forward_pass(x)

    # dx, dw, db = conv1.conv_backward_pass(dout)

    # print('Testing conv_backward_naive function')
    # print('dx error: ', rel_error(dx, dx_num))
    # print('dw error: ', rel_error(dw, dw_num))
    # print('db error: ', rel_error(db, db_num))



    # x_shape = (2, 3, 4, 4)
    # w_shape = (3, 3, 4, 4)
    # x = np.linspace(-0.1, 0.5, num=np.prod(x_shape)).reshape(x_shape)
    # w = np.linspace(-0.2, 0.3, num=np.prod(w_shape)).reshape(w_shape)
    # b = np.linspace(-0.1, 0.2, num=3)

    # conv = convolution(x, w, b, stride=2, pad=1)
    # out = conv.conv_forward_pass(x)

    # correct_out = np.array([[[[-0.08759809, -0.10987781],
    #                        [-0.18387192, -0.2109216 ]],
    #                       [[ 0.21027089,  0.21661097],
    #                        [ 0.22847626,  0.23004637]],
    #                       [[ 0.50813986,  0.54309974],
    #                        [ 0.64082444,  0.67101435]]],
    #                      [[[-0.98053589, -1.03143541],
    #                        [-1.19128892, -1.24695841]],
    #                       [[ 0.69108355,  0.66880383],
    #                        [ 0.59480972,  0.56776003]],
    #                       [[ 2.36270298,  2.36904306],
    #                        [ 2.38090835,  2.38247847]]]])

    # # Compare your output to ours; difference should be around 2e-8
    # print('Testing conv_forward_naive')
    # print('difference: ', rel_error(out, correct_out))

    w_bn = [1, 1]
    w_bn = np.asarray(w_bn)
    b_bn = [0, 0]
    b_bn = np.asarray(b_bn)

    w_conv = [[[[-0.2003,  0.0963, -0.1448],
          [-0.0518, -0.1966, -0.0791],
          [ 0.1317, -0.0848,  0.2189]],

         [[-0.1510,  0.0845, -0.2298],
          [-0.1989,  0.1889, -0.1374],
          [-0.2256, -0.1747, -0.1167]]],


        [[[ 0.0490,  0.1305, -0.0639],
          [ 0.1029,  0.0405,  0.1497],
          [-0.0183, -0.2147,  0.0184]],

         [[ 0.0194, -0.2110, -0.1312],
          [ 0.1037,  0.2240,  0.2042],
          [-0.1471,  0.1170, -0.0210]]]]
    w_conv = np.asarray(w_conv)
    b_conv = [0, 0]
    b_conv = np.asarray(b_conv)


    w = [[-0.5047, -0.7036],
        [-0.0035, -0.5236],
        [-0.0898,  0.3441],
        [ 0.0046,  0.0931],
        [-0.4542,  0.6274]]
    w = np.asarray(w)
    b = [ 0.3702, -0.4575,  0.0874, -0.5373, -0.3242]
    b = np.asarray(b)

    x = [0,4,1,2,18,-1,2,2,0,1,3,2,1,-7,2,2]
    x = np.asarray(x)
    x = x.reshape(2, 2, 2, 2)
    y = [1,1]
    y = np.asarray(y)


    bn = batch_norm_conv(x, w_bn, b_bn)
    out = bn.batchnorm_conv_forward_pass(x)
    # print(out)
    rr = relu(out)
    out = rr.relu_forward_pass(out)
    # print(out)
    conv = convolution(out, w_conv, b_conv)
    out = conv.conv_forward_pass(out)
    # print(out)
    gp = GAP(out)
    out = gp.GAP_forward_pass(out)
    # print(out)
    fc = full_connected(out, w, b)
    out= fc.full_connected_forward(out)
    # print(out)
    sf = softmax_loss(out, y)
    loss = sf.softmax_loss_forward(out, y)
    loss_back = sf.softmax_loss_backward()
    dx, dw, db = fc.full_connected_backward(loss_back)
    dx_2 = gp.GAP_backward_pass(dx)
    dx, dw, db = conv.conv_backward_pass(dx_2)
    dx = rr.relu_backward_pass(dx)
    dx, dw, db = bn.batchnorm_conv_backward_pass(dx)

    print("loss")
    print(loss)
    # # # print("loss_back")
    # # # print(loss_back)
    print("dx")
    print(dx)
    print("dw")
    print(dw)
    print("db")
    print(db)