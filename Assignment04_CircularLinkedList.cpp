#include <iostream>
#include <string>
#include <random>

using namespace std;

// struct for each space on the board
struct Property {
    string name;
    int cost;
    string owner;
    Property* next;

    Property(string propName, int propCost) {
        name = propName;
        cost = propCost;
        owner = "Bank";
        next = nullptr;
    }
};

// struct to track player stats
struct Player {
    string name;
    int money;
    Property* currentPosition;

    Player(string playerName, int startingMoney, Property* startTile) {
        name = playerName;
        money = startingMoney;
        currentPosition = startTile;
    }
};

// custom circular linked list
class MonopolyBoard {
private:
    Property* head;

public:
    MonopolyBoard() {
        head = nullptr;
    }

    Property* getHead() {
        return head;
    }

    // add new property to the list
    void addProperty(string name, int cost) {
        Property* newProp = new Property(name, cost);
        if (head == nullptr) {
            head = newProp;
            head->next = head; // wrap back around
        }
        else {
            Property* temp = head;
            while (temp->next != head) {
                temp = temp->next;
            }
            temp->next = newProp;
            newProp->next = head; // keep circular
        }
    }

    // display the whole board
    void printBoard() {
        if (head == nullptr) {
            cout << "Board is empty!\n";
            return;
        }
        Property* temp = head;
        cout << "\n=== MONOPOLY BOARD PROPERTIES ===\n";
        do {
            cout << "- " << temp->name << " ($" << temp->cost
                << ") | Owner: " << temp->owner << "\n";
            temp = temp->next;
        } while (temp != head);
        cout << "=================================\n\n";
    }

    // search for property by name
    Property* searchProperty(string name) {
        if (head == nullptr) return nullptr;
        Property* temp = head;
        do {
            if (temp->name == name) {
                return temp;
            }
            temp = temp->next;
        } while (temp != head);
        return nullptr;
    }

    // count how many properties a given owner holds, and list them
    int countOwned(string ownerName) {
        if (head == nullptr) return 0;

        int count = 0;
        Property* temp = head;
        do {
            if (temp->owner == ownerName) {
                cout << "   - " << temp->name << " ($" << temp->cost << ")\n";
                count++;
            }
            temp = temp->next;
        } while (temp != head);

        return count;
    }

    // remove a property by name
    bool removeProperty(string name) {
        if (head == nullptr) return false;
        Property* current = head;
        Property* prev = nullptr;

        // find tail node first to handle head deletion
        Property* tail = head;
        while (tail->next != head) {
            tail = tail->next;
        }

        do {
            if (current->name == name) {
                if (current == head) {
                    if (head->next == head) { // only 1 node
                        delete head;
                        head = nullptr;
                    }
                    else {
                        head = head->next;
                        tail->next = head;
                        delete current;
                    }
                }
                else {
                    prev->next = current->next;
                    delete current;
                }
                return true;
            }
            prev = current;
            current = current->next;
        } while (current != head);
        return false;
    }

    // clean up all nodes
    ~MonopolyBoard() {
        if (head == nullptr) return;
        Property* current = head->next;
        while (current != head) {
            Property* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        delete head;
    }
};

// game simulation
int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution dice(1, 6);

    MonopolyBoard board;

    // set up board with 10 properties
    board.addProperty("GO", 0);
    board.addProperty("Mediterranean Ave", 60);
    board.addProperty("Baltic Ave", 60);
    board.addProperty("Reading Railroad", 200);
    board.addProperty("Vermont Ave", 100);
    board.addProperty("Connecticut Ave", 120);
    board.addProperty("St. Charles Place", 140);
    board.addProperty("States Ave", 140);
    board.addProperty("Virginia Ave", 160);
    board.addProperty("Boardwalk", 400);

    board.printBoard();

    Player p1("D'marreion", 1000, board.getHead());
    Player p2("Kaylee", 1000, board.getHead());

    cout << "--- STARTING 10-TURN SIMULATION ---\n";

    // run 10 turns
    for (int turn = 1; turn <= 10; turn++) {
        cout << "\n--- TURN " << turn << " ---\n";
        Player* currentPlayer = (turn % 2 != 0) ? &p1 : &p2;

        int roll = dice(gen);
        cout << currentPlayer->name << " rolled a " << roll << "!\n";

        // move player through circular linked list
        for (int i = 0; i < roll; i++) {
            currentPlayer->currentPosition = currentPlayer->currentPosition->next;
        }

        Property* landedProp = currentPlayer->currentPosition;
        cout << currentPlayer->name << " landed on " << landedProp->name << ".\n";

        if (landedProp->name == "GO") {
            currentPlayer->money += 200;
            cout << currentPlayer->name << " passed GO and collected $200! Cash: $" << currentPlayer->money << "\n";
        }
        else if (landedProp->owner == "Bank") {
            if (currentPlayer->money >= landedProp->cost) {
                currentPlayer->money -= landedProp->cost;
                landedProp->owner = currentPlayer->name;
                cout << currentPlayer->name << " bought " << landedProp->name
                    << " for $" << landedProp->cost << "! Remaining cash: $" << currentPlayer->money << "\n";
            }
            else {
                cout << currentPlayer->name << " couldn't afford " << landedProp->name << ".\n";
            }
        }
        else if (landedProp->owner != currentPlayer->name) {
            int rent = landedProp->cost / 2;
            currentPlayer->money -= rent;
            cout << landedProp->name << " is owned by " << landedProp->owner
                << "! " << currentPlayer->name << " pays $" << rent << " rent.\n";
        }
        else {
            cout << currentPlayer->name << " landed on their own property.\n";
        }
    }

    board.printBoard();

    cout << "Testing search for 'Boardwalk'...\n";
    Property* found = board.searchProperty("Boardwalk");
    if (found) {
        cout << "Found property: " << found->name << " ($" << found->cost << ")\n";
    }

    cout << "\nTesting removing 'Baltic Ave'...\n";
    if (board.removeProperty("Baltic Ave")) {
        cout << "Successfully removed Baltic Ave!\n";
    }

    board.printBoard();

    // final player status, with a property count twist
    cout << "\n--- FINAL PLAYER STATUS ---\n";
    cout << p1.name << ": $" << p1.money << "\n";
    cout << " Properties owned:\n";
    int p1Count = board.countOwned(p1.name);
    if (p1Count == 0) cout << "   (none)\n";

    cout << p2.name << ": $" << p2.money << "\n";
    cout << " Properties owned:\n";
    int p2Count = board.countOwned(p2.name);
    if (p2Count == 0) cout << "   (none)\n";

    return 0;
}