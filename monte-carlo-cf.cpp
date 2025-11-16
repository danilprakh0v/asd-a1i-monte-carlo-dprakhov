// Подключение необходимых библиотек
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>

// --- Структуры данных ---
struct Point {
    double x;
    double y;
};

struct Circle {
    Point center;
    double radius;
};

struct BoundingBox {
    double min_x, max_x;
    double min_y, max_y;

    double get_area() const {
        return (max_x - min_x) * (max_y - min_y);
    }
};

// --- Вспомогательные функции ---
bool is_point_inside_circle(const Point& p, const Circle& c) {
    double sq_dist = pow(p.x - c.center.x, 2) + pow(p.y - c.center.y, 2);
    return sq_dist <= pow(c.radius, 2);
}

// --- Основная логика симуляции ---
double run_monte_carlo(const long long n_points, const std::vector<Circle>& circles,
                       const BoundingBox& rect_box, std::mt19937& rng) {
    if (rect_box.get_area() == 0.0) {
        return 0.0;
    }

    std::uniform_real_distribution<double> distX(rect_box.min_x, rect_box.max_x);
    std::uniform_real_distribution<double> distY(rect_box.min_y, rect_box.max_y);

    long long points_inside = 0;

    for (long long i = 0; i < n_points; ++i) {
        Point random_point = {distX(rng), distY(rng)};
        bool is_inside_all = true;
        for (const auto& circle : circles) {
            if (!is_point_inside_circle(random_point, circle)) {
                is_inside_all = false;
                break;
            }
        }
        if (is_inside_all) {
            points_inside++;
        }
    }

    return (static_cast<double>(points_inside) / n_points) * rect_box.get_area();
}


// --- Точка входа в программу ---

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // --- Чтение входных данных ---
    std::vector<Circle> circles(3);
    for (int i = 0; i < 3; ++i) {
        std::cin >> circles[i].center.x >> circles[i].center.y >> circles[i].radius;
    }

    BoundingBox bounding_box;
    bounding_box.min_x = circles[0].center.x - circles[0].radius;
    bounding_box.max_x = circles[0].center.x + circles[0].radius;
    bounding_box.min_y = circles[0].center.y - circles[0].radius;
    bounding_box.max_y = circles[0].center.y + circles[0].radius;

    for (int i = 1; i < 3; ++i) {
        bounding_box.min_x = std::min(bounding_box.min_x, circles[i].center.x - circles[i].radius);
        bounding_box.max_x = std::max(bounding_box.max_x, circles[i].center.x + circles[i].radius);
        bounding_box.min_y = std::min(bounding_box.min_y, circles[i].center.y - circles[i].radius);
        bounding_box.max_y = std::max(bounding_box.max_y, circles[i].center.y + circles[i].radius);
    }

    // Инициализация генератора случайных чисел.
    std::random_device rd;
    std::mt19937 random_engine(rd());

    // --- Запуск симуляции ---
    constexpr long long number_of_points_total = 7000000;

    const double approximate_area = run_monte_carlo(number_of_points_total, circles, bounding_box, random_engine);

    // --- Вывод результата ---
    std::cout << std::fixed << std::setprecision(20) << approximate_area << std::endl;

    return 0;
}