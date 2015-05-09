#include <stdlib.h>
#include <stdio.h>
#include <time.h> //libreria para tomar numeros del timer de la maquina
#include <iostream>
#include <fstream>
#include <vector>
#include "Matrix.h"
#include <cstdint>
#include <queue> 


using namespace std;

class ComparisonClass {
	bool operator() (pair<uint8_t, unsigned int> first, pair<uint8_t, unsigned int> second) {
		return first.second < second.second;
	}
};

uint8_t kNN(Matrix& train, vector<uint8_t>& trainlabels, Matrix& unknown, unsigned int k){
	priority_queue< pair<uint8_t, unsigned int> , vector< pair<uint8_t, unsigned int> >, ComparisonClass > queue;
}