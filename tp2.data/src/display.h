#include "Matrix.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

void showMatCons(const Matrix& mat) {
	int m = mat.getm();
	int n = mat.getn();
	cout << endl;
	for (int i = 0; i < n; i++) {
		cout << "| ";
		for (int j = 0; j< m; j++) {
			printf("%1.5f ", mat(i, j));
		}
		cout << "|" << endl;
	}
	cout << endl;
}

void showVecCons(const vector<double>& vec) {
	int n = vec.size();
	cout << endl;
	cout << "| ";
	for (int j = 0; j < n; j++) {
		printf("%1.2f ", vec[j]);
	}
	cout << "|" << endl;
	cout << endl;
}

void showMatFile(Matrix& mat, char* file) {

}