// Подключение необходимых библиотек для ввода-вывода, работы с векторами,
// математических вычислений, генерации случайных чисел и форматирования вывода.
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <string>

// --- Структуры данных ---

// Структура для представления точки на двумерной плоскости с координатами x и y.
struct Point {
    double x; // Координата X
    double y; // Координата Y
};

// Структура для представления окружности.
// Определяется центром (типа Point) и радиусом.
struct Circle {
    Point center; // Центр окружности
    double radius; // Радиус
};

// Структура для описания прямоугольной области - ("ограничивающего прямоугольника"),
// в которой будут генерироваться случайные точки.
struct BoundingBox {
    std::string name;    // Имя области для идентификации - (например, "Wide" или "Narrow")
    double min_x, max_x; // Границы по оси X
    double min_y, max_y; // Границы по оси Y

    // Метод для вычисления площади прямоугольника.
    double get_area() const {
        return (max_x - min_x) * (max_y - min_y);
    }
};


// --- Вспомогательные функции ---

/**
 * @brief Проверяет, находится ли точка внутри (или на границе) окружности.
 * @param p Точка для проверки.
 * @param c Окружность.
 * @return true, если точка внутри или на окружности, иначе false.
 *
 * Для оптимизации вычислений используется сравнение квадрата расстояния
 * от точки до центра окружности с квадратом ее радиуса. Это позволяет
 * избежать дорогостоящей операции извлечения квадратного корня (sqrt).
 */
bool is_point_inside_circle(const Point& p, const Circle& c) {
    // Вычисляем квадрат расстояния по формуле: (x2-x1)^2 + (y2-y1)^2
    double sq_dist = pow(p.x - c.center.x, 2) + pow(p.y - c.center.y, 2);
    // Сравниваем с квадратом радиуса.
    return sq_dist <= pow(c.radius, 2);
}


// --- Основная логика симуляции ---

/**
 * @brief Запускает одну полную симуляцию методом Монте-Карло.
 * @param n_points Количество случайных точек для генерации.
 * @param circles Вектор окружностей, область пересечения которых ищется.
 * @param rect_box Прямоугольная область, в которой генерируются точки.
 * @param rng Генератор случайных чисел.
 * @return Приближенное значение площади пересечения.
 */
double run_monte_carlo(const long long n_points, const std::vector<Circle>& circles,
                       const BoundingBox& rect_box, std::mt19937& rng) {
    // Создаем распределения для получения случайных координат X и Y
    // равномерно в пределах заданной прямоугольной области (box).
    std::uniform_real_distribution<double> distX(rect_box.min_x, rect_box.max_x);
    std::uniform_real_distribution<double> distY(rect_box.min_y, rect_box.max_y);

    long long points_inside = 0; // Счетчик точек, попавших в целевую область.

    // Основной цикл симуляции.
    for (long long i = 0; i < n_points; ++i) {
        // Генерируем случайную точку.
        Point random_point = {distX(rng), distY(rng)};

        // Проверяем, находится ли точка ВНУТРИ ВСЕХ окружностей.
        bool is_inside_all = true;
        for (const auto& circle : circles) {
            // Если точка не попадает хотя бы в одну окружность...
            if (!is_point_inside_circle(random_point, circle)) {
                is_inside_all = false; // ...устанавливаем флаг в false...
                break;                 // ...и выходим из цикла, так как дальнейшая проверка не имеет смысла.
            }
        }

        // Если точка прошла проверку для всех окружностей, увеличиваем счетчик.
        if (is_inside_all) {
            points_inside++;
        }
    }

    // Вычисляем итоговую площадь по формуле Монте-Карло:
    // Площадь = (Доля точек внутри) * (Площадь области генерации)
    return (static_cast<double>(points_inside) / n_points) * rect_box.get_area();
}


// --- Точка входа в программу ---

int main() {
    // Оптимизация стандартных потоков ввода-вывода C++.
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // --- Определение исходных данных ---

    // Описание трех окружностей, площадь пересечения которых мы ищем.
    const std::vector<Circle> circles = {
        {{1.0, 1.0}, 1.0},
        {{1.5, 2.0}, std::sqrt(5.0) / 2.0},
        {{2.0, 1.5}, std::sqrt(5.0) / 2.0}
    };

    // Определение двух прямоугольных областей для эксперимента.

    // 1. Широкая область, которая очевидно покрывает всю фигуру.
    const BoundingBox wide_box = {"Wide", 0.0, 3.0, 0.0, 3.0};

    // 2. Узкая, более оптимальная область.
    // Она подобрана так, чтобы быть как можно ближе к границам искомой фигуры,
    // минимизируя "пустую" площадь и повышая эффективность симуляции.
    const BoundingBox narrow_box = {"Narrow", 0.8, 2.1, 0.8, 2.1};

    // Точное аналитическое значение площади для сравнения и вычисления погрешности.
    const double exact_area = 0.25 * acos(-1.0) + 1.25 * asin(0.8) - 1.0;

    // Инициализация генератора случайных чисел для воспроизводимости и качества.
    std::random_device rd;
    std::mt19937 random_engine(rd());

    // --- Проведение эксперимента и вывод результатов ---

    // Вывод заголовка для CSV-формата.
    std::cout << "N,Wide_Area,Narrow_Area,Wide_Rel_Error,Narrow_Rel_Error\n";

    // Цикл для проведения симуляций с разным количеством точек (от 100 до 100 000).
    for (long long n = 100; n <= 100000; n += 500) {
        // Запускаем симуляцию для широкой и узкой областей.
        const double wide_approx = run_monte_carlo(n, circles, wide_box, random_engine);
        const double narrow_approx = run_monte_carlo(n, circles, narrow_box, random_engine);

        // Вычисляем относительную погрешность для каждого случая.
        const double wide_err = std::abs(wide_approx - exact_area) / exact_area;
        const double narrow_err = std::abs(narrow_approx - exact_area) / exact_area;

        // Выводим результаты в CSV-формате с фиксированной точностью.
        std::cout << std::fixed << std::setprecision(8)
                  << n << "," << wide_approx << "," << narrow_approx << ","
                  << wide_err << "," << narrow_err << "\n";
    }

    return 0;
}