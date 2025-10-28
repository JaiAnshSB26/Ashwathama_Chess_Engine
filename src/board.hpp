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
        Sides_Update();
        // Convert one-hot bits to integer squares
        int srcSquare = __builtin_ctzll(m.src_pos);
        int dstSquare = __builtin_ctzll(m.dst_pos);

        // Identify which piece is moving
        PieceType movingPiece = chessboard[srcSquare];
        if (movingPiece == e) {
            std::cerr << "Warning: No piece found at source position for this move.\n";
            return;
        }
        bool isWhiteMoving = isWhitePiece(movingPiece);

        //CASTLING RIGHTS UPDATES
        //King_moved update
        if (movingPiece == K && isWhiteMoving){
            King_moved = true;
        } else if (movingPiece == k && !isWhiteMoving) {
            King_moved = true;
        }
        //Rook_KingSide_moved update
        if (movingPiece == R && isWhiteMoving && (WhiteRookKingSideSquare && m.src_pos)){
            Rook_KingSide_moved = true;
        } else if (movingPiece == r && !isWhiteMoving && (BlackRookKingSideSquare && m.src_pos)) {
            Rook_KingSide_moved = true;
        }
        //Rook_QueenSide_moved update
        if (movingPiece == R && isWhiteMoving && (WhiteRookQueenSideSquare && m.src_pos)){
            Rook_KingSide_moved = true;
        } else if (movingPiece == r && !isWhiteMoving && (BlackRookQueenSideSquare && m.src_pos)) {
            Rook_KingSide_moved = true;
        }


        // CASTLING DETECTION 
        // Example: White King from e1 -> g1 = squares 4 -> 6
        bool isCastling = false;
        if (movingPiece == K && (srcSquare == 4) && (dstSquare == 6 || dstSquare == 2)) {
            // White king castling
            isCastling = true;
            if (dstSquare == 6) {
                // White kingside: move rook from h1(7) to f1(5)
                move_piece_in_board(7, 5);
            } else {
                // White queenside: move rook from a1(0) to d1(3)
                move_piece_in_board(0, 3);
            }
        } else if (movingPiece == k && (srcSquare == 60) && (dstSquare == 62 || dstSquare == 58)) {
            // Black king castling
            isCastling = true;
            if (dstSquare == 62) {
                // Black kingside: move rook from h8(63) to f8(61)
                move_piece_in_board(63, 61);
            } else {
                // Black queenside: move rook from a8(56) to d8(59)
                move_piece_in_board(56, 59);
            }
        }

        // If castling, we just do the king move, remove en passant. Done
        if (isCastling) {
            // Move the king
            move_piece_in_board(srcSquare, dstSquare);
            en_passant_square = -1;
            // Switch turn
            boardTurn = (boardTurn == White) ? Black : White;
            return;
        }

        // EN PASSANT DETECTION
        // Check if the piece is a pawn that moves diagonally into an empty square
        bool enPassantCapture = false;
        if ((movingPiece == P || movingPiece == p)) {
            int srcFile = srcSquare % 8, srcRank = srcSquare / 8;
            int dstFile = dstSquare % 8, dstRank = dstSquare / 8;
            if (en_passant_square != -1) {
                if (dstSquare == en_passant_square && 
                    (std::abs(dstFile - srcFile) == 1) && 
                    ((isWhiteMoving && dstRank == srcRank + 1) || 
                     (!isWhiteMoving && dstRank == srcRank - 1))) {
                    // en passant capture
                    enPassantCapture = true;
                    int capturedPawnSquare = isWhiteMoving 
                                             ? (en_passant_square - 8) 
                                             : (en_passant_square + 8);
                    remove_piece_at(1ULL << capturedPawnSquare);
                }
            }
        }

        // If normal capture (not en-passant), remove piece at dst if any
        if (!enPassantCapture) {
            remove_piece_at(m.dst_pos);
        }

        // Move the piece from src to dst
        bool movedOk = move_piece_in_board(srcSquare, dstSquare);
        if (!movedOk) {
            std::cerr << "Warning: Could not move piece from " 
                      << srcSquare << " to " << dstSquare << "\n";
            en_passant_square = -1;
            // Switch turn anyway
            boardTurn = (boardTurn == White) ? Black : White;
            return;
        }

        // PROMOTION
        if ((movingPiece == P || movingPiece == p) && (m.promotion != '\0')) {
            // For white, rank=7; for black, rank=0
            int rank = dstSquare / 8;
            if ((movingPiece == P && rank == 7) || (movingPiece == p && rank == 0)) {
                // Convert char e.g. 'q' to PieceType
                // (Notice your old code used 'k' to mean knight, but let's adapt.)
                PieceType newPT = e;
                switch(m.promotion) {
                    case 'q': newPT = (isWhiteMoving ? Q : q); break;
                    case 'r': newPT = (isWhiteMoving ? R : r); break;
                    case 'b': newPT = (isWhiteMoving ? B : b); break;
                    case 'k': // ironically 'k' means knight in your code
                    case 'n': newPT = (isWhiteMoving ? N : n); break;
                    default:
                        std::cerr << "Warning: Invalid promotion char: " << m.promotion << "\n";
                        break;
                }
                // Remove the old pawn from the bitboard
                bitboards[movingPiece] &= ~(1ULL << dstSquare);
                // Set the new piece
                bitboards[newPT] |= (1ULL << dstSquare);
                // Update the chessboard array
                chessboard[dstSquare] = newPT;
            }
        }

        // EN PASSANT SQUARE SETTING (double push from rank1->3 or rank6->4)
        en_passant_square = -1; // default
        if ((movingPiece == P && (srcSquare / 8) == 1 && (dstSquare / 8) == 3)) {
            en_passant_square = srcSquare + 8; // rank 2
        } else if ((movingPiece == p && (srcSquare / 8) == 6 && (dstSquare / 8) == 4)) {
            en_passant_square = srcSquare - 8; // rank 5
        }

        // Switch turn
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