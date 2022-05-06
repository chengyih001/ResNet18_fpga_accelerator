#include <stdio.h>
#include "model.h"

int main() {

	data_t batch_X[N][C][H_0][W_0] = {
		#include "parameters\testing_images.h"
	};

	int batch_y[N] = {
		#include "parameters\validate_images.h"
	};

//	float accuracy = -1.0;

//	data_t* score[N];
//	for (int i = 0; i < N; i++) {
//		score[i] = new data_t[CLASS];
//	}
	data_t score[N][CLASS];


//    resnet18_forward(batch_X, batch_y, &accuracy);
//    printf("accuracy = %f\n", float(accuracy));


	resnet18_forward(batch_X, batch_y, &score);
	for (int n=0; n < N; n++) {
		for (int c=0; c < CLASS; c++) {
			printf("Class %d score: %f\n", c+1, float(score[n][c]));
		}
	}


}
