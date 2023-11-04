/* 	
 *  	Построение нелинейных портретов
 * 	Основное отличие в том, что сначала нам надо найти особые точки нелинейной системы
 * 	Их может быть несколько. Чтобы корректно построить портрет, нам необходимо брать начальные точки в окрестности особых точек
 */
#include <iostream>
#include <cmath>
#include <vector>
//Библиотека для графики
#include <SFML/Graphics.hpp>
//Всякие функции для рисования изображений 
#include "quadtree.h"
//Для использования функции solve() - поиска особых точек системы
#include "systems.h"

using namespace std;

//Определение шага для построения фазовых портретов
#define STEP 1

//Определение размера изображения
#define X 1601
#define Y 1601

//Объект изображения
sf::Image image;


//Определение нелинейных функций системы и их производных
//-------------------------------------------------------
double f1(double x, double y) { return -0*x + 3*y - x*y; }
double f2(double x, double y) { return -2*x - 3*y + x*x; }

double f1x(double x, double y) { return -y; }
double f1y(double x, double y) { return 3 - x; }

double f2x(double x, double y) { return -2 + 2*x; }
double f2y(double x, double y) { return -3; }
//-------------------------------------------------------


//Длина вектора
double dist(sf::Vector2f p) {
	return sqrt(pow(tr(p.x), 2) + pow(tr(p.y), 2));
}

//Функция построения фазовой кривой. Аналогична функции для линейных систем
void phase_curve(sf::Vector2f p, int direction) {
	//Значение функции в конкретной точке
	double f_  = 0, g_ = 0;
	//Строим кривую, пока не выйдем за границу изображения
	bool l_on_screen = true;
	do {
		double f_ = STEP * f1(tr(p.x), tr(p.y)) * direction;
		double g_ = STEP * f2(tr(p.x), tr(p.y)) * direction;
		if(abs(f_) < 1e-4 || abs(g_) < 1e-4) break;
		l_on_screen = line(sf::Vector2f(p.x, p.y), sf::Vector2f((p.x + f_), (p.y + g_)), sf::Color::Yellow, image);
		p.x = p.x + f_;
		p.y = p.y + g_;
	} while(l_on_screen); 
}

//Функция построения фазовых портретов 
void phase_portrait(vector<sf::Vector2f> points) {
	//Идем по всем особым точкам, берем квадрат размером 1x1 и центром в особой точке
	//На его границе берем начальные точки с шагом и строим в 2 стороны фазовые кривые
	for(int i = 0; i < points.size(); i++) {
		for(double x = points[i].x - 0.5; x < points[i].x + 0.5; x+=0.1) {
			phase_curve(sf::Vector2f(rtr(x), rtr(points[i].y - 0.5)), -1);
			phase_curve(sf::Vector2f(rtr(x), rtr(points[i].y + 0.5)), -1);
			phase_curve(sf::Vector2f(rtr(x), rtr(points[i].y - 0.5)), 1);
			phase_curve(sf::Vector2f(rtr(x), rtr(points[i].y + 0.5)), 1);
		}
		for(double y = points[i].y - 0.5; y < points[i].y + 0.5; y+=0.1) {
			phase_curve(sf::Vector2f(rtr(points[i].x + 0.5), rtr(y)), -1);
			phase_curve(sf::Vector2f(rtr(points[i].x - 0.5), rtr(y)), -1);
			phase_curve(sf::Vector2f(rtr(points[i].x + 0.5), rtr(y)), 1);
			phase_curve(sf::Vector2f(rtr(points[i].x - 0.5), rtr(y)), 1);
		}
	}
	//Строим фазовые кривые на краях изображения, как для линейных систем
	for(double x = -10; x <= 10; x+= 1) {
		phase_curve(sf::Vector2f(rtr(x), 2), -1);
		phase_curve(sf::Vector2f(rtr(x), X-2), -1);
		phase_curve(sf::Vector2f(rtr(x), 2), 1);
		phase_curve(sf::Vector2f(rtr(x), X-2), 1);
	}
	for(double y = -10; y <= 10; y+= 1) {
		phase_curve(sf::Vector2f(2, rtr(y)), -1);
		phase_curve(sf::Vector2f(X-2, rtr(y)), -1);
		phase_curve(sf::Vector2f(2, rtr(y)), 1);
		phase_curve(sf::Vector2f(X-2, rtr(y)), 1);
	}
}

int main() {
	setSize(X, Y);
	setSystem(f1, f2, f1x, f1y, f2x, f2y);
	image.create(X, Y, sf::Color::Black);
	
	//Ищем особые точки системы
	vector<sf::Vector2f> points = solve();
	
	cout << "Особые точки: \n";
	for(int  i = 0; i < points.size(); i++) { 
		points[i].x = round(points[i].x*100)/100;
		points[i].y = round(points[i].y*100)/100;
		cout << "(" << points[i].x << ", " << points[i].y << ")\n";
	}
	//Построение фазового портрета
	phase_portrait(points);
	
	//Сохранение изображения
	image.flipVertically();
	image.saveToFile("portrait.png");
	return 0;
}
