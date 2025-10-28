#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>      // For input and output
#include <fstream>       // For reading/writing files
#include <vector>        // For dynamic arrays
#include <map>           // For std::map
#include <set>           // For std::set
#include <array>         // For fixed-size arrays
#include <string>        // For strings
#include <algorithm>     // For algorithms like std::sort
#include <cstdint>       // For fixed-width integer types (e.g., uint64_t)
#include <bitset>        // For low-level bit manipulations
#include <cassert>       // For debugging with assertions


typedef uint64_t Bitboard;

enum direction {
    north = 8,
    south = -8,
    east = 1,
    west = -1,
    north_east = 9,
    north_west = 7,
    south_east = -7,
    south_west = -9
};

// Constants for file boundaries (to avoid wraparound in bit shifts and invalid bit-operations)
constexpr Bitboard FILE_A = 0b1000000010000000100000001000000010000000100000001000000010000000ULL; // Left-most column
constexpr Bitboard FILE_H = 0b0000000100000001000000010000000100000001000000010000000100000001ULL; // Right-most column
constexpr Bitboard RANK_1 = 0b1111111100000000000000000000000000000000000000000000000000000000ULL; // South-most row
constexpr Bitboard RANK_8 = 0b0000000000000000000000000000000000000000000000000000000011111111ULL; // North-most row

//Constants for Promotions
constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;

// Constants for checking the presence of rooks and kings at these positions to determine if they moved
constexpr Bitboard WhiteRookQueenSideSquare = 0b0000000000000000000000000000000000000000000000000000000010000000ULL;
constexpr Bitboard WhiteRookKingSideSquare = 0b0000000000000000000000000000000000000000000000000000000000000001ULL;
constexpr Bitboard BlackRookQueenSideSquare = 0b1000000000000000000000000000000000000000000000000000000000000000ULL;
constexpr Bitboard BlackRookKingSideSquare = 0b0000000100000000000000000000000000000000000000000000000000000000ULL;
constexpr Bitboard White_King_square = 0b0000000000000000000000000000000000000000000000000000000000001000ULL;
constexpr Bitboard Black_King_square = 0b0000100000000000000000000000000000000000000000000000000000000000ULL;

// Constants to determine if the way is clear for castling
constexpr Bitboard WhiteRookQueenSideAlley = 0b0000000000000000000000000000000000000000000000000000000001110000ULL;
constexpr Bitboard WhiteRookKingSideAlley = 0b0000000000000000000000000000000000000000000000000000000000000110ULL;
constexpr Bitboard BlackRookQueenSideAlley = 0b0111000000000000000000000000000000000000000000000000000000000000ULL;
constexpr Bitboard BlackRookKingSideAlley = 0b0000011000000000000000000000000000000000000000000000000000000000ULL;

// 


// Used in moves.hpp to compute moves
enum MoveType { 
    LEGAL, 
    PSEUDO_LEGAL, 
    EVASIONS 
};

// Uppercase: white pieces // Lowercase: black pieces // e: empty
enum PieceType {
    P,  // White pawn
    R,  // White rook
    N,  // White knight
    B,  // White bishop
    Q,  // White queen
    K,  // White king

    p,  // Black pawn
    r,  // Black rook
    n,  // Black knight
    b,  // Black bishop
    q, // Black queen
    k,  // Black king

    e  // empty square
};

// 3 categories (helpful for checking legality, enpassant, etc.)
enum Color {
    White,
    Black,
    Empty
};

// Structure to represent a move
struct Move {
    Bitboard src_pos;    // Source square (0-63)
    Bitboard dst_pos;    // Destination square (0-63)

    char promotion;    // 'Q', 'R', 'B', 'N' (optional, for pawn promotion)
    bool is_castling;  // True if the move is castling
    bool is_en_passant; // True if the move is en passant capture

    Move() 
        : src_pos(0ULL), dst_pos(0ULL), promotion('\0'), 
          is_castling(false), is_en_passant(false) {}

    // **Parameterized Constructor**
    Move(Bitboard src, Bitboard dst, char prom = '\0', 
         bool castling = false, bool en_passant = false)
        : src_pos(src), dst_pos(dst), promotion(prom), 
          is_castling(castling), is_en_passant(en_passant) {}
};


// Global or external variable to track the number of moves
inline static int num_of_moves = 0;


// ~~~~~~~~ Some bitboard utilities ~~~~~~~~
inline void set_pos_in_bit(Bitboard &b, int square) {
    b |= (1ULL << square);
}


inline bool clear_pos_in_bit(Bitboard &b, int square) {
    // Return whether the bit was actually set
    if ((b & (1ULL << square)) == 0) {
        return false;
    }
    b &= ~(1ULL << square);
    return true;
}

// popcount is "remove first set bit and return its index"
inline int popcount(Bitboard &b) {
    if (b == 0) {
        std::cerr << "Error: popcount cannot be called with a zero bitboard" << std::endl;
        exit(EXIT_FAILURE);
    }
    // Store the index of the lowest set bit
    int idx = __builtin_ctzll(b);
    // Clear that bit
    b &= b - 1;
    return idx;
}

inline Bitboard union_bitboards(Bitboard b1, Bitboard b2) {
    return b1 | b2;
}

inline Bitboard intersection_bitboards(Bitboard b1, Bitboard b2) {
    return b1 & b2;
}

inline Bitboard difference_bitboards(Bitboard b1, Bitboard b2) {
    return b1 & ~b2;
}

// Helper to check if a piece type is white
inline bool isWhitePiece(PieceType pt) {
    return (pt == P || pt == R || pt == N || pt == B || pt == Q || pt == K);
}

inline bool isBlackPiece(PieceType pt) {
    return (pt == p || pt == r || pt == n || pt == b || pt == q || pt == k);
}

// Convert a Move to e.g. "e2e4"
inline std::string moveToAlgebraic(const Move& move) {
    // Identify source and destination squares from bitboard
    int src = __builtin_ctzll(move.src_pos); 
    int dst = __builtin_ctzll(move.dst_pos);
    
    // Extract file/rank as 0..7
    int src_file = src % 8;
    int src_rank = src / 8;
    int dst_file = dst % 8;
    int dst_rank = dst / 8;

    // Build a 4-character string
    //   file = 'a'..'h'
    //   rank = '1'..'8'
    char notation[5];
    notation[0] = 'a' + src_file;        // e.g. 'e'
    notation[1] = '1' + src_rank;       // e.g. '2'
    notation[2] = 'a' + dst_file;       // e.g. 'e'
    notation[3] = '1' + dst_rank;       // e.g. '4'
    notation[4] = '\0';

    return std::string(notation);
}

// Writes exactly one move to a plain ASCII file
inline void writeMoveToFile(const char* filename, const Move& move) {
    // Open the file in overwrite mode (default)
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Error: could not open output file " << filename << std::endl;
        return;
    }

    // Convert the move to standard notation
    std::string moveStr = moveToAlgebraic(move);
    ofs << moveStr << "\n";    // Write the move and end with a newline
    ofs.close();               // Close the file to flush the content

    std::cout << "File reset and move written: " << moveStr << std::endl;
}



