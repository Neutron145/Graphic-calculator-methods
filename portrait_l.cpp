/* 
 * 	Данный файл используется для построения портретов для линейных систем 
 */
#include <iostream>
#include <cmath>
//Библиотека для комлпексных чисел
#include <complex>
//Библиотека для графики, чтобы получать изображения потретов
#include <SFML/Graphics.hpp>
#include "quadtree.h"

//Структура вектора комплексных чисел
struct Vector2c {
	std::complex<double> x;
	std::complex<double> y;
	Vector2c() {
		x = 0;
		y = 0;
	}
};

using namespace std;
//Пространство имен SFML
using namespace sf;

//Для генерации изображения. Размер картинки
#define X 1601
#define Y 1601
//Объект изображения
sf::Image image;

//Размер шага при построении фазовых кривых 
#define STEP 1

//Цвет, которым красим фазовые кривые. Зависит от направления кривых
Color color;

//Первая функция системы
double f(double x, double y, double c) { return 0*x+2*y + c - 2; }
//Вторая функция системы
double g(double x, double y, double c) { return +4*x +0*y + c - 2; }

//Матрицы системы
double A[2][2]{{3, 2}, {-5, 5}};

//Решение системы методом Гаусса
Vector2f gauss(double A[2][2]) {
	//Особая точка портрета
	Vector2f point;
	double A_[2][2]{{A[0][0], A[0][1]}, {A[1][0], A[1][1]}};

	//Нормируем относительно первого столбца
	for(int i = 0; i < 2; i++) {
		double a = A_[i][0];
		for(int j = 0; j < 2; j++) A_[i][j] /= a;
	}
	for(int i = 0; i < 2; i++) {
		A_[1][i] -= A_[0][i];
	}
	//Вычисление особой точки
	point.y = 0 / A_[1][1]; 		
	point.x = 0 - A_[0][1]*point.y;
	return point;
}

//Вычисление длины вектора
double dist(Vector2f p) {
	return sqrt(pow(tr(p.x), 2) + pow(tr(p.y), 2));
}

//Функция поиска собственных значений матрицы 
Vector2c eigen(double A[2][2]) {
	//2 собственных значения
	Vector2c lambdas;
	//Поиск через квадратное уравнение
	double discriminant = pow(A[0][0] + A[1][1], 2) - 4 * (A[0][0]* A[1][1] - A[0][1] * A[1][0]);
	complex<double> sqrtD = sqrt(complex<double>(discriminant, 0));
	lambdas.x = ((A[0][0] + A[1][1]) + sqrtD)/2.;
	lambdas.y = ((A[0][0] + A[1][1]) - sqrtD)/2.;
	return lambdas;
}

//Функция построения фазовой кривой, проходящей через заданную начальную точку
void phase_curve(Vector2f p, int direction) {
	//Значение функций системы в конкретной точке
	double f_  = 0, g_ = 0;
	//Строим кривую, пока не выйдем за границы изображения(экрана)
	bool l_on_screen = true;
	do {
		//Вычисляем значения в зависимости от шага и направления
		double f_ = STEP * f(tr(p.x), tr(p.y), 0) * direction;
		double g_ = STEP * g(tr(p.x), tr(p.y), 0) * direction;
		//Если изменение слишком малое, то останавливаем построение(Достигли особой точки)
		if(abs(f_) < 1e-5 || abs(g_) < 1e-5) break;
		//Строим прямую
		l_on_screen = line(Vector2f(p.x, p.y), Vector2f((p.x+f_), (p.y + g_)), color, image);
		//Делаем сдвиг
		p.x = p.x + f_;
		p.y = p.y + g_;
	} while(l_on_screen); 
}

//Функция выбора начальных точек на границе прямоугольника для фазовых кривых
//w, h - высота и ширина прямоугольника, center - его центр, step - шаг между начальными точками, direction - направление фазовой кривой
void phase_rect(double w, double h, Vector2f center, double step, int direction) {
	//cout << "draw...\n";
	for(double x = center.x - h/2; x <= center.x + h/2; x += step) {
		phase_curve(Vector2f(rtr(x), rtr(center.y + w/2)), direction);
		phase_curve(Vector2f(rtr(x), rtr(center.y - w/2)), direction);
	}
	for(double y = center.y - w/2; y <= center.y + w/2; y+=step) {
		phase_curve(Vector2f(rtr(center.x + h/2), rtr(y)), direction);
		phase_curve(Vector2f(rtr(center.x - h/2), rtr(y)), direction);
	}
}

//Функция построения фазового портрета
void phase_portrait() {
	//Сначала определим особую точку и ее тип
	Vector2c lambdas = eigen(A);
	cout << "Лямбды: " << lambdas.x << ", " << lambdas.y << '\n';

	//В зависимости от типа особой точки, выбираем начальные точки для рисования фазовых кривых
	//Желтый цвет - к точке(устойчивая точка), Зеленый цвет - от точки(неустойчивая точка)
	if(lambdas.x.imag() != 0) {
		//Центр
		if(lambdas.x.real() == 0) { 
			color = Color::Yellow; 
			phase_rect(19, 19, Vector2f(0, 0), 1, 1);
			phase_rect(2, 2, Vector2f(0, 0), 1, 1);
		}
		//Неустойчивый фокус
		else if(lambdas.x.real() > 0) {
			color = Color::Green;
			phase_rect(19, 19, Vector2f(0, 0), 1, -1);
		}
		//Устойчивый фокус
		else if(lambdas.x.real() < 0) {
			color = Color::Yellow;
			phase_rect(19, 19, Vector2f(0, 0), 1, 1);
		}
	}
	else {
		if(lambdas.x.real() != lambdas.y.real()) {
			//Неустойчивый узел
			if(lambdas.x.real() > 0 && lambdas.y.real() > 0) {
				color = Color::Green;
				phase_rect(19, 19, Vector2f(0, 0), 1, -1);
			}
			//Устойчивый узел
			else if(lambdas.x.real() < 0 && lambdas.y.real() < 0) {
				color = Color::Yellow;
				phase_rect(19, 19, Vector2f(0, 0), 1, 1);
			}
			//Седло
			else if(lambdas.x.real() * lambdas.y.real() < 0) {
				color = Color::Yellow;
				phase_rect(19, 19, Vector2f(0, 0), 1, 1);
			}
		}
		else {
			if(lambdas.x.real() > 0) { 
				color = Color::Green;
				//Неустойчивый дикритический узел
				if(A[0][1] == 0 && A[1][1] == 0) {
					phase_rect(19, 19, Vector2f(0, 0), 1, -1);
				}
				//Неустойчивый вырожденный узел
				else { 
					phase_rect(19, 19, Vector2f(0, 0), 1, -1);
				}
			}
			else {
				color = Color::Yellow;
				//Устойчивый дикритический узел
				if(A[0][1] == 0 && A[1][1] == 0) {
					phase_rect(19, 19, Vector2f(0, 0), 1, 1);
				}
				//Устойчивый вырожденный узел
				else {
					phase_rect(19, 19, Vector2f(0, 0), 1, 1);
				}
			}
		}
	}
}

int main() {
	setSize(X, Y);
	image.create(X, Y, Color::Black);

	//Поиск особой точки
	Vector2f singular = gauss(A);
	cout << "Особая точка: (" << singular.x << ", " << singular.y << ")\n";

	//Построение фазового портрета
	phase_portrait();

	//Сохранение изображения
	image.flipVertically();
	image.saveToFile("portrait.png");
	return 0;
}
