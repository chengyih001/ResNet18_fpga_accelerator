import struct
import numpy as np

import torch
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms

print("Running on GPU? :", torch.cuda.is_available())
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# 3x3 convolution
def conv3x3(in_channels, out_channels, stride=1):
    return nn.Conv2d(in_channels, out_channels, kernel_size=3, 
                     stride=stride, padding=1, bias=True)

# Residual block
class ResidualBlock(nn.Module):
    def __init__(self, in_channels, out_channels, stride=1, downsample=False):
        super(ResidualBlock, self).__init__()
        self.conv1 = conv3x3(in_channels, out_channels, stride)
        self.bn1 = nn.BatchNorm2d(out_channels)
        self.relu = nn.ReLU(inplace=True)
        self.conv2 = conv3x3(out_channels, out_channels)
        self.bn2 = nn.BatchNorm2d(out_channels)
        self.downsample = downsample
        
    def forward(self, x):
        # residual = x
        out = self.conv1(x)
        out = self.bn1(out)
        # if self.downsample:
        #     residual = out
        out = self.relu(out)
        residual = out
        out = self.conv2(out)
        out = self.bn2(out)
        # if self.downsample:
        #     residual = self.downsample(x)
        out += residual
        out = self.relu(out)
        return out

# ResNet
class ResNet(nn.Module):
    def __init__(self, block, layers, num_classes=10):
        super(ResNet, self).__init__()
        self.in_channels = 16
        self.conv = conv3x3(3, 16)
        self.bn = nn.BatchNorm2d(16)
        self.relu = nn.ReLU(inplace=True)
        self.layer1 = self.make_layer(block, 16, layers[0])
        self.layer2 = self.make_layer(block, 32, layers[1], 2)
        self.layer3 = self.make_layer(block, 64, layers[2], 2)
        self.avg_pool = nn.AvgPool2d(8)
        self.fc = nn.Linear(64, num_classes)
        
    def make_layer(self, block, out_channels, blocks, stride=1):
        downsample = False
        if (stride != 1) or (self.in_channels != out_channels):
            # downsample = nn.Sequential(
            #     conv3x3(self.in_channels, out_channels, stride=stride),
            #     nn.BatchNorm2d(out_channels))
            downsample = True
        layers = []
        layers.append(block(self.in_channels, out_channels, stride, downsample))
        self.in_channels = out_channels
        for i in range(1, blocks):
            layers.append(block(out_channels, out_channels))
        return nn.Sequential(*layers)
    
    def forward(self, x):
        out = self.conv(x)
        out = self.bn(out)
        out = self.relu(out)
        out = self.layer1(out) 
        out = self.layer2(out)
        out = self.layer3(out)
        out = self.avg_pool(out)
        out = out.view(out.size(0), -1)
        out = self.fc(out)
        return out

model = ResNet(ResidualBlock, [2, 3, 4]).to(device)
model.load_state_dict(torch.load("resnet_main2.ckpt"))

test_dataset = torchvision.datasets.CIFAR10(root='../../data/',
                                            train=False, 
                                            transform=transforms.ToTensor())


test_loader = torch.utils.data.DataLoader(dataset=test_dataset,
                                          batch_size=1,
                                          shuffle=False)
correct = 0
total = 0
model.eval()

correct = 0
total = 0
with torch.no_grad():
    correct = 0
    total = 0
    i=0
    for images, labels in test_loader:
        images = images.to(device)
        labels = labels.to(device)
        outputs = model(images)
        _, predicted = torch.max(outputs.data, 1)
        total += labels.size(0)
        correct += (predicted == labels).sum().item()
        i += 1
        if i==100:
            print('Accuracy of the model on the test images 100: {} %'.format(100 * correct / total))
        if i==1000:
            print('Accuracy of the model on the test images 1000: {} %'.format(100 * correct / total))
        if i==5000:
            print('Accuracy of the model on the test images 5000: {} %'.format(100 * correct / total))

    print('Accuracy of the model on the test images 10000: {} %'.format(100 * correct / total))
# def array4_to_param(X, filename):
#     A, B, C, D = X.shape
#     x_list = X.reshape(A*B*C*D,).tolist()
#     file = open('parameters_py/'+filename, 'wb')
#     s = struct.pack('f'*len(x_list), *x_list)
#     file.write(s)
#     file.close()

# def array2_to_param(X, filename):
#     A, B= X.shape
#     x_list = X.reshape(A*B,).tolist()
#     file = open('parameters_py/'+filename, 'wb')
#     s = struct.pack('f'*len(x_list), *x_list)
#     file.write(s)
#     file.close()

