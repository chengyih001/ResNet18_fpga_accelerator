import numpy as np
from testing_functions import *
# from softmax_loss import *

class full_connected:
    def __init__(self, x, w, b):

        # x: (N, C, H, W)
        # w: (M, D)
        # b: (M, )

        # out: (N, M)

        self.x = x      # (N, C, H, W)
        self.w = w      # (M, D)
        self.b = b      # (M, )

        self.N, self.C, self.H, self.W = x.shape
        self.M, self.D = w.shape

        # self.x_reshape = np.zeros(shape=(self.N, self.D))

    def matrix_reshape(self, A):
        N, C, H, W = A.shape
        x_reshape = np.zeros(shape=(N, C*H*W))
        for n in range(N):
            for c in range(C):
                for h in range(H):
                    for w in range(W):
                        x_reshape[n, c * H * W + h * W + w] = A[n, c, h, w]
        return x_reshape

    def matrix_multiplication(self, A, B):
        I, J = A.shape
        _, K = B.shape
        out = np.zeros(shape=(I, K))
        for i in range(I):
            for k in range(K):
                for j in range(J):
                    out[i, k] = out[i, k] + A[i, j] * B[j, k]
        return out

    def matrix_transformation(self, A):
        N, M = A.shape
        out = np.zeros(shape=(M, N))
        for m in range(M):
            for n in range(N):
                out[m, n] = A[n, m]
        return out

    def full_connected_forward(self, x):
        self.x = x
        x_reshape = self.matrix_reshape(x)
        out = np.zeros(shape=(self.N, self.M))
        w_transform = self.matrix_transformation(self.w)
        out = self.matrix_multiplication(x_reshape, w_transform)
        for n in range(self.N):
            for m in range(self.M):
                out[n, m] = out[n, m] + self.b[m]
        
        # print(x)
        # print(x_reshape)
        # print(out)

        return out

    def full_connected_backward(self, dout):        # dout: (N, M)
        x_reshape = self.matrix_reshape(self.x)
        dx = np.zeros(shape=(self.x.shape))
        dw = np.zeros(shape=(self.w.shape))
        db = np.zeros(shape=(self.b.shape))

        dx_temp = np.zeros(shape=(self.N, self.C, self.H, self.W))
        dx_temp = self.matrix_multiplication(dout, self.w)      # (N, M) * (M, D) = (N, D)
        for n in range(self.N):
            for c in range(self.C):
                for h in range(self.H):
                    for w in range(self.W):
                        dx[n, c, h, w] = dx_temp[n, c * self.H * self.W + h * self.W + w]

        temp = self.matrix_multiplication(self.matrix_transformation(x_reshape), dout)   # (D, N) * (N, M) = (D, M)
        dw = self.matrix_transformation(temp)
        
        for n in range(self.N):
            for m in range(self.M):
                db[m] = db[m] + dout[n, m]

        # print(dx)
        # print(dw)
        # print(db)

        return dx, dw, db

if __name__ == "__main__":
    
    w = [[ 0.4355, -0.4319,  0.4577,  0.4015],
        [-0.0727, -0.0666,  0.3868, -0.2279],
        [ 0.3418,  0.2175,  0.1949,  0.1292],
        [ 0.0154, -0.2140, -0.4854,  0.0175],
        [-0.4074,  0.4407,  0.3272,  0.4753]]
    w = np.asarray(w)
    b = [0.3613, -0.1253, -0.2475,  0.0011,  0.3703]
    b = np.asarray(b)

    x = [[0,4,1,2,18,-1,2,2,0,1,3,2]]
    x = np.asarray(x)
    x = x.reshape(3, 1, 2, 2)
    y = [1,1,1]
    y = np.asarray(y)

    fc = full_connected(x, w, b)
    out= fc.full_connected_forward(x)
    # print(out)
    sf = softmax_loss(out, y)
    loss = sf.softmax_loss_forward(out, y)
    loss_back = sf.softmax_loss_backward()
    dx, dw, db = fc.full_connected_backward(loss_back)
    print("loss")
    print(loss)
    print("loss_back")
    print(loss_back)
    print("dx")
    print(dx)
    print("dw")
    print(dw)
    print("db")
    print(db)
    

    # num_inputs = 2
    # input_shape = (4, 5, 6)
    # output_dim = 3

    # input_size = num_inputs * np.prod(input_shape)
    # weight_size = output_dim * np.prod(input_shape)

    # # x = np.linspace(-0.1, 0.5, num=input_size).reshape(num_inputs, *input_shape)
    # # w = np.linspace(-0.2, 0.3, num=weight_size).reshape(np.prod(input_shape), output_dim)
    # # b = np.linspace(-0.3, 0.1, num=output_dim)
    # x = [0,4,1,2,18,-1,2,2,0,1,3,2,1,-7,2,2]
    # x = np.asarray(x)
    # x = x.reshape(4, 4, 1, 1)
    # w = [[-0.3194, -0.4941, -0.1468,  0.2596],
    #     [-0.3881,  0.3235, -0.0270,  0.3152],
    #     [-0.4510, -0.1233, -0.0899,  0.2907],
    #     [ 0.0687,  0.2494,  0.2706, -0.4014],
    #     [-0.0940, -0.4945, -0.3241, -0.0790]]
    # w = np.asarray(w)
    # b = [0.3508, 0.0411, 0.0950, 0.2683, 0.0413]
    # b = np.asarray(b)


    # fc = full_connected(x, w, b)
    # out= fc.full_connected_forward(x)
    # print(out)
    # correct_out = np.array([[ 1.49834967,  1.70660132,  1.91485297],
    #                         [ 3.25553199,  3.5141327,   3.77273342]])

    # # Compare your output with ours. The error should be around 1e-9.
    # print('Testing affine_forward function:')
    # print('difference: ', rel_error(out, correct_out))

    # Test the affine_backward function
    # np.random.seed(231)
    # x = np.random.randn(2, 16, 1, 1)
    # w = np.random.randn(16, 10)
    # b = np.random.randn(10)
    # dout = np.random.randn(2, 10)

    # dx_num = eval_numerical_gradient_array(lambda x: affine_forward(x, w, b)[0], x, dout)
    # dw_num = eval_numerical_gradient_array(lambda w: affine_forward(x, w, b)[0], w, dout)
    # db_num = eval_numerical_gradient_array(lambda b: affine_forward(x, w, b)[0], b, dout)

    # fc = full_connected(x, np.transpose(w), b)
    # out = fc.full_connected_forward(x)
    # dx, dw, db = fc.full_connected_backward(dout)

    # # The error should be around 1e-10
    # print('Testing affine_backward function:')
    # print('dx error: ', rel_error(dx_num, dx))
    # print('dw error: ', rel_error(dw_num, np.transpose(dw)))
    # print('db error: ', rel_error(db_num, db))