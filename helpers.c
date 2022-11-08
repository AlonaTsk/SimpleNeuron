#include "helpers.h"

double dot(double *v, double *u, uint32_t n){
	
	double result = 0.0;
	for(uint32_t i = 0; i < n; i++){
		result += v[i] * u[i];
	}

	return result;
}
