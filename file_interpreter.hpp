// file_interpreter.hpp
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>  // For parsing CSV lines
#include <vector>
#include <stdint.h>
#include "utils.hpp" // Include utils.hpp to use the centralized Move structure

// Read moves from a CSV file
std::vector<Move> read_inputfile(const char* inputfile) {
    std::ifstream inputFile(inputfile);

    if (!inputFile) {
        std::cerr << "Error: could not open History file: " << inputfile << std::endl;
        return {};
    }

    std::vector<Move> moves;
    std::string line;

    while (std::getline(inputFile, line)) {
        // Parse each line as a CSV (move in long algebraic UCI format)
        std::istringstream lineStream(line);
        std::string moveStr;
        char promotion = '\0';

        // Read the move string
        if (std::getline(lineStream, moveStr, ',')) {
            if (moveStr.length() < 4 || moveStr.length() > 5) {
                std::cerr << "Error: invalid move format: " << moveStr << std::endl;
                continue;
            }

            char src_file = moveStr[0];
            char src_rank = moveStr[1];
            char dst_file = moveStr[2];
            char dst_rank = moveStr[3];

            if (moveStr.length() == 5) {
                promotion = moveStr[4];
            }

            // Convert to 0-based indices
            int src_file_index = src_file - 'a';
            int src_rank_index = src_rank - '1';
            int dst_file_index = dst_file - 'a';
            int dst_rank_index = dst_rank - '1';

            if (src_file_index < 0 || src_file_index > 7 ||
                src_rank_index < 0 || src_rank_index > 7 ||
                dst_file_index < 0 || dst_file_index > 7 ||
                dst_rank_index < 0 || dst_rank_index > 7) {
                std::cerr << "Error: invalid move coordinates in move: " << moveStr << std::endl;
                continue;
            }

            int squareSrc = src_rank_index * 8 + src_file_index;
            int squareDst = dst_rank_index * 8 + dst_file_index;

            // One-hot bit
            Bitboard src_pos = (1ULL << squareSrc);
            Bitboard dst_pos = (1ULL << squareDst);

            // Create a Move object from utils.hpp
            Move move;
            move.src_pos = src_pos;
            move.dst_pos = dst_pos;
            move.promotion = promotion;

            // Detect if the move is a castling move
            // Castling moves are king moves from e1 to g1/c1 or e8 to g8/c8
            if ((squareSrc == 4 && (squareDst == 6 || squareDst == 2)) ||
                (squareSrc == 60 && (squareDst == 62 || squareDst == 58))) {
                move.is_castling = true;
            }

            // Add the move to the list
            moves.push_back(move);
            num_of_moves++;
        }
    }

    inputFile.close();
    std::cout << "Successfully read " << moves.size() << " moves from the file.\n";
    return moves;
}
