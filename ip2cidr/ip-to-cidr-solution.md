# IP to CIDR — Solution Journey

## The Problem

Given a start IP address and a count `n`, cover exactly the range `[ip, ip + n - 1]` using the fewest CIDR blocks possible, with no addresses outside the range.

## Background: What is a CIDR Block?

A CIDR block `base_ip/prefix_length` describes a contiguous range of IP addresses:

- The prefix length `k` means: "the first `k` bits are fixed; the remaining `32 - k` bits can be anything."
- A block with prefix length `k` covers exactly `2^(32-k)` addresses.

**Example**: `192.168.1.0/24` — prefix length 24, so 32 - 24 = 8 free bits, covering 256 addresses:

```
192.168.1.0   = 11000000 10101000 00000001 00000000
192.168.1.255 = 11000000 10101000 00000001 11111111
                ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ --------
                fixed 24 bits               8 free bits
```

**Example**: `5.6.7.8/32` — all 32 bits fixed, covers exactly 1 address.

### The Alignment Rule

A CIDR block of size `2^m` **must** start at an address divisible by `2^m`. The last `m` bits (the "free" bits) must all be zero in the base address. For example, a `/29` block (size 8) can only start at multiples of 8.

---

## The Initial Solution

The first solution used a **"start small, grow"** approach:

1. At each step, compute the **alignment limit**: the largest power of 2 that divides `start_ip`, found via the lowest set bit (`start & (-start)`).
2. Starting from `block_size = 1`, keep doubling while both constraints hold: `block_size * 2 <= n` and `block_size * 2 <= alignment_limit`.
3. Compute `prefix_len = 32 - trailing_zeroes(block_size)`.

This approach was suggested first because it maps directly to the two constraints of the problem:

- **Alignment constraint**: block size can't exceed the lowest set bit of the address.
- **Count constraint**: block size can't exceed `n`.

The code used helper functions `lowest_set_bit_value()` and `get_block_size()` to encapsulate these.

### Why it was suggested

The "grow from 1" approach is the most common way this problem is presented in algorithm references. It's mechanically straightforward — start conservative, expand until you can't. The bit trick `start & (-start)` computes the alignment limit in one operation, making the code compact.

---

## Issues We Encountered Along the Way

### Understanding `start & (-start)`

The bit trick `start & (-start)` isolates the lowest set bit but isn't immediately obvious. We explored three equivalent ways to express it:

| Form | Style |
|---|---|
| `start & (-start)` | Most concise |
| `start & (~start + 1)` | Explicit two's complement |
| `1 << count_trailing_zeros(start)` | Most readable, no bit tricks |

### The `2 ^ 31` Bug

In the special case where `start_ip == 0`, we needed to return `2^31` as the alignment limit. The code used `2 ^ 31`, but in C++ `^` is **bitwise XOR**, not exponentiation. `2 ^ 31` evaluates to 29, not 2147483648. The fix: `1 << 31` or `(uint32_t)1 << 31`.

### Signed Integer Overflow

`1 << 31` as a signed `int` produces `-2147483648` (the minimum int value), causing undefined behavior in comparisons and arithmetic. This manifested as wrong results and runtime sanitizer errors. The fix: use `1u << 31` (unsigned shift) or `uint32_t` types consistently.

### The `trailing_zeroes(0)` Edge Case

`trailing_zeroes(0)` returned 0 because the `while (n && ...)` loop exits immediately when `n == 0`. But address 0 is aligned to every power of 2, so `m` should be 31. This required either a special case in the main loop or fixing the helper function.

---

## What Motivated the Change in Approach

The user's original intuition was: **"just count trailing zeros of the IP address, compute the prefix, and go."** This is a more natural way to think about the problem — you look at the address, see how aligned it is, and that tells you the block size.

That raw intuition doesn't work alone because it ignores the count constraint (you might overshoot `n`). But it can be fixed with a simple cap: "start with the alignment answer, then shrink until it fits."

This led to the **"start optimistic, then cap"** approach, which:

1. Is closer to how you'd reason about the problem by hand
2. Eliminates `lowest_set_bit_value()` and `get_block_size()` — everything is expressed through `m`
3. Reads as three clear English sentences: start optimistic, cap by count, emit

---

## The Final Solution

The solution is built around a single variable `m` — the number of **free bits** in the CIDR block. A block with `m` free bits covers `2^m` addresses and has prefix length `32 - m`.

### Algorithm

