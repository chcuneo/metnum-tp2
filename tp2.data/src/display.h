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
			printf("%1.0f ", mat(i, j));
		}
		cout << "|" << endl;
	}
	cout << endl;
}

void showMatFile(Matrix& mat, char* file) {

}