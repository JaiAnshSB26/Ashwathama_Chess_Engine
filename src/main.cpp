#include "board.hpp"
#include <cstring>
#include "evaluate.hpp"
#include "file_interpreter.hpp"
#include "utils.hpp"
#include "uci.hpp"
#include <sys/stat.h> // For checking file existence

// Function to check if a file exists
bool fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Function to ensure moves.csv exists
void ensureMovesFileExists(const char* filename) {
    if (!fileExists(filename)) {
        std::ofstream ofs(filename);
        if (!ofs) {
            std::cerr << "Error: Could not create moves.csv" << std::endl;
            exit(1); // Exit with error if file creation fails
        }
        ofs.close();
    }
}
int main(int argc, char* argv[]) {
    // Check for UCI mode
    if (argc > 1 && std::strcmp(argv[1], "--uci") == 0) {
        UCI::run_uci_loop();
        return 0;
    }

    //if (argc != 3){
    //    std::cerr << "Error: More than 3 arguments given!" << std::endl;
    //    return 1;
    //};

    // argv[1] = input file
    // argv[2] = output file
    const char* input_file = "tests/history.csv";
    const char* output_file = "move.csv";

    // Parse command-line arguments
    // Usage: ./Ashwathama -H <history_file> -m <move_file>
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-H") == 0 && (i + 1 < argc)) {
            input_file = argv[i + 1];
            i++;
        } else if (std::strcmp(argv[i], "-m") == 0 && (i + 1 < argc)) {
            output_file = argv[i + 1];
            i++;
        }
    }
    // Ensure moves.csv exists
    ensureMovesFileExists(output_file);

     // 1) Create a board object
    board chess_board;

    // 2) Print the board after applying moves from the history file.
    //    (Internally, 'print_board' calls 'read_inputfile' -> 'apply_moves'.)
    std::cout << "Chess Board after applying moves from: " << input_file << "\n";
    chess_board.print_board(input_file);

    // 3) Now that the board reflects all moves from history.csv,
    //    let's run a minimax + alpha-beta search to find the best next move.
    Evaluator evaluator;
    evaluator.max_depth = 3; // or any desired depth

    std::cout << "Running alpha-beta search...\n";
    evaluator.test_evaluate(chess_board);
    std::cout << "Testing Random Move Generator:\n";
    evaluator.test_random_move(chess_board);
    
    Move bestMove = evaluator.get_best_move(chess_board);


    // 4) Overwrite moves.csv with the next move
    writeMoveToFile(output_file, bestMove);

    std::cout << "Next move written to: " << output_file << "\n";


    return 0;

   
    


}
