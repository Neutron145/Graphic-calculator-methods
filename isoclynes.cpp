/*
 *	Данный файл используется для решения задачи Коши, построения изоклин и построения поля изоклин
 */
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <chrono>
//Алгоритмы для построения неявных функций
#include "quadtree.h"

//Размер изображения по Х и Y
#define X 1601
#define Y 1601

using namespace std;
using namespace sf;

//Изображение
sf::Image image;

//Постоянная для отрисовки изоклин с разными линиями уровня
double C = -4;

//Уравнение y` = 2x(1-y)
double f(double x, double y, double c) { return y - x * x + c;  }

//Проверка, не вышли ли мы за границы изображения 
bool in_screen(sf::Vector2f point) {
	if(point.x < 1 || point.x > X - 1) 
		return false;
	else if(point.y < 1 || point.y > Y - 1) 
		return false;
	return true;
}

//Отрисовываем поле изоклин
void tangent_field() {
	//Кол-во векторов по оси Х
	int xc = 20;
	//Кол-во векторов по оси Y
	int yc = 20;
	//Делаем определенный шаг по х и у, вычисляем касательную к решению и строим единичный вектор в направлнии касательной
	for(int x = 1; x < xc; x++) {
		for(int y = 1; y < yc; y++) {
			//Вычисление угла касательной к оХ
			double k = f(tr(x * (X/xc)), tr(y*(Y/yc)), 0);
			double alpha = atan(k);
			//Вычисление координат вектора через прямоугольный треугольник
			double dy = sin(alpha) * 0.5;
			double dx = cos(alpha) * 0.5;
			//Отрисовка вектора 
			line(sf::Vector2f(x * (X/xc), y * (Y/yc)), sf::Vector2f((x + dx) * (X/xc), (y + dy) * (Y/yc) + dy), sf::Color::White, image);	
		}
	}
}

//Рисование нескольких изоклин
//Просто отрисовываем уравнение с разными постоянными
void isoclynes() {
	for(int i = 0; i < 4; i++) {
		C += 2;			
		plot(image, f, C);
	}
}

//Построение интегральньой кривой с заданной задачей Коши
//point - задача Коши
void integral_curve(Vector2f point) {
	//Строим кривую в одном направлении, пока не дойдем до конца экрана 
	Vector2f p(point.x, point.y);
	while(in_screen(p)) {
		//Вычисляем угол наклона
		double k = f(tr(p.x), tr(p.y), 0);	
		double alpha = atan(k);
		//Вычисляем координаты конца вектора через прямоугольный треугольник
		double dy = sin(alpha) * 1.0;
		double dx = cos(alpha) * 1.0;
		line(sf::Vector2f(p.x,p.y), sf::Vector2f((p.x+dx), (p.y+dy)), sf::Color::Red, image);
		p.x = p.x + dx;
		p.y = p.y + dy;
	}
	//Строим кривую в противоположном направлении, пока не дойдем до конца экрана
	p = point;
	while(in_screen(p)) {
		//Вычисляем угол наклона
		double k = f(tr(p.x), tr(p.y), 0);	
		double alpha = atan(k);
		//Вычисляем координаты конца вектора через прямоугольный треугольник
		double dy = sin(alpha) * (-1.0);
		double dx = cos(alpha) * (-1.0);
		line(sf::Vector2f(p.x,p.y), sf::Vector2f((p.x+dx), (p.y+dy)), sf::Color::Red, image);
		p.x = p.x + dx;
		p.y = p.y + dy;
	}

}

int main() {
	setSize(X, Y);
	//Задача Коши для уравения
	Vector2f cauchy(1.4*80 + 800, 5*80 + 800);
	//Генерация черного изображения
	image.create(X, Y, sf::Color::Black);

	//Засекание времени
	auto start = std::chrono::steady_clock::now();	
	//Выполнение всех вычислений
	isoclynes();
	tangent_field();	
	integral_curve(cauchy);
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time elapsed in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << '\n';

	//Сохранение изображения
	image.flipVertically();
	image.saveToFile("isoclynes.png");
	return 0;
}
