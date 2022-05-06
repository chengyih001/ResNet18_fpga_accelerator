import numpy as np
from testing_functions import *

def sgd(weight, dw, b, db, layer, learning_rate = 1e-4, config=None):
    # print(weight)
    # print(dw)
    # print(layer)
    # config = None
    if (layer == "conv"):
        F, C, H, W = weight.shape
        next_w = np.zeros(shape=(F,C,H,W))
        next_b = np.zeros(shape=(F,))
        for f in range(F):
            next_b[f] = b[f] - learning_rate * db[f]
            for c in range(C):
                for h in range(H):
                    for w in range(W):
                        next_w[f, c, h, w] = weight[f, c, h, w] - learning_rate * dw[f, c, h, w]
        # print(next_w)
        return next_w, next_b, config

    elif (layer == "batch_norm_conv"):
        C, = weight.shape
        next_w = np.zeros(shape=(C,))
        next_b = np.zeros(shape=(C,))
        for c in range(C):
            next_w[c] = weight[c] - learning_rate * dw[c]
            next_b[c] = b[c] - learning_rate * db[c]
        # print(next_w)
        return next_w, next_b, config

    elif (layer == "full_connected"):
        M, D = weight.shape
        next_w = np.zeros(shape=(M, D))
        next_b = np.zeros(shape=(M,))
        for m in range(M):
            next_b[m] = b[m] - learning_rate * db[m]
            for d in range(D):
                next_w[m, d] = weight[m, d] - learning_rate * dw[m, d]
        # print(next_w)
        return next_w, next_b, config

    else:
        raise Exception("Optim layer name Wrong!")

def sgd_momentum(weight, dw, b, db, layer, learning_rate = 1e-4, config=None):     

    # config = {
    #   - momentum
    #   - w_velocity
    #   - b_velocity
    # }

    momentum = config['momentum']
    w_velocity = config['w_velocity']
    b_velocity = config['b_velocity']
    
    #print(w_velocity)

    if (layer == "conv"):
        F, C, H, W = weight.shape
        next_w = np.zeros(shape=(F,C,H,W))
        next_b = np.zeros(shape=(F,))
        # print(w_velocity)
        # print(dw)
        for f in range(F):
            b_velocity[f] = momentum * b_velocity[f] -learning_rate * db[f]
            next_b[f] = b[f] + b_velocity[f]
            for c in range(C):
                for h in range(H):
                    for w in range(W):
                        w_velocity[f, c, h, w] = momentum * w_velocity[f, c, h, w] - learning_rate * dw[f, c, h, w]
                        next_w[f, c, h, w] = weight[f, c, h, w] + w_velocity[f, c, h, w]
        config['w_velocity'] = w_velocity
        config['b_velocity'] = b_velocity
        #print(config['w_velocity'])
        # print(next_b)
        return next_w, next_b, config

    elif (layer == "batch_norm_conv"):
        C, = weight.shape
        next_w = np.zeros(shape=(C,))
        next_b = np.zeros(shape=(C,))
        for c in range(C):
            w_velocity[c] = momentum * w_velocity[c] - learning_rate * dw[c]
            b_velocity[c] = momentum * b_velocity[c] - learning_rate * db[c]

            next_w[c] = weight[c] + w_velocity[c]
            next_b[c] = b[c] + b_velocity[c]
        config['w_velocity'] = w_velocity
        config['b_velocity'] = b_velocity
        return next_w, next_b, config

    elif (layer == "full_connected"):
        M, D = weight.shape
        next_w = np.zeros(shape=(M,D))
        next_b = np.zeros(shape=(M,))
        for m in range(M):
            b_velocity[m] = momentum * b_velocity[m] - learning_rate * db[m]
            next_b[m] = b[m] + b_velocity[m]
            for d in range(D):
                #w_velocity[d, m] = momentum * w_velocity[d, m] + self.dw[d, m]
                #self.w[d, m] = self.w[d, m] - self.learning_rate * w_velocity[d, m]
                w_velocity[m, d] = momentum * w_velocity[m, d] - learning_rate * dw[m, d]
                next_w[m, d] = weight[m, d] + w_velocity[m, d]
        config['w_velocity'] = w_velocity
        config['b_velocity'] = b_velocity
        return next_w, next_b, config

    else:
        raise Exception("Optim layer name Wrong!")

if __name__ == "__main__":
    N, D = 4, 5
    w = np.linspace(-0.4, 0.6, num=N*D).reshape(N, D)
    dw = np.linspace(-0.6, 0.4, num=N*D).reshape(N, D)
    dummy = np.zeros(shape=(5))
    v = np.linspace(0.6, 0.9, num=N*D).reshape(N, D)

    #config = {'learning_rate': 1e-3, 'velocity': v}
    next_w, _, config = sgd_momentum(w, dw, dummy, dummy, "full_connected", 1e-3, config={'momentum':0.9, 'w_velocity':v, 'b_velocity':dummy})
    next_velocity = config["w_velocity"]

    expected_next_w = np.asarray([
    [ 0.1406,      0.20738947,  0.27417895,  0.34096842,  0.40775789],
    [ 0.47454737,  0.54133684,  0.60812632,  0.67491579,  0.74170526],
    [ 0.80849474,  0.87528421,  0.94207368,  1.00886316,  1.07565263],
    [ 1.14244211,  1.20923158,  1.27602105,  1.34281053,  1.4096    ]])
    expected_velocity = np.asarray([
    [ 0.5406,      0.55475789,  0.56891579, 0.58307368,  0.59723158],
    [ 0.61138947,  0.62554737,  0.63970526,  0.65386316,  0.66802105],
    [ 0.68217895,  0.69633684,  0.71049474,  0.72465263,  0.73881053],
    [ 0.75296842,  0.76712632,  0.78128421,  0.79544211,  0.8096    ]])

    print('next_w error: ', rel_error(next_w, expected_next_w))
    print('velocity error: ', rel_error(expected_velocity, next_velocity))
