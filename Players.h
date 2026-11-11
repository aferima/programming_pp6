#pragma once

#include "DuplexCoroutine.h"
#include <optional>

inline constexpr int MIN_NUMBER = 1;
inline constexpr int MAX_NUMBER = 100;

using Player = DuplexCoroutine<int, int>;

bool isInGameRange(int number);
int compareNumbers(int guess, int secret);
Player thinker(std::optional<int> fixedSecret);
Player guesser();