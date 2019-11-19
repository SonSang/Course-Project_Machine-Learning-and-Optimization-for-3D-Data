#include "random.hpp"
#include <random>

namespace AF {
    double random::double_(double beg, double end) {
        std::random_device rd;
		std::mt19937 mersenne(rd());
		std::uniform_real_distribution<> die(beg, end);
		return die(mersenne);
    }
    int random::int_(double beg, double end) {
        std::random_device rd;
		std::mt19937 mersenne(rd());
		std::uniform_int_distribution<> die((int)beg, (int)end);
		return die(mersenne);
    }
}