# def array1_to_param(X, filename):
#     A = X.shape
#     x_list = X.reshape(A,).tolist()
#     file = open('parameters_py/'+filename, 'wb')
#     s = struct.pack('f'*len(x_list), *x_list)
#     file.write(s)
#     file.close()

# array4_to_param(test_images, filename)
# array1_to_param(labels, val_filename)





# # print("model param")
# # for param_tensor in model.state_dict():
# #     print(param_tensor, "\t", model.state_dict()[param_tensor].size())

# array4_to_param(model.state_dict()["conv.weight"].cpu().numpy(), 'conv_weight_0')
# array1_to_param(model.state_dict()["conv.bias"].cpu().numpy(), 'conv_bias_0')
# temp = model.state_dict()["bn.weight"].cpu().numpy()
# temp2 = model.state_dict()["bn.bias"].cpu().numpy()
# temp3 = model.state_dict()["bn.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["bn.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_0')


# # layer 2
# array4_to_param(model.state_dict()["layer1.0.conv1.weight"].cpu().numpy(), 'conv_weight_1_1')
# array1_to_param(model.state_dict()["layer1.0.conv1.bias"].cpu().numpy(), 'conv_bias_1_1')
# temp = model.state_dict()["layer1.0.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer1.0.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer1.0.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer1.0.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_1_1')

# array4_to_param(model.state_dict()["layer1.0.conv2.weight"].cpu().numpy(), 'conv_weight_1_2')
# array1_to_param(model.state_dict()["layer1.0.conv2.bias"].cpu().numpy(), 'conv_bias_1_2')
# temp = model.state_dict()["layer1.0.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer1.0.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer1.0.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer1.0.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_1_2')

# # layer 3
# array4_to_param(model.state_dict()["layer1.1.conv1.weight"].cpu().numpy(), 'conv_weight_2_1')
# array1_to_param(model.state_dict()["layer1.1.conv1.bias"].cpu().numpy(), 'conv_bias_2_1')
# temp = model.state_dict()["layer1.1.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer1.1.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer1.1.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer1.1.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_2_1')

# array4_to_param(model.state_dict()["layer1.1.conv2.weight"].cpu().numpy(), 'conv_weight_2_2')
# array1_to_param(model.state_dict()["layer1.1.conv2.bias"].cpu().numpy(), 'conv_bias_2_2')
# temp = model.state_dict()["layer1.1.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer1.1.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer1.1.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer1.1.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_2_2')

# # layer 4
# array4_to_param(model.state_dict()["layer2.0.conv1.weight"].cpu().numpy(), 'conv_weight_3_1')
# array1_to_param(model.state_dict()["layer2.0.conv1.bias"].cpu().numpy(), 'conv_bias_3_1')
# temp = model.state_dict()["layer2.0.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer2.0.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer2.0.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer2.0.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_3_1')

# array4_to_param(model.state_dict()["layer2.0.conv2.weight"].cpu().numpy(), 'conv_weight_3_2')
# array1_to_param(model.state_dict()["layer2.0.conv2.bias"].cpu().numpy(), 'conv_bias_3_2')
# temp = model.state_dict()["layer2.0.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer2.0.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer2.0.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer2.0.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_3_2')

# # layer 5
# array4_to_param(model.state_dict()["layer2.1.conv1.weight"].cpu().numpy(), 'conv_weight_4_1')
# array1_to_param(model.state_dict()["layer2.1.conv1.bias"].cpu().numpy(), 'conv_bias_4_1')
# temp = model.state_dict()["layer2.1.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer2.1.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer2.1.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer2.1.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_4_1')

# array4_to_param(model.state_dict()["layer2.1.conv2.weight"].cpu().numpy(), 'conv_weight_4_2')
# array1_to_param(model.state_dict()["layer2.1.conv2.bias"].cpu().numpy(), 'conv_bias_4_2')
# temp = model.state_dict()["layer2.1.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer2.1.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer2.1.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer2.1.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_4_2')

# # layer 6
# array4_to_param(model.state_dict()["layer2.2.conv1.weight"].cpu().numpy(), 'conv_weight_5_1')
# array1_to_param(model.state_dict()["layer2.2.conv1.bias"].cpu().numpy(), 'conv_bias_5_1')
# temp = model.state_dict()["layer2.2.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer2.2.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer2.2.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer2.2.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_5_1')

# array4_to_param(model.state_dict()["layer2.2.conv2.weight"].cpu().numpy(), 'conv_weight_5_2')
# array1_to_param(model.state_dict()["layer2.2.conv2.bias"].cpu().numpy(), 'conv_bias_5_2')
# temp = model.state_dict()["layer2.2.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer2.2.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer2.2.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer2.2.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_5_2')

