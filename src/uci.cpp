#include "uci.hpp"
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace UCI {

/**
 * Main UCI loop - reads commands from stdin
 */
void run_uci_loop() {
    std::string line;
    board chess_board;
    Evaluator evaluator;
    evaluator.max_depth = 5; // Default search depth

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci") {
            // Identify the engine
            std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION << std::endl;
            std::cout << "id author " << AUTHOR << std::endl;
            std::cout << "uciok" << std::endl;

        } else if (command == "isready") {
            // Respond that engine is ready
            std::cout << "readyok" << std::endl;

        } else if (command == "ucinewgame") {
            // Reset board for new game
            chess_board = board();

        } else if (command == "position") {
            // Set up board position
            handle_position(chess_board, line);

        } else if (command == "go") {
            // Calculate best move
            handle_go(chess_board, line);

        } else if (command == "quit") {
            // Exit
            break;

        } else if (command == "d") {
            // Debug: print board (non-standard but useful)
            chess_board.print_board("");

        } else {
            // Unknown command - ignore (UCI spec says to ignore)
        }
    }
}

/**
 * Parse FEN string and set up the board
 */
void parse_fen(board& chess_board, const std::string& fen) {
    // FEN format: <pieces> <turn> <castling> <en_passant> <halfmove> <fullmove>
    // Example: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    
    std::istringstream iss(fen);
    std::string pieces, turn, castling, enpassant;
    int halfmove, fullmove;
    
    iss >> pieces >> turn >> castling >> enpassant >> halfmove >> fullmove;

    // Reset board
    chess_board = board();

    // Parse piece placement (ranks 8 to 1)
    int square_idx = 56; // Start at a8 (rank 8, file a)
    for (char c : pieces) {
        if (c == '/') {
            // Move to next rank (down one row)
            square_idx -= 16; // Go back 8 (file) + skip 8 (next rank)
        } else if (c >= '1' && c <= '8') {
            // Empty squares
            square_idx += (c - '0');
        } else {
            // Piece
            PieceType piece;
            switch (c) {
                case 'P': piece = P; break;
                case 'R': piece = R; break;
                case 'N': piece = N; break;
                case 'B': piece = B; break;
                case 'Q': piece = Q; break;
                case 'K': piece = K; break;
                case 'p': piece = p; break;
                case 'r': piece = r; break;
                case 'n': piece = n; break;
                case 'b': piece = b; break;
                case 'q': piece = q; break;
                case 'k': piece = k; break;
                default: piece = e; break;
            }
            
            if (piece != e) {
                chess_board.chessboard[square_idx] = piece;
                chess_board.bitboards[piece] |= (1ULL << square_idx);
            }
            square_idx++;
        }
    }

    // Set turn
    chess_board.boardTurn = (turn == "w") ? White : Black;

    // Parse castling rights - map to your existing structure
    // Note: Your board uses King_moved, Rook_KingSide_moved, etc.
    // For now, we'll just track if castling is generally possible
    // TODO: Properly map UCI castling rights to your board's castling flags

    // TODO: Handle en passant square if needed
}

/**
 * Handle "position" command
 */
void handle_position(board& b, const std::string& command) {
    std::istringstream iss(command);
    std::string token;
    
    iss >> token; // Skip "position"
    iss >> token; // Get "startpos" or "fen"

    if (token == "startpos") {
        // Start position FEN
        parse_fen(b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else if (token == "fen") {
        // Read FEN string (rest of line until "moves")
        std::string fen;
        while (iss >> token && token != "moves") {
            if (!fen.empty()) fen += " ";
            fen += token;
        }
        parse_fen(b, fen);
    }

    // Apply moves if present
    if (token == "moves" || (iss >> token && token == "moves")) {
        while (iss >> token) {
            Move move = uci_to_move(token, b);
            if (move.src_pos != 0 && move.dst_pos != 0) {
                b.apply_move(move);
            }
        }
    }
}

/**
 * Handle "go" command - calculate and output best move
 */
void handle_go(board& b, const std::string& command) {
    std::istringstream iss(command);
    std::string token;
    
    iss >> token; // Skip "go"
    
    // Parse go parameters
    int depth = 5; // Default depth
    while (iss >> token) {
        if (token == "depth") {
            iss >> depth;
        } else if (token == "movetime") {
            // TODO: Implement time-based search
            int movetime;
            iss >> movetime;
        } else if (token == "infinite") {
            depth = 10; // Search deeper for infinite
        }
    }

    // Calculate best move
    Evaluator evaluator;
    evaluator.max_depth = depth;
    Move bestMove = evaluator.get_best_move(b);

    // Output best move in UCI format
    std::string uci_move = move_to_uci(bestMove, b);
    std::cout << "bestmove " << uci_move << std::endl;
}

/**
 * Convert internal Move to UCI format
 */
std::string move_to_uci(const Move& move, const board& b) {
    std::string from = square_to_algebraic(move.src_pos);
    std::string to = square_to_algebraic(move.dst_pos);
    
    // Handle promotion
    std::string promotion_str = "";
    if (move.promotion != '\0') {
        promotion_str = std::string(1, std::tolower(move.promotion));
    }
    
    return from + to + promotion_str;
}

/**
 * Convert UCI move to internal Move
 */
Move uci_to_move(const std::string& uci_move, const board& b) {
    if (uci_move.length() < 4) {
        return Move(0, 0, '\0'); // Invalid move
    }

    std::string from_str = uci_move.substr(0, 2);
    std::string to_str = uci_move.substr(2, 2);
    
    Bitboard from = algebraic_to_square(from_str);
    Bitboard to = algebraic_to_square(to_str);
    
    char promotion = '\0';
    if (uci_move.length() >= 5) {
        promotion = std::toupper(uci_move[4]);
    }
    
    return Move(from, to, promotion);
}

/**
 * Convert square bitboard to algebraic notation
 */
std::string square_to_algebraic(Bitboard square) {
    if (square == 0) return "none";
    
    // Find the bit position (0-63)
    int pos = 0;
    Bitboard temp = square;
    while (temp > 1) {
        temp >>= 1;
        pos++;
    }
    
    int file = pos % 8;
    int rank = pos / 8;
    
    char file_char = 'a' + file;
    char rank_char = '1' + rank;
    
    return std::string(1, file_char) + std::string(1, rank_char);
}

/**
 * Convert algebraic notation to square bitboard
 */
Bitboard algebraic_to_square(const std::string& algebraic) {
    if (algebraic.length() < 2) return 0;
    
    int file = algebraic[0] - 'a'; // 0-7
    int rank = algebraic[1] - '1'; // 0-7
    
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return 0; // Invalid
    }
    
    int square_idx = rank * 8 + file;
    return 1ULL << square_idx;
}

} // namespace UCI
