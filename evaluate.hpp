#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <climits>

#include "board.hpp"
#include "utils.hpp"
#include "file_interpreter.hpp"
#include "moves.hpp"
#include "attacks.hpp"


/**
 * Example of explicit PST definitions for all pieces (White & Black).
 *
 * Note: These are sample values. You can tune them or replace them with
 * more sophisticated / official PSTs from open-source chess engines.
 */
struct Evaluator {
    int max_depth = 1;

    // Piece values for White & Black pieces. 
    // Indexing:  0=P,1=R,2=N,3=B,4=Q,5=K, 6=p,7=r,8=n,9=b,10=q,11=k
    int piece_values[12] = {
        /* P */  100,
        /* R */  500,
        /* N */  320,
        /* B */  330,
        /* Q */  900,
        /* K */  10000,

        /* p */  100,
        /* r */  500,
        /* n */  320,
        /* b */  330,
        /* q */  900,
        /* k */  10000
    };

    // Slight bonus for having both bishops.
    const int bishop_pair_bonus = 30;

    //==================================================
    // 1) White Piece-Square Tables
    //==================================================

    // White Pawn PST
    int white_pawn_pst[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,   // rank0 (a1..h1)
         5, 10, 10,-20,-20, 10, 10,  5,   // rank1
         5, -5, -5,  0,  0, -5, -5,  5,   // rank2
         0,  0,  0, 20, 20,  0,  0,  0,   // rank3
         5,  5, 10, 25, 25, 10,  5,  5,   // rank4
        10, 10, 20, 30, 30, 20, 10, 10,   // rank5
        50, 50, 50, 50, 50, 50, 50, 50,   // rank6
         0,  0,  0,  0,  0,  0,  0,  0    // rank7
    };

