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
//Como todavia nose como lo voy a necesitar llamar, nose como meter el unknown, pero el algoritmo es este.
uint8_t kNN(Matrix& train, vector<uint8_t>& trainlabels, Matrix& unknown, unsigned int k){
	assert(train.getm == unknown.getm);
	priority_queue< pair<uint8_t, double>, vector< pair<uint8_t, double> >, ComparisonClass > queue;
	
	//Para cada imagen de train...
	for (int testsN = 0; testsN < train.getn(); testsN++) {
		//Calculo la distancia con el unknown
		long int sum = 0;
		for (int y = 0; y < train.getm(); y++) {
			sum += (train(testsN, y) - unknown(1, y))*(train(testsN, y) - unknown(1, y));
		}
		double dist = sqrt(sum);
		//Lo agrego a la cola de prioridad (cola con el menor siempre en la cabeza)
		queue.push(pair<uint8_t, double>(trainlabels[testsN], dist));
	}
	
	//Hago votacion de los k mas cercanos
	int cantidad[10] = { };
	for (int kn = 0; kn < k; kn++) {
		cantidad[queue.top().first]++;
		queue.pop();
	}

	//Y elijo el grupo con mas votaciones
	uint8_t max = 0;
	for (int m = 0; m < 10; m++) {
		if (cantidad[m] > max) max = cantidad[m];
	}
	return max;
}

vector<double> meanVector(Matrix& mat) {
	int n = mat.getn();
	int m = mat.getm();
	vector<double> mean(mat.getm, 0);
	for (int mx = 0; mx < m; mx++) {
		for (int nx = 0; nx < n; nx++) {
			mean[mx] += mat(nx, mx);

		}
		mean[mx] /= n;
	}
}

//Tomando una matrix nxm Devuelve una matriz nxn resultado de multiplicar la matrix recivida por su transpuesta
Matrix multiplyTransp(const Matrix& mat) {
	int n = mat.getn();
	int m = mat.getm();
	Matrix res(n, n);
	for (int rx = 0; rx < n; rx++) {
		for (int ry = 0; ry < n; ry++) {
			for (int mxy = 0; mxy < m; mxy++) {
				res(rx, ry) += mat(rx, mxy) * mat(ry, mxy);
			}
		}
	}
	return res;
}

//Tomando una matrix nxm (una matriz con n muestras de m variables) devuelve la matriz de covarianzas.
Matrix covarianceMatrix(Matrix& mat) {
	int n = mat.getn();
	int m = mat.getm();
	Matrix temp(n, m);
	vector<double> mean(meanVector(mat));
	//Creo la matriz x con fila x(i) = (x(i) - u) / sqrt(n - 1)
	for (int x = 0; x < n; x++) {
		for (int y = 0; y < n; y++) {
			temp(x, y) = (mat(x, y) - mean[x]) / sqrt(n - 1);
		}
	}
	Matrix covMat(multiplyTransp(temp));
	return covMat;
}

//Devuelve la norma vectorial ||V||
double normaVectorial(vector<double>& v) {
	double res = 0;
	int n = v.size();
	for (int x = 0; x < n; x++) {
		res += v[x] * v[x];
	}
	res = sqrt(res);
	return res;
}

//Divide escalarmente al vector "v" por k.
void vectorScalarDiv(vector<double>& v, double k) {
	int n = v.size();
	for (int x = 0; x < n; x++) {
		v[x] /= k;
	}
}

//Devuelve la multiplicacion de dos vectores, "a" vector fila y "v" vector columna.
double vectorMul(const vector<double>& a, const vector<double>& v) {
	int n = v.size();
	double res = 0;
	assert(n == a.size());
	for (int x = 0; x < n; x++) {
		res += a[x] * v[x];
	}
	return res;
}

//Devuelve el autovector de A con autovalor asociado mas grande
vector<double> PowerMethod(vector<double>& v, Matrix& A, int maxIters) {
	//double tolerance = 1e-6;
	int iteration = 0; 
	double lambdaOld = 0;
	for (int iteration = 0; iteration <= maxIters; iteration++) {
		v = A*v;
		vectorScalarDiv(v, normaVectorial(v));				//Aca ya tengo en v el siguiente candidato a autovector
		/*if ((abs((lambda - lambdaOld) / lambda)) < tolerance) {
		//	break;
		//}
		lambdaOld = lambda;*/
	}
	return v;
}