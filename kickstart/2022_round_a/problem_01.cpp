#include <iostream>
#include <string>


struct TestCase {
    std::string expected;
    std::string input;

    TestCase() {  // NOLINT
      std::cin >> expected >> input;
    };
};


std::string solve(TestCase& c) {
  if (c.input.size() < c.expected.size()) {
    // input not long enough
    return "IMPOSSIBLE";
  }

  if (c.input.size() == c.expected.size()) {
    if (c.input == c.expected) {
      // same string
      return "0";
    } else {
      // otherwise, can't remove any
      return "IMPOSSIBLE";
    }
  }

  size_t exp_ptr = c.expected.size() - 1;
  size_t inp_ptr = c.input.size() - 1;
  int drop_cnt = 0;
  while (exp_ptr > 0 && inp_ptr > 0) {
    // until we run out of characters, move right to left
    if (c.expected[exp_ptr] == c.input[inp_ptr]) {
      // match, dec both
      exp_ptr--;
      inp_ptr--;
    } else {
      // no match, need to drop from input
      drop_cnt++;
      inp_ptr--;
    }
  }
  if (exp_ptr > 0) {
    // did not reach the end of the expected string
    return "IMPOSSIBLE";
  }
  // we've hit the end of the expected string, so any remaining
  // inputs must be dropped as well...
  return std::to_string(drop_cnt + inp_ptr);
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