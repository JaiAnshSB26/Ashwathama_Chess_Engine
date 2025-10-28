#ifndef UCI_HPP
#define UCI_HPP

#include <string>
#include <sstream>
#include <iostream>
#include "board.hpp"
#include "evaluate.hpp"

/**
 * UCI (Universal Chess Interface) Protocol Handler
 * 
 * This implements the UCI protocol to allow the engine to communicate
 * with chess GUIs (like CuteChess, Arena) and web APIs.
 * 
 * Key UCI Commands:
 * - uci: Identify the engine
 * - isready: Check if engine is ready
 * - position: Set up the board position
 * - go: Start calculating the best move
 * - quit: Exit the engine
 */

namespace UCI {
    // Engine information
    const std::string ENGINE_NAME = "Ashwathama";
    const std::string ENGINE_VERSION = "1.0";
    const std::string AUTHOR = "Jai Ansh Bindra, Kingshuk Gupta, Mohamed Iyad Boualem, Ziji Wang, Tomas Gradowski";

    /**
     * Main UCI loop - reads commands from stdin and responds
     */
    void run_uci_loop();

    /**
     * Parse FEN string and set up the board
     * FEN = Forsyth-Edwards Notation (standard chess position format)
     * Example: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
     */
    void parse_fen(board& b, const std::string& fen);

    /**
     * Parse "position" UCI command
     * Examples:
     *   position startpos
     *   position startpos moves e2e4 e7e5
     *   position fen <fen_string> moves <moves>
     */
    void handle_position(board& b, const std::string& command);

    /**
     * Parse "go" UCI command and calculate best move
     * Examples:
     *   go depth 5
     *   go movetime 1000
     *   go infinite
     */
    void handle_go(board& b, const std::string& command);

    /**
     * Convert internal Move to UCI format (e.g., "e2e4", "e7e8q" for promotion)
     */
    std::string move_to_uci(const Move& move, const board& b);

    /**
     * Convert UCI move string to internal Move
     */
    Move uci_to_move(const std::string& uci_move, const board& b);

    /**
     * Helper: Convert square bitboard to algebraic notation (e.g., 0x10 -> "e1")
     */
    std::string square_to_algebraic(Bitboard square);

    /**
     * Helper: Convert algebraic notation to square bitboard (e.g., "e1" -> 0x10)
     */
    Bitboard algebraic_to_square(const std::string& algebraic);
}

#endif // UCI_HPP