```
while n > 0:
    1. Start optimistic: m = trailing_zeroes(start_ip)   // alignment allows this
    2. Cap by count:     shrink m until 2^m <= n          // don't overshoot
    3. Emit:             block_size = 2^m, prefix = 32 - m
```

### C++ Code

```cpp
class Solution {
public:
    int ip2int(const string& ip) {
        int result = 0, octet = 0;
        for (auto c : ip) {
            if (c == '.') {
                result = (result << 8) | octet;
                octet = 0;
            } else {
                octet = octet * 10 + (c - '0');
            }
        }
        result = (result << 8) | octet;
        return result;
    }

    string int2ip(int val) {
        return to_string((val >> 24) & 255) + "." +
               to_string((val >> 16) & 255) + "." +
               to_string((val >> 8) & 255) + "." + to_string(val & 255);
    }

    int trailing_zeroes(int n) {
        if (n == 0) {
            return 32;
        }

        int tz = 0;
        while (!(n & 1)) {
            tz++;
            n >>= 1;
        }
        return tz;
    }

    vector<string> ipToCIDR(string ip, int n) {
        // Let m be the number of free bits in the CIDR block — the bits that
        // can vary freely (be 0 or 1).
        // A CIDR block with m free bits covers 2^m addresses, and has prefix
        // length 32 - m.
        // Example: m = 3
        // 3 free bits --> block covers 2^3 = 8 addresses
        // Prefix length = 32 - 3 = 29 --> a /29 block
        // The base address must end in 000 (3 trailing zeros)
        // So the algorithm reads as:

        // Start with m = as many free bits as alignment allows
        // Shrink m until 2^m doesn't exceed the remaining count
        // Emit a /(32-m) block

        vector<string> result;
        int start_ip = ip2int(ip);

        while (n > 0) {
            // Largest block that alignment allows
            // We cap at 31 because a CIDR block can have at most 31 free bits.
            // A /0 block would mean "all 2^32 addresses on the internet," which
            // doesn't make sense for our problem. So 31 is the largest useful
            // value of m.
            int m = min(trailing_zeroes(start_ip), 31);

            // At this point, if we make cidr: ip/(32 - m), the cidr block will
            // contain 2^m addresses, which might be more ip addresses than n.
            // We need to fix that.

            // Cap by count: shrink until block size (2^m) fits within n
            while (m > 0 && ((1u << m) > (uint32_t)n)) {
                m--;
            }

            int prefix_len = 32 - m;
            string cidr = int2ip(start_ip) + "/" + to_string(prefix_len);
            result.push_back(cidr);

            // num_ips is the number of IP addresses covered by the current
            // CIDR block. It equals 2^m, where m is the number of free bits.
            int num_ips = 1 << m;
            start_ip += num_ips;
            n -= num_ips;
        }

        return result;
    }
};
```

Can you give a concrete example showing why we had to do the cast to unsigned here?

            while (m > 0 && ((1u << m) > (uint32_t)n)) {
                m--;
            }

Say m = 31 and n = 1000:

Without unsigned:


1 << 31 = -2147483648    (signed overflow: bit 31 is the sign bit)
-2147483648 > 1000  →  false!  (negative < positive)
The loop doesn't shrink m at all. We'd emit a block of 2^31 addresses when we only need 1000.

With unsigned:


1u << 31 = 2147483648    (valid unsigned value)
2147483648 > 1000  →  true

### Key Details

- **`m`**: the number of free bits. This single variable drives block_size (`2^m`) and prefix_len (`32 - m`).
- **`trailing_zeroes(0)` returns 31**: because address 0 is aligned to every power of 2.
- **`1u << m`**: the `u` suffix ensures unsigned arithmetic, avoiding signed overflow when `m = 31`.
- **Greedy correctness**: using the largest valid block at each step is optimal because smaller blocks would require more entries to cover the same addresses.

### Walkthrough: `ip = "255.0.0.7"`, `n = 10`

| Step | `start_ip` | `m` (from trailing zeros) | `m` (after cap) | Block | CIDR |
|---|---|---|---|---|---|
| 1 | 7 = `...0111` | 0 | 0 | 1 | `255.0.0.7/32` |
| 2 | 8 = `...1000` | 3 | 3 (8 <= 9) | 8 | `255.0.0.8/29` |
| 3 | 16 = `...10000` | 4 | 0 (1 <= 1) | 1 | `255.0.0.16/32` |

Result: `["255.0.0.7/32", "255.0.0.8/29", "255.0.0.16/32"]`
