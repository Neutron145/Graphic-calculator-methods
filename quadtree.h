/*
 *
 *	Все необходимое для построения неявных функций
 *
 */

//Функция, которую надо отрисовать
double (*f_)(double, double, double) = NULL;
//Свободный член функции
double C_;

//Размер изображения
double X, Y;

//Преобразование из координат изображения в координаты плоскости OXY
double tr(double a) { return (a - 800)/80; }
//Обратное преобразование
double rtr(double a) {return a*80+800; }
//Проверка знака числа
int sign(double x) { return x <= 0 ? -1 : 1; }
//Своп
void swap(float& a, float& b) {
	float temp = a;
	a = b;
	b = temp;
}
//Проверка, принадлежат ли координаты изображению
bool on_screen(sf::Vector2f point) {
	if(point.x < 1 || point.x > (X-1)) 
		return false;
	else if(point.y < 1 || point.y > (Y-1))
		return false;
	return true;
}

//Структура квадранта
struct Quadtree {
	//Потомки
	Quadtree* childs;
	//Координаты верхнего левого угла квадратнта на изображении
	double x, y;
	//Размер квадранта
	double size;
	//Булева переменная, отвечающая за разбиение квадранта
	bool subdiv;
	//Знаки функции в углах
	int* signs;

	Quadtree() {
		x = 0;
		y = 0;
		size = 0;
		subdiv = false;
		signs = new int[4];
		signs[0] = sign(f_(tr(x), tr(y), C_));
		signs[1] = sign(f_(tr(x+size), tr(y), C_));
		signs[2] = sign(f_(tr(x+size), tr(y+size), C_));
		signs[3] = sign(f_(tr(x), tr(y+size), C_));
	}
	Quadtree(double x, double y, double size) {
		this->x = x;
		this->y = y;
		this->size = size;
		subdiv = false;
		signs = new int[4];
		signs[0] = sign(f_(tr(x), tr(y), C_));
		signs[1] = sign(f_(tr(x+size), tr(y), C_));
		signs[2] = sign(f_(tr(x+size), tr(y+size), C_));
		signs[3] = sign(f_(tr(x), tr(y+size), C_));
	}
	
	//Разбиение на 4 квадранта
	void subdivide(){
		childs = new Quadtree[4];
		double child_size = size / 2;
		childs[0] = Quadtree(x, y, child_size);
		childs[1] = Quadtree(x + child_size, y, child_size);
		childs[2] = Quadtree(x + child_size, y + child_size, child_size);
		childs[3] = Quadtree(x, y + child_size, child_size);
		subdiv = true;
	}
	
	//Проверка, перескает ли функция квадрант
	bool isIntersect() {
		for(int i = 1; i < 4; i++)
			if(signs[i] != signs[i-1]) return true;
		return false;
	}
};

//Алгоритм Брезенхема
bool line(sf::Vector2f start, sf::Vector2f end, sf::Color color, sf::Image& image) {
	bool steep = false;
	if(abs(start.x - end.x) < abs(start.y - end.y)) {
		swap(start.x, start.y);
		swap(end.x, end.y);
		steep = true;
	}
	if(start.x > end.x) {
		swap(start.x, end.x);
		swap(start.y, end.y);
	}
	double dx = end.x - start.x;
	double dy = end.y - start.y;
	double derror = dy/dx;
	derror = derror > 0 ? derror : -1 * derror;
	double error = 0;
	int y = start.y;	
	for(int x = start.x; x <= end.x; x++) {
		if(steep && !on_screen(sf::Vector2f(y, x)) || !steep && !on_screen(sf::Vector2f(x, y))) { 
			return false;		
		}
		if(steep) {
			image.setPixel(y, x, color);
		}
		else { 
			image.setPixel(x, y, color);
		}
		error += derror;
		if(error > 0.5) {
			int d = end.y > start.y? 1: -1;
			y += (end.y > start.y ? 1 : -1);
			error -= 1.;
		}
	}
	return true;
}


sf::Vector2f  bisection(sf::Vector2f p1, sf::Vector2f p2) {
	return sf::Vector2f(0.5 * (p1.x + p1.y), 0.5 * (p2.x + p2.y));
}

//Интерполяция по Х
sf::Vector2f interpolateX(sf::Vector2f p1, sf::Vector2f p2) {
	double f1 = f_(tr(p1.x), tr(p1.y), C_);
	double f2 = f_(tr(p2.x), tr(p2.y), C_);
	double r = f2 - f1;	
	if(r != r) return bisection(p1, p2);
	return sf::Vector2f(p2.x - f2 * (p2.x - p1.x)/r, p1.y);
}

