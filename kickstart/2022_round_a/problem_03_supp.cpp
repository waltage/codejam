/*
* Copyright 2022 Daniel Walt
*/

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>

#define LIMIT_6BIT 64

typedef std::unordered_set<char> charset;
typedef std::unordered_set<std::string> strset;

struct Node {
    std::string key;
    charset possible;
    strset children;
    bool has_cycle = false;
    char for_cycle = '_';
    [[nodiscard]] std::string repr() const;
};

strset unsafe_5;
strset unsafe_6;
std::unordered_map<std::string, Node> graph;


// dealing in integer space, check if a bitset rep is a palindrome
void check_palindrome(int i);
// build a set of length 5, and a set of length 6 palindromes
void build_palindromes(bool);
// for dfs, simplify the search space by finding possible next char
void build_graph();
// for instances with wildcard run-length > 1, we can limit to only cycles
void find_cycles();
// utility for printing the graph as a static const in the real problem
void print_graph();


int main() {
  build_palindromes(true);
  build_graph();
  find_cycles();
  print_graph();
  return 0;
}


void build_palindromes(bool enable_print) {
  for (int i = 0; i < LIMIT_6BIT; i++) {
    check_palindrome(i);
  }
  if (enable_print) {
    std::cout << "static const strset unsafe_5{";
    int cnt = 0;
    for (const auto &p : unsafe_5) {
      cnt++;
      std::cout << "{\"" << p << "\"}";
      if (cnt != unsafe_5.size()) {
        std::cout << ", ";
      }
    }
    std::cout << "};" << std::endl;

    std::cout << "static const strset unsafe_6{";
    cnt = 0;
    for (const auto &p : unsafe_6) {
      cnt++;
      std::cout << "{\"" << p << "\"}";
      if (cnt != unsafe_5.size()) {
        std::cout << ", ";
      }
    }
    std::cout << "};" << std::endl;
  }
}


void build_graph() {
  for (int i = 0; i < LIMIT_6BIT; i++) {
    std::string s = std::bitset<5>(i).to_string();
    if (unsafe_5.find(s) != unsafe_5.end()) {
      continue;
    }
    Node n;
    s += '_';
    std::string prefix = s.substr(0, 5);
    std::string suffix;
    for (char c : {'0', '1'}) {
      s[5] = c;
      suffix = s.substr(1, 5);
      if (unsafe_6.find(s) == unsafe_6.end() &&
          unsafe_5.find(suffix) == unsafe_5.end()) {
        n.possible.insert(c);
        n.children.insert(suffix);
      }
    }
    if (!n.possible.empty()) {
      n.key = prefix;
      graph[prefix] = n;
    }
  }
}


void find_cycles() {
  strset has_cycle;
  for (const auto& starts : graph) {
    std::vector<std::string> queue{starts.first};
    while (!queue.empty()) {
      std::string curr = queue.back();
      queue.pop_back();
      if (curr.size() > 100 + 5) {
        continue;
      }
      auto nxt = graph.find(curr.substr(curr.size()-5, 5));
      if (nxt == graph.end()) {
        continue;
      }
      for (const auto& child : nxt->second.children) {
        if (child == starts.first && curr.size() > 5) {
          has_cycle.insert(starts.first);
        }
        std::string app = curr + child[4];
        queue.push_back(app);
      }
    }
  }

  for (auto &e : graph) {
    if (has_cycle.find(e.first) == has_cycle.end()) {
      continue;
    }

    e.second.has_cycle = true;
    for (const auto &c : e.second.children) {
      if (has_cycle.find(c) == has_cycle.end()) {
        continue;
      }
      if (e.second.for_cycle == '_') {
        e.second.for_cycle = c[4];
      }
    }
  }
}


void check_palindrome(int i) {
  std::string suffix = std::bitset<6>(i).to_string();
  int l = 1;
  int r = static_cast<int>(suffix.size() - 1);
  bool is_pdrome = true;
  while (l < r) {
    if (suffix[l] != suffix[r]) {
      is_pdrome = false;
      break;
    }
    l++;
    r--;
  }
  if (is_pdrome) {
    unsafe_5.insert(suffix.substr(1, 5));
  }

  l = 0;
  r = static_cast<int>(suffix.size() - 1);
  is_pdrome = true;
  while (l < r) {
    if (suffix[l] != suffix[r]) {
      is_pdrome = false;
      break;
    }
    l++;
    r--;
  }
  if (is_pdrome) {
    unsafe_6.insert(suffix);
  }
}

[[nodiscard]] std::string Node::repr() const {
  std::stringstream ss;
  ss << "{";
  ss << "\"" << key << "\", ";

  ss << "{";
  for (const auto& c : possible) {
    ss << "'" << c << "', ";
  }
  ss << "}, ";

  ss << "{";
  for (const auto& c : children) {
    ss << "\"" << c << "\", ";
  }
  ss << "}, ";

  ss << (has_cycle ? "true" : "false") << ", ";
  ss << "'" << for_cycle << "'";
  ss << "}";

  return ss.str();
}

void print_graph() {
  std::cout << "static const std::unordered_map<std::string, Node> ";
  std::cout << "graph = {" << std::endl;
  for (const auto &c : graph) {
    std::cout << "{ \"" << c.first << "\", ";
    std::cout << c.second.repr() << " }, "<< std::endl;
  }
  std::cout << "};";
}
