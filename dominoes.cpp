#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
using namespace std;

// ============================================================================
// CDominoes Class
// ============================================================================
class CDominoes {
private:
    int head;
    int tail;

public:
    CDominoes(int h, int t) : head(h), tail(t) {}
    int getHead() const { return head; }
    int getTail() const { return tail; }

    void flip() {
        int temp = head;
        head = tail;
        tail = temp;
    }

    bool canMatch(int num) const {
        return head == num || tail == num;
    }

    void display() const {
        cout << "[" << head << "|" << tail << "]";
    }
};

// ============================================================================
// CPlayer Class
// ============================================================================
class CPlayer {
private:
    string name;
    vector<CDominoes*> hand;

public:
    CPlayer(string n) : name(n) {}

    string getName() const { return name; }
    int getHandSize() const { return hand.size(); }

    void addDomino(CDominoes* d) { hand.push_back(d); }

    CDominoes* getDomino(int index) {
        if (index < 0 || index >= (int)hand.size()) return nullptr;
        return hand[index];
    }

    CDominoes* removeDomino(int index) {
        if (index < 0 || index >= (int)hand.size()) return nullptr;
        CDominoes* d = hand[index];
        hand.erase(hand.begin() + index);
        return d;
    }

    void displayHand() const {
        cout << name << "'s Hand: ";
        for (int i = 0; i < (int)hand.size(); i++) {
            cout << i << ":";
            hand[i]->display();
            cout << " ";
        }
        cout << endl;
    }

    bool hasPlayable(int head, int tail) const {
        for (auto d : hand)
            if (d->canMatch(head) || d->canMatch(tail))
                return true;
        return false;
    }
};

// ============================================================================
// CTable Class
// ============================================================================
class CTable {
private:
    vector<CDominoes*> playedDominoes;
    vector<CDominoes*> availablePieces;
    CPlayer* player1;
    CPlayer* player2;
    CPlayer* currentPlayer;
    int currentHead;
    int currentTail;

public:
    CTable(string name1, string name2) {
        generateDominoes();
        shuffleDominoes();

        player1 = new CPlayer(name1);
        player2 = new CPlayer(name2);

        // Deal 7 to each player
        for (int i = 0; i < 7; i++) {
            player1->addDomino(takeFromAvailable());
            player2->addDomino(takeFromAvailable());
        }

        currentPlayer = player1;
        currentHead = -1;
        currentTail = -1;
    }

    ~CTable() {
        delete player1;
        delete player2;
        for (auto d : availablePieces) delete d;
        for (auto d : playedDominoes) delete d;
    }

    void generateDominoes() {
        for (int i = 0; i <= 6; i++) {
            for (int j = i; j <= 6; j++) {
                availablePieces.push_back(new CDominoes(i, j));
            }
        }
    }

    void shuffleDominoes() {
        srand(time(0));
        random_shuffle(availablePieces.begin(), availablePieces.end());
    }

