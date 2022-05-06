#pragma once
#include "params.h"
#include "bn.h"
#include "conv.h"
#include "full_connected.h"
#include "global_average_pooling.h"
#include "relu.h"
#include "residual.h"
#include "parameters.h"
#include <stdio.h>

//void resnet18_forward(data_t batch_X[N][C][H_0][W_0],
//                      int batch_y[N],
//                      float* accuracy);

void resnet18_forward(data_t batch_X[N][C][H_0][W_0],
                      int batch_y[N],
					  data_t (*out)[N][CLASS]);
