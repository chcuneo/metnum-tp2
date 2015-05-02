#include "Matrix.h"


Matrix::Matrix(int n, int m) {
	_mat.resize(n);
	for (int i = 0; i < (int)_mat.size(); i++) {
		_mat[i].resize(m, 0);
	}
	_n = n;
	_m = m;
	_transp = false;
}

Matrix::Matrix(const Matrix &c) {
	_mat = c._mat;
	_n = c.getn();
	_m = c.getm();
	_transp = c._transp;

}

Matrix::~Matrix() {}

Matrix& Matrix::operator=(const Matrix &s) {
	assert(_m == s._m && _n == s._n);

	if (&s == this)
		return *this;

	int new_n = s._n;
	int new_m = s._m;

	_mat.resize(new_n);
	for (int i = 0; i < (int)_mat.size(); i++) {
		_mat[i].resize(new_m);
	}

	for (int i = 0; i < new_n; i++) {
		for (int j = 0; j < new_m; j++) {
			_mat[i][j] = s(i, j);
		}
	}
	_n = new_n;
	_m = new_m;

	return *this;
}
Matrix Matrix::operator+(const Matrix &s) {
	assert(_m == s._m && _n == s._n);

	Matrix C(_n, _n);

	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < _m; j++) {
			C(i, j) = _mat[i][j] + s(i, j);
		}
	}

	return C;
}
Matrix& Matrix::operator+=(const Matrix& s) {
	assert(_m == s._m && _n == s._n);

	int n = s.getn();
	int m = s.getm();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			_mat[i][j] += s(i, j);
		}
	}
	return *this;
}
Matrix Matrix::operator-(const Matrix &s) {
	assert(_m == s._m && _n == s._n);

	Matrix C(_n, _n);
	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < _m; j++) {
			C(i, j) = _mat[i][j] - s(i, j);
		}
	}
	return C;
}
Matrix& Matrix::operator-=(const Matrix& s) {
	assert(_m == s._m && _n == s._n);

	int n = s.getn();
	int m = s.getm();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			_mat[i][j] -= s(i, j);
		}
	}
	return *this;
}
Matrix Matrix::operator*(const Matrix& B) {

	int nB = B.getn();
	int mB = B.getm();

	assert(_m == nB);

	Matrix C(_n, mB);

	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < mB; j++) {
			for (int k = 0; k < _m; k++) {
				C(i, j) += _mat[i][k] * B(k, j);
			}
		}
	}

	return C;
}
Matrix& Matrix::operator*=(const Matrix& B) {
	Matrix C = (*this) * B;
	(*this) = C;
	return *this;
}

Matrix Matrix::operator+(const double& b) {
	Matrix C(_n, _n);

	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < _m; j++) {
			C(i, j) = _mat[i][j] + b;
		}
	}

	return C;
}
Matrix Matrix::operator-(const double& b) {
	Matrix C(_n, _n);

	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < _m; j++) {
			C(i, j) = _mat[i][j] - b;
		}
	}

	return C;
}
Matrix Matrix::operator*(const double& b) {
	Matrix C(_n, _n);

	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < _m; j++) {
			C(i, j) = _mat[i][j] * b;
		}
	}

	return C;
}
Matrix Matrix::operator/(const double& b) {
	Matrix C(_n, _n);

	for (int i = 0; i < _n; i++) {
		for (int j = 0; j < _m; j++) {
			C(i, j) = _mat[i][j] / b;
		}
	}

	return C;
}

vector<double> Matrix::diag_vec() {
	std::vector<double> result(_n);

	for (int i = 0; i < _n; i++) {
		result[i] = _mat[i][i];
	}

	return result;
}
void Matrix::transpuesta() {
	_transp = !_transp;
}


double& Matrix::operator()(const int& x, const int& y) {
	return !_transp ? _mat[x][y] : _mat[y][x];
}
const double& Matrix::operator()(const int& x, const int& y)const {
	return !_transp ? _mat[x][y] : _mat[y][x];
}

int Matrix::getn() const {
	return !_transp ? _n : _m;
}
int Matrix::getm() const {
	return !_transp ? _m : _n;
}