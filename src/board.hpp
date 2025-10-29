#ifndef BOARD_HPP
#define BOARD_HPP

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdint.h>
#include <array>

#include "file_interpreter.hpp"
#include "utils.hpp"

struct board {

public:
    // 12-element array of Bitboards, one for each piece type:
    //   0: White Pawn (P)
    //   1: White Rook (R)
    //   2: White Knight (N)
    //   3: White Bishop (B)
    //   4: White Queen (Q)
    //   5: White King (K)
    //   6: Black Pawn (p)
    //   7: Black Rook (r)
    //   8: Black Knight (n)
    //   9: Black Bishop (b)
    //   10: Black Queen (q)
    //   11: Black King (k)
    std::array<Bitboard, 12> bitboards;

    // 64-element array storing which piece is at each square
    std::array<PieceType, 64> chessboard; 

    // Additional tracking info
    Color boardTurn;          // Whose move is it?
    int en_passant_square;    // -1 if none
    // If you also need castling rights, you'd store them similarly

    bool King_moved;      // used for castling rights
    bool Rook_KingSide_moved;      // used for castling rights
    bool Rook_QueenSide_moved;      // used for castling rights
    bool Occupied_KingSide_Castling_Alley;     // used for castling rights
    bool Occupied_QueenSide_Castling_Alley;     // used for castling rights

    // -------------------------
    // Castling helper masks / squares
    // -------------------------

    // Which squares originally held each rook
    Bitboard WhiteRookKingSideSquare  = (1ULL << 7);   // h1
    Bitboard WhiteRookQueenSideSquare = (1ULL << 0);   // a1
    Bitboard BlackRookKingSideSquare  = (1ULL << 63);  // h8
    Bitboard BlackRookQueenSideSquare = (1ULL << 56);  // a8

    // Alleys the king must cross / must be empty to allow castling
    // White: f1,g1 and b1,c1,d1
    // Black: f8,g8 and b8,c8,d8
    Bitboard WhiteRookKingSideAlley   = (1ULL << 5) | (1ULL << 6);        // f1,g1
    Bitboard WhiteRookQueenSideAlley  = (1ULL << 1) | (1ULL << 2) | (1ULL << 3); // b1,c1,d1
    Bitboard BlackRookKingSideAlley   = (1ULL << 61) | (1ULL << 62);      // f8,g8
    Bitboard BlackRookQueenSideAlley  = (1ULL << 57) | (1ULL << 58) | (1ULL << 59); // b8,c8,d8
    

    // -- Constructor --
    board() {
        // Initialize bitboards
        bitboards[P] = 0x000000000000FF00ULL; // White pawns
        bitboards[R] = 0x0000000000000081ULL; // White rooks
        bitboards[N] = 0x0000000000000042ULL; // White knights
        bitboards[B] = 0x0000000000000024ULL; // White bishops
        bitboards[Q] = 0x0000000000000008ULL; // White queen
        bitboards[K] = 0x0000000000000010ULL; // White king

        bitboards[p] = 0x00FF000000000000ULL; // Black pawns
        bitboards[r] = 0x8100000000000000ULL; // Black rooks
        bitboards[n] = 0x4200000000000000ULL; // Black knights
        bitboards[b] = 0x2400000000000000ULL; // Black bishops
        bitboards[q] = 0x0800000000000000ULL; // Black queen
        bitboards[k] = 0x1000000000000000ULL; // Black king

        // Fill in the 64-element array
        chessboard.fill(e);
        // White major pieces
        chessboard[0] = R; chessboard[1] = N; chessboard[2] = B; chessboard[3] = Q;
        chessboard[4] = K; chessboard[5] = B; chessboard[6] = N; chessboard[7] = R;
        // White pawns
        for(int i=8; i<16; i++) chessboard[i] = P;
        // Black pawns
        for(int i=48; i<56; i++) chessboard[i] = p;
        // Black major pieces
        chessboard[56] = r; chessboard[57] = n; chessboard[58] = b; chessboard[59] = q;
        chessboard[60] = k; chessboard[61] = b; chessboard[62] = n; chessboard[63] = r;

        // en passant not available at the start
        en_passant_square = -1;

        King_moved = false;
        Rook_KingSide_moved = false;
        Rook_QueenSide_moved = false;

        Occupied_KingSide_Castling_Alley = false;
        Occupied_QueenSide_Castling_Alley = false;



        // Decide whose turn it is based on number of moves read so far
        boardTurn = (num_of_moves % 2 == 0) ? White : Black;
    }