# # layer 7
# array4_to_param(model.state_dict()["layer3.0.conv1.weight"].cpu().numpy(), 'conv_weight_6_1')
# array1_to_param(model.state_dict()["layer3.0.conv1.bias"].cpu().numpy(), 'conv_bias_6_1')
# temp = model.state_dict()["layer3.0.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.0.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.0.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.0.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_6_1')

# array4_to_param(model.state_dict()["layer3.0.conv2.weight"].cpu().numpy(), 'conv_weight_6_2')
# array1_to_param(model.state_dict()["layer3.0.conv2.bias"].cpu().numpy(), 'conv_bias_6_2')
# temp = model.state_dict()["layer3.0.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.0.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.0.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.0.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_6_2')

# # layer 8
# array4_to_param(model.state_dict()["layer3.1.conv1.weight"].cpu().numpy(), 'conv_weight_7_1')
# array1_to_param(model.state_dict()["layer3.1.conv1.bias"].cpu().numpy(), 'conv_bias_7_1')
# temp = model.state_dict()["layer3.1.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.1.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.1.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.1.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_7_1')

# array4_to_param(model.state_dict()["layer3.1.conv2.weight"].cpu().numpy(), 'conv_weight_7_2')
# array1_to_param(model.state_dict()["layer3.1.conv2.bias"].cpu().numpy(), 'conv_bias_7_2')
# temp = model.state_dict()["layer3.1.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.1.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.1.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.1.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_7_2')

# # layer 9
# array4_to_param(model.state_dict()["layer3.2.conv1.weight"].cpu().numpy(), 'conv_weight_8_1')
# array1_to_param(model.state_dict()["layer3.2.conv1.bias"].cpu().numpy(), 'conv_bias_8_1')
# temp = model.state_dict()["layer3.2.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.2.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.2.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.2.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_8_1')

# array4_to_param(model.state_dict()["layer3.2.conv2.weight"].cpu().numpy(), 'conv_weight_8_2')
# array1_to_param(model.state_dict()["layer3.2.conv2.bias"].cpu().numpy(), 'conv_bias_8_2')
# temp = model.state_dict()["layer3.2.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.2.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.2.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.2.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_8_2')

# # layer 10
# array4_to_param(model.state_dict()["layer3.3.conv1.weight"].cpu().numpy(), 'conv_weight_9_1')
# array1_to_param(model.state_dict()["layer3.3.conv1.bias"].cpu().numpy(), 'conv_bias_9_1')
# temp = model.state_dict()["layer3.3.bn1.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.3.bn1.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.3.bn1.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.3.bn1.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_9_1')

# array4_to_param(model.state_dict()["layer3.3.conv2.weight"].cpu().numpy(), 'conv_weight_9_2')
# array1_to_param(model.state_dict()["layer3.3.conv2.bias"].cpu().numpy(), 'conv_bias_9_2')
# temp = model.state_dict()["layer3.3.bn2.weight"].cpu().numpy()
# temp2 = model.state_dict()["layer3.3.bn2.bias"].cpu().numpy()
# temp3 = model.state_dict()["layer3.3.bn2.running_mean"].cpu().numpy()
# temp4 = model.state_dict()["layer3.3.bn2.running_var"].cpu().numpy()
# temp = np.append(temp, temp2)
# temp = np.append(temp, temp3)
# temp = np.append(temp, temp4)
# array1_to_param(temp, 'bn_weight_9_2')

# array2_to_param(model.state_dict()["fc.weight"].cpu().numpy(), 'fc_weight')
# array1_to_param(model.state_dict()["fc.bias"].cpu().numpy(), 'fc_bias')

# test_dataset = torchvision.datasets.CIFAR10(root='../../data/',
#                                             train=False, 
#                                             transform=transforms.ToTensor())
# test_loader = torch.utils.data.DataLoader(dataset=test_dataset,
#                                           batch_size=1,
#                                           shuffle=False)


# activation = {}
# def get_activation(name):
#     def hook(model, input, output):
#         activation[name] = output.detach()
#     return hook



# # print(outputs.data[0])
# # print(model.state_dict()["fc.weight"].cpu().numpy()[0])


# new_model = nn.Sequential(*list(model.children())[:6])
# # new_model.add_module(*list(model.children())[4][0])
# new_model[5][3].bn2.register_forward_hook(get_activation('bn2'))
# new_model.eval()
# # print(new_model)

# print(new_model[5][3])
# new_model(array)
# print(activation['bn2'][0][0][0])

# # print(new_model[4][1].bn2.running_var)
# # # print(model.state_dict()["layer2.1.bn2.weight"].cpu().numpy())


# # print(list(model.children())[:4])
# # print(list(model.children())[4][0])

# # print(new_model(array)[0][0][0])