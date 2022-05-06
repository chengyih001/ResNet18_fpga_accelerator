from data_utils import data_utils
from resnet18 import resnet18_cifar10
from helper import *
import numpy as np

TRAIN_SIZE = 1
VAL_SIZE = 1

HLS_TRAIN_SIZE = 1

LEARNING_RATE = 3e-3
WEIGHT_SCALE = 5e-2
BATCH_SIZE = 1
EPOCHS = 1

def main():

    # resnet18_cifar10: (X_train, y_train, X_val, y_val, weight_scale, batch_size=100)

    dataset = data_utils('./dataset_cifar-10')
    data = dataset.data_preprocessing()
    for key, value in data.items():
        print('%s:' % key, value.shape)

    # Construct smaller dataset for initial testing
    X_train = data['X_train']
    Y_train = data['Y_train']
    X_valid = data['X_valid']
    Y_valid = data['Y_valid']
    
    smaller_data = {}
    smaller_data['X_train'] = X_train[0:TRAIN_SIZE, :, :, :]
    smaller_data['Y_train'] = Y_train[0:TRAIN_SIZE]
    smaller_data['X_valid'] = X_valid[0:VAL_SIZE, :, :, :]
    smaller_data['Y_valid'] = Y_valid[0:VAL_SIZE]

    array4_to_param(X_train[0:HLS_TRAIN_SIZE, :, :, :], 'testing_images')
    array1_to_param(Y_train[0:HLS_TRAIN_SIZE],'validate_images')
    print("Exported HLS data!!!")

    # smaller_data['X_train'] = np.ones(shape=(1, 3, 32, 32))
    # smaller_data['Y_train'] = np.ones(shape=(1,))
    # smaller_data['X_valid'] = np.ones(shape=(1, 3, 32, 32))
    # smaller_data['Y_valid'] = np.ones(shape=(1,))

    # model = resnet18_cifar10(data['X_train'], data['Y_train'], data['X_valid'], data['Y_valid'], weight_scale=5e-2, batch_size=10)
    model = resnet18_cifar10(smaller_data['X_train'], smaller_data['Y_train'], smaller_data['X_valid'], smaller_data['Y_valid'], \
        learning_rate=LEARNING_RATE, weight_scale=WEIGHT_SCALE, batch_size=BATCH_SIZE, val_size=VAL_SIZE)
    #model.train(epochs=EPOCHS)
    X_train_batch, y_train_batch, X_val_batch, y_val_batch = model.construct_batch()
    model.resnet18_forward(X_train_batch['batch_0'], y_train_batch['batch_0'], mode="train")

    print("Exporting weights!!!")

    # Write data to parameter files
    # layer 1
    array4_to_param(model.conv_1.weight, 'conv_weight_0')
    array1_to_param(model.conv_1.bias, 'conv_bias_0')
    array1_to_param(model.batch_norm_conv1.running_mean, 'bn_mean_0')
    array1_to_param(model.batch_norm_conv1.running_var, 'bn_var_0')
    array1_to_param(model.batch_norm_conv1.gamma, 'bn_gamma_0')
    array1_to_param(model.batch_norm_conv1.beta, 'bn_beta_0')

    # layer 2
    array4_to_param(model.basic_block_2.conv1.weight, 'conv_weight_1_1')
    array1_to_param(model.basic_block_2.conv1.bias, 'conv_bias_1_1')
    array1_to_param(model.basic_block_2.bn1.running_mean, 'bn_mean_1_1')
    array1_to_param(model.basic_block_2.bn1.running_var, 'bn_var_1_1')
    array1_to_param(model.basic_block_2.bn1.gamma, 'bn_gamma_1_1')
    array1_to_param(model.basic_block_2.bn1.beta, 'bn_beta_1_1')

    array4_to_param(model.basic_block_2.conv2.weight, 'conv_weight_1_2')
    array1_to_param(model.basic_block_2.conv2.bias, 'conv_bias_1_2')
    array1_to_param(model.basic_block_2.bn2.running_mean, 'bn_mean_1_2')
    array1_to_param(model.basic_block_2.bn2.running_var, 'bn_var_1_2')
    array1_to_param(model.basic_block_2.bn2.gamma, 'bn_gamma_1_2')
    array1_to_param(model.basic_block_2.bn2.beta, 'bn_beta_1_2')

    # layer 3
    array4_to_param(model.basic_block_3.conv1.weight, 'conv_weight_2_1')
    array1_to_param(model.basic_block_3.conv1.bias, 'conv_bias_2_1')
    array1_to_param(model.basic_block_3.bn1.running_mean, 'bn_mean_2_1')
    array1_to_param(model.basic_block_3.bn1.running_var, 'bn_var_2_1')
    array1_to_param(model.basic_block_3.bn1.gamma, 'bn_gamma_2_1')
    array1_to_param(model.basic_block_3.bn1.beta, 'bn_beta_2_1')

    array4_to_param(model.basic_block_3.conv2.weight, 'conv_weight_2_2')
    array1_to_param(model.basic_block_3.conv2.bias, 'conv_bias_2_2')
    array1_to_param(model.basic_block_3.bn2.running_mean, 'bn_mean_2_2')
    array1_to_param(model.basic_block_3.bn2.running_var, 'bn_var_2_2')
    array1_to_param(model.basic_block_3.bn2.gamma, 'bn_gamma_2_2')
    array1_to_param(model.basic_block_3.bn2.beta, 'bn_beta_2_2')

    # layer 4
    array4_to_param(model.basic_block_4.conv1.weight, 'conv_weight_3_1')
    array1_to_param(model.basic_block_4.conv1.bias, 'conv_bias_3_1')
    array1_to_param(model.basic_block_4.bn1.running_mean, 'bn_mean_3_1')
    array1_to_param(model.basic_block_4.bn1.running_var, 'bn_var_3_1')
    array1_to_param(model.basic_block_4.bn1.gamma, 'bn_gamma_3_1')
    array1_to_param(model.basic_block_4.bn1.beta, 'bn_beta_3_1')

    array4_to_param(model.basic_block_4.conv2.weight, 'conv_weight_3_2')
    array1_to_param(model.basic_block_4.conv2.bias, 'conv_bias_3_2')
    array1_to_param(model.basic_block_4.bn2.running_mean, 'bn_mean_3_2')
    array1_to_param(model.basic_block_4.bn2.running_var, 'bn_var_3_2')
    array1_to_param(model.basic_block_4.bn2.gamma, 'bn_gamma_3_2')
    array1_to_param(model.basic_block_4.bn2.beta, 'bn_beta_3_2')
    
    # layer 5
    array4_to_param(model.basic_block_5.conv1.weight, 'conv_weight_4_1')
    array1_to_param(model.basic_block_5.conv1.bias, 'conv_bias_4_1')
    array1_to_param(model.basic_block_5.bn1.running_mean, 'bn_mean_4_1')
    array1_to_param(model.basic_block_5.bn1.running_var, 'bn_var_4_1')
    array1_to_param(model.basic_block_5.bn1.gamma, 'bn_gamma_4_1')
    array1_to_param(model.basic_block_5.bn1.beta, 'bn_beta_4_1')

    array4_to_param(model.basic_block_5.conv2.weight, 'conv_weight_4_2')
    array1_to_param(model.basic_block_5.conv2.bias, 'conv_bias_4_2')
    array1_to_param(model.basic_block_5.bn2.running_mean, 'bn_mean_4_2')
    array1_to_param(model.basic_block_5.bn2.running_var, 'bn_var_4_2')
    array1_to_param(model.basic_block_5.bn2.gamma, 'bn_gamma_4_2')
    array1_to_param(model.basic_block_5.bn2.beta, 'bn_beta_4_2')

    # layer 6
    array4_to_param(model.basic_block_6.conv1.weight, 'conv_weight_5_1')
    array1_to_param(model.basic_block_6.conv1.bias, 'conv_bias_5_1')
    array1_to_param(model.basic_block_6.bn1.running_mean, 'bn_mean_5_1')
    array1_to_param(model.basic_block_6.bn1.running_var, 'bn_var_5_1')
    array1_to_param(model.basic_block_6.bn1.gamma, 'bn_gamma_5_1')
    array1_to_param(model.basic_block_6.bn1.beta, 'bn_beta_5_1')

    array4_to_param(model.basic_block_6.conv2.weight, 'conv_weight_5_2')
    array1_to_param(model.basic_block_6.conv2.bias, 'conv_bias_5_2')
    array1_to_param(model.basic_block_6.bn2.running_mean, 'bn_mean_5_2')
    array1_to_param(model.basic_block_6.bn2.running_var, 'bn_var_5_2')
    array1_to_param(model.basic_block_6.bn2.gamma, 'bn_gamma_5_2')
    array1_to_param(model.basic_block_6.bn2.beta, 'bn_beta_5_2')

    # layer 7
    array4_to_param(model.basic_block_7.conv1.weight, 'conv_weight_6_1')
    array1_to_param(model.basic_block_7.conv1.bias, 'conv_bias_6_1')
    array1_to_param(model.basic_block_7.bn1.running_mean, 'bn_mean_6_1')
    array1_to_param(model.basic_block_7.bn1.running_var, 'bn_var_6_1')
    array1_to_param(model.basic_block_7.bn1.gamma, 'bn_gamma_6_1')
    array1_to_param(model.basic_block_7.bn1.beta, 'bn_beta_6_1')

    array4_to_param(model.basic_block_7.conv2.weight, 'conv_weight_6_2')
    array1_to_param(model.basic_block_7.conv2.bias, 'conv_bias_6_2')
    array1_to_param(model.basic_block_7.bn2.running_mean, 'bn_mean_6_2')
    array1_to_param(model.basic_block_7.bn2.running_var, 'bn_var_6_2')
    array1_to_param(model.basic_block_7.bn2.gamma, 'bn_gamma_6_2')
    array1_to_param(model.basic_block_7.bn2.beta, 'bn_beta_6_2')

    # layer 8
    array4_to_param(model.basic_block_8.conv1.weight, 'conv_weight_7_1')
    array1_to_param(model.basic_block_8.conv1.bias, 'conv_bias_7_1')
    array1_to_param(model.basic_block_8.bn1.running_mean, 'bn_mean_7_1')
    array1_to_param(model.basic_block_8.bn1.running_var, 'bn_var_7_1')
    array1_to_param(model.basic_block_8.bn1.gamma, 'bn_gamma_7_1')
    array1_to_param(model.basic_block_8.bn1.beta, 'bn_beta_7_1')

    array4_to_param(model.basic_block_8.conv2.weight, 'conv_weight_7_2')
    array1_to_param(model.basic_block_8.conv2.bias, 'conv_bias_7_2')
    array1_to_param(model.basic_block_8.bn2.running_mean, 'bn_mean_7_2')
    array1_to_param(model.basic_block_8.bn2.running_var, 'bn_var_7_2')
    array1_to_param(model.basic_block_8.bn2.gamma, 'bn_gamma_7_2')
    array1_to_param(model.basic_block_8.bn2.beta, 'bn_beta_7_2')

    # layer 9
    array4_to_param(model.basic_block_9.conv1.weight, 'conv_weight_8_1')
    array1_to_param(model.basic_block_9.conv1.bias, 'conv_bias_8_1')
    array1_to_param(model.basic_block_9.bn1.running_mean, 'bn_mean_8_1')
    array1_to_param(model.basic_block_9.bn1.running_var, 'bn_var_8_1')
    array1_to_param(model.basic_block_9.bn1.gamma, 'bn_gamma_8_1')
    array1_to_param(model.basic_block_9.bn1.beta, 'bn_beta_8_1')

    array4_to_param(model.basic_block_9.conv2.weight, 'conv_weight_8_2')
    array1_to_param(model.basic_block_9.conv2.bias, 'conv_bias_8_2')
    array1_to_param(model.basic_block_9.bn2.running_mean, 'bn_mean_8_2')
    array1_to_param(model.basic_block_9.bn2.running_var, 'bn_var_8_2')
    array1_to_param(model.basic_block_9.bn2.gamma, 'bn_gamma_8_2')
    array1_to_param(model.basic_block_9.bn2.beta, 'bn_beta_8_2')

    # layer 10
    array4_to_param(model.basic_block_10.conv1.weight, 'conv_weight_9_1')
    array1_to_param(model.basic_block_10.conv1.bias, 'conv_bias_9_1')
    array1_to_param(model.basic_block_10.bn1.running_mean, 'bn_mean_9_1')
    array1_to_param(model.basic_block_10.bn1.running_var, 'bn_var_9_1')
    array1_to_param(model.basic_block_10.bn1.gamma, 'bn_gamma_9_1')
    array1_to_param(model.basic_block_10.bn1.beta, 'bn_beta_9_1')

    array4_to_param(model.basic_block_10.conv2.weight, 'conv_weight_9_2')
    array1_to_param(model.basic_block_10.conv2.bias, 'conv_bias_9_2')
    array1_to_param(model.basic_block_10.bn2.running_mean, 'bn_mean_9_2')
    array1_to_param(model.basic_block_10.bn2.running_var, 'bn_var_9_2')
    array1_to_param(model.basic_block_10.bn2.gamma, 'bn_gamma_9_2')
    array1_to_param(model.basic_block_10.bn2.beta, 'bn_beta_9_2')

    # layer 11
    # layer 12 - fc
    w_reshape = model.full_connected_12.matrix_transformation(model.full_connected_12.w)
    array2_to_param(w_reshape, 'fc_weight')
    array1_to_param(model.full_connected_12.b, 'fc_bias')


if __name__ == "__main__":
    main()