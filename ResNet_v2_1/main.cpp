#include <stdio.h>
#include "ResNet18_ps.h"

int main() {
    float* y = load_data("parameters/validate_images.bin");
    float* in = load_data("parameters/testing_images.bin");
    float* out = (float *)calloc(N*CLASS, sizeof(float));
    ResNet18_network(in, out);

    float score[N][CLASS];
    data_scatter_2d(out, &score[0][0], N, CLASS);

    // int n, c;
    // for (n=0; n < N; n++) {
    //     for (c=0; c < CLASS; c++) {
    //         printf("Score of class %d: %f\n", c+1, float(out[n*CLASS+c]));
    //     }
    // }

    int y_pred[N];
    float max_score = 0.0;
    for (int i=0; i < N; i++) {
        max_score = score[i][0];
        y_pred[i] = 0;
        for (int j=1; j < CLASS; j++) {
            if (score[i][j] > max_score) {
                max_score = score[i][j];
                y_pred[i] = j;
            }
        }
    }

    float sum = 0.0;
    float total = 0.0;
    for (int i=0; i < N; i++) {
        total += 1.0;
        // printf("%d %d \n", y_pred[i], int(y[i]));
        if (y_pred[i] == int(y[i])) {
            sum += 1.0;
        }
    }

    printf("accuracy = %f", float(sum/total));

    free(y);
    free(in);
    free(out);
}