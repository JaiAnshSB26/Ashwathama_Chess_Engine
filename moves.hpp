#ifndef MOVES_HPP
#define MOVES_HPP
#include <random>
#include "attacks.hpp"
#include "board.hpp"
#include  "utils.hpp"
#include <cstring>  // for memcpy if you use it

// Generate all possible moves checking all rules except check
std::vector<Move> board::generatePseudoLegalMoves() const {
    bool isWhiteTurn = (boardTurn == White);
    std::vector<Move> moves;
    

    //
    // 2) Generate moves for non-pawn pieces using your attack_map approach
    //    (pawns are skipped here, because we handled them above!)
    //

    // 1) Create local attack_map
    std::array<Bitboard, 64> attack_map;
    attack_map.fill(0ULL);

    // 2) Calculate attacks
    calculate_attacks(bitboards, chessboard, isWhiteTurn, attack_map);

    // 3) Initialize the moves vector which will be returned
    

    // 4) Convert each square’s attack bitboard into Moves
    for (int square = 0; square < 64; ++square) {
        PieceType piece = chessboard[square];
        if (piece == e) continue; // Skip empty squares

        // Skip opponent’s pieces on source
        if (isWhiteTurn && isBlackPiece(piece)) continue;
        if (!isWhiteTurn && isWhitePiece(piece)) continue;

        Bitboard &targets = attack_map[square];
        while (targets) {
            int dstSquare = popcount(targets); // popcount modifies 'targets'
            // Debug output: print source, destination, and destination piece type
                        
            // Skip if destination square has a friendly piece.
            if ((isWhiteTurn && isWhitePiece(chessboard[dstSquare])) ||
                (!isWhiteTurn && isBlackPiece(chessboard[dstSquare]))) {
                 
                continue;
            } else {

            Move m((1ULL << square), (1ULL << dstSquare), '\0');
            moves.push_back(m);
            }
        }
    }

    // 5) Generate castling moves
    generateCastlingMoves(moves);

    // 6) Generate promotion moves
    generatePromotions(moves);
    generatePawnMoves(moves);

    /*for(int i =0 ;i<moves.size();i++){
        if (moves[i].src_pos == 1ULL << 15) {
            print_bitboard(1ULL << 15);
            std::cout << "aSasasaSa";
        }

    }*/

    return moves;
}

// Check if current player is in check
bool check_for_check(board b){
    std::vector<Move> Moves = b.generatePseudoLegalMoves();
    for (auto & move : Moves) {
        if (move.dst_pos == b.EnemykingPosition()) {
            return true;
        }
    }
    return false;
};

// Make a copy of the board and apply a move
board PeekMove(const board &Board, const Move &move) {
    board Board1;
    // Copy the entire board structure
    std::memcpy(&Board1, &Board, sizeof(Board));
    Board1.apply_move(move);
    return Board1;
}

// Generate only legal moves (i.e., exclude moves that leave your king in check)
std::vector<Move> board::generateLegalMoves() const {
    std::vector<Move> legal_moves;

    // Generate all pseudo-legal moves
    std::vector<Move> pseudo = this->generatePseudoLegalMoves();
    for (const auto& mv : pseudo) {
        board next_board = PeekMove(*this, mv);
        if (!check_for_check(next_board)) {
            legal_moves.push_back(mv);
        }
    }
    



    return legal_moves;
}

void board::generateCastlingMoves(std::vector<Move>& moves) const{
    // If the king has moved already, no castling
    if (King_moved) return;

    // If the kingside rook has moved or the alley is blocked, skip kingside
    if (!(Rook_KingSide_moved || Occupied_KingSide_Castling_Alley)) {
        if (boardTurn == White) {
            // White king from e1 -> g1
            Move m(White_King_square, WhiteRookKingSideSquare, '\0', /*is_castling=*/true);
            moves.push_back(m);
        } else {
            // Black king from e8 -> g8
            Move m(Black_King_square, BlackRookKingSideSquare, '\0', /*is_castling=*/true);
            moves.push_back(m);
        }
    }

    // If the queenside rook has moved or the alley is blocked, skip queenside
    if (!(Rook_QueenSide_moved || Occupied_QueenSide_Castling_Alley)) {
        if (boardTurn == White) {
            // White king from e1 -> c1
            Move m(White_King_square, WhiteRookQueenSideSquare, '\0', /*is_castling=*/true);
            moves.push_back(m);
        } else {
            // Black king from e8 -> c8
            Move m(Black_King_square, BlackRookQueenSideSquare, '\0', /*is_castling=*/true);
            moves.push_back(m);
        }
    }
}


/*
void board::generatePromotions(std::vector<Move>& moves) const{
    // White promotions
    if (boardTurn == White) {
        // Check if any white pawns are on rank 7
        Bitboard pawns_on_7 = bitboards[P] & RANK_7;
        while (pawns_on_7) {
            int sq = popcount(pawns_on_7); // popcount modifies pawns_on_7
            int next_sq = sq + 8;         // Move from rank 7 to rank 8
            if (next_sq < 64) {
                // Add promotion moves: (Q, R, B, N)
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'q');
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'r');
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'b');
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'n');
            }
        }
    }
    // Black promotions
    else {
        // Check if any black pawns are on rank 2
        Bitboard pawns_on_2 = bitboards[p] & RANK_2;
        while (pawns_on_2) {
            int sq = popcount(pawns_on_2);
            int next_sq = sq - 8; // Move from rank 2 to rank 1
            if (next_sq >= 0) {
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'q');
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'r');
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'b');
                moves.emplace_back((1ULL << sq), (1ULL << next_sq), 'n');
            }
        }
    }
}
*/



