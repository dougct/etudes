#pragma once

// A CIDR block `base_ip/prefix_length` describes a contiguous range of IP
// addresses:

// - The prefix length `k` means: "the first `k` bits are fixed; the remaining
// `32 - k` bits can be anything."
// - A block with prefix length `k` covers exactly `2^(32-k)` addresses.

// **Example**: `192.168.1.0/24` — prefix length 24, so 32 - 24 = 8 free bits,
// covering 256 addresses:

// ```
// 192.168.1.0   = 11000000 10101000 00000001 00000000
// 192.168.1.255 = 11000000 10101000 00000001 11111111
//                 ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ --------
//                 fixed 24 bits               8 free bits
// ```

int ip2int(const std::string& ip) {
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

std::string int2ip(int val) {
  return std::to_string((val >> 24) & 255) + "." +
         std::to_string((val >> 16) & 255) + "." +
         std::to_string((val >> 8) & 255) + "." + std::to_string(val & 255);
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

std::vector<std::string> ipToCIDR(std::string ip, int n) {
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

  std::vector<std::string> result;
  int start_ip = ip2int(ip);

  while (n > 0) {
    // Largest block that alignment allows
    // We cap at 31 because a CIDR block can have at most 31 free bits.
    // A /0 block would mean "all 2^32 addresses on the internet," which
    // doesn't make sense for our problem. So 31 is the largest useful
    // value of m.
    int m = std::min(trailing_zeroes(start_ip), 31);

    // At this point, if we make cidr: ip/(32 - m), the cidr block will
    // contain 2^m addresses, which might be more ip addresses than n.
    // We need to fix that.

    // Cap by count: shrink until block size (2^m) fits within n
    while (m > 0 && ((1u << m) > (uint32_t)n)) {
      m--;
    }

    int prefix_len = 32 - m;
    std::string cidr = int2ip(start_ip) + "/" + std::to_string(prefix_len);
    result.push_back(cidr);

    // num_ips is the number of IP addresses covered by the current
    // CIDR block. It equals 2^m, where m is the number of free bits.
    int num_ips = 1 << m;
    start_ip += num_ips;
    n -= num_ips;
  }

  return result;
}
