/*
* Copyright 2022 Daniel Walt
*/

#include <iostream>
#include <string>
#include <sstream>

/*
 * Because of the size of N in the large cases, we need to treat all cases
 * as strings.
 *
 * Sum of digits must also be 9;
 * Find the digit to insert that makes the sum 9...
 * Iterate through the digits large to small until
 *    we find a digit bigger than our insertion digit...
 *    Insert our new digit at this place.
 */


inline int cconv(char c) {
  return c - '0';
}


struct TestCase {
    std::string n;
    int sum_z8 = 0;

    TestCase() {  // NOLINT
      std::cin >> n;
      for (char i : n) {
        sum_z8 += cconv(i);
        if (sum_z8 > 9) {
          // cheap way to avoid mod
          sum_z8 -= 9;
        }
      }
    };
};

std::string solve(TestCase& c) {
  std::stringstream result;
  int insertion = 9 - c.sum_z8;
  int n_start = 0;
  if (insertion == 0) {
    n_start = 1;
  }

  while (cconv(c.n[n_start]) <= insertion && n_start < c.n.size()) {
    n_start++;
  }

  result << c.n.substr(0, n_start);
  result << insertion;
  result << c.n.substr(n_start, c.n.length() - n_start);
  return result.str();
}



// ===================================================================
void run_all();

int main() {
	run_all();
	return 0;
}

void run_all() {
	int t;
	std::cin >> t;
	for (int cid = 1; cid < t + 1; cid++) {
		TestCase c;
		auto result = solve(c);
		std::cout << "Case #" << cid << ": " << result << std::endl;
	}
}