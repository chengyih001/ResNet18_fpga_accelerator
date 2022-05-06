import numpy as np
import math

from testing_functions import *

class softmax_loss:
    def __init__(self, x, y):

        # x: (N, M)
        # y: (N,)

        # loss: int
        # dx: (N, M)

        self.x = x
        self.y = y

        self.N, self.M = x.shape
        self.out = np.zeros(shape=(self.N, self.M))

    def softmax_loss_forward(self, x, y):
        self.x = x
        self.y = y
        loss = 0
        exp_sum = np.zeros(shape=(self.N))
        
        # print(x)
        for n in range(self.N):
            for m in range(self.M):
                #print(self.x[n, m])
                exp_sum[n] = exp_sum[n] + math.exp(self.x[n, m])
        for n in range(self.N):
            for m in range(self.M):
                self.out[n, m] = math.exp(self.x[n, m]) / exp_sum[n]
                if (self.y[n] == m):
                    loss = loss - math.log(self.out[n, m])
        loss = loss/self.N

        # print(self.out)
        # print(y)

        return loss
    
    def softmax_loss_backward(self):
        dx = np.zeros(shape=(self.N, self.M))
        for n in range(self.N):
            for m in range(self.M):
                if (self.y[n] == m):
                    dx[n, m] = (self.out[n, m] - 1) / self.N
                else:
                    dx[n, m] = (self.out[n, m]) / self.N

        return dx

if __name__ == "__main__":
    a = [0,4,1,2,18,-1,2,2,0,1,3,2]
    a = np.asarray(a)
    a = a.reshape(3, 4)
    b = [1, 1, 1]
    b =np.asarray(b)

    model = softmax_loss(a, b)
    loss = model.softmax_loss_forward(a, b)
    dx = model.softmax_loss_backward()
    print(loss)
    print(dx)

    # np.random.seed(231)
    # num_classes, num_inputs = 10, 50
    # x = 0.001 * np.random.randn(num_inputs, num_classes)
    # y = np.random.randint(num_classes, size=num_inputs)

    # dx_num = eval_numerical_gradient(lambda x: softmax(x, y)[0], x, verbose=False)
    # k, j = softmax(x, y)

    # # a = [0,4,1,2,18,-1,2,2,0,1,3,2]
    # # a = np.asarray(a)
    # # a = a.reshape(3, 4)
    # # b = [1, 1, 1]
    # # b =np.asarray(b)
    
    # sl = softmax_loss(x, y)
    # loss = sl.softmax_loss_forward(x, y)
    # dx = sl.softmax_loss_backward()
    # # print(loss)

    # # Test softmax_loss function. Loss should be 2.3 and dx error should be 1e-8
    # print('\nTesting softmax_loss:')
    # print('loss: ', loss)
    # print('dx error: ', rel_error(dx_num, dx))