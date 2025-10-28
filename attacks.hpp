//Changes made in this version with respect to the Jai's one:
// 1. I was getting an error that Sliding_attacks can only take one bit set as input.So I made some changes in calculate attacks
// in this part of the code :else if (piece == R) {
 //   attack_map[i] |= sliding_attacks(
   //     RookBitboard | QueenBitboard,
     //   friendBlockers, opponentBlockers, north
    //); RookBitboard | QueenBitboard typically has many bits set if you have multiple rooks (and queens) on the board.

//Hence, __builtin_popcountll(...) returns a number ≥1 and if it’s not exactly 1, it triggers Error: Sliding_attacks can only take one bit set as input!
// So I added this line in calculate attacks to resolve the error
// // singleBit = exactly one piece at square i
//     Bitboard singleBit = (1ULL << i);


// Further changes made to fix pawn and queen attacks:
//1. Prevents pawns from capturing on empty diagonals by filtering captures with opponentBlockers.
//Prevents queens/rooks/bishops from sliding through friendly pieces by stopping the slide when encountering any piece.
// Doesn’t block itself (the piece we’re sliding from) in friendBlockers, so the piece does not immediately see itself as a “friend blocker.”


//Check the 2 movers of each of the first pawns as well.
#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include <cstdint> // For uint64_t
#include <array>   // For precomputed attack arrays
#include <iostream>
#include <bitset>
#include "utils.hpp"

using Bitboard = uint64_t;

/**
 * If you need to convert from an old to new bitboard convention, use this.
 * Otherwise, you can ignore or remove it.
**/

Bitboard convert_bitboard_to_new_convention(Bitboard old_bitboard) {
    Bitboard new_bitboard = 0ULL;
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int old_index = rank * 8 + file;
            int new_index = (7 - rank) * 8 + file;
            if (old_bitboard & (1ULL << old_index)) {
                new_bitboard |= (1ULL << new_index);
            }
        }
    }
    return new_bitboard;
}

// ~~~~~~~~ Move helpers for shifting a single square ~~~~~~~~
constexpr Bitboard move_north(Bitboard b)       { return (b << 8)  & ~RANK_8; }
constexpr Bitboard move_south(Bitboard b)       { return (b >> 8)  & ~RANK_1; }
constexpr Bitboard move_east(Bitboard b)        { return (b << 1)  & ~FILE_H; }
constexpr Bitboard move_west(Bitboard b)        { return (b >> 1)  & ~FILE_A; }
constexpr Bitboard move_north_east(Bitboard b)  { return (b << 9)  & ~(RANK_8 | FILE_H); }
constexpr Bitboard move_north_west(Bitboard b)  { return (b << 7)  & ~(RANK_8 | FILE_A); }
constexpr Bitboard move_south_east(Bitboard b)  { return (b >> 7)  & ~(RANK_1 | FILE_H); }
constexpr Bitboard move_south_west(Bitboard b)  { return (b >> 9)  & ~(RANK_1 | FILE_A); }

// Knight attacks for a single square
constexpr Bitboard knight_attacks(Bitboard b) {
    // Shift the square in knight patterns
    return move_north(move_north_west(b) | move_north_east(b)) |
           move_south(move_south_west(b) | move_south_east(b)) |
           move_west(move_north_west(b) | move_south_west(b))  |
           move_east(move_north_east(b) | move_south_east(b));
}

