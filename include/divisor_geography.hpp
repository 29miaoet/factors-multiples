#pragma once

#include <array>
#include <vector>

class DivisorGeography {
public:
    static constexpr int MIN_NUMBER = 1;
    static constexpr int MAX_NUMBER = 100;

    // 0 = first player, 1 = second player.
    int turn = 0;

    DivisorGeography() {
        available_.fill(true);
        available_[0] = false;
        build_graph();
    }

    /*
     * Attempts to make a move.
     *
     * Returns:
     *   true  - move was legal and the position was updated
     *   false - move was illegal and the position was unchanged
     */
    bool make_move(int move) {
        if (!is_valid_number(move) || !available_[move]) {
            return false;
        }

        // Special rule for the very first move.
        if (turn == 0 && last_move_ == -1) {
            if (move % 2 != 0 || move >= 50) {
                return false;
            }
        } else {
            // Every subsequent move must be a factor or multiple
            // of the previous move.
            if (last_move_ == -1 || !is_adjacent(last_move_, move)) {
                return false;
            }
        }

        available_[move] = false;
        last_move_ = move;
        turn ^= 1;

        return true;
    }

    /*
     * Finds a winning move if one exists.
     *
     * If the position is losing, but legal moves remain, returns
     * the first legal move in numerical order.
     *
     * Returns -1 only when there are no legal moves.
     */
    int best_move() const {
        /*
         * Initial position.
         *
         * Choosing v makes v the starting vertex of the opponent's
         * vertex-geography position on the original graph.
         *
         * v is a winning opening move iff
         *
         *     nu(G - v) == nu(G)
         *
         * because that means v can be left unmatched in a maximum
         * matching, making the opponent's position losing.
         */
        if (turn == 0 && last_move_ == -1) {
            std::array<bool, MAX_NUMBER + 1> active{};

            for (int v = MIN_NUMBER; v <= MAX_NUMBER; ++v) {
                active[v] = available_[v];
            }

            const int base_matching = maximum_matching_size(active);

            for (int move = 2; move < 50; move += 2) {
                if (!available_[move]) {
                    continue;
                }

                active[move] = false;

                const int after_move_matching =
                    maximum_matching_size(active);

                active[move] = true;

                if (after_move_matching == base_matching) {
                    return move;
                }
            }

            // No forced win exists. Return any legal opening move.
            for (int move = 2; move < 50; move += 2) {
                if (available_[move]) {
                    return move;
                }
            }

            return -1;
        }

        if (last_move_ == -1) {
            return -1;
        }

        /*
         * The graph relevant to the current player consists of the
         * currently available vertices.
         */
        std::array<bool, MAX_NUMBER + 1> active{};
        bool has_legal_move = false;

        for (int v = MIN_NUMBER; v <= MAX_NUMBER; ++v) {
            active[v] = available_[v];

            if (available_[v] && is_adjacent(last_move_, v)) {
                has_legal_move = true;
            }
        }

        if (!has_legal_move) {
            return -1;
        }

        const int base_matching = maximum_matching_size(active);

        /*
         * Suppose we move to v.
         *
         * The opponent's current vertex is now v, and the remaining
         * graph is exactly active[].
         *
         * Therefore v is a losing starting vertex for the opponent iff
         *
         *     nu(G - v) == nu(G).
         */
        for (int move = MIN_NUMBER; move <= MAX_NUMBER; ++move) {
            if (!active[move] || !is_adjacent(last_move_, move)) {
                continue;
            }

            active[move] = false;

            const int after_move_matching =
                maximum_matching_size(active);

            active[move] = true;

            if (after_move_matching == base_matching) {
                return move;
            }
        }

        /*
         * This position is losing: every legal move gives the opponent
         * a winning position. There is no mathematically distinguished
         * "best" move, so return a deterministic legal move.
         */
        for (int move = MIN_NUMBER; move <= MAX_NUMBER; ++move) {
            if (available_[move] && is_adjacent(last_move_, move)) {
                return move;
            }
        }

        return -1;
    }

    /*
     * Sets the available board positions and the previous move.
     *
     * availableNumbers contains numbers which have NOT been crossed out.
     *
     * lastMove:
     *   -1 = no previous move
     *   otherwise = the most recently crossed-out number
     *
     * The turn variable is intentionally not modified.
     * This makes the method convenient for reconstructing arbitrary
     * positions from an external program.
     *
     * Returns false if the supplied position is invalid.
     */
    bool set_board_position(
        const std::vector<int>& availableNumbers,
        int lastMove
    ) {
        std::array<bool, MAX_NUMBER + 1> next{};

        for (int value : availableNumbers) {
            if (!is_valid_number(value) || next[value]) {
                return false;
            }

            next[value] = true;
        }

        if (lastMove != -1 && !is_valid_number(lastMove)) {
            return false;
        }

        // The last move must already be crossed out.
        if (lastMove != -1 && next[lastMove]) {
            return false;
        }

        available_ = next;
        last_move_ = lastMove;

        return true;
    }

