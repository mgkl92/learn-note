#include <cstdlib>
#include <ctime>
#include <initializer_list>
#include <iostream>

unsigned bounded_rand(unsigned range) {
  for (unsigned x, r;;) {
    if (x = rand(); r = x % range, x - r <= -range) {
      return r;
    }
  }
}

int main() {
  std::srand(std::time({}));
  const int random_value = std::rand();
  std::cout << "Random value on [0, " << RAND_MAX << "]: " << random_value
            << std::endl;

  for (const unsigned sides : {2, 4, 6, 8}) {
	// 不同面数骰子掷 8 次
    std::cout << "Roll " << sides << "-sided die 8 times: ";
    for (int n = 8; n; --n) {
      std::cout << 1 + bounded_rand(sides) << ' ';
    }
	std::cout << std::endl;
  }

  return 0;
}