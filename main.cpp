#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <iterator>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <random>

template<class T>
class vector3d {
public:
    T x;
    T y;
    T z;

    vector3d(T x, T y, T z) : x(x), y(y), z(z) {}

    friend std::ostream &operator<<(std::ostream &os, const vector3d &rhs) {
        os << "{" << rhs.x << "," << rhs.y << "," << rhs.z << "}";
        return os;
    }

    std::string str() const {
        std::ostringstream ss;
        ss << std::fixed;
        ss << std::setprecision(2);
        ss << "{" << x << "," << y << "," << z << "}";
        return ss.str();
    }
};

template<class T>
class curve3d {
public:
    curve3d(T x, T y, T z)
            : position(x, y, z) {}

    virtual ~curve3d() = default;

    virtual vector3d<T> calculate(double angle) const = 0;

    virtual vector3d<T> derivative(double angle) const = 0;

public:
    vector3d<T> position;
};

template<class T>
class ellipse3d : public curve3d<T> {
public:
    ellipse3d(T x, T y, T a, T b)
            : ellipse3d(x, y, static_cast<T>(0), a, b) {}

    ellipse3d(T x, T y, T z, T a, T b)
            : curve3d<T>(x, y, z), a(a), b(b) {}

    vector3d<T> calculate(double angle) const override {
        return vector3d<T>(static_cast<T>(this->position.x + a * cos(angle)),
                          static_cast<T>(this->position.y + b * sin(angle)),
                          this->position.z);
    }

    vector3d<T> derivative(double angle) const override {
        return vector3d<T>(static_cast<T>(a * -sin(angle)),
                          static_cast<T>(b * cos(angle)),
                          0);
    }

public:
    T a;
    T b;
};

template<class T>
class circle3d : public ellipse3d<T> {
public:
    explicit circle3d(T x, T y, T r)
            : circle3d(x, y, 0, r) {}

    explicit circle3d(T x, T y, T z, T r)
            : ellipse3d<T>(x, y, z, r, r) {}
};

template<class T>
class helix3d : public curve3d<T> {
public:
    explicit helix3d(T x, T y, T z, T r, T step)
            : helix3d<T>(x, y, z, r, r, step, 0) {}

    explicit helix3d(T x, T y, T z, T a, T b, T step, double angle_start)
            : curve3d<T>(x, y, z), a(a), b(b), step(step), angle_start(angle_start) {}

    vector3d<T> calculate(double angle) const override {
        return vector3d<T>(
                static_cast<T>(this->position.x + a * cos(angle)),
                static_cast<T>(this->position.y + b * sin(angle)),
                this->position.z + (angle_start + angle)/(M_PI * 2) * step
        );
    }

    vector3d<T> derivative(double angle) const override {
        return vector3d<T>(
                static_cast<T>(a * -sin(angle)),
                static_cast<T>(b * cos(angle)),
                step / (M_PI * 2)
        );
    }

public:
    T a;
    T b;
    T step;
    double angle_start;
};

int main() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> dis(1, 100);
    
    std::vector<std::shared_ptr<curve3d<double>>> curves;
    for(size_t i=0; i<100; i++) {
        switch(i%3) {
            case 0:
                curves.push_back(std::make_shared<ellipse3d<double>>(dis(gen), dis(gen), dis(gen), dis(gen)));
                break;
            case 1:
                curves.push_back(std::make_shared<circle3d<double>>(dis(gen), dis(gen), dis(gen)));
                break;
            case 2:
                curves.push_back(std::make_shared<helix3d<double>>(dis(gen), dis(gen), dis(gen), dis(gen), dis(gen)));
                break;
            
        }
    }
    
    double angle = M_PI_4;
    for (const auto& curve : curves) {
        vector3d<double> point = curve->calculate(angle);
        vector3d<double> derivative = curve->derivative(angle);

        std::cout << "Point: " << point << std::endl;
        std::cout << "Derivative: " << derivative << std::endl;
    }

    std::vector<std::unique_ptr<curve3d<double>>> curves1;
    std::vector<circle3d<double>*> circleContainer;

    for (const auto& curve : curves) {
        if (auto* circle = dynamic_cast<circle3d<double>*>(curve.get())) {
            circleContainer.push_back(circle);
        }
    }

    std::sort(circleContainer.begin(), circleContainer.end(), [](circle3d<double>* a, circle3d<double>* b) {
        return a->a < b->a;
    });

    std::cout<<"first: "<<circleContainer.front()->a<<", last: "<<circleContainer.back()->a<<std::endl;
    double totalSum = 0.0;
    for (const auto& circle : circleContainer) {
        totalSum += circle->a;
    }

    std::cout << "Total Sum of Radii: " << totalSum << std::endl;
    return 0;
}