void board::generatePromotions(std::vector<Move>& moves) const {
    // White promotions
    if (boardTurn == White) {
        // Check if any white pawns are on rank 7 using the RANK_7 mask
        Bitboard pawns_on_7 = bitboards[P] & RANK_7;
        while (pawns_on_7) {
            int sq = popcount(pawns_on_7);  // Get and clear one pawn position on rank 7
            int next_sq = sq + 8;           // Destination square one rank ahead (promotion square)
            // Only add promotion moves if the destination is on board and empty
            if (next_sq < 64 && chessboard[next_sq] == e) {
                Bitboard cur = (1ULL << sq);
                Bitboard next =  (1ULL << next_sq);

                Bitboard c = valid_pawn_captures(cur, (boardTurn == White), this->opponentPieces());

                moves.emplace_back(cur ,next , 'q');
                moves.emplace_back(cur ,next , 'r');
                moves.emplace_back(cur ,next , 'b');
                moves.emplace_back(cur ,next , 'n');

                while(c){
                    int i = popcount(c);
                    moves.emplace_back(cur ,(1ULL << i) , 'q');
                    moves.emplace_back(cur ,(1ULL << i) , 'r');
                    moves.emplace_back(cur ,(1ULL << i) , 'b');
                    moves.emplace_back(cur ,(1ULL << i) , 'n');
                }
            }
        }
    }
    // Black promotions
    else {
        // Check if any black pawns are on rank 2 using the RANK_2 mask
        Bitboard pawns_on_2 = bitboards[p] & RANK_2;
        while (pawns_on_2) {
            int sq = popcount(pawns_on_2);  // Get and clear one pawn position on rank 2
            int next_sq = sq - 8;           // Destination square one rank down (promotion square)
            // Only add promotion moves if the destination is on board and empty
            if (next_sq >= 0 && chessboard[next_sq] == e) {
                Bitboard cur = (1ULL << sq);
                Bitboard next =  (1ULL << next_sq);

                Bitboard c = valid_pawn_captures(cur, (boardTurn == White), this->opponentPieces());

                moves.emplace_back(cur ,next , 'q');
                moves.emplace_back(cur ,next , 'r');
                moves.emplace_back(cur ,next , 'b');
                moves.emplace_back(cur ,next , 'n');

                while(c){
                    int i = popcount(c);
                    moves.emplace_back(cur ,(1ULL << i) , 'q');
                    moves.emplace_back(cur ,(1ULL << i) , 'r');
                    moves.emplace_back(cur ,(1ULL << i) , 'b');
                    moves.emplace_back(cur ,(1ULL << i) , 'n');
                }
            }
        }
    }
}


void board::generatePawnMoves(std::vector<Move>& moves) const {
    bool isWhiteTurn = (boardTurn == White);
    int direction = isWhiteTurn ? 8 : -8;  // north for White, south for Black
    Bitboard pawns = isWhiteTurn ? bitboards[P] : bitboards[p];

    while (pawns) {
        int srcSq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;  // remove the pawn we're processing

        // Single forward move
        int dstSq = srcSq + direction;
        if (dstSq >= 0 && dstSq < 64 && chessboard[dstSq] == e) {
            // If moving into promotion rank, skip here, handled by promotions
            if ((isWhiteTurn && (dstSq / 8 == 7)) || (!isWhiteTurn && (dstSq / 8 == 0))) {
                // Promotion moves are handled separately in generatePromotions
            } else {
                moves.emplace_back((1ULL << srcSq), (1ULL << dstSq), '\0');
            }

            // Double forward move from starting rank
            int startRank = isWhiteTurn ? 1 : 6;
            if ((srcSq / 8) == startRank) {
                int dstSq2 = srcSq + 2 * direction;
                if (chessboard[dstSq2] == e) {
                    moves.emplace_back((1ULL << srcSq), (1ULL << dstSq2), '\0');
                }
            }
        }

        // Captures: diagonally forward moves
        for (int df = -1; df <= 1; df += 2) {  // file shift: -1 (west), +1 (east)
            int captureSq = srcSq + direction + df;
            if (captureSq >= 0 && captureSq < 64) {
                // Ensure the move stays on board (prevent wrap-around due to file changes)
                if ((df == -1 && srcSq % 8 == 0) || (df == 1 && srcSq % 8 == 7))
                    continue;

                PieceType target = chessboard[captureSq];
                if (target != e && ((isWhiteTurn && isBlackPiece(target)) || (!isWhiteTurn && isWhitePiece(target)))) {
                    // If moving into promotion rank, skip here, handled by promotions
                    if ((isWhiteTurn && (captureSq / 8 == 7)) || (!isWhiteTurn && (captureSq / 8 == 0))) {
                        // Promotion captures handled in generatePromotions
                    } else {
                        moves.emplace_back((1ULL << srcSq), (1ULL << captureSq), '\0');
                    }
                }

                // En passant capture logic could be added here if needed
            }
        }
    }
}



    


Move board::generateRandomLegalMove() const {
    // Generate all legal moves
    std::vector<Move> legal_moves = this->generateLegalMoves();

    // If no legal moves are available (e.g., checkmate or stalemate), return an empty move
    if (legal_moves.empty()) {
        return Move();
    }

    // Set up random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, legal_moves.size() - 1);

    // Pick a random move
    size_t random_index = dist(gen);
    return legal_moves[random_index];
}

#endif // MOVES_HPP
