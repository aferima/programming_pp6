#include "GuessGame.h"
#include "Players.h"
#include <stdexcept>
#include <string>

namespace {

const int MAX_STEPS = MAX_NUMBER - MIN_NUMBER + 1;

const char* reactionText(int reaction) {
    if (reaction < 0) {
        return "замало";
    }
    return reaction > 0 ? "забагато" : "вгадано";
}

int require(const std::optional<int>& value, const char* who) {
    if (!value) {
        throw std::logic_error(std::string(who) + " завершився раніше, ніж число вгадано");
    }
    return *value;
}

void printStep(int step, int guess, int reaction, std::ostream& out) {
    out << "Крок " << step << ": спроба " << guess << ", " << reaction << " (" << reactionText(reaction) << ")\n";
}

void finishPlayers(Player& thinkerPlayer, Player& guesserPlayer) {
    if (guesserPlayer.send(0).has_value() || thinkerPlayer.next().has_value()) {
        throw std::logic_error("сопрограма не завершилась після вгадування");
    }
}

}

GameResult playGame(std::optional<int> fixedSecret, std::ostream& out) {
    Player thinkerPlayer = thinker(fixedSecret);
    Player guesserPlayer = guesser();
    int guess = require(guesserPlayer.next(), "вгадувач");
    for (int step = 1; step <= MAX_STEPS; ++step) {
        const int reaction = require(thinkerPlayer.send(guess), "загадувач");
        printStep(step, guess, reaction, out);
        if (reaction == 0) {
            finishPlayers(thinkerPlayer, guesserPlayer);
            const Winner winner = step % 2 == 0 ? Winner::Thinker : Winner::Guesser;
            return GameResult{guess, step, winner};
        }
        guess = require(guesserPlayer.send(reaction), "вгадувач");
    }
    throw std::logic_error("число не вгадано за " + std::to_string(MAX_STEPS) + " кроків");
}

void printResult(const GameResult& result, std::ostream& out) {
    const char* parity = result.steps % 2 == 0 ? " (парна)" : " (непарна)";
    const char* winner = result.winner == Winner::Thinker ? "загадувач" : "вгадувач";
    out << "Загадане число: " << result.secret << '\n';
    out << "Кількість спроб: " << result.steps << parity << '\n';
    out << "Переможець: " << winner << '\n';
}