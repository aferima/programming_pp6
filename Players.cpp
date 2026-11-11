#include "Players.h"
#include <random>
#include <stdexcept>

bool isInGameRange(int number) {
    return number >= MIN_NUMBER && number <= MAX_NUMBER;
}

int compareNumbers(int guess, int secret) {
    if (guess < secret) {
        return -1;
    }
    return guess > secret ? 1 : 0;
}

namespace {

int randomNumber() {
    static std::mt19937 rng(std::random_device{}());
    return std::uniform_int_distribution<int>(MIN_NUMBER, MAX_NUMBER)(rng);
}

}

Player thinker(std::optional<int> fixedSecret) {
    const int secret = fixedSecret ? *fixedSecret : randomNumber();
    if (!isInGameRange(secret)) {
        throw std::invalid_argument("загадане число поза межами гри");
    }
    int guess = co_await Receive{};
    for (;;) {
        if (!isInGameRange(guess)) {
            throw std::invalid_argument("спроба поза межами гри");
        }
        const int reaction = compareNumbers(guess, secret);
        guess = co_yield reaction;
        if (reaction == 0) {
            co_return;
        }
    }
}

Player guesser() {
    int low = MIN_NUMBER;
    int high = MAX_NUMBER;
    for (int reaction = 1; reaction != 0;) {
        if (low > high) {
            throw std::logic_error("відповіді загадувача суперечать одна одній");
        }
        const int guess = (low + high) / 2;
        reaction = co_yield guess;
        if (reaction != -1 && reaction != 0 && reaction != 1) {
            throw std::invalid_argument("реакція має бути -1, 0 або 1");
        }
        if (reaction == -1) {
            low = guess + 1;
        } else if (reaction == 1) {
            high = guess - 1;
        }
    }
}