    int last_move() const {
        return last_move_;
    }

    std::vector<int> available_numbers() const {
        std::vector<int> result;

        for (int value = MIN_NUMBER; value <= MAX_NUMBER; ++value) {
            if (available_[value]) {
                result.push_back(value);
            }
        }

        return result;
    }

private:
    std::array<bool, MAX_NUMBER + 1> available_{};
    std::array<std::vector<int>, MAX_NUMBER + 1> adjacency_{};

    int last_move_ = -1;

    bool is_valid_number(int value) const {
        return value >= MIN_NUMBER && value <= MAX_NUMBER;
    }

    bool is_adjacent(int a, int b) const {
        return a != b && ((a % b == 0) || (b % a == 0));
    }

    void build_graph() {
        for (int a = MIN_NUMBER; a <= MAX_NUMBER; ++a) {
            for (int b = a + 1; b <= MAX_NUMBER; ++b) {
                if (b % a == 0) {
                    adjacency_[a].push_back(b);
                    adjacency_[b].push_back(a);
                }
            }
        }
    }

    /*
     * Edmonds' blossom algorithm.
     *
     * Returns the cardinality of a maximum matching in the graph
     * induced by active[].
     */
    int maximum_matching_size(
        const std::array<bool, MAX_NUMBER + 1>& active
    ) const {
        std::array<int, MAX_NUMBER + 1> match{};
        std::array<int, MAX_NUMBER + 1> parent{};
        std::array<int, MAX_NUMBER + 1> base{};
        std::array<bool, MAX_NUMBER + 1> used{};
        std::array<bool, MAX_NUMBER + 1> blossom{};
        std::array<int, MAX_NUMBER + 1> queue{};

        match.fill(-1);
        parent.fill(-1);
        base.fill(0);

        auto lca = [&](int a, int b) {
            std::array<bool, MAX_NUMBER + 1> seen{};

            while (true) {
                a = base[a];
                seen[a] = true;

                if (match[a] == -1) {
                    break;
                }

                a = parent[match[a]];
            }

            while (true) {
                b = base[b];

                if (seen[b]) {
                    return b;
                }

                b = parent[match[b]];
            }
        };

        auto mark_path = [&](int v, int b, int child) {
            while (base[v] != b) {
                blossom[base[v]] = true;
                blossom[base[match[v]]] = true;

                parent[v] = child;
                child = match[v];
                v = parent[match[v]];
            }
        };

        auto find_path = [&](int root) {
            used.fill(false);
            parent.fill(-1);

            for (int v = MIN_NUMBER; v <= MAX_NUMBER; ++v) {
                base[v] = v;
            }

            int head = 0;
            int tail = 0;

            queue[tail++] = root;
            used[root] = true;

            while (head < tail) {
                const int v = queue[head++];

                for (int u : adjacency_[v]) {
                    if (!active[u]) {
                        continue;
                    }

                    if (base[v] == base[u] || match[v] == u) {
                        continue;
                    }

                    if (
                        u == root ||
                        (match[u] != -1 && parent[match[u]] != -1)
                    ) {
                        const int current_base = lca(v, u);

                        blossom.fill(false);

                        mark_path(v, current_base, u);
                        mark_path(u, current_base, v);

                        for (
                            int i = MIN_NUMBER;
                            i <= MAX_NUMBER;
                            ++i
                        ) {
                            if (!blossom[base[i]]) {
                                continue;
                            }

                            base[i] = current_base;

                            if (!used[i]) {
                                used[i] = true;
                                queue[tail++] = i;
                            }
                        }
                    } else if (parent[u] == -1) {
                        parent[u] = v;

                        if (match[u] == -1) {
                            int current = u;

                            while (current != -1) {
                                const int previous = parent[current];

                                const int next =
                                    previous == -1
                                        ? -1
                                        : match[previous];

                                match[current] = previous;

                                if (previous != -1) {
                                    match[previous] = current;
                                }

                                current = next;
                            }

                            return true;
                        }

                        u = match[u];
                        used[u] = true;
                        queue[tail++] = u;
                    }
                }
            }

            return false;
        };

        int matching_size = 0;

        for (int v = MIN_NUMBER; v <= MAX_NUMBER; ++v) {
            if (
                active[v] &&
                match[v] == -1 &&
                find_path(v)
            ) {
                ++matching_size;
            }
        }

        return matching_size;
    }
};
