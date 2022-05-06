import numpy  as np
from testing_functions import *
# from conv import *
# from global_average_pooling import *
# from full_connected import *
# from softmax_loss import *

class relu:
    def __init__(self, x):

        # x: (N, C, H, W)

        self.x = x
        self.N, self.C, self.H, self.W = x.shape

    def relu_forward_pass(self, x):
        self.x = x
        out = np.zeros(shape=(self.N, self.C, self.H, self.W))
        for n in range(self.N):
            for c in range(self.C):
                for h in range(self.H):
                    for w in range(self.W):
                        if (self.x[n, c, h, w] > 0):
                            out[n, c, h, w] = self.x[n, c, h, w]
                        else:
                            out[n, c, h, w] = 0

        # print(x)
        # print(out)

        return out

    def relu_backward_pass(self, dout):
        dx = np.zeros(shape=(self.N, self.C, self.H, self.W))
        for n in range(self.N):
            for c in range(self.C):
                for h in range(self.H):
                    for w in range(self.W):
                        if (self.x[n, c, h, w] > 0):
                            dx[n, c, h, w] = dout[n, c, h, w]
                        else:
                            dx[n, c, h, w] = 0
        return dx
    
    # def relu_backward_pass(self, dout):
    #     dx = np.zeros(shape=(self.N, self.C, self.H, self.W))
    #     for n in range(self.N):
    #         for c in range(self.C):
    #             for h in range(self.H):
    #                 for w in range(self.W):
    #                         dx[n, c, h, w] = dout[n, c, h, w]
    #     return dx
                    
if __name__ == "__main__":
    # a = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2]
    # a = np.asarray(a)
    # a = a.reshape(2, 2, 2, 2)
    # # B = np.array(B)
    # relu_test = relu(a)
    # out = relu_test.relu_forward_pass(a)
    # out = relu_test.relu_backward_pass(out)
    # print(out)


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

    rr = relu(x)
    out = rr.relu_forward_pass(x)
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

    print("loss")
    print(loss)
    # # print("loss_back")
    # # print(loss_back)
    print("dx")
    print(dx)
    # print("dw")
    # print(dw)
    # print("db")
    # print(db)