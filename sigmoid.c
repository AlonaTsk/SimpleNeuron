#include <stdio.h>
#include <math.h>

#include "sigmoid.h"

double sigmoid(double x){
	
	double result;
	
	result = 1/(1+exp(-x));
	
	return result;
}

double sigmoid_derivative(double x){
	double result;

	/* result = sigmoid(x) * (1 - sigmoid(x)); */
	result = x * (1 - x);

	return result;
}