    CDominoes* takeFromAvailable() {
        if (availablePieces.empty()) return nullptr;
        CDominoes* piece = availablePieces.back();
        availablePieces.pop_back();
        return piece;
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == player1) ? player2 : player1;
    }

    bool isGameOver() const {
        return player1->getHandSize() == 0 || player2->getHandSize() == 0;
    }

    void displayGameState() const {
        cout << "\nTable: ";
        if (playedDominoes.empty()) cout << "[empty]";
        else {
            for (auto d : playedDominoes) d->display();
        }
        cout << "\nHead: " << currentHead << " | Tail: " << currentTail << endl;
        cout << "Remaining in pool: " << availablePieces.size() << endl;
    }

    void placeDominoAtHead(CDominoes* d) {
        playedDominoes.insert(playedDominoes.begin(), d);
        currentHead = d->getHead();
        if (playedDominoes.size() == 1) currentTail = d->getTail();
    }

    void placeDominoAtTail(CDominoes* d) {
        playedDominoes.push_back(d);
        currentTail = d->getTail();
        if (playedDominoes.size() == 1) currentHead = d->getHead();
    }

    // =========================================================================
    // Human Turn Function
    // =========================================================================
    bool playTurn() {
        cout << "\n+++ " << currentPlayer->getName() << "'s Turn +++" << endl;
        currentPlayer->displayHand();

        // Prompt for action
        string input;
        cout << "Enter domino index to play (0-" << currentPlayer->getHandSize() - 1
             << "), 'draw' to draw, or 'exit' to quit: ";
        getline(cin, input);

        if (input == "exit" || input == "Exit" || input == "EXIT") {
            cout << "\nGame exited by user.\n";
            exit(0);
        }

        if (input == "draw" || input == "Draw") {
            if (availablePieces.empty()) {
                cout << "No dominoes left to draw!\n";
            } else {
                CDominoes* drawnPiece = takeFromAvailable();
                currentPlayer->addDomino(drawnPiece);
                cout << currentPlayer->getName() << " drew ";
                drawnPiece->display();
                cout << endl;
            }
            switchPlayer();
            return true;
        }

        // Convert to index
        int index;
        try {
            index = stoi(input);
        } catch (...) {
            cout << "Invalid input.\n";
            return true;
        }

        if (index < 0 || index >= currentPlayer->getHandSize()) {
            cout << "Invalid index.\n";
            return true;
        }

        CDominoes* selectedPiece = currentPlayer->getDomino(index);
        if (!selectedPiece) {
            cout << "Invalid domino selected.\n";
            return true;
        }

        // Check if playable
        if (!playedDominoes.empty() && !selectedPiece->canMatch(currentHead) && !selectedPiece->canMatch(currentTail)) {
            cout << "That domino cannot be played on either side. Try again.\n";
            return true;
        }

        // Ask for side
        string side;
        if (!playedDominoes.empty()) {
            cout << "Play on 'head' or 'tail'? ";
            getline(cin, side);
            if (side == "exit") {
                cout << "\nGame exited by user.\n";
                exit(0);
            }
        } else {
            side = "tail";
        }

        selectedPiece = currentPlayer->removeDomino(index);

        if (side == "head") {
            if (selectedPiece->getTail() != currentHead) selectedPiece->flip();
            placeDominoAtHead(selectedPiece);
        } else {
            if (selectedPiece->getHead() != currentTail) selectedPiece->flip();
            placeDominoAtTail(selectedPiece);
        }

        cout << currentPlayer->getName() << " played ";
        selectedPiece->display();
        cout << " on the " << side << ".\n";

        switchPlayer();
        return true;
    }

    void displayFinalResults() const {
        cout << "\n===================================" << endl;
        cout << "            GAME OVER              " << endl;
        cout << "===================================\n";

        if (player1->getHandSize() == 0)
            cout << player1->getName() << " wins!\n";
        else if (player2->getHandSize() == 0)
            cout << player2->getName() << " wins!\n";
        else
            cout << "Game ended prematurely.\n";
    }
};

// ============================================================================
// MAIN
// ============================================================================
int main() {
    cout << "========================================" << endl;
    cout << "      DOMINOES GAME - 2 PLAYER MODE     " << endl;
    cout << "========================================" << endl;

    string name1, name2;
    cout << "Enter Player 1 name: ";
    getline(cin, name1);
    cout << "Enter Player 2 name: ";
    getline(cin, name2);

    CTable* table = new CTable(name1, name2);

    cout << "\n[GAME INITIALIZED]" << endl;
    cout << "Type 'exit' anytime to quit.\n";

    int turnCount = 0;
    while (!table->isGameOver()) {
        turnCount++;
        cout << "\n--- Turn " << turnCount << " ---" << endl;
        table->playTurn();
        table->displayGameState();
    }

    table->displayFinalResults();
    delete table;

    cout << "\n[Game ended safely. Goodbye!]" << endl;
    return 0;
}
