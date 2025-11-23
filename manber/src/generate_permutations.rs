/*
Problem:
    Generate all permutations of a given word

Solution:
    Base case (pos=0): word of size one
        Return the word as a single permutation

    Induction hypothesis:
        We know how to generate all permutations of a word with one less character

    Induction step:
        Remove the character at position pos from the word.
        Generate all permutations of the word with the character removed (using induction hypothesis).
        For each permutation, interpolate the removed character at every possible position.
        Return the concatenation of all these interpolated results.
*/

/*
Insert character ch at every possible position in word.

We need to iterate up to len(word) + 1 because we want to insert the character ch at:
- Before the first character (position 0)
- Between each pair of characters (positions 1, 2, ..., len(word)-1)
- After the last character (position len(word))

For example, with word = "ab" (length 2), we need 3 positions:
- i = 0: "" + ch + "ab" --> "cab"
- i = 1: "a" + ch + "b" --> "acb"
- i = 2: "ab" + ch + "" --> "abc"
*/
fn interpolate(ch: char, word: &str) -> Vec<String> {
    let mut res = Vec::with_capacity(word.len() + 1);
    for i in 0..word.len() + 1 {
        // &word[..i] will be empty when i == 0
        // &word[i..] will be empty when i == word.len()
        let interpolation = format!("{}{}{}", &word[..i], ch, &word[i..]);
        res.push(interpolation);
    }
    return res;
}

pub fn generate_permutations(word: &str, pos: usize) -> Vec<String> {
    if pos == 0 {
        return vec![word.to_string()];
    }

    // Word with ch removed where ch is the char at word[pos]
    let chars: Vec<char> = word.chars().collect();
    let smaller_word = format!("{}{}", &word[..pos], &word[pos + 1..]);

    // All permutations of word without ch
    let perms = generate_permutations(&smaller_word, pos - 1);

    // Interpolate ch back into every position of every permutation of the word
    let ch = chars[pos];
    let mut res = Vec::new();
    for perm in &perms {
        let interpolations = interpolate(ch, perm);
        res.extend(interpolations);
    }

    return res;
}

pub fn print_permutations(word: &str) {
    if word.is_empty() {
        return;
    }
    let perms = generate_permutations(word, word.len() - 1);
    for perm in &perms {
        println!("{}", perm);
    }
    println!();
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;

    #[test]
    fn test_generate_permutations_a() {
        let result = generate_permutations("A", 0);
        let expected = vec!["A"];
        assert_eq!(result, expected);
    }

    #[test]
    fn test_generate_permutations_ab() {
        let result = generate_permutations("AB", 1);
        let expected = vec!["BA", "AB"];
        assert_eq!(result, expected);
    }

    #[test]
    fn test_generate_permutations_abc() {
        let result = generate_permutations("ABC", 2);
        let expected = vec!["CBA", "BCA", "BAC", "CAB", "ACB", "ABC"];
        assert_eq!(result, expected);
    }

    #[test]
    fn test_generate_permutations_abcd() {
        let result = generate_permutations("ABCD", 3);

        // ABCD has 24 permutations
        assert_eq!(result.len(), 24);

        // Check that all permutations are unique
        let unique_perms: HashSet<_> = result.iter().collect();
        assert_eq!(unique_perms.len(), 24);

        // Check that each permutation contains all original characters
        for perm in &result {
            assert_eq!(perm.len(), 4);
            assert!(perm.contains('A'));
            assert!(perm.contains('B'));
            assert!(perm.contains('C'));
            assert!(perm.contains('D'));
        }
    }

    #[test]
    fn test_interpolate() {
        let result = interpolate('X', "AB");
        let expected = vec!["XAB", "AXB", "ABX"];
        assert_eq!(result, expected);
    }
}
