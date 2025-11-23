/*
Problem:
    Generate a list of all binary number of n digits

Solution:
    Base case (n=0):
        Empty list

    Induction hypothesis:
        We know how to generate S, the list of all binary numbers of n-1 digits

    Induction step:
        Generate all binary numbers of n-1 digits (using induction hypothesis).
        Let S0 be the list with '0' appended to all numbers in S.
        Let S1 be the list with '1' appended to all numbers in S.
        Return the concatenation S0 and S1.
*/

pub fn generate_binary_nums(n: usize) -> Vec<String> {
    if n == 0 {
        return vec![String::new()];
    }

    let binary_nums = generate_binary_nums(n - 1);

    let mut res = Vec::new();
    for num in &binary_nums {
        for suffix in ["0", "1"] {
            res.push(format!("{}{}", num, suffix));
        }
    }

    return res;
}

#[allow(dead_code)]
pub fn print_binary_nums(n: usize) {
    let nums = generate_binary_nums(n);
    for num in &nums {
        println!("{}", num);
    }
    println!();
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_generate_binary_nums_0() {
        let result = generate_binary_nums(0);
        let expected = vec![""];
        assert_eq!(result, expected);
    }

    #[test]
    fn test_generate_binary_nums_1() {
        let result = generate_binary_nums(1);
        let expected = vec!["0", "1"];
        assert_eq!(result, expected);
    }

    #[test]
    fn test_generate_binary_nums_2() {
        let result = generate_binary_nums(2);
        let expected = vec!["00", "01", "10", "11"];
        assert_eq!(result, expected);
    }

    #[test]
    fn test_generate_binary_nums_3() {
        let result = generate_binary_nums(3);
        let expected = vec!["000", "001", "010", "011", "100", "101", "110", "111"];
        assert_eq!(result, expected);
    }
}
