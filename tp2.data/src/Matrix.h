#pragma once
#include <vector>
#include <assert.h>  

using namespace std;
class Matrix {
public:
	Matrix(int n, int m);
	Matrix(const Matrix &s);
	~Matrix();
	Matrix& operator=(const Matrix& B);


	//Entre Matrices
	Matrix operator+(const Matrix& B);
	Matrix& operator+=(const Matrix& B);
	Matrix operator-(const Matrix& B);
	Matrix& operator-=(const Matrix& B);
	Matrix operator*(const Matrix& B);
	Matrix& operator*=(const Matrix& B);
	void transpuesta();

	//Escalares
	Matrix operator+(const double& b);
	Matrix operator-(const double& b);
	Matrix operator*(const double& b);
	Matrix& operator*=(const double& b);
	Matrix operator/(const double& b);

	//Con Vector
	vector<double> operator*(const vector<double>& b);
	std::vector<double> diag_vec();

	double& operator()(const int& n, const int& m);
	const double& operator()(const int& n, const int& m) const;
	vector<double>& operator()(const int& n);
	const vector<double>& operator()(const int& n) const;

	int getn() const;
	int getm() const;

private:
	int _n;
	int _m;
	bool _transp;
	vector< vector<double> > _mat;
};