    Bitboard friendPieces() const {
        return (boardTurn == White)
        ? (bitboards[P] | bitboards[R] | bitboards[N] | bitboards[B] | bitboards[Q] | bitboards[K])
        : (bitboards[p] | bitboards[r] | bitboards[n] | bitboards[b] | bitboards[q] | bitboards[k]);
         ;
    }

    Bitboard opponentPieces() const {
        return (boardTurn == White)
        ? (bitboards[p] | bitboards[r] | bitboards[n] | bitboards[b] | bitboards[q] | bitboards[k])
        : (bitboards[P] | bitboards[R] | bitboards[N] | bitboards[B] | bitboards[Q] | bitboards[K]);
    }


    Bitboard getOccupied() const {
        Bitboard occupied = 0;
        for (const auto& bb : bitboards) {
            occupied |= bb;
        }
        return occupied;
    }

    Bitboard getOccupiedByColor(bool isWhite) const {
        Bitboard occupied = 0;
        int start = isWhite ? 0 : 6;
        for (int i = start; i < start + 6; ++i) {
            occupied |= bitboards[i];
        }
        return occupied;
    }

    void Sides_Update(){
    //King Side update
    if ((boardTurn == White) && (getOccupied() && WhiteRookKingSideAlley)){
            Occupied_KingSide_Castling_Alley = true;
        } else if ((boardTurn == Black) && (getOccupied() && BlackRookKingSideAlley)) {
            Occupied_KingSide_Castling_Alley = true;
        }

    //Queen Side update
    if ((boardTurn == White) && (getOccupied() && WhiteRookQueenSideAlley)){
            Occupied_QueenSide_Castling_Alley = true;
        } else if ((boardTurn == Black) && (getOccupied() && BlackRookQueenSideAlley)) {
            Occupied_QueenSide_Castling_Alley = true;
        }
    }

     // initialising the functions to be defined in moves.hpp
    bool isKingInCheck(Color turn) const;
    std::vector<Move> generateLegalMoves() const;
    std::vector<Move> generatePseudoLegalMoves() const;
    Move generateRandomLegalMove() const;

    void generateCastlingMoves(std::vector<Move>& moves) const;
;
    void generatePromotions(std::vector<Move>& moves) const;
    void generatePawnMoves(std::vector<Move>& moves) const;

    

    // Helper to get char from a PieceType
    char pieceTypeToChar(PieceType pt) const {
        switch(pt) {
            case P: return 'P';  case R: return 'R';  case N: return 'N';  case B: return 'B';
            case Q: return 'Q';  case K: return 'K';  
            case p: return 'p';  case r: return 'r';  case n: return 'n';  case b: return 'b';
            case q: return 'q';  case k: return 'k';
            default: return '.';
        }
    }

    // Return the piece character at a given square [0..63]
    // rank 0 = a1..h1, rank 7 = a8..h8
    char get_piece_at_square(int square) {
        return pieceTypeToChar(chessboard[square]);
    }

    // Remove whichever piece occupies a given one-hot bit position
    //  e.g. pos_bit = (1ULL << 36)
    void remove_piece_at(uint64_t pos_bit) {
        // Find the index of that bit
        int square = __builtin_ctzll(pos_bit);
        if (square < 0 || square >= 64) {
            return; // invalid
        }
        // Which piece is currently there?
        PieceType pt = chessboard[square];
        if (pt == e) {
            return; // Nothing to remove
        }

        // Clear it from that piece's bitboard
        bitboards[pt] &= ~pos_bit;
        // Clear the array
        chessboard[square] = e;
    }

    // Move a piece from src to dst. This is a convenience function
    // that tries to move it in `bitboards` and `chessboard`.
    // Returns true if the piece was found at src and successfully moved
    bool move_piece_in_board(int srcSquare, int dstSquare) {
        PieceType pt = chessboard[srcSquare];
        if (pt == e) {
            return false; // no piece
        }
        // Remove from old location in its bitboard
        bitboards[pt] &= ~(1ULL << srcSquare);
        // Place in new location
        bitboards[pt] |= (1ULL << dstSquare);

        // Update the chessboard array
        chessboard[dstSquare] = pt;
        chessboard[srcSquare] = e;

        return true;
    }

