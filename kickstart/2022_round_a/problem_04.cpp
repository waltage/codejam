/*
 * Copyright 2022 Daniel Walt
 */

#include <iostream>





struct TestCase {
    int i = 0;

    TestCase() {  // NOLINT
    // Parse Case.

    std::cin >> i;

    }
};


int solve(TestCase& c) {



  return 0;
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