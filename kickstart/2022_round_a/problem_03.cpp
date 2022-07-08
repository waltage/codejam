/*
 * Copyright 2022 Daniel Walt
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

typedef std::unordered_set<char> charset;
typedef std::unordered_set<std::string> strset;

struct Node {
    std::string key;
    charset possible;
    strset children;
    bool has_cycle = false;
    char for_cycle = '_';
};

struct Wildcard {
    // For compression of multiple consecutive '?'.
    int str_pos;
    int run_length;
};

enum DfsOpType {
    Backward,
    Forward,
};

struct DfsOp {
    DfsOpType type;
    int wildcard_pos;
    int string_pos;
    char value;
};

/* See problem_03_supp.cpp for the derivation/generation of these constants */
// NOLINTNEXTLINE
static const strset unsafe_5{{"11111"}, {"10101"}, {"01110"}, {"01010"},
                             {"11011"}, {"00100"}, {"10001"}, {"00000"}};
// NOLINTNEXTLINE
static const strset unsafe_6{{"101101"}, {"011110"}, {"001100"}, {"100001"},
                             {"111111"}, {"010010"}, {"110011"}, {"000000"}};

// NOLINTNEXTLINE
static const std::unordered_map<std::string, Node> graph{
    { "11110", {"11110", {'0', '1', }, {"11100", "11101", }, false, '_'} },
    { "11010", {"11010", {'0', }, {"10100", }, true, '0'} },
    { "11000", {"11000", {'0', }, {"10000", }, false, '_'} },
    { "10110", {"10110", {'0', }, {"01100", }, true, '0'} },
    { "11001", {"11001", {'0', }, {"10010", }, true, '0'} },
    { "10100", {"10100", {'0', '1', }, {"01001", "01000", }, true, '1'} },
    { "10011", {"10011", {'0', '1', }, {"00110", "00111", }, true, '0'} },
    { "11100", {"11100", {'0', '1', }, {"11000", "11001", }, false, '_'} },
    { "00001", {"00001", {'0', '1', }, {"00010", "00011", }, false, '_'} },
    { "01100", {"01100", {'0', '1', }, {"11000", "11001", }, true, '1'} },
    { "01101", {"01101", {'0', }, {"11010", }, true, '0'} },
    { "00101", {"00101", {'1', }, {"01011", }, true, '1'} },
    { "01011", {"01011", {'0', '1', }, {"10110", "10111", }, true, '0'} },
    { "01000", {"01000", {'0', }, {"10000", }, false, '_'} },
    { "01001", {"01001", {'1', }, {"10011", }, true, '1'} },
    { "00110", {"00110", {'1', }, {"01101", }, true, '1'} },
    { "11101", {"11101", {'0', }, {"11010", }, false, '_'} },
    { "00011", {"00011", {'0', '1', }, {"00110", "00111", }, false, '_'} },
    { "10010", {"10010", {'1' }, {"00101" }, true, '1'} },
    { "00010", {"00010", {'1' }, {"00101" }, false, '_'} },
    { "10111", {"10111", {'1' }, {"01111" }, false, '_'} },
    { "00111", {"00111", {'1' }, {"01111" }, false, '_'} },
};

struct TestCase;
bool verify(const std::string& hay, int check_pos);
void next_op(int idx, TestCase* c, std::vector<DfsOp>* stack);


struct TestCase {
    int n = 0;
    bool givens_fail = false;
    std::string input_s;
    std::vector<Wildcard> wildcards;
    TestCase() {  // NOLINT
      std::cin >> n >> input_s;

      // we care if there are multiple wildcards in a row.
      // work backwards in order to get the run-length
      int last_wild = -1;
      int run_length = 1;
      for (int i = static_cast<int>(input_s.length()) - 1; i > -1; i--) {
        if (!verify(input_s, i)) {
          givens_fail = true;
        }
        if (input_s[i] == '?') {
          Wildcard w{i, run_length};
          if (i == last_wild - 1) {
            run_length++;
            w.run_length = run_length;
          } else {
            run_length = 1;
          }
          last_wild = i;
          wildcards.push_back(w);
        }
      }
      // setup for left to right
      std::reverse(wildcards.begin(), wildcards.end());
    }
};


std::string solve(TestCase* c) {
  if (c->givens_fail) {
    // some non-wildcards already fail
    return "IMPOSSIBLE";
  }

  if (c->wildcards.empty()) {
    // givens must not have failed, and we have no wildcards, so all good
    return "POSSIBLE";
  }

  std::vector<DfsOp> stack;
  next_op(0, c, &stack);

  while (!stack.empty()) {
    // get the next operation
    DfsOp curr = stack.back();
    stack.pop_back();
    switch (curr.type) {
      case DfsOpType::Backward:
      {
        c->input_s[curr.string_pos] = '?';
      }
      break;
      case DfsOpType::Forward:
      {
        c->input_s[curr.string_pos] = curr.value;
        if (verify(c->input_s, curr.string_pos)) {
          if (curr.wildcard_pos == c->wildcards.size() - 1) {
            // std::cout << "RESULT:" << c.input_s << std::endl;
            return "POSSIBLE";
          }
          next_op(curr.wildcard_pos + 1, c, &stack);
        }
      }
      break;
      default:
        break;
    }
  }
  return "IMPOSSIBLE";
}

bool verify(const std::string& hay, int check_pos) {
  for (int s = check_pos - 4; s < check_pos + 1; s++) {
    if (s < 0 || s + 4 > hay.size() - 1)
      continue;
    std::string needle = hay.substr(s, 5);
    if (unsafe_5.find(needle) != unsafe_5.end()) {
      return false;
    }
  }
  for (int s = check_pos - 5; s < check_pos + 1; s++) {
    if (s < 0 || s + 5 > hay.size() - 1)
      continue;
    std::string needle = hay.substr(s, 6);
    if (unsafe_6.find(needle) != unsafe_6.end()) {
      return false;
    }
  }
  return true;
}


void next_op(int idx, TestCase* c, std::vector<DfsOp>* stack) {
  if (idx > c->wildcards.size() - 1) {
    return;
  }
  Wildcard w = c->wildcards[idx];
  if (w.str_pos < 5) {
    // either are possible
    stack->push_back({DfsOpType::Backward, idx, w.str_pos, '?'});
    stack->push_back({DfsOpType::Forward, idx, w.str_pos, '0'});
    stack->push_back({DfsOpType::Backward, idx, w.str_pos, '?'});
    stack->push_back({DfsOpType::Forward, idx, w.str_pos, '1'});
    return;
  }
  std::string prefix = c->input_s.substr(w.str_pos - 5, 5);
  auto itr = graph.find(prefix);
  if (itr == graph.end()) {
    // no new viable ops
    return;
  }
  if (w.run_length > 1) {
    // need a cycle...
    if (!itr->second.has_cycle) {
      // no new viable ops
      return;
    }
    // add only the cycle char
    stack->push_back(
        {DfsOpType::Backward, idx, w.str_pos, '?'});
    stack->push_back(
        {DfsOpType::Forward, idx, w.str_pos, itr->second.for_cycle});
    return;
  } else {
    // anything in the possible list will work
    for (const char p : itr->second.possible) {
      stack->push_back({DfsOpType::Backward, idx, w.str_pos, '?'});
      stack->push_back({DfsOpType::Forward, idx, w.str_pos, p});
    }
    return;
  }
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
          TestCase c{};
          auto result = solve(&c);
          std::cout << "Case #" << cid << ": " << result << std::endl;
  }
}
