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
pub fn evaluate_polynomial(coefficients: &[f64], x: f64) -> f64 {
    let mut p = 0.0;
    for i in 0..coefficients.len() {
        p = x * p + coefficients[i];
    }
    return p;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_empty_polynomial() {
        // Empty coefficients should represent 0 polynomial
        let coefficients = vec![];
        assert_eq!(evaluate_polynomial(&coefficients, 5.0), 0.0);
    }

    #[test]
    fn test_constant_polynomial() {
        // P(x) = 5
        let coefficients = vec![5.0];
        assert_eq!(evaluate_polynomial(&coefficients, 2.0), 5.0);
        assert_eq!(evaluate_polynomial(&coefficients, 0.0), 5.0);
        assert_eq!(evaluate_polynomial(&coefficients, -1.0), 5.0);
    }

    #[test]
    fn test_linear_polynomial() {
        // P(x) = 3x + 2
        let coefficients = vec![3.0, 2.0];
        assert_eq!(evaluate_polynomial(&coefficients, 0.0), 2.0);
        assert_eq!(evaluate_polynomial(&coefficients, 1.0), 5.0);
        assert_eq!(evaluate_polynomial(&coefficients, -1.0), -1.0);
        assert_eq!(evaluate_polynomial(&coefficients, 2.0), 8.0);
    }

    #[test]
    fn test_quadratic_polynomial() {
        // P(x) = 2x^2 + 3x + 1
        let coefficients = vec![2.0, 3.0, 1.0];
        assert_eq!(evaluate_polynomial(&coefficients, 0.0), 1.0);
        assert_eq!(evaluate_polynomial(&coefficients, 1.0), 6.0);
        assert_eq!(evaluate_polynomial(&coefficients, -1.0), 0.0);
        assert_eq!(evaluate_polynomial(&coefficients, 2.0), 15.0);
    }

    #[test]
    fn test_cubic_polynomial() {
        // P(x) = x^3 - 2x^2 + 3x - 4
        let coefficients = vec![1.0, -2.0, 3.0, -4.0];
        assert_eq!(evaluate_polynomial(&coefficients, 0.0), -4.0);
        assert_eq!(evaluate_polynomial(&coefficients, 1.0), -2.0);
        assert_eq!(evaluate_polynomial(&coefficients, 2.0), 2.0);
    }

    #[test]
    fn test_quartic_polynomial() {
        // P(x) = x^4 - 3x^3 + 2x^2 - x + 5
        let coefficients = vec![1.0, -3.0, 2.0, -1.0, 5.0];
        assert_eq!(evaluate_polynomial(&coefficients, 0.0), 5.0);
        assert_eq!(evaluate_polynomial(&coefficients, 1.0), 4.0);
        assert_eq!(evaluate_polynomial(&coefficients, -1.0), 12.0);
        assert_eq!(evaluate_polynomial(&coefficients, 2.0), 3.0);
    }

    #[test]
    fn test_quintic_polynomial() {
        // P(x) = 2x^5 - x^4 + 3x^3 - 2x^2 + x - 1
        let coefficients = vec![2.0, -1.0, 3.0, -2.0, 1.0, -1.0];
        assert_eq!(evaluate_polynomial(&coefficients, 0.0), -1.0);
        assert_eq!(evaluate_polynomial(&coefficients, 1.0), 2.0);
        assert_eq!(evaluate_polynomial(&coefficients, -1.0), -10.0);
    }

    #[test]
    fn test_negative_x() {
        // P(x) = x^2 + 2x + 1 = (x + 1)^2
        let coefficients = vec![1.0, 2.0, 1.0];
        assert_eq!(evaluate_polynomial(&coefficients, -2.0), 1.0);
        assert_eq!(evaluate_polynomial(&coefficients, -1.0), 0.0);
    }

    #[test]
    fn test_fractional_coefficients() {
        // P(x) = 0.5x + 1.5
        let coefficients = vec![0.5, 1.5];
        assert_eq!(evaluate_polynomial(&coefficients, 2.0), 2.5);
        assert_eq!(evaluate_polynomial(&coefficients, 4.0), 3.5);
    }
}
