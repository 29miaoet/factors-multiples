#include <iostream>

#include "divisor_geography.hpp"

int main() {
    DivisorGeography game;

    std::cout << "Divisor Geography\n";
    std::cout << "You are player 0; the computer is player 1.\n\n";

    while (true) {
        if (game.turn == 0) {
            // best_move() returning -1 is also an efficient way to check
            // whether any legal move exists.
            if (game.best_move() == -1) {
                std::cout << "You have no legal moves.\n";
                std::cout << "Computer wins.\n";
                break;
            }

            int move;

            if (game.last_move() == -1) {
                std::cout
                    << "Choose an even number from 2 to 48: ";
            } else {
                std::cout
                    << "Choose a factor or multiple of "
                    << game.last_move() << ": ";
            }

            while (true) {
                if (!(std::cin >> move)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Enter an integer: ";
                    continue;
                }

                if (game.make_move(move)) {
                    break;
                }

                std::cout << "Illegal move. Try again: ";
            }
        } else {
            const int move = game.best_move();

            if (move == -1) {
                std::cout << "Computer has no legal moves.\n";
                std::cout << "You win.\n";
                break;
            }

            std::cout << "Computer chooses: " << move << '\n';

            // This should always succeed because best_move() returns
            // a legal move.
            if (!game.make_move(move)) {
                std::cerr << "Internal error: computer made an illegal move.\n";
                return 1;
            }
        }
    }

    return 0;
}
