#pragma once

#include "../shared/public/system.h"

#define INITIAL_SEED 1337

namespace ink::runtime::internal {
	/**
	 * @brief pseudo random number generator based on Linear Congruential Generator.
	 */
	class prng {
		static constexpr uint32_t C = 12345;
		static constexpr uint32_t A = 1103515245;
		static constexpr uint32_t M = 1<<31;
	public:
		void srand(int32_t seed) {
			_seed = seed;
			_x = seed;
		}
		uint32_t rand() {
			_x = (A*_x+ C) % M;
			return _x;
		}
		int32_t rand(int32_t max) {
			uint64_t prod = rand();
			prod *= max;
			return static_cast<int32_t>(prod / M);
		}
		uint32_t seed() {
			return _seed;
		}
	private:
		uint32_t _x = INITIAL_SEED;
		uint32_t _seed = INITIAL_SEED;
	};
}
