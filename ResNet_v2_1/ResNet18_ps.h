#include "ResNet18.h"
#include <stdlib.h>
#include <sys/stat.h>

float* load_data(const char* filename);

void ResNet18_network(float* in, float* out);