    // White Knight PST
    int white_knight_pst[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    // White Bishop PST
    int white_bishop_pst[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    // White Rook PST
    int white_rook_pst[64] = {
          0,  0,  5, 10, 10,  5,  0,  0,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          5, 10, 10, 10, 10, 10, 10,  5,
          0,  0,  5, 10, 10,  5,  0,  0
    };

    // White Queen PST
    int white_queen_pst[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -10,  5,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    // White King PST (middle game, for example)
    int white_king_pst[64] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    };

    //==================================================
    // 2) Black Piece-Square Tables (mirrored versions)
    //==================================================
    // The logic here is to flip the White PST so rank0 in the white array
    // becomes rank7 in the black array, rank1 -> rank6, etc.
    // We do not multiply by -1 because the sign is already accounted for
    // when we add/subtract in the evaluation routine.

    // Black Pawn PST
    int black_pawn_pst[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,   // rank0 for black = rank7 for white
        50, 50, 50, 50, 50, 50, 50, 50,   // rank1 for black = rank6 for white
        10, 10, 20, 30, 30, 20, 10, 10,   // rank2 for black = rank5
         5,  5, 10, 25, 25, 10,  5,  5,   // rank3 for black = rank4
         0,  0,  0, 20, 20,  0,  0,  0,   // rank4 for black = rank3
         5, -5, -5,  0,  0, -5, -5,  5,   // rank5 for black = rank2
         5, 10, 10,-20,-20, 10, 10,  5,   // rank6 for black = rank1
         0,  0,  0,  0,  0,  0,  0,  0    // rank7 for black = rank0
    };

    // Black Knight PST
    int black_knight_pst[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,  // rank0 = white rank7
        -40,-20,  0,  5,  5,  0,-20,-40,  // rank1 = white rank6
        -30,  5, 10, 15, 15, 10,  5,-30,  // rank2 = white rank5
        -30,  0, 15, 20, 20, 15,  0,-30,  // rank3 = white rank4
        -30,  5, 15, 20, 20, 15,  5,-30,  // rank4 = white rank3
        -30,  0, 10, 15, 15, 10,  0,-30,  // rank5 = white rank2
        -40,-20,  0,  0,  0,  0,-20,-40,  // rank6 = white rank1
        -50,-40,-30,-30,-30,-30,-40,-50   // rank7 = white rank0
    };

    // Black Bishop PST
    int black_bishop_pst[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,  // rank0
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20   // rank7
    };

    // Black Rook PST
    int black_rook_pst[64] = {
          0,  0,  5, 10, 10,  5,  0,  0,  // rank0 = white rank7
          5, 10, 10, 10, 10, 10, 10,  5,  // rank1 = white rank6
         -5,  0,  0,  0,  0,  0,  0, -5,  // rank2 = white rank5
         -5,  0,  0,  0,  0,  0,  0, -5,  // rank3 = white rank4
         -5,  0,  0,  0,  0,  0,  0, -5,  // rank4 = white rank3
         -5,  0,  0,  0,  0,  0,  0, -5,  // rank5 = white rank2
         -5,  0,  0,  0,  0,  0,  0, -5,  // rank6 = white rank1
          0,  0,  5, 10, 10,  5,  0,  0   // rank7 = white rank0
    };

    // Black Queen PST
    int black_queen_pst[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,  // rank0 = white rank7
        -10,  0,  0,  0,  0,  0,  5,-10,
        -10,  0,  5,  5,  5,  5,  5,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  5,-10,
        -10,  0,  0,  0,  0,  0,  5,-10,
        -20,-10,-10, -5, -5,-10,-10,-20   // rank7 = white rank0
    };

    // Black King PST (middle game, for example)
    int black_king_pst[64] = {
         20, 30, 10,  0,  0, 10, 30, 20,  // rank0 = white rank7
         20, 20,  0,  0,  0,  0, 20, 20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30   // rank7 = white rank0
    };

    //==================================================
    // get_piece_square_bonus() with explicit black PST
    //==================================================
    int get_piece_square_bonus(PieceType pt, int sq) {
        switch (pt) {
            case P: return white_pawn_pst[sq];
            case N: return white_knight_pst[sq];
            case B: return white_bishop_pst[sq];
            case R: return white_rook_pst[sq];
            case Q: return white_queen_pst[sq];
            case K: return white_king_pst[sq];

            case p: return black_pawn_pst[sq];
            case n: return black_knight_pst[sq];
            case b: return black_bishop_pst[sq];
            case r: return black_rook_pst[sq];
            case q: return black_queen_pst[sq];
            case k: return black_king_pst[sq];

            default: return 0;
        }
    }

    //==================================================
    // 3) Evaluate position with heuristics
    //==================================================
    int evaluate_position(const board &chess_board) {
        int score = 0;
        int whiteBishops = 0;
        int blackBishops = 0;

        // Track pawns by file for rook open-file bonuses
        int whitePawnFileCount[8] = {0};
        int blackPawnFileCount[8] = {0};

        std::vector<int> whiteRookSquares, blackRookSquares;

        for (int sq = 0; sq < 64; sq++) {
            PieceType pt = chess_board.chessboard[sq];
            if (pt == e) continue;

            // Material
            int val = piece_values[pt];
            // PST bonus
            int pst_bonus = get_piece_square_bonus(pt, sq);

            bool isWhite = isWhitePiece(pt);
            int sign = (isWhite ? 1 : -1);

            // Accumulate
            score += sign * (val + pst_bonus);

            // Count bishops for bishop pair
            if (pt == B) whiteBishops++;
            else if (pt == b) blackBishops++;

            // Pawn file tracking
            if (pt == P) {
                whitePawnFileCount[sq % 8]++;
            } else if (pt == p) {
                blackPawnFileCount[sq % 8]++;
            }

            // Rook squares
            if (pt == R) whiteRookSquares.push_back(sq);
            if (pt == r) blackRookSquares.push_back(sq);
        }

        // Bishop pair bonus
        if (whiteBishops >= 2) score += bishop_pair_bonus;
        if (blackBishops >= 2) score -= bishop_pair_bonus;

        // Rook open/semi-open file bonus
        for (int sq : whiteRookSquares) {
            int file = sq % 8;
            int all_pawns_in_file = whitePawnFileCount[file] + blackPawnFileCount[file];
            if (all_pawns_in_file == 0) score += 15;    // open
            else if (whitePawnFileCount[file] == 0) score += 10;  // semi-open
        }
        for (int sq : blackRookSquares) {
            int file = sq % 8;
            int all_pawns_in_file = whitePawnFileCount[file] + blackPawnFileCount[file];
            if (all_pawns_in_file == 0) score -= 15;    // open
            else if (blackPawnFileCount[file] == 0) score -= 10;  // semi-open
        }
        score += evaluateKingSafety(chess_board);

        return score;
    }
    //==================================================
    // Random Move Selector
    //==================================================
    Move get_random_move(board &chess_board) {
        return chess_board.generateRandomLegalMove();
    }

    //==================================================
    // 4) Alpha-Beta Search
    //==================================================
    /*int alphabeta(board &chess_board, int depth, int alpha, int beta, bool maximizing_player) {
        if (depth == 0) {
            return evaluate_position(chess_board);
        }

        // Get moves (in a real engine, you'd generate legal moves)
       std::vector<Move> moves = chess_board.generateLegalMoves();

        // If no moves, it's checkmate or stalemate
        if (moves.empty()) {
            return maximizing_player ? -9999999 : 9999999;
        }

        if (maximizing_player) {
            int best_eval = std::numeric_limits<int>::min();
            for (auto &m : moves) {
                board oldboard = chess_board;
                chess_board.apply_move(m);

                int eval = alphabeta(chess_board, depth - 1, alpha, beta, false);
                chess_board = oldboard;

                if (eval > best_eval) best_eval = eval;
                if (eval > alpha) alpha = eval;
                if (beta <= alpha) break;  // alpha-beta prune
            }
            return best_eval;
        }
        else {
            int best_eval = std::numeric_limits<int>::max();
            for (auto &m : moves) {
                board oldboard = chess_board;
                chess_board.apply_move(m);

                int eval = alphabeta(chess_board, depth - 1, alpha, beta, true);
                chess_board = oldboard;

                if (eval < best_eval) best_eval = eval;
                if (eval < beta) beta = eval;
                if (beta <= alpha) break;  // prune
            }
            return best_eval;
        }
    }*/
    int alphabeta(board &chess_board, int depth, int alpha, int beta, bool maximizing_player) {
    if (depth == 0) {
        return evaluate_position(chess_board);
    }

    std::vector<Move> moves = chess_board.generateLegalMoves();
    // or generatePseudoLegalMoves(), depending on your code,
    // but typically you want actual legal moves.

    // ============================
    // Additional “pawn‐push pruning” step
    // ============================
    // Filter out any moves that are “pawn forward push” into an occupied square.
    std::vector<Move> prunedMoves;
    prunedMoves.reserve(moves.size());

    for (auto &m : moves) {
        // Identify if it’s a pawn
        int srcIdx = __builtin_ctzll(m.src_pos);
        PieceType pt = chess_board.chessboard[srcIdx];

        bool isPawn = (pt == P || pt == p);
        if (!isPawn) {
            // Not a pawn → keep this move
            prunedMoves.push_back(m);
            continue;
        }

        // Check if the move is strictly forward (no file change).
        int dstIdx = __builtin_ctzll(m.dst_pos);
        int srcFile = srcIdx % 8;
        int dstFile = dstIdx % 8;
        int srcRank = srcIdx / 8;
        int dstRank = dstIdx / 8;

        bool sameFile = (srcFile == dstFile);

        // White forward means dstRank > srcRank
        // Black forward means dstRank < srcRank
        bool isForwardPush = false;
        if (pt == P) { 
            isForwardPush = (sameFile && (dstRank == srcRank + 1 || dstRank == srcRank + 2));
        } else {
            // black pawn
            isForwardPush = (sameFile && (dstRank == srcRank - 1 || dstRank == srcRank - 2));
        }

        if (isForwardPush) {
            // If the destination is occupied by *any* piece, skip the move
            PieceType occupant = chess_board.chessboard[dstIdx];
            if (occupant != e) {
                // Occupied, so skip this move = do not add to prunedMoves
                continue;
            }
        }

        // Otherwise, it’s a capture or it’s a valid push → keep it
        prunedMoves.push_back(m);
    }
    // Now ‘prunedMoves’ has no forward‐push‐into‐occupied squares.

    if (prunedMoves.empty()) {
        // Then no moves left, checkmate or stalemate
        return maximizing_player ? -9999999 : 9999999;
    }

    // Proceed with alpha-beta
    if (maximizing_player) {
        int best_eval = std::numeric_limits<int>::min();
        for (auto &move : prunedMoves) {
            board old_board = chess_board;
            chess_board.apply_move(move);

            int eval = alphabeta(chess_board, depth - 1, alpha, beta, false);
            chess_board = old_board;

            best_eval = std::max(best_eval, eval);
            alpha     = std::max(alpha, eval);
            if (beta <= alpha) {
                break; // alpha-beta cutoff
            }
        }
        return best_eval;
    } else {
        int best_eval = std::numeric_limits<int>::max();
        for (auto &move : prunedMoves) {
            board old_board = chess_board;
            chess_board.apply_move(move);

            int eval = alphabeta(chess_board, depth - 1, alpha, beta, true);
            chess_board = old_board;

            best_eval = std::min(best_eval, eval);
            beta      = std::min(beta, eval);
            if (beta <= alpha) {
                break; // cutoff
            }
        }
        return best_eval;
    }
}


    //==================================================
    // 5) Get the best move at the root
    //==================================================
    Move get_best_move(board &chess_board) {
        bool maximizing = (chess_board.boardTurn == White);
        std::vector<Move> moves = chess_board.generateLegalMoves();

        if (moves.empty()) {
            Move nullMove{};
            return nullMove;  // no moves
        }

        int alpha = std::numeric_limits<int>::min();
        int beta  = std::numeric_limits<int>::max();
        int bestEval = maximizing ? alpha : beta;

        Move best_move = moves[0];
        for (auto &m : moves) {
            board oldBoard = chess_board;
            chess_board.apply_move(m);

            int eval = alphabeta(chess_board, max_depth - 1, alpha, beta, !maximizing);

            chess_board = oldBoard;

            if (maximizing) {
                if (eval > bestEval) {
                    bestEval = eval;
                    best_move = m;
                }
                if (eval > alpha) alpha = eval;
                if (beta <= alpha) break;
            } else {
                if (eval < bestEval) {
                    bestEval = eval;
                    best_move = m;
                }
                if (eval < beta) beta = eval;
                if (beta <= alpha) break;
            }
        }
        return best_move;
    }

    //==================================================
    // Mock move generator
    //==================================================
   

    // Optional: Quick test
    void test_evaluate(board &chess_board) {
    Move best = get_best_move(chess_board);
    if (best.src_pos == 0ULL && best.dst_pos == 0ULL) {
        std::cout << "No moves available.\n";
        return;
    }
    int src = __builtin_ctzll(best.src_pos);
    int dst = __builtin_ctzll(best.dst_pos);
    char src_file = 'a' + (src % 8);
    char src_rank = '1' + (src / 8);
    char dst_file = 'a' + (dst % 8);
    char dst_rank = '1' + (dst / 8);

    std::cout << "Best move: "
              << src_file << src_rank 
              << dst_file << dst_rank;
    if (best.promotion) {
        std::cout << best.promotion;  // Append the promotion character directly
    }
    std::cout << "\n";
}

    //==================================================
    // Test Random Move Generator
    //==================================================
    void test_random_move(board &chess_board) {
        Move random_move = get_random_move(chess_board);
        if (random_move.src_pos == 0ULL && random_move.dst_pos == 0ULL) {
            std::cout << "No legal moves available.\n";
            return;
        }

        int src = __builtin_ctzll(random_move.src_pos);
        int dst = __builtin_ctzll(random_move.dst_pos);
        char src_file = 'a' + (src % 8);
        char src_rank = '1' + (src / 8);
        char dst_file = 'a' + (dst % 8);
        char dst_rank = '1' + (dst / 8);

        std::cout << "Random move: "
                  << src_file << src_rank 
                  
                  << dst_file << dst_rank << "\n";
    }

    //======================
// King Safety Function
//======================
int evaluateKingSafety(const board &chess_board) {
    int safetyScore = 0;
    const int directions[8] = {8, 9, 7, 1, -1, -7, -9, -8}; // N, NE, NW, E, W, SW, SE, S

    bool whiteToMove = (chess_board.boardTurn == White);
    Bitboard kingBB = whiteToMove ? chess_board.bitboards[K] : chess_board.bitboards[k];
    if (kingBB == 0ULL) {
        return whiteToMove ? -10000 : 10000;
    }
    int kingSquare = __builtin_ctzll(kingBB);

    std::vector<int> pawnShieldSquares;
    if (whiteToMove) {
        for (int d = 5; d <= 7; ++d) { 
            int sq = kingSquare + directions[d];
            if (sq >= 0 && sq < 64) pawnShieldSquares.push_back(sq);
        }
    } else {
        for (int d = 0; d <= 2; ++d) {
            int sq = kingSquare + directions[d];
            if (sq >= 0 && sq < 64) pawnShieldSquares.push_back(sq);
        }
    }

    for (int sq : pawnShieldSquares) {
        PieceType piece = chess_board.chessboard[sq];
        if ((whiteToMove && piece == P) || (!whiteToMove && piece == p)) {
            safetyScore += 10;
        } else {
            safetyScore -= 5;
        }
    }

    for (int d = 0; d < 8; ++d) {
        int adjacentSq = kingSquare + directions[d];
        if (adjacentSq < 0 || adjacentSq >= 64) continue;
        int kingFile = kingSquare % 8;
        int adjFile = adjacentSq % 8;
        if (abs(kingFile - adjFile) > 1) continue;

        PieceType adjacentPiece = chess_board.chessboard[adjacentSq];
        if (adjacentPiece != e && 
            ((whiteToMove && isBlackPiece(adjacentPiece)) || 
             (!whiteToMove && isWhitePiece(adjacentPiece)))) {
            safetyScore -= 20;
        }
    }

    return safetyScore;
}
};
