#include <vector>

#include "divisor_geography.hpp"

extern "C" {

void* game_create() {
    return new DivisorGeography();
}

void game_destroy(void* handle) {
    delete static_cast<DivisorGeography*>(handle);
}

int game_make_move(void* handle, int move) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr) {
        return 0;
    }

    return game->make_move(move) ? 1 : 0;
}

int game_best_move(void* handle) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr) {
        return -1;
    }

    return game->best_move();
}

int game_turn(void* handle) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr) {
        return -1;
    }

    return game->turn;
}

int game_last_move(void* handle) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr) {
        return -1;
    }

    return game->last_move();
}

int game_is_available(void* handle, int number) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr) {
        return 0;
    }

    const std::vector<int> available = game->available_numbers();

    for (int value : available) {
        if (value == number) {
            return 1;
        }
    }

    return 0;
}

void game_set_turn(void* handle, int turn) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr) {
        return;
    }

    game->turn = turn;
}

int game_set_board_position(
    void* handle,
    const int* available,
    int count,
    int last_move
) {
    auto* game = static_cast<DivisorGeography*>(handle);

    if (game == nullptr || available == nullptr || count < 0) {
        return 0;
    }

    std::vector<int> numbers(
        available,
        available + count
    );

    return game->set_board_position(
        numbers,
        last_move
    ) ? 1 : 0;
}

}
