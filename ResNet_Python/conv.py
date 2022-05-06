import numpy as np
# from testing_functions import *
# from global_average_pooling import *
# from full_connected import *
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

    def padding(self, array):
        #x_padded = np.zeros(shape=(self.N, self.C, self.H + self.pad * 2, self.W + self.pad * 2))
        N, C, H, W = array.shape
        H_pad = H + self.pad*2
        W_pad = W + self.pad*2
        array_padded = np.zeros(shape=(N, C, H_pad, W_pad))
        for n in range(N):
            for c in range(C):
                for h in range(H):
                    for w in range(W):
                        array_padded[n, c, h + self.pad, w + self.pad] = self.x[n, c, h, w]
        return array_padded

    # def rotate_array(self):
    #     w_rotate = np.zeros_like(self.weight)
    #     for f in range(self.F):
    #         for c in range(self.C):
    #             for hh in range(self.HH):
    #                 for ww in range(self.WW):
    #                    w_rotate[f, c, self.HH-1-hh, self.WW-1-ww] = self.weight[f, c, hh, ww]
    #     return w_rotate 
    
    # def padding_stride(self, array):
    #     temp_h = 0
    #     temp_w = 0
    #     if (self.H_out == 1):
    #         temp_h = 2
    #     else:
    #         temp_h = self.H_out
    #     if (self.W_out == 1):
    #         temp_w = 2
    #     else: 
    #         temp_w = self.W_out
    #     print("xx %d" % ((self.HH-1)*2 + self.H_out + (self.stride-1)*(temp_h-1)))
    #     out_dilate = np.zeros(shape=(self.N, self.F, (self.HH-1)*2 + self.H_out + (self.stride-1)*(temp_h-1), (self.WW-1)*2 + self.W_out + (self.stride-1)*(temp_w-1)))
    #     for n in range(self.N):
    #         for f in range(self.F):
    #             for h in range(self.H_out):
    #                 for w in range(self.W_out):
    #                     out_dilate[n, f, (self.HH-1)+h*(self.stride), (self.WW-1)+w*(self.stride)] = array[n, f, h, w]
    #     return out_dilate

    def conv_forward_pass(self, x):
        self.x = x
        out = np.zeros(shape=(self.N, self.F, self.H_out, self.W_out))
        x_padded = self.padding(x)
        for n in range(self.N):         # n-th image
            for f in range(self.F):     # f-th filter
                for h in range(self.H_out):
                    for w in range(self.W_out):
                        out[n, f, h, w] = out[n, f, h, w] + self.bias[f]
                        for c in range(self.C):
                            for hh in range(self.HH):
                                for ww in range(self.WW):
                                    out[n, f, h, w] = out[n, f, h, w] + self.weight[f, c, hh, ww] * x_padded[n, c, h*self.stride+hh, w*self.stride+ww]
        # print(out)
        # print(self.weight)
        # print(x_padded)
        return out

    def conv_backward_pass(self, dout):
        dw = np.zeros_like(self.weight)
        dx = np.zeros(shape=(self.N, self.C, self.H, self.W))
        db = np.zeros_like(self.bias)
        dx_padded = np.zeros(shape=(self.N, self.C, self.H+self.pad*2, self.W+self.pad*2))

        x_padded = self.padding(self.x)

        for n in range(self.N):
            for f in range(self.F):
                for h in range(self.H_out):
                    for w in range(self.W_out):
                        db[f] = db[f] + dout[n, f, h, w]
                        for c in range(self.C):
                            for hh in range(self.HH):
                                for ww in range(self.WW):
                                    temp = x_padded[n, c, h*self.stride+hh, w*self.stride+ww]
                                    dw[f, c, hh, ww] = dw[f, c, hh, ww] + temp * dout[n, f, h, w]

                                    dx_padded[n, c, h*self.stride+hh, w*self.stride+ww] = dx_padded[n, c, h*self.stride+hh, w*self.stride+ww] + self.weight[f, c, hh, ww] * dout[n, f, h, w]

        for n in range(self.N):
            for c in range(self.C):
                for h in range(self.H):
                    for w in range(self.W):
                        dx[n, c, h, w] = dx_padded[n, c, h+self.pad, w+self.pad]
        
        return dx, dw, db

if __name__ == '__main__':

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

    conv = convolution(x, w_conv, b_conv)
    out = conv.conv_forward_pass(x)
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

    print("loss")
    print(loss)
    # print("loss_back")
    # print(loss_back)
    print("dx")
    print(dx)
    print("dw")
    print(dw)
    print("db")
    print(db)
    # print(dx_2)



    # x = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2]
    # x = np.asarray(x)
    # x = x.reshape(2, 2, 2, 2)
    # w = [[[[ 0.2153, -0.1611, -0.0650],
    #       [-0.2075, -0.2086,  0.2234],
    #       [ 0.1922,  0.1426,  0.0115]],

    #      [[-0.0904, -0.1216, -0.0012],
    #       [-0.1330,  0.0902, -0.1963],
    #       [ 0.0412,  0.0044, -0.1748]]],


    #     [[[ 0.0301, -0.0586, -0.1629],
    #       [-0.1788,  0.0232,  0.0659],
    #       [-0.0339,  0.0840, -0.0033]],

    #      [[ 0.1612, -0.2057, -0.2251],
    #       [-0.1904, -0.2266,  0.2258],
    #       [-0.1366, -0.1058,  0.1500]]]]
    # w = np.asarray(w)
    # b = [0,0]
    # b = np.asarray(b)

    # conv1 = convolution(x, w, b, stride=1, pad=1)
    # out = conv1.conv_forward_pass(x)
    # print(out)
    # out2, _,_ = conv1.conv_backward_pass(out)
    # print(out2)
    # x = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2]
    # x = np.asarray(x)
    # x = x.reshape(2, 2, 2, 2)
    # w = [[[[ 0.0600, -0.2206,  0.1261],
    #       [-0.1691,  0.0356, -0.1468],
    #       [-0.1091,  0.1170,  0.1809]],

    #      [[ 0.0387, -0.0100,  0.2092],
    #       [-0.2308,  0.1756,  0.2351],
    #       [-0.1058,  0.1862,  0.1311]]],


    #     [[[ 0.0315,  0.2341,  0.1958],
    #       [ 0.1976,  0.0155, -0.1542],
    #       [-0.1499,  0.2206, -0.0214]],

    #      [[ 0.1770, -0.1947,  0.1421],
    #       [ 0.1677, -0.0840, -0.0774],
    #       [-0.0249,  0.0398,  0.1890]]]]
    # w = np.asarray(w)
    # b = [0,0]
    # b = np.asarray(b)

    # conv1 = convolution(x, w, b, stride=1, pad=1)
    # out = conv1.conv_forward_pass(x)
    # print(out)
    # out2, _,_ = conv1.conv_backward_pass(out)
    # print(out2)




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