    Bitboard EnemykingPosition(){
        if (boardTurn == White){
            return bitboards[k];
        }else{
            return bitboards[K];
        }
    }

    Bitboard KingPosition(){
        if (boardTurn == White){
            return bitboards[K];
        }else{
            return bitboards[k];
        }
    }



    // Apply a single move to the board, including castling, en passant, promotion
    void apply_move(const ::Move &m) {
        // Keep any derived occupancy / alley info updated if you rely on it.
        // (We might revisit this if it causes side effects.)
        Sides_Update();

        // Convert one-hot bits to integer squares
        int srcSquare = __builtin_ctzll(m.src_pos);
        int dstSquare = __builtin_ctzll(m.dst_pos);

        // Identify which piece is moving
        PieceType movingPiece = chessboard[srcSquare];
        if (movingPiece == e) {
            std::cerr << "Warning: No piece at source square " << srcSquare << "\n";
            return;
        }

        bool isWhiteMoving = isWhitePiece(movingPiece);

        // ============================
        // CASTLING RIGHTS PRE-UPDATE
        // ============================

        // If the king moved, that side cannot castle anymore
        if (movingPiece == K && isWhiteMoving) {
            King_moved = true;
        } else if (movingPiece == k && !isWhiteMoving) {
            King_moved = true;
        }

        // If a rook moved from its original squares, kill that rook's castling right.
        // NOTE: we compare equality (==), not boolean &&
        if (movingPiece == R && isWhiteMoving) {
            if (m.src_pos == WhiteRookKingSideSquare) {
                Rook_KingSide_moved = true;
            }
            if (m.src_pos == WhiteRookQueenSideSquare) {
                Rook_QueenSide_moved = true;
            }
        } else if (movingPiece == r && !isWhiteMoving) {
            if (m.src_pos == BlackRookKingSideSquare) {
                Rook_KingSide_moved = true;
            }
            if (m.src_pos == BlackRookQueenSideSquare) {
                Rook_QueenSide_moved = true;
            }
        }

        // ============================
        // CASTLING MOVE EXECUTION
        // ============================

        bool isCastling = false;
        // White king castles: e1 (4) -> g1 (6) or c1 (2)
        if (movingPiece == K && srcSquare == 4 && (dstSquare == 6 || dstSquare == 2)) {
            isCastling = true;
            if (dstSquare == 6) {
                // White O-O: rook h1(7) -> f1(5)
                move_piece_in_board(7, 5);
            } else {
                // White O-O-O: rook a1(0) -> d1(3)
                move_piece_in_board(0, 3);
            }

            // Move the king itself
            move_piece_in_board(srcSquare, dstSquare);

            // After castling, white's castling rights are gone for sure
            King_moved = true;
            Rook_KingSide_moved = true;
            Rook_QueenSide_moved = true;

            // No en passant target after castling
            en_passant_square = -1;

            // Flip turn
            boardTurn = (boardTurn == White) ? Black : White;
            return;
        }

        // Black king castles: e8 (60) -> g8 (62) or c8 (58)
        if (movingPiece == k && srcSquare == 60 && (dstSquare == 62 || dstSquare == 58)) {
            isCastling = true;
            if (dstSquare == 62) {
                // Black O-O: rook h8(63) -> f8(61)
                move_piece_in_board(63, 61);
            } else {
                // Black O-O-O: rook a8(56) -> d8(59)
                move_piece_in_board(56, 59);
            }

            move_piece_in_board(srcSquare, dstSquare);

            King_moved = true;
            Rook_KingSide_moved = true;
            Rook_QueenSide_moved = true;

            en_passant_square = -1;
            boardTurn = (boardTurn == White) ? Black : White;
            return;
        }

        // ============================
        // EN PASSANT CAPTURE CHECK
        // ============================

        bool enPassantCapture = false;
        if (movingPiece == P || movingPiece == p) {
            int srcFile = srcSquare % 8;
            int srcRank = srcSquare / 8;
            int dstFile = dstSquare % 8;
            int dstRank = dstSquare / 8;

            if (en_passant_square != -1) {
                if (dstSquare == en_passant_square &&
                    (std::abs(dstFile - srcFile) == 1) &&
                    (
                        (isWhiteMoving && dstRank == srcRank + 1) ||
                        (!isWhiteMoving && dstRank == srcRank - 1)
                    )
                ) {
                    // This is en passant
                    enPassantCapture = true;

                    int capturedPawnSquare = isWhiteMoving
                        ? (en_passant_square - 8)  // white takes black pawn behind
                        : (en_passant_square + 8); // black takes white pawn behind

                    remove_piece_at(1ULL << capturedPawnSquare);
                }
            }
        }

        // ============================
        // NORMAL CAPTURE (NOT EN PASSANT)
        // ============================
        if (!enPassantCapture) {
            // If there's an enemy piece actually on dstSquare, remove it
            remove_piece_at(m.dst_pos);
        }

        // ============================
        // PHYSICALLY MOVE PIECE src->dst
        // ============================
        bool movedOk = move_piece_in_board(srcSquare, dstSquare);
        if (!movedOk) {
            std::cerr << "Warning: move_piece_in_board failed from "
                    << srcSquare << " to " << dstSquare << "\n";
            en_passant_square = -1;
            boardTurn = (boardTurn == White) ? Black : White;
            return;
        }

        // ============================
        // PROMOTION
        // ============================
        if ((movingPiece == P || movingPiece == p) && (m.promotion != '\0')) {
            int dstRank = dstSquare / 8;
            bool whitePromote = (movingPiece == P && dstRank == 7);
            bool blackPromote = (movingPiece == p && dstRank == 0);

            if (whitePromote || blackPromote) {
                PieceType newPT = e;
                switch (m.promotion) {
                    case 'q': newPT = (isWhiteMoving ? Q : q); break;
                    case 'r': newPT = (isWhiteMoving ? R : r); break;
                    case 'b': newPT = (isWhiteMoving ? B : b); break;
                    case 'n': // 'n' for knight, also accept 'k' if you used that internally
                    case 'k': newPT = (isWhiteMoving ? N : n); break;
                    default:
                        std::cerr << "Warning: Invalid promotion char " << m.promotion << "\n";
                        break;
                }

                // clear pawn bit at dst
                bitboards[movingPiece] &= ~(1ULL << dstSquare);
                // set new piece bit
                bitboards[newPT] |= (1ULL << dstSquare);
                // fix mailbox
                chessboard[dstSquare] = newPT;
            }
        }

        // ============================
        // EN PASSANT TARGET SQUARE UPDATE
        // ============================
        // default: no en passant available
        en_passant_square = -1;

        // white pawn double push from rank 2 -> 4 (1 -> 3)
        if (movingPiece == P) {
            int srcRank = srcSquare / 8;
            int dstRank = dstSquare / 8;
            if (srcRank == 1 && dstRank == 3) {
                en_passant_square = srcSquare + 8; // the square it "jumped over"
            }
        }
        // black pawn double push from rank 7 -> 5 (6 -> 4)
        else if (movingPiece == p) {
            int srcRank = srcSquare / 8;
            int dstRank = dstSquare / 8;
            if (srcRank == 6 && dstRank == 4) {
                en_passant_square = srcSquare - 8;
            }
        }

        // ============================
        // SWITCH SIDE TO MOVE
        // ============================
        boardTurn = (boardTurn == White) ? Black : White;
    }


    // Apply a sequence of moves to the board
    void apply_moves(const std::vector<::Move> &moves) {
        for (const auto &m : moves) {
            apply_move(m);
        }
    }

    // Print the board after applying moves from inputfile
    void print_board(const char* inputfile) {
        // 1) read the moves from the file
        std::vector<::Move> moves = read_inputfile(inputfile);
        // 2) apply them
        apply_moves(moves);

        // 3) print
        std::cout << "     a   b   c   d   e   f   g   h  \n";
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << " " << (rank + 1) << " ";
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                char piece = get_piece_at_square(square);
                std::cout << "| " << piece << " ";
            }
            std::cout << "| " << (rank + 1) << "\n";
            std::cout << "   +---+---+---+---+---+---+---+---+\n";
        }
        std::cout << "     a   b   c   d   e   f   g   h  \n";
    }
};

#endif //BOARD_HPP