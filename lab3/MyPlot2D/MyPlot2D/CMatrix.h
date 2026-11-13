#pragma once
#include <iostream>
#include <fstream>
#include <cmath>

class CMatrix
{
	double** array;
	int n_rows; // Число строк
	int n_cols; // Число столбцов

public:
	CMatrix();                                // Конструктор по умолчанию (1x1)
	CMatrix(int r, int c);                    // Конструктор матрицы r x c
	CMatrix(int n);                           // Вектор-столбец (n элементов)
	CMatrix(const CMatrix&);                  // Конструктор копирования
	~CMatrix();

	double& operator()(int i, int j);         // Доступ к элементу матрицы (i, j)
	double& operator()(int i);                // Доступ к элементу вектора (i)
	CMatrix operator=(const CMatrix&);        // Оператор присваивания M1 = M2
	CMatrix operator*(CMatrix&);              // Умножение матриц: M1 * M2
	CMatrix operator+(CMatrix&);              // Сложение: M1 + M2
	CMatrix operator-(CMatrix&);              // Вычитание: M1 - M2

	int rows() const { return n_rows; }       // Количество строк
	int cols() const { return n_cols; }       // Количество столбцов

	CMatrix RedimMatrix(int r, int c);        // Изменение размера матрицы
	CMatrix RedimMatrix(int n);               // Изменение размера вектора

	double MaxElement();                      // Максимальный элемент
	double MinElement();                      // Минимальный элемент
};