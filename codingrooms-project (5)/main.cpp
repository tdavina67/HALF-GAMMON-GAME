//******************************************************************************
// PROGRAM : HALFGAMMON UNDONE 
// COURSE : CS 141 - PROGRAM DESIGN II
// DAVINA TITUS 
// DATE : 16TH APRIL 2025

//DESPCRIPTION:

//******************************************************************************
#include <iostream>
#include <string>
#include "HalfGammonBoard.h"
#include "mersenne-twister.h"

using namespace std;

int rollDie();

// Simulates rolling a die, choosing a result 1 to 6
// The seed function must have already been called
// Returns an int, chosen randomly, 1-6
int rollDie() {
	return chooseRandomNumber(1, 6);
}

class UndoStack {
private:
    struct UndoNode {
    HalfGammonBoard boardState;
    UndoNode* next;

    UndoNode(HalfGammonBoard state, UndoNode* nextNode = nullptr) 
        : boardState(state), next(nextNode) {}
};
    UndoNode* head;
    int size;

public:
    UndoStack() : head(nullptr), size(0) {}
    ~UndoStack() { clear(); }

    void push( const HalfGammonBoard& state){
        head = new UndoNode(state,head);
        size++;
    }

    bool pop(HalfGammonBoard& outState) {
        if (isEmpty()) return false;
        
        UndoNode* temp = head;
        outState = head->boardState;
        head = head->next;
        delete temp;
        size--;
        return true;
    }

	HalfGammonBoard top() const {
        return head->boardState;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    int getSize() const {
        return size;
    }

    void clear() {
        while (!isEmpty()){
            HalfGammonBoard temp;
            pop(temp);
        }
    }
};

int main() {
	// Initializes the random number generator with seed from the user
	int randSeed;
	cout << "Enter seed: ";
	cin >> randSeed;
	seed(randSeed);

	// Repeat, allows user to play multiple games
	string keepPlaying;
	do {
		// Game board used to keep track of the current game
		HalfGammonBoard board;
        UndoStack undoStack;

		int roll = rollDie();
        undoStack.push(board);

		// Display the board and roll dice
		board.displayBoard();
		board.displayRoll(roll);

		while (!board.gameOver()) {
			// If the player has a bumped piece, then they must move that
			if (board.hasBumpedPiece()) {
				cout << "Bumped checker must move." << endl;
				if (!board.isMovePossible(roll)) {
					cout << "No move possible." << endl;
				}
				else {
					undoStack.push(board);
                    board.moveBumpedPiece(roll);
				}
			}
			else {
				// Otherwise they can move any of their pieces
				if (!board.isMovePossible(roll)) {
					// No move is possiible with their roll on the current board
					cout << "No move possible." << endl << endl;
				}
				else {
					string movePositionString;
					bool moveSuccessful;

					// Get move from user, and make that move. Repeat loop until a valid move has been chosen
					cout << "What position would you like to move (Q to quit, U to undo)? ";
					cin >> movePositionString;

                    //Handle quit command
					if (movePositionString == "q" || movePositionString == "Q") {
						break;
					} // Handle undo command 
                     if (movePositionString=="u" || movePositionString=="U"){
                        // Cant undo initial state
                        if (undoStack.getSize() <= 1){
                            cout << "Cannot undo." << endl;
                        } else {
                            HalfGammonBoard prevState;
                            undoStack.pop(prevState); 
							board = undoStack.top();

                            board.displayBoard();
                            roll = rollDie();
                            board.displayRoll(roll);  
                        }
                        continue;
                    }

                    undoStack.push(board);
                    int movePosition = stoi(movePositionString);
					moveSuccessful = board.performMove(movePosition, roll);
					if (!moveSuccessful) {
						cout << "Invalid move. Try again." << endl;
						// Continue, so the user can enter input again
						// (this skips changing current player and displaying the board, rolling dice)
						continue;
					}
				}
			}
			// Switch who the current player is
			board.changePlayer();

			if (!board.gameOver()) {
				// Display the board and roll dice
				board.displayBoard();
				roll = rollDie();
				board.displayRoll(roll);
				undoStack.push(board);
			}
		}

		// If we have left the loop, someone has won--determine whether it's X or O
		if (board.isXWin()) {
			cout << "Player X Wins!" << endl;
		}
		else if (board.isOWin()) {
			cout << "Player O Wins!" << endl;
		}

		cout << endl;
		cout << "Do you want to play again (y/n)? ";
		cin >> keepPlaying;
	} while (tolower(keepPlaying.at(0)) == 'y');

    return 0;
}
