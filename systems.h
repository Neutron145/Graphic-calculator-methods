/*
 *
 *	Все необходимое для решения нелинейных систем методом Ньютона
 *
 */

double (*f1_)(double, double) = NULL;
double (*f2_)(double, double) = NULL;

double (*f1x_)(double, double) = NULL;
double (*f1y_)(double, double) = NULL;

double (*f2x_)(double, double) = NULL;
double (*f2y_)(double, double) = NULL;

//Структура матрицы
struct matrix {
	double** m;
	int h, w;

	matrix(int h, int w) {
		this->h = h;
		this->w = w;
		m = new double*[h];
		for(int i = 0; i < h; i++) {
			m[i] = new double[w];
			for(int j = 0; j < w; j++) {
				m[i][j] = 0;
			}
		}
	}

};

//Значение переменных функции в конкретной точке
matrix value(double x, double y) {
	matrix val(2, 1);	
	val.m[0][0] = f1_(x, y);
	val.m[1][0] = f2_(x, y);
	return val;
}

//Матрица Якоби в конкретной точке
matrix W(double x, double y){
	matrix W_(2, 2);
	W_.m[0][0] = f1x_(x, y);
	W_.m[0][1] = f1y_(x, y);
	W_.m[1][0] = f2x_(x, y);
	W_.m[1][1] = f2y_(x, y);
	return W_;
}

//Обратная матрица Якоби
matrix reverse_W(matrix W_) {
	matrix rW(2, 2);
	double detW = (W_.m[0][0] * W_.m[1][1]) - (W_.m[1][0] * W_.m[0][1]);
	rW.m[0][0] = W_.m[1][1];
	rW.m[1][1] = W_.m[0][0];
	rW.m[1][0] = -W_.m[1][0];
	rW.m[0][1] = -W_.m[0][1];
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++) {
			rW.m[i][j] /= detW;
		}
	}
	return rW;
}

//Норма разности вектора 
double norm(sf::Vector2f a, sf::Vector2f b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));	
}

//Умножение матриц
matrix dot(matrix a, matrix b) { 
	matrix res(a.h, b.w);
	for(int i = 0; i < a.h; i++) {
		for(int j = 0; j < b.w; j++) {
			for(int k = 0; k < a.w; k++) {
				res.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
	return res;
}

//Метод Ньютона для систем
sf::Vector2f newton(sf::Vector2f p0, double eps) {
	sf::Vector2f p1;
	sf::Vector2f p2 = p0;
	do {
		p1 = p2;		
		matrix val = value(p1.x, p1.y);
		matrix W_ = reverse_W(W(p1.x, p1.y));
		matrix delta = dot(W_, val);
		p2.x = p1.x - delta.m[0][0] * 0.8;
		p2.y = p1.y - delta.m[1][0] * 0.8;
	} while(norm(p1, p2) > eps); 
	return p2;
}

//Отбор особых точек из всех решений
std::vector<sf::Vector2f> filtrate_points(std::vector<sf::Vector2f> points_all) {
	//Бежим по всем точкам и исключаем те, которые не сильно отличаются на те, что есть в векторе особых точек
	std::vector<sf::Vector2f> points;
	points.push_back(points_all[0]);
	bool has_el = false;
	for(int i = 0; i < points_all.size(); i++) {
		has_el = false;
		for(int j = 0; j < points.size(); j++) {
			if(norm(points[j], points_all[i]) < 0.1) {
				has_el = true;
				break;
			}
		}
		if(!has_el) points.push_back(points_all[i]);
	}
	return points;
}


void setSystem(double (*F1)(double, double), double (*F2)(double, double),
		double (*F1x)(double, double), double (*F1y)(double, double),
		double (*F2x)(double, double), double (*F2y)(double, double)) 
{
	f1_ = F1;
	f2_ = F2;
	f1x_ = F1x;
	f1y_ = F1y;
	f2x_ = F2x;
	f2y_ = F2y;
}

//Решатель нелинейной системы
std::vector<sf::Vector2f> solve() {
	//Генерируем начальные точки на сетке
	std::vector<sf::Vector2f> points_all;
	for(int x = -9; x < 9; x+= 2) {
		for(int y = -9; y < 9; y+=2) {
			//Ищем решение для каждой точки
			points_all.push_back(newton(sf::Vector2f(x, y), 1e-4));
		}
	}
	//Отбираем особые точки и возвращаем их
	return filtrate_points(points_all);
}
