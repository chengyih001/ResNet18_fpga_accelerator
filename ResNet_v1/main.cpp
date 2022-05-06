#include <stdio.h>
#include "model.h"

int main() {

	data_t batch_X[N][C][H_0][W_0] = {
		#include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\testing_images.h"
	};

	int batch_y[N] = {
		#include "C:\Users\Joey\Google_Drive\1_FYT\ResNet_cpp\parameters\validate_images.h"
	};

	float accuracy = -1.0;

    resnet18_forward(batch_X, batch_y, &accuracy);

    printf("accuracy = %f\n", float(accuracy));

}
