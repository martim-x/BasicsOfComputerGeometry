#include "pch.h"
#include "CMatrix.h"

CMatrix::CMatrix()
{
	n_rows = 1;
	n_cols = 1;
	array = new double* [1];
	array[0] = new double[1];
	array[0][0] = 0.0;
}

CMatrix::CMatrix(int r, int c)
{
	n_rows = r;
	n_cols = c;
	array = new double* [n_rows];
	for (int i = 0; i < n_rows; i++)
	{
		array[i] = new double[n_cols];
		for (int j = 0; j < n_cols; j++)
			array[i][j] = 0.0;
	}
}

CMatrix::CMatrix(int n)
{
	n_rows = n;
	n_cols = 1;
	array = new double* [n_rows];
	for (int i = 0; i < n_rows; i++)
	{
		array[i] = new double[1];
		array[i][0] = 0.0;
	}
}

CMatrix::CMatrix(const CMatrix& M)
{
	n_rows = M.n_rows;
	n_cols = M.n_cols;
	array = new double* [n_rows];
	for (int i = 0; i < n_rows; i++)
	{
		array[i] = new double[n_cols];
		for (int j = 0; j < n_cols; j++)
			array[i][j] = M.array[i][j];
	}
}

CMatrix::~CMatrix()
{
	for (int i = 0; i < n_rows; i++)
		delete[] array[i];
	delete[] array;
}

double& CMatrix::operator()(int i, int j)
{
	return array[i][j];
}

double& CMatrix::operator()(int i)
{
	return array[i][0];
}

CMatrix CMatrix::operator=(const CMatrix& M)
{
	if (this == &M) return *this;

	for (int i = 0; i < n_rows; i++)
		delete[] array[i];
	delete[] array;

	n_rows = M.n_rows;
	n_cols = M.n_cols;
	array = new double* [n_rows];
	for (int i = 0; i < n_rows; i++)
	{
		array[i] = new double[n_cols];
		for (int j = 0; j < n_cols; j++)
			array[i][j] = M.array[i][j];
	}
	return *this;
}

CMatrix CMatrix::operator*(CMatrix& M)
{
	CMatrix Res(n_rows, M.n_cols);
	for (int i = 0; i < n_rows; i++)
		for (int j = 0; j < M.n_cols; j++)
		{
			Res(i, j) = 0.0;
			for (int k = 0; k < n_cols; k++)
				Res(i, j) += array[i][k] * M(k, j);
		}
	return Res;
}

CMatrix CMatrix::operator+(CMatrix& M)
{
	CMatrix Res(n_rows, n_cols);
	for (int i = 0; i < n_rows; i++)
		for (int j = 0; j < n_cols; j++)
			Res(i, j) = array[i][j] + M(i, j);
	return Res;
}

CMatrix CMatrix::operator-(CMatrix& M)
{
	CMatrix Res(n_rows, n_cols);
	for (int i = 0; i < n_rows; i++)
		for (int j = 0; j < n_cols; j++)
			Res(i, j) = array[i][j] - M(i, j);
	return Res;
}

CMatrix CMatrix::RedimMatrix(int r, int c)
{
	for (int i = 0; i < n_rows; i++)
		delete[] array[i];
	delete[] array;

	n_rows = r;
	n_cols = c;
	array = new double* [n_rows];
	for (int i = 0; i < n_rows; i++)
	{
		array[i] = new double[n_cols];
		for (int j = 0; j < n_cols; j++)
			array[i][j] = 0.0;
	}
	return *this;
}

CMatrix CMatrix::RedimMatrix(int n)
{
	return RedimMatrix(n, 1);
}

double CMatrix::MaxElement()
{
	double maxVal = array[0][0];
	for (int i = 0; i < n_rows; i++)
		for (int j = 0; j < n_cols; j++)
			if (array[i][j] > maxVal)
				maxVal = array[i][j];
	return maxVal;
}

double CMatrix::MinElement()
{
	double minVal = array[0][0];
	for (int i = 0; i < n_rows; i++)
		for (int j = 0; j < n_cols; j++)
			if (array[i][j] < minVal)
				minVal = array[i][j];
	return minVal;
}