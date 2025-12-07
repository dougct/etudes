/*
Problem:
    Given a sequence of real numbers a_n, a_{n-1}, ..., a_1, a_0, and a real number x,
    compute the value of polynomial P_n(x) = a_n*x^n + a_{n-1}*x^{n-1} + ... + a_1*x + a_0.

Solution (Horner's Rule):
    Base case (n=0):
        Return n.

    Induction hypothesis:
        We know how to evaluate the polynomial represented by the coefficients a_n, a_{n-1}, ..., a_1 at the point x (i.e., we know how to compute P'_{n-1}(x)).

    Induction step:
        P_n(x) = x * P'_{n-1}(x) + a_0.
*/

/*
- Coefficients: [a_n, a_{n-1}, ..., a_1, a_0] (highest to lowest degree)
- Algorithm: Start with the highest degree coefficient and work down

For example, for polynomial 2x^{3} + 3x^{2} + 4x + 5:
  - Coefficients: [2, 3, 4, 5]
  - Horner's rule:
    a. Start with 2
    b. 2*x + 3
    c. (2*x + 3)*x + 4
    d. ((2*x + 3)*x + 4)*x + 5
*/

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

double evaluate_polynomial(const std::vector<double>& coefficients, double x) {
    double p = 0.0;
    for (auto coeff : coefficients) {
        p = x * p + coeff;
    }
    return p;
}

// Tests

void test_empty_polynomial() {
    // Empty coefficients should represent 0 polynomial
    std::vector<double> coefficients = {};
    assert(evaluate_polynomial(coefficients, 5.0) == 0.0);
    std::cout << "test_empty_polynomial passed\n";
}

void test_constant_polynomial() {
    // P(x) = 5
    std::vector<double> coefficients = {5.0};
    assert(evaluate_polynomial(coefficients, 2.0) == 5.0);
    assert(evaluate_polynomial(coefficients, 0.0) == 5.0);
    assert(evaluate_polynomial(coefficients, -1.0) == 5.0);
    std::cout << "test_constant_polynomial passed\n";
}

void test_linear_polynomial() {
    // P(x) = 3x + 2
    std::vector<double> coefficients = {3.0, 2.0};
    assert(evaluate_polynomial(coefficients, 0.0) == 2.0);
    assert(evaluate_polynomial(coefficients, 1.0) == 5.0);
    assert(evaluate_polynomial(coefficients, -1.0) == -1.0);
    assert(evaluate_polynomial(coefficients, 2.0) == 8.0);
    std::cout << "test_linear_polynomial passed\n";
}

void test_quadratic_polynomial() {
    // P(x) = 2x^2 + 3x + 1
    std::vector<double> coefficients = {2.0, 3.0, 1.0};
    assert(evaluate_polynomial(coefficients, 0.0) == 1.0);
    assert(evaluate_polynomial(coefficients, 1.0) == 6.0);
    assert(evaluate_polynomial(coefficients, -1.0) == 0.0);
    assert(evaluate_polynomial(coefficients, 2.0) == 15.0);
    std::cout << "test_quadratic_polynomial passed\n";
}

void test_cubic_polynomial() {
    // P(x) = x^3 - 2x^2 + 3x - 4
    std::vector<double> coefficients = {1.0, -2.0, 3.0, -4.0};
    assert(evaluate_polynomial(coefficients, 0.0) == -4.0);
    assert(evaluate_polynomial(coefficients, 1.0) == -2.0);
    assert(evaluate_polynomial(coefficients, 2.0) == 2.0);
    std::cout << "test_cubic_polynomial passed\n";
}

void test_quartic_polynomial() {
    // P(x) = x^4 - 3x^3 + 2x^2 - x + 5
    std::vector<double> coefficients = {1.0, -3.0, 2.0, -1.0, 5.0};
    assert(evaluate_polynomial(coefficients, 0.0) == 5.0);
    assert(evaluate_polynomial(coefficients, 1.0) == 4.0);
    assert(evaluate_polynomial(coefficients, -1.0) == 12.0);
    assert(evaluate_polynomial(coefficients, 2.0) == 3.0);
    std::cout << "test_quartic_polynomial passed\n";
}

void test_quintic_polynomial() {
    // P(x) = 2x^5 - x^4 + 3x^3 - 2x^2 + x - 1
    std::vector<double> coefficients = {2.0, -1.0, 3.0, -2.0, 1.0, -1.0};
    assert(evaluate_polynomial(coefficients, 0.0) == -1.0);
    assert(evaluate_polynomial(coefficients, 1.0) == 2.0);
    assert(evaluate_polynomial(coefficients, -1.0) == -10.0);
    std::cout << "test_quintic_polynomial passed\n";
}

void test_negative_x() {
    // P(x) = x^2 + 2x + 1 = (x + 1)^2
    std::vector<double> coefficients = {1.0, 2.0, 1.0};
    assert(evaluate_polynomial(coefficients, -2.0) == 1.0);
    assert(evaluate_polynomial(coefficients, -1.0) == 0.0);
    std::cout << "test_negative_x passed\n";
}

void test_fractional_coefficients() {
    // P(x) = 0.5x + 1.5
    std::vector<double> coefficients = {0.5, 1.5};
    assert(evaluate_polynomial(coefficients, 2.0) == 2.5);
    assert(evaluate_polynomial(coefficients, 4.0) == 3.5);
    std::cout << "test_fractional_coefficients passed\n";
}

int main() {
    test_empty_polynomial();
    test_constant_polynomial();
    test_linear_polynomial();
    test_quadratic_polynomial();
    test_cubic_polynomial();
    test_quartic_polynomial();
    test_quintic_polynomial();
    test_negative_x();
    test_fractional_coefficients();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
