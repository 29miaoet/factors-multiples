#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

class FactorsGame {
public:
    int turn; 
    int last_move;

private:
    uint64_t board_low;  // Bitboard for numbers 1 to 63
    uint64_t board_high; // Bitboard for numbers 64 to 100
    
    // Hash key for the Transposition Table (Memoization)
    struct TTKey {
        uint64_t low;
        uint64_t high;
        int last;
        bool operator==(const TTKey& o) const {
            return low == o.low && high == o.high && last == o.last;
        }
    };

    struct TTKeyHash {
        std::size_t operator()(const TTKey& k) const {
            std::size_t h = 14695981039346656037ULL; // FNV-1a offset
            h ^= k.low; h *= 1099511628211ULL;
            h ^= k.high; h *= 1099511628211ULL;
            h ^= k.last; h *= 1099511628211ULL;
            return h;
        }
    };

    // Maps game states to terminal values: 1 (Win) or -1 (Loss)
    std::unordered_map<TTKey, int, TTKeyHash> memo;

public:
    FactorsGame() {
        board_low = 0xFFFFFFFFFFFFFFFEULL; // bits 1-63 set to 1 (0 is unused)
        board_high = 0x1FFFFFFFFFULL;      // bits 0-36 set to 1 (represents 64-100)
        turn = 0;
        last_move = 0; // 0 indicates no move has been made yet
    }

    // Returns true and crosses out the number if valid, false if invalid
    bool make_move(int n) {
        if (!is_valid_move_internal(board_low, board_high, last_move, n)) {
            return false; 
        }
        remove_number(board_low, board_high, n);
        last_move = n;
        turn = 1 - turn; // Oscillates between 0 and 1
        return true;
    }

    // Overwrites the internal board state based on an external vector
    void set_board_position(const std::vector<int>& available_numbers, int last_move_made) {
        board_low = 0;
        board_high = 0;
        for (int n : available_numbers) {
            if (n >= 1 && n <= 100) {
                if (n < 64) board_low |= (1ULL << n);
                else board_high |= (1ULL << (n - 64));
            }
        }
        last_move = last_move_made;
    }

    // Exact perfect-play solver using Negamax, Alpha-Beta, and Move Ordering
    int best_move() {
        auto moves = get_valid_moves(board_low, board_high, last_move);
        if (moves.empty()) return -1; 
        
        int best_score = -10000;
        int best_m = -1;
        
        // Move ordering: try moves that give the opponent the fewest responses first
        std::vector<std::pair<int, int>> ordered_moves;
        for(int m : moves) {
            uint64_t next_low = board_low, next_high = board_high;
            remove_number(next_low, next_high, m);
            int responses = get_valid_moves(next_low, next_high, m).size();
            ordered_moves.push_back({responses, m});
        }
        std::sort(ordered_moves.begin(), ordered_moves.end());
        
        for (auto pair : ordered_moves) {
            int m = pair.second;
            uint64_t next_low = board_low, next_high = board_high;
            remove_number(next_low, next_high, m);
            
            // Search down the game tree
            int score = -negamax(next_low, next_high, m, -1, 1);
            
            if (score > best_score) {
                best_score = score;
                best_m = m;
            }
            // If a forced win is found (score == 1), break immediately
            if (best_score == 1) break; 
        }
        
        return best_m;
    }

private:
    inline void remove_number(uint64_t& low, uint64_t& high, int n) const {
        if (n < 64) low &= ~(1ULL << n);
        else high &= ~(1ULL << (n - 64));
    }

    bool is_valid_move_internal(uint64_t low, uint64_t high, int last_m, int n) const {
        if (n < 1 || n > 100) return false;
        
        bool available = (n < 64) ? (low & (1ULL << n)) : (high & (1ULL << (n - 64)));
        if (!available) return false;
        
        if (last_m == 0) {
            return (n % 2 == 0) && (n < 50);
        }
        
        return (n % last_m == 0) || (last_m % n == 0);
    }

    std::vector<int> get_valid_moves(uint64_t low, uint64_t high, int last_m) const {
        std::vector<int> moves;
        moves.reserve(16); // Prevents vector reallocation overhead

        // Initial move rule
        if (last_m == 0) {
            for (int i = 2; i < 50; i += 2) {
                if (low & (1ULL << i)) moves.push_back(i);
            }
            return moves;
        }
        
        // Find available multiples
        for (int i = last_m * 2; i <= 100; i += last_m) {
            if (i < 64) {
                if (low & (1ULL << i)) moves.push_back(i);
            } else {
                if (high & (1ULL << (i - 64))) moves.push_back(i);
            }
        }
        
        // Find available factors
        for (int i = 1; i <= last_m / 2; ++i) {
            if (last_m % i == 0) {
                // strict factors of <=100 are always < 64, so we only check 'low'
                if (i < 64 && (low & (1ULL << i))) {
                    moves.push_back(i);
                }
            }
        }
        return moves;
    }

    int negamax(uint64_t low, uint64_t high, int last_m, int alpha, int beta) {
        TTKey key = {low, high, last_m};
        
        // Transposition Table lookup
        auto it = memo.find(key);
        if (it != memo.end()) return it->second;

        auto moves = get_valid_moves(low, high, last_m);
        if (moves.empty()) return -1; // Loss state for current active player

        std::vector<std::pair<int, int>> ordered_moves;
        ordered_moves.reserve(moves.size());
        
        for(int m : moves) {
            uint64_t next_low = low, next_high = high;
            remove_number(next_low, next_high, m);
            int responses = get_valid_moves(next_low, next_high, m).size();
            ordered_moves.push_back({responses, m});
        }
        std::sort(ordered_moves.begin(), ordered_moves.end());

        for (auto pair : ordered_moves) {
            int m = pair.second;
            uint64_t next_low = low, next_high = high;
            remove_number(next_low, next_high, m);

            int score = -negamax(next_low, next_high, m, -beta, -alpha);
            
            if (score >= beta) {
                memo[key] = score;
                return score;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
        
        memo[key] = alpha;
        return alpha;
    }
};