sf::Vector2f interpolateY(sf::Vector2f p1, sf::Vector2f p2) {
	double f1 = f_(tr(p1.x), tr(p1.y), C_);
	double f2 = f_(tr(p2.x), tr(p2.y), C_);
	double r = f2 - f1;
	if(r != r) return bisection(p1, p2);
	return sf::Vector2f(p1.x, p2.y - f2 * (p2.y - p1.y)/r);
}
//Интерполяция функции в квадранте
sf::Vector2f* interpolate(Quadtree cell, sf::Image& image) {
	sf::Vector2f* points = new sf::Vector2f[2];
	if(cell.signs[0] != cell.signs[1] && cell.signs[1] == cell.signs[2] && cell.signs[1] == cell.signs[3]) {
		points[0] = interpolateX(sf::Vector2f(cell.x, cell.y), sf::Vector2f(cell.x + cell.size, cell.y));
		points[1] = interpolateY(sf::Vector2f(cell.x, cell.y), sf::Vector2f(cell.x, cell.y + cell.size));
	}
	if(cell.signs[1] != cell.signs[0] && cell.signs[0] == cell.signs[2] && cell.signs[0] == cell.signs[3]) { 
		points[0] = interpolateX(sf::Vector2f(cell.x, cell.y), sf::Vector2f(cell.x + cell.size, cell.y));
		points[1] = interpolateY(sf::Vector2f(cell.x + cell.size, cell.y), sf::Vector2f(cell.x + cell.size, cell.y + cell.size));
	}
	if(cell.signs[2] != cell.signs[0] && cell.signs[0] == cell.signs[1] && cell.signs[0] == cell.signs[3]) {
		points[0] = interpolateX(sf::Vector2f(cell.x, cell.y + cell.size), sf::Vector2f(cell.x + cell.size, cell.y + cell.size));
		points[1] = interpolateY(sf::Vector2f(cell.x + cell.size, cell.y), sf::Vector2f(cell.x + cell.size, cell.y + cell.size));
	}
	if(cell.signs[3] != cell.signs[0] && cell.signs[0] == cell.signs[1] && cell.signs[0] == cell.signs[2]) { 
		points[0] = interpolateX(sf::Vector2f(cell.x, cell.y + cell.size), sf::Vector2f(cell.x + cell.size, cell.y + cell.size));
		points[1] = interpolateY(sf::Vector2f(cell.x, cell.y), sf::Vector2f(cell.x, cell.y + cell.size));
	}
	if(cell.signs[0] == cell.signs[3] && cell.signs[0] != cell.signs[1] && cell.signs[1] == cell.signs[2]) {
		points[0] = interpolateX(sf::Vector2f(cell.x, cell.y), sf::Vector2f(cell.x + cell.size, cell.y));
		points[1] = interpolateX(sf::Vector2f(cell.x, cell.y + cell.size), sf::Vector2f(cell.x + cell.size, cell.y + cell.size));
	}
	if(cell.signs[0] == cell.signs[1] && cell.signs[1] != cell.signs[2] && cell.signs[2] == cell.signs[3]) {
		points[0] = interpolateY(sf::Vector2f(cell.x, cell.y), sf::Vector2f(cell.x, cell.y + cell.size));
		points[1] = interpolateY(sf::Vector2f(cell.x + cell.size, cell.y), sf::Vector2f(cell.x + cell.size, cell.y + cell.size));
	}
	return points;	
}



//Рисование квадранта. Использовался для визуальной проверки разбиения пространства
void draw_quad(Quadtree root, sf::Image &image) {
		line(sf::Vector2f(root.x, root.y), sf::Vector2f(root.x + root.size, root.y), sf::Color::White, image);
		line(sf::Vector2f(root.x + root.size, root.y), sf::Vector2f(root.x + root.size, root.y+root.size), sf::Color::White, image);
		line(sf::Vector2f(root.x + root.size, root.y + root.size), sf::Vector2f(root.x, root.y+root.size), sf::Color::White, image);
		line(sf::Vector2f(root.x, root.y + root.size), sf::Vector2f(root.x, root.y), sf::Color::White, image);
}

//Разбиение при адаптивном выборе глубины
/*void grid_search_line_adapt(Quadtree* root, sf::Image &image){
	if(root->isIntersect()) {
		sf::Vector2f* points = interpolate(*root, image);
		double x1 = tr(points[0].x); 
		double y1 = tr(points[0].y); 

		double i = sqrt(pow(pow(fx(x1, y1), 2) + pow(fy(x1, y1), 2), 3)); 
		double k = pow(fy(x1, y1), 2) * fxx(x1, y1) - 2 * fx(x1, y1) * fy(x1, y1) * fxy(x1, y1) + pow(fx(x1, y1), 2) * fyy(x1, y1); 
		double R = abs(i/k); 
		if(3 * root->size > R) { 
			root->subdivide();
			for(int i = 0; i < 4; i++) {
				grid_search_line_adapt((root->childs)+i, image);
			}
			return;
		}
		else {
			line(points[0], points[1], sf::Color::Yellow, image);
			return;
		}
	}
	return;
}*/

//Разбиение при фиксированной глубине
void grid_search_line(Quadtree* root, sf::Image &image, int search_depth){
	if(root->isIntersect()) {
		//draw_quad(*root, image);	
		sf::Vector2f* points = interpolate(*root, image);
		if(search_depth < 5) {
			root->subdivide();
			for(int i = 0; i < 4; i++)
				grid_search_line((root->childs)+i, image, search_depth + 1);
			return;
		}
		else {
			line(sf::Vector2f(points[0].x, points[0].y), sf::Vector2f(points[1].x, points[1].y), sf::Color::Yellow, image);
			return;
		}
	}
	return;
}

void setSize(double x, double y) {
	X = x;
	Y = y;
}

//Функция отрисовки функции
void plot(sf::Image& image, double (*F)(double, double, double), double c) {
	f_ = F;
	C_ = c;
	Quadtree root(0, 0, 1600);
	root.subdivide();
	for(int i = 0; i < 4; i++)
		grid_search_line((root.childs) + i, image, 0);
}
