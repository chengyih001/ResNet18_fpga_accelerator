import numpy as np
from testing_functions import *

class batch_normalization:
    def __init__(self, x, gamma, beta, mode="train", eps=1e-5, momentum=0.9):

        # x: (N, D)
        # gamma: (D,)
        # beta: (D,)
        # running_mean: (D,)
        # running_var: (D,)

        self.x = x
        self.N, self.D = x.shape
        self.gamma = gamma
        self.beta = beta
        self.mode = mode
        self.eps = eps
        self.momentum = momentum
        self.mean = np.zeros(shape=(self.D))
        self.var = np.zeros(shape=(self.D))
        self.running_mean = np.zeros(shape=(self.D))
        self.running_var = np.zeros(shape=(self.D))
        self.out = np.zeros(shape=(self.N, self.D))
        self.normalized_x = np.zeros(shape=(self.N, self.D))

    def batchnorm_forward_pass(self, x):
        self.x = x
        if (self.mode == "train"):
            for d in range(self.D): 
                for n in range(self.N):     # find mean
                    self.mean[d] = self.mean[d] + self.x[n, d]
                self.mean[d] = self.mean[d]/self.N

                for n in range(self.N):     # find var
                    self.var[d] = self.var[d] + (self.x[n, d] - self.mean[d])**2
                self.var[d] = self.var[d]/self.N

                for n in range(self.N):
                    self.normalized_x[n, d] = (self.x[n, d] - self.mean[d]) / (self.var[d] + self.eps)**0.5
                    self.out[n, d] = self.gamma[d] * self.normalized_x[n, d] + self.beta[d]

                self.running_mean[d] = self.momentum * self.running_mean[d] + (1-self.momentum) * self.mean[d]
                self.running_var[d] = self.momentum * self.running_var[d] + (1-self.momentum) * self.var[d]

        elif (self.mode == "test"):
            for d in range(self.D):  
                for n in range(self.N):
                    self.normalized_x[n, d] = (self.x[n, d] - self.running_mean[d]) / (self.running_var[d] + self.eps)**0.5
                    self.out[n, d] = self.gamma[d] * self.normalized_x[n, d] + self.beta[d]
        
        else:
            print("Mode error!!")

        return self.out

    def batchnorm_backward_pass(self, dout):
        dx = np.zeros(shape=(self.N, self.D))
        dgamma = np.zeros(shape=(self.D))
        dbeta = np.zeros(shape=(self.D))

        dgamma_norm_x = np.zeros(shape=(self.N, self.D))
        dnorm_x = np.zeros(shape=(self.N, self.D))
        divar = np.zeros(shape=(self.D))
        dx_mu1 = np.zeros(shape=(self.N, self.D))
        dsqrtvar = np.zeros(shape=(self.D))
        dvar = np.zeros(shape=(self.D))
        dsq = np.zeros(shape=(self.N, self.D))
        dx_mu2 = np.zeros(shape=(self.N, self.D))
        dmu = np.zeros(shape=(self.D))
        dx1 = np.zeros(shape=(self.N, self.D))
        dx2 = np.zeros(shape=(self.N, self.D))

        for d in range(self.D):
            for n in range(self.N):
                dbeta[d] = dbeta[d] + dout[n, d]
                dgamma_norm_x[n, d] = 1 * dout[n, d]

                dgamma[d] = dgamma[d] + dgamma_norm_x[n, d] * self.normalized_x[n,d]
                dnorm_x[n, d] = dgamma_norm_x[n, d] * self.gamma[d]
                divar[d] = divar[d] + dnorm_x[n, d] * (self.x[n, d]-self.mean[d])

        for d in range(self.D):
            for n in range(self.N):
                dx_mu1[n, d] = dnorm_x[n, d] * (1/(self.var[d] + self.eps)**0.5)

                dsqrtvar[d] = divar[d] * (-1/(self.var[d] + self.eps))

                dvar[d] = dsqrtvar[d] * 0.5 * (1/(self.var[d] + self.eps)**0.5)

                dsq[n, d] = 1/self.N * dvar[d]

                dx_mu2[n, d] = 2 * (self.x[n, d] - self.mean[d]) * dsq[n, d]
                dmu[d] = dmu[d] + (-1 * (dx_mu1[n, d] + dx_mu2[n, d]))

        for d in range(self.D):
            for n in range(self.N):
                dx1[n, d] = 1 * (dx_mu1[n, d] + dx_mu2[n, d])

                dx2[n, d] = 1 * dmu[d] / self.N

                dx[n, d] = dx1[n, d] + dx2[n, d]


        return dx, dgamma, dbeta
        
if __name__ == "__main__":

    # Gradient check batchnorm backward pass
    np.random.seed(231)
    N, D = 4, 5
    x = 5 * np.random.randn(N, D) + 12
    gamma = np.random.randn(D)
    beta = np.random.randn(D)
    dout = np.random.randn(N, D)

    bn_param = {'mode': 'train'}
    fx = lambda x: batchnorm_forward(x, gamma, beta, bn_param)[0]
    fg = lambda a: batchnorm_forward(x, a, beta, bn_param)[0]
    fb = lambda b: batchnorm_forward(x, gamma, b, bn_param)[0]

    dx_num = eval_numerical_gradient_array(fx, x, dout)
    da_num = eval_numerical_gradient_array(fg, gamma.copy(), dout)
    db_num = eval_numerical_gradient_array(fb, beta.copy(), dout)

    bn = batch_normalization(x, gamma, beta, "train")
    out = bn.batchnorm_forward_pass(x)
    dx, dgamma, dbeta = bn.batchnorm_backward_pass(dout)
    print('dx error: ', rel_error(dx_num, dx))
    print('dgamma error: ', rel_error(da_num, dgamma))
    print('dbeta error: ', rel_error(db_num, dbeta))



    # # # Check the training-time forward pass by checking means and variances
    # # # of features both before and after batch normalization

    # # # Simulate the forward pass for a two-layer network
    # np.random.seed(231)
    # N, D1, D2, D3 = 200, 50, 60, 3
    # X = np.random.randn(N, D1)
    # W1 = np.random.randn(D1, D2)
    # W2 = np.random.randn(D2, D3)
    # a = np.maximum(0, X.dot(W1)).dot(W2)

    # print('Before batch normalization:')
    # print('  means: ', a.mean(axis=0))
    # print('  stds: ', a.std(axis=0))

    # bn = batch_normalization(a, np.ones(D3), np.zeros(D3), "train")
    # out = bn.batchnorm_forward_pass(X)

    # # Means should be close to zero and stds close to one
    # print('After batch normalization (gamma=1, beta=0)')
    # print('  mean: ', out.mean(axis=0))
    # print('  std: ', out.std(axis=0))

    # # Now means should be close to beta and stds close to gamma
    # gamma = np.asarray([1.0, 2.0, 3.0])
    # beta = np.asarray([11.0, 12.0, 13.0])
    # bn2 = batch_normalization(a, gamma, beta, "train")
    # out = bn2.batchnorm_forward_pass(X)
    # print('After batch normalization (nontrivial gamma, beta)')
    # print('  means: ', out.mean(axis=0))
    # print('  stds: ', out.std(axis=0))