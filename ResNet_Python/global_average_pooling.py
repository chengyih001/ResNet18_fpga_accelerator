import numpy as np
from testing_functions import *
# from softmax_loss import *
# from full_connected import *

class GAP:
    def __init__(self, x):
        # x: (n, c, h, w)

        # out: (n, c, 1, 1)

        self.x = x
        self.N, self.C, self.H, self.W = x.shape
    
    def GAP_forward_pass(self, x):
        self.x = x
        out = np.zeros(shape=(self.N, self.C, 1, 1))
        for n in range(self.N):
            for c in range(self.C):
                for h in range(self.H):
                    for w in range(self.W):
                        out[n, c, 0, 0] = out[n, c, 0, 0] + x[n, c, h, w]
                out[n, c, 0, 0] = out[n, c, 0, 0] / (self.H * self.W)

        # print(x)
        # print(out)

        return out

    def GAP_backward_pass(self, dout):      # dout: (N, C, H, W)
        dx = np.zeros(shape=(self.N, self.C, self.H, self.W))
        for n in range(self.N):
            for c in range(self.C):
                for h in range(self.H):
                    for w in range(self.W):
                        dx[n, c, h, w] = dout[n, c, 0, 0] / (self.H * self.W)
        return dx

if __name__ == "__main__":

       
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

    gp = GAP(x)
    out = gp.GAP_forward_pass(x)
    # print(out)
    fc = full_connected(out, w, b)
    out= fc.full_connected_forward(out)
    # print(out)
    sf = softmax_loss(out, y)
    loss = sf.softmax_loss_forward(out, y)
    loss_back = sf.softmax_loss_backward()
    dx, dw, db = fc.full_connected_backward(loss_back)
    dx_2 = gp.GAP_backward_pass(dx)

    print("loss")
    print(loss)
    # print("loss_back")
    # print(loss_back)
    # print("dx")
    # print(dx)
    # print("dw")
    # print(dw)
    # print("db")
    # print(db)
    print(dx_2)




    # a = [0,1,1,2,1,-1,2,2,0,1,1,2,1,-1,2,2]
    # a = np.asarray(a)
    # a = a.reshape(2, 2, 2, 2)

    # gap = GAP(a)
    # out = gap.GAP_forward_pass(a)
    # print(out)
    # out = gap.GAP_backward_pass(out)
    # print(out)