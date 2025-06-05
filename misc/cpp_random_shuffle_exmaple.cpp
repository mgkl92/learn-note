#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

int main() {
  std::vector<int> ivec(10);
  std::iota(ivec.begin(), ivec.end(), 0);

  std::cout << "Origin: ";
  std::copy(ivec.begin(), ivec.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(ivec.begin(), ivec.end(), g);

  std::cout << "Shuffle: ";
  std::copy(ivec.begin(), ivec.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';

  return 0;
}