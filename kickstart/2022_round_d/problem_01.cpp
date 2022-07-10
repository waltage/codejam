/*
 * Copyright 2022 Daniel Walt
 */
#include <iostream>
#include <vector>
#include <algorithm>



struct TestCase {
  int regions = 0;
  int categories = 0;
  std::vector<uint32_t> participants;

  TestCase() {  // NOLINT
    std::cin >> regions >> categories;
    participants.resize(regions);
    for (int i = 0; i < regions; i++) {
      std::cin >> participants[i];
    }
  }
};


double solve(TestCase* c) {
  // with sorted participants
  // last M - 1 regions all as themselves,
  // first N-M in a single group

  std::sort(c->participants.begin(), c->participants.end());
  double agg = 0;
  int ptr = static_cast<int>(c->participants.size() - 1);
  for (int i = c->categories; i > 1; i--) {
    agg += c->participants[ptr];
    ptr--;
  }
  if (ptr % 2 == 0) {
    // odd number
    agg += c->participants[ptr/2];
  } else {
    // even, split
    double p1 = c->participants[ptr/2];
    double p2 = c->participants[ptr/2 + 1];
    agg += (p1 + p2) / 2;
  }

  return agg;
}



// ===================================================================
void run_all();

int main() {
  std::cout.precision(7);
  run_all();
  return 0;
}

void run_all() {
  int t;
  std::cin >> t;
  for (int cid = 1; cid < t + 1; cid++) {
    TestCase c;
    auto result = solve(&c);
    std::cout << "Case #" << cid << ": " << std::fixed << result << std::endl;
  }
}
