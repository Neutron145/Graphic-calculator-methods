/*
 *	Данный файл используется для построения функции
 */
#include <cmath>
#include <iostream>
#include <chrono>
//Библиотека для графики
#include <SFML/Graphics.hpp>
//Содержит функции для отрисовки изображений
#include "quadtree.h"

//Размер изображения 
#define X 1601
#define Y 1601

using namespace std;

//Сама функция, которую мы хотим отрисовать
double f(double x, double y, double C) { return y - x * x + C; }

//Свободный член функции
double C = 0;

int main() {
	//Устанавливаем размер изображения, на котором надо отрисовать функцию
	setSize(X, Y);
	sf::Image image;
	image.create(X, Y, sf::Color::Black);
	auto start = chrono::steady_clock::now();
	//Рисуем график
	plot(image, f, C);
	auto end = chrono::steady_clock::now();
	cout << "Elapsed time in milliseconds " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << '\n';
	//Сохраняем изображение 
	image.flipVertically();
	image.saveToFile("quadtree.png");		
	return 0;
}
