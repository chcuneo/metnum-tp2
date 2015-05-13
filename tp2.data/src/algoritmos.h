#include <stdlib.h>
#include <stdio.h>
#include <time.h> //libreria para tomar numeros del timer de la maquina
#include <vector>
#include "Matrix.h"
#include <math.h>
#include <stdint.h>
#include <queue> 
#include <functional>     // std::greater


using namespace std;

//PRE: unknown y train tienen mismo m
//Ver que unknown sea matriz 1 x m (fila)!!!!
//Como todavia nose como lo voy a necesitar llamar, nose como meter el unknown, pero el algoritmo es este.
uint8_t kNN(Matrix& train, vector<uint8_t>& trainlabels, vector<double> unknown, int k){
	assert(train.getm() == unknown.size());
	priority_queue< pair<double, uint8_t>, vector< pair<double, uint8_t> >, greater< pair<double, uint8_t> > > queue;
	int n = train.getn();
	for (int testsN = 0; testsN < n; testsN++) {
	
		double sum = 0;
		for (int y = 0; y < train.getm(); y++) {
			sum += (train(testsN, y) - unknown[y])*(train(testsN, y) - unknown[y]);
		}
		double dist = sqrt(sum);
	
		queue.push(pair<double, uint8_t>(dist, trainlabels[testsN]));
	}
	//Hago votacion de los k mas cercanos
	int cantidad[10] = { };
	for (int kn = 0; kn < k; kn++) {

		cantidad[queue.top().second]++;
		queue.pop();
	}

	//Y elijo el grupo con mas votaciones
	int max = cantidad[0];
	uint8_t maxi = 0;
	for (int m = 0; m < 10; m++) {
		if (cantidad[m] > max) {
			max = cantidad[m];
			maxi = m;
		}
	}
	return maxi;
}

//Devuelve un vector con la media de cada variable de la matriz recivida
vector<double> meanVector(Matrix& mat) {
	int n = mat.getn();
	int m = mat.getm();
	vector<double> mean(mat.getm(), 0);
	for (int mx = 0; mx < m; mx++) {
		for (int nx = 0; nx < n; nx++) {
			mean[mx] += mat(nx, mx);

		}
		mean[mx] /= n;
	}
	return mean;
}

//Tomando una matrix nxm Devuelve una matriz nxn resultado de multiplicar la matrix recivida por su transpuesta
Matrix multiplyTransp(const Matrix& mat) {
	int n = mat.getn();
	int m = mat.getm();
	Matrix res(m, m);
	for (int rx = 0; rx < m; rx++) {
		printf("%i ", rx);
		cout.flush();
		for (int ry = 0; ry < m; ry++) {
			for (int nxy = 0; nxy < n; nxy++) {
				res(rx, ry) += mat(nxy, ry) * mat(nxy, rx);
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
	cout << "Calculando vector de medias..." << endl;
	vector<double> mean(meanVector(mat));
	//Creo la matriz x con fila x(i) = (x(i) - u) / sqrt(n - 1)
	cout << "Calculo matriz X..." << endl;
	for (int x = 0; x < n; x++) {
		for (int y = 0; y < m; y++) {
			temp(x, y) = (mat(x, y) - mean[y]) / sqrt(n - 1);
		}
	}
	cout << "Calculo multiplicacion por transpuesta..." << endl;
	Matrix covMat(multiplyTransp(temp));
	cout << "Listo matriz de covarianza..." << endl;
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

//Devuelve la multiplicacion de dos vectores, a colimna y v fila
Matrix vectorMulMat(const vector<double>& a, const vector<double>& v) {
	int n = v.size();
	Matrix res(n, n);
	for (int x = 0; x < n; x++) {
		for (int y = 0; y < n; y++) {
			res(x,y) = a[x] * v[y];
		}
	}
	return res;
}

//Devuelve el autovector de A con autovalor asociado mas grande
//double PowerIteration(vector<double>& v, Matrix& A, int maxIters) {
//	//double tolerance = 1e-6;
//	int iteration = 0; 
//	double autoval = 0.0;
//	double lastnorm = normaVectorial(v);
//	for (int iteration = 0; iteration <= maxIters; iteration++) {
//		v = A*v;
//		double len = normaVectorial(v);
//		vectorScalarDiv(v, len);
//		autoval = len / lastnorm;
//		lastnorm = len;
//		/*if ((abs((lambda - lambdaOld) / lambda)) < tolerance) {
//		//	break;
//		//}
//		lambdaOld = lambda;*/
//	}
//	return autoval;
//}
double PowerIteration(vector<double>& v, Matrix& A, int maxIters) {
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
	vector<double> temp(A*v);
	double res = vectorMul(temp, v) / vectorMul(v, v);
	return res;
}

void Deflation(vector<double>& v, Matrix& A, double autoval) {
	int m = A.getm();
	for (int ii = 0; ii < m; ii++) {
		for (int jj = 0; jj < m; jj++) {
			A(ii, jj) -= autoval * v[ii] * v[jj];
		}
	}
}