import numpy as np
import struct

def array4_to_param(X, filename):
    A, B, C, D = X.shape
    x_list = X.reshape(A*B*C*D,).tolist()
    file = open('parameters/'+filename+'.bin', 'wb')
    s = struct.pack('f'*len(x_list), *x_list)
    file.write(s)
    file.close()

def array2_to_param(X, filename):
    A, B= X.shape
    x_list = X.reshape(A*B,).tolist()
    file = open('parameters/'+filename+'.bin', 'wb')
    s = struct.pack('f'*len(x_list), *x_list)
    file.write(s)
    file.close()

def array1_to_param(X, filename):
    A = X.shape
    x_list = X.reshape(A,).tolist()
    file = open('parameters/'+filename+'.bin', 'wb')
    s = struct.pack('f'*len(x_list), *x_list)
    file.write(s)
    file.close()

# def array4_to_param(X, filename):
#     A, B, C, D = X.shape
#     x_list = X.reshape(A*B*C*D,).tolist()
#     S = ", ".join(str(e)[0:8] for e in x_list)
#     file = open('parameters/'+filename+'.h', 'w')
#     file.write(S)
#     file.close()

# def array2_to_param(X, filename):
#     A, B = X.shape
#     x_list = X.reshape(A*B,).tolist()
#     S = ", ".join(str(e)[0:8] for e in x_list)
#     file = open('parameters/'+filename+'.h', 'w')
#     file.write(S)
#     file.close()

    
# def array1_to_param(X, filename):
#     A = X.shape
#     x_list = X.reshape(A,).tolist()
#     S = ", ".join(str(e)[0:8] for e in x_list)
#     file = open('parameters/'+filename+'.h', 'w')
#     file.write(S)
#     file.close()

