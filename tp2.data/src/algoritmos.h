#include <stdlib.h>
#include <stdio.h>
#include <time.h> //libreria para tomar numeros del timer de la maquina
#include <vector>
#include "Matrix.h"
#include <math.h>
#include <cstdint>
#include <queue> 


using namespace std;

class ComparisonClass {
	bool operator() (pair<uint8_t, double> first, pair<uint8_t, double> second) {
		return first.second < second.second;
	}
};

//PRE: unknown y train tienen mismo m
//Ver que unknown sea matriz 1 x m (fila)!!!!
uint8_t kNN(Matrix& train, vector<uint8_t>& trainlabels, Matrix& unknown, unsigned int k){
	priority_queue< pair<uint8_t, double>, vector< pair<uint8_t, double> >, ComparisonClass > queue;
	for (int testsN = 0; testsN < train.getn(); testsN++) {
		long int sum = 0;
		for (int y = 0; y < train.getm(); y++) {
			sum += (train(testsN, y) - unknown(1, y))*(train(testsN, y) - unknown(1, y));
		}
		double dist = sqrt(sum);
		queue.push(pair<uint8_t, double>(trainlabels[testsN], dist));
	}
	int cantidad[10] = { };
	for (int kn = 0; kn < k; kn++) {
		cantidad[queue.top().first]++;
	}
	uint8_t max = 0;
	for (int m = 0; m < 10; m++) {
		if (cantidad[m] > max) max = cantidad[m];
	}
	return max;
}