// King attacks for a single square
constexpr Bitboard king_attacks(Bitboard b) {
    return move_north(b)       | move_south(b)       |
           move_west(b)        | move_east(b)        |
           move_north_west(b)  | move_north_east(b)  |
           move_south_west(b)  | move_south_east(b);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Pawn captures only if an enemy piece is present (no en-passant).
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

inline Bitboard valid_pawn_captures(Bitboard singleBit, bool is_white, Bitboard opponentPieces)
{
    Bitboard diag = is_white
        ? (move_north_east(singleBit) | move_north_west(singleBit))
        : (move_south_east(singleBit) | move_south_west(singleBit));

    // Keep only squares that contain opponent pieces
    return diag & opponentPieces;
}

//Corected version
/*
inline Bitboard valid_pawn_captures(Bitboard singleBit, bool is_white, const std::array<PieceType, 64>& chessboard) {
    Bitboard diag = is_white
        ? (move_north_east(singleBit) | move_north_west(singleBit))
        : (move_south_east(singleBit) | move_south_west(singleBit));

    Bitboard captures = 0ULL;
    while(diag) {
        int targetSquare = __builtin_ctzll(diag);
        diag &= diag - 1;  // Clear the lowest set bit from diag

        PieceType targetPiece = chessboard[targetSquare];
        if (is_white & isBlackPiece(targetPiece)) {
            captures |= (1ULL << targetSquare);
        } else if(!is_white & isWhitePiece(targetPiece)) {
            captures |= (1ULL << targetSquare);
        }
    }
    return captures;
}*/

//inline Bitboard double_pawn_


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sliding attacks (rook, bishop, queen directions).
// We shift in a direction until hitting any piece or going off-board.
// friendBlockers => squares occupied by your own pieces (minus the sliding piece).
// opponentBlockers => squares occupied by the opponent.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bitboard sliding_attacks(Bitboard singleBit,
                         Bitboard friendBlockers,   // does not include this piece
                         Bitboard opponentBlockers,
                         direction dir)
{
    // Must have exactly one bit set
    if (__builtin_popcountll(singleBit) != 1) {
        std::cerr << "Error: Sliding_attacks can only take one bit set as input!\n";
        exit(EXIT_FAILURE);
    }

    Bitboard attack_map = 0ULL;
    Bitboard b = singleBit;

    // Continue shifting in the given direction
    while (true) {
        // Move b one square in the chosen direction
        switch(dir) {
            case north:       b = move_north(b);       break;
            case south:       b = move_south(b);       break;
            case east:        b = move_east(b);        break;
            case west:        b = move_west(b);        break;
            case north_east:  b = move_north_east(b);  break;
            case north_west:  b = move_north_west(b);  break;
            case south_east:  b = move_south_east(b);  break;
            case south_west:  b = move_south_west(b);  break;
            default:
                std::cerr << "Error: Invalid direction in sliding_attacks!\n";
                exit(EXIT_FAILURE);
        }

        // If we moved off-board (b == 0), stop
        if (b == 0ULL) {
            break;
        }

        // If there's a friendly piece, we can't capture or pass => stop
        if (b & friendBlockers) {
            break;
        }

        // We can attack/occupy this square
        attack_map |= b;

        // If there's an opponent piece here, we can capture but then must stop
        if (b & opponentBlockers) {
            break;
        }
    }

    return attack_map;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug function: prints a bitboard
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void print_bitboard(Bitboard b) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((b & (1ULL << square)) ? '1' : '0');
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

/**
 * calculate_attacks(...) populates `attack_map[i]` with all squares that the piece
 * on square i can attack. We only consider the side to move (is_white).
 *
 * For sliding pieces, we must remove the piece itself from friendBlockers so it
 * doesn't "block" its own sliding at square i.
 */

inline Bitboard pawn_attacks(Bitboard singleBit, bool is_white) {
    return is_white
        ? (move_north_east(singleBit) | move_north_west(singleBit))
        : (move_south_east(singleBit) | move_south_west(singleBit));
}

void calculate_attacks(const std::array<Bitboard, 12>& bitboards,
                       const std::array<PieceType, 64>& chessboard,
                       bool is_white,
                       std::array<Bitboard, 64>& attack_map)
{
    // 1) Clear attack_map
    attack_map.fill(0ULL);

    // 2) friendBlockers / opponentBlockers for the current side
    Bitboard friendPieces = is_white
        ? (bitboards[P] | bitboards[R] | bitboards[N] | bitboards[B] | bitboards[Q] | bitboards[K])
        : (bitboards[p] | bitboards[r] | bitboards[n] | bitboards[b] | bitboards[q] | bitboards[k]);

    Bitboard opponentPieces = is_white
        ? (bitboards[p] | bitboards[r] | bitboards[n] | bitboards[b] | bitboards[q] | bitboards[k])
        : (bitboards[P] | bitboards[R] | bitboards[N] | bitboards[B] | bitboards[Q] | bitboards[K]);

    // 3) For each square
    for(int i = 0; i < 64; i++) {
        PieceType piece = chessboard[i];
        if (piece == e) continue; // empty
        // Skip if piece color doesn't match
        if (is_white && isBlackPiece(piece)) continue;
        if (!is_white && isWhitePiece(piece)) continue;

        // The single bit representing this piece
        Bitboard singleBit = (1ULL << i);

        // Remove *this piece* from friendBlockers so we don't block ourselves
        Bitboard friendBlockers = friendPieces & ~singleBit;

        // 4) Dispatch by piece type
        if (piece == P) {
            
            Bitboard captures = valid_pawn_captures(singleBit, true, opponentPieces);
            
            Bitboard pseudo_map = (captures & ~(move_north(singleBit) & (friendBlockers) & (opponentPieces)));
            attack_map[i] |= (pseudo_map & (move_north(singleBit) & (~friendBlockers) & (~opponentPieces)));
            
            
            
            
            
        }
        else if (piece == N) {
            attack_map[i] |= knight_attacks(singleBit);
        }
        else if (piece == B) {
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_west);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_west);
        }
        else if (piece == R) {
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, west);
        }
        else if (piece == Q) {
            // Rook-like
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, west);
            // Bishop-like
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_west);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_west);
        }
        else if (piece == K) {
            attack_map[i] |= king_attacks(singleBit) & ~friendPieces;
        }
        else if (piece == p) {
            //Black pawn diagonal captures 
            Bitboard captures = valid_pawn_captures(singleBit, false, opponentPieces);
            Bitboard pseudo_map = (captures & ~(move_south(singleBit) & (friendBlockers) & (opponentPieces)));
            attack_map[i] |= (pseudo_map & (move_south(singleBit) & (~friendBlockers) & (~opponentPieces)));
            
            
            
        }
        else if (piece == n) {
            attack_map[i] |= knight_attacks(singleBit);
        }
        else if (piece == b) {
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_west);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_west);
        }
        else if (piece == r) {
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, west);
        }
        else if (piece == q) {
            // Rook-like
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, west);
            // Bishop-like
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, north_west);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_east);
            attack_map[i] |= sliding_attacks(singleBit, friendBlockers, opponentPieces, south_west);
        }
        else if (piece == k) {
            attack_map[i] |= king_attacks(singleBit) & ~friendPieces;;
        }
        // else 'e' was skipped, do nothing
        //std::cout<<"h2 attack map:" << "\n";
        //print_bitboard(attack_map[17]);
    }

}

#endif // ATTACKS_HPP