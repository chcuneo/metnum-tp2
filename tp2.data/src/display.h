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

void saveMatFile(Matrix& mat, const char* file) {
	ofstream output;
	output.open(file, ofstream::out);
	int n = mat.getn();
	int m = mat.getm();
	output << n << " " << m << " " << scientific;
	output.precision(16);
	for (int x = 0; x < n; x++) {
		for (int y = 0; y < m; y++) {
			output << mat(x, y) << " ";
		}
	}
	output.flush();
	output.close();
}

Matrix loadMatFile(const char* file) {
	ifstream input;
	input.open(file, ifstream::in);
	int n;
	int m;
	input >> n >> m;
	Matrix mat(n, m);
	for (int x = 0; x < n; x++) {
		for (int y = 0; y < m; y++) {
			input >> mat(x, y);
		}
	}
	input.close();
	return mat;
}

void printNewLine(string out) {
	cout << out << endl;
	cout.flush();
}

void printUpdateLine(string out) {
	cout << "\r" << out;
	cout.flush();
}