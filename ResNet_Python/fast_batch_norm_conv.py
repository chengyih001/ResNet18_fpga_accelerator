import numpy as np

# from relu import *
# from conv import *
# from global_average_pooling import *
# from full_connected import *
# from softmax_loss import *

class batch_norm_conv:
    def __init__(self, x, gamma, beta, eps=1e-5, momentum=0.9):

        # x: (N, C, H, W)
        # gamma: (C,)
        # beta: (C,)

        # out: (N, C, H, W)

        self.x = x
        self.N, self.C, self.H, self.W = x.shape
        self.gamma = gamma
        self.beta = beta
        self.eps = eps
        self.momentum = momentum
        self.mean = np.zeros(shape=(self.C))
        self.var = np.zeros(shape=(self.C))
        self.normalized_x = np.zeros(shape=(self.N, self.C, self.H, self.W))
        self.running_mean = np.zeros(shape=(self.C))
        self.running_var = np.zeros(shape=(self.C))


    def batchnorm_conv_forward_pass(self, x, mode="train"):
        self.x = x
        out = np.zeros(shape=(self.N, self.C, self.H, self.W))
        if (mode == "train"):
            self.mean = np.mean(self.x, axis = (0, 2, 3))
            self.var = np.var(self.x, axis=(0, 2, 3))
            for c in range(self.C):
                for n in range(self.N):
                    for h in range(self.H):
                        for w in range(self.W):
                            self.normalized_x[n, c, h, w] = (self.x[n, c, h, w] - self.mean[c]) / (self.var[c] + self.eps)**0.5
                            out[n, c, h, w] = self.gamma[c] * self.normalized_x[n, c, h, w] + self.beta[c]

                self.running_mean[c] = self.momentum * self.running_mean[c] + (1-self.momentum) * self.mean[c]
                self.running_var[c] = self.momentum * self.running_var[c] + (1-self.momentum) * self.var[c]
        
        elif (mode == "test"):
            for c in range(self.C):
                for n in range(self.N):
                    for h in range(self.H):
                        for w in range(self.W):
                            self.normalized_x[n, c, h, w] = (self.x[n, c, h, w] - self.running_mean[c]) / (self.running_var[c] + self.eps)**0.5
                            out[n, c, h, w] = self.gamma[c] * self.normalized_x[n, c, h, w] + self.beta[c]
        
        else:
            print("Mode error!!")

        return out

    def batchnorm_conv_backward_pass(self, dout):

        dx = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dgamma = np.zeros(shape=(self.C))
        dbeta = np.zeros(shape=(self.C))

        dgamma_norm_x = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dnorm_x = np.zeros(shape=(self.N, self.C, self.H, self.W))
        divar = np.zeros(shape=(self.C))
        dx_mu1 = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dsqrtvar = np.zeros(shape=(self.C))
        dvar = np.zeros(shape=(self.C))
        dsq = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dx_mu2 = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dmu = np.zeros(shape=(self.C))
        dx1 = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dx2 = np.zeros(shape=(self.N, self.C, self.H, self.W))


        for c in range(self.C):
            for n in range(self.N):
                for h in range(self.H):
                    for w in range(self.W):
                        dbeta[c] = dbeta[c] + dout[n, c, h, w]
                        dgamma_norm_x[n, c, h, w] = 1 * dout[n, c, h, w]

                        dgamma[c] = dgamma[c] + dgamma_norm_x[n, c, h, w] * self.normalized_x[n, c, h, w]
                        dnorm_x[n, c, h, w] = dgamma_norm_x[n, c, h, w] * self.gamma[c]
                        divar[c] = divar[c] + dnorm_x[n, c, h, w] * (self.x[n, c, h, w]-self.mean[c])

        for c in range(self.C):
            for n in range(self.N):
                for h in range(self.H):
                    for w in range(self.W):
                        dx_mu1[n, c, h, w] = dnorm_x[n, c, h, w] * (1/(self.var[c] + self.eps)**0.5)

                        dsqrtvar[c] = divar[c] * (-1/(self.var[c] + self.eps))

                        dvar[c] = dsqrtvar[c] * 0.5 * (1/(self.var[c] + self.eps)**0.5)

                        dsq[n, c, h, w] = 1/(self.N * self.H * self.W) * dvar[c]

                        dx_mu2[n, c, h, w] = 2 * (self.x[n, c, h, w] - self.mean[c]) * dsq[n, c, h, w]
                        dmu[c] = dmu[c] + (-1 * (dx_mu1[n, c, h, w] + dx_mu2[n, c, h, w]))

        for c in range(self.C):
            for n in range(self.N):
                for h in range(self.H):
                    for w in range(self.W):
                        dx1[n, c, h, w] = 1 * (dx_mu1[n, c, h, w] + dx_mu2[n, c, h, w])

                        dx2[n, c, h, w] = 1 * dmu[c] / (self.N * self.H * self.W)

                        dx[n, c, h, w] = dx1[n, c, h, w] + dx2[n, c, h, w]

        return dx, dgamma, dbeta

if __name__ == "__main__":
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