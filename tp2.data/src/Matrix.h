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
	Matrix operator/(const double& b);

	//Con Vector
	vector<double> operator*(const vector<double>& b);
	std::vector<double> diag_vec();

	float& operator()(const int& n, const int& m);
	const float& operator()(const int& n, const int& m) const;

	int getn() const;
	int getm() const;

private:
	int _n;
	int _m;
	bool _transp;
	vector< vector<float> > _mat;
};

