#pragma once

#include <optional>
#include <ostream>

enum class Winner { Thinker, Guesser };

struct GameResult {
    int secret = 0;
    int steps = 0;
    Winner winner = Winner::Thinker;
};

GameResult playGame(std::optional<int> fixedSecret, std::ostream& out);
void printResult(const GameResult& result, std::ostream& out);