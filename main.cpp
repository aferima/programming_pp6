#include "GuessGame.h"
#include "Players.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {

const std::string DEFAULT_FILE = "games.txt";
const std::string RANDOM_WORD = "random";
const std::string UTF8_BOM = "\xEF\xBB\xBF";

struct GameInput {
    int lineNumber = 0;
    std::optional<int> secret;
};

std::optional<int> parseSecret(const std::string& word) {
    if (word == RANDOM_WORD) {
        return std::nullopt;
    }
    for (char symbol : word) {
        if (symbol < '0' || symbol > '9') {
            throw std::invalid_argument("'" + word + "': очікувалось ціле число або random");
        }
    }
    if (word.size() > 3) {
        throw std::invalid_argument("'" + word + "': число має містити не більше трьох цифр");
    }
    const int number = std::stoi(word);
    if (!isInGameRange(number)) {
        throw std::invalid_argument("число " + word + " поза межами [1, 100]");
    }
    return number;
}

bool parseLine(const std::string& line, GameInput& game) {
    std::istringstream words(line.substr(0, line.find('#')));
    std::string word;
    if (!(words >> word)) {
        return false;
    }
    game.secret = parseSecret(word);
    std::string extra;
    if (words >> extra) {
        throw std::invalid_argument("зайві дані '" + extra + "'");
    }
    return true;
}

std::vector<GameInput> readGames(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("не вдалося відкрити файл '" + path + "'");
    }
    std::vector<GameInput> games;
    std::string line;
    for (int lineNumber = 1; std::getline(file, line); ++lineNumber) {
        if (lineNumber == 1 && line.rfind(UTF8_BOM, 0) == 0) {
            line.erase(0, UTF8_BOM.size());
        }
        GameInput game;
        game.lineNumber = lineNumber;
        try {
            if (parseLine(line, game)) {
                games.push_back(game);
            }
        } catch (const std::invalid_argument& error) {
            std::cerr << "Рядок " << lineNumber << " пропущено: " << error.what() << '\n';
        }
    }
    if (file.bad()) {
        throw std::runtime_error("помилка читання файлу '" + path + "'");
    }
    return games;
}

void printSummary(size_t games, int thinkerWins) {
    std::cout << "\nПІДСУМОК\n";
    std::cout << "Зіграно ігор: " << games << '\n';
    std::cout << "Перемог загадувача: " << thinkerWins << '\n';
    std::cout << "Перемог вгадувача: " << games - thinkerWins << '\n';
}

void runGames(const std::vector<GameInput>& games) {
    int thinkerWins = 0;
    for (size_t i = 0; i < games.size(); ++i) {
        const char* source = games[i].secret ? "число задано у файлі" : "випадкове число";
        std::cout << "\nГРА №" << i + 1 << " (рядок " << games[i].lineNumber << "): " << source << '\n';
        const GameResult result = playGame(games[i].secret, std::cout);
        printResult(result, std::cout);
        if (result.winner == Winner::Thinker) {
            ++thinkerWins;
        }
    }
    printSummary(games.size(), thinkerWins);
}

}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    if (argc > 2) {
        std::cerr << "Використання: " << argv[0] << " [файл]\n";
        return 1;
    }
    const std::string path = argc == 2 ? argv[1] : DEFAULT_FILE;
    try {
        std::cout << "Файл: " << path << '\n';
        const std::vector<GameInput> games = readGames(path);
        if (games.empty()) {
            std::cerr << "Помилка: у файлі немає жодної коректної гри.\n";
            return 1;
        }
        runGames(games);
    } catch (const std::exception& error) {
        std::cerr << "Помилка: " << error.what() << '\n';
        return 1;
    }
    return 0;
}