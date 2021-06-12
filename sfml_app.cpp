#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_set>
#include "Connector.hpp"

// for hashing pairs in unordered_set
struct pair_hash {
    inline std::size_t operator()(const std::pair<int, int>& v) const {
        return v.first * 31 + v.second;
    }
};

using namespace sf;

#define QUEEN  1 
#define KING   2
#define ROOK   3
#define KNIGHT 4
#define BISHOP 5
#define PAWN   6

// True for on, false for off
#define RANDOMMOVE_ON true
// chess piece size
int size = 60;
std::string position = "";
bool isGameOver = false;
bool isBlackCheck = false;
bool isWhiteCheck = false;
std::pair<int, int> selectedPiece = { -1, -1 };
std::pair<int, int> whiteKingPos = {7, 4}; // y, x
std::pair<int, int> blackKingPos = { 0, 4 }; // y, x
std::unordered_set<std::pair<int, int>, pair_hash> availableMoves;
// used for castling
bool whiteKingMoved = false;
bool blackKingMoved = false;
// used for turn keeping
bool isWhiteTurn = true;
// board initual layout
int board[8][8] =
{ -3,-4,-5,-1,-2,-5,-4,-3,
 -6,-6,-6,-6,-6,-6,-6,-6,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 6, 6, 6, 6,
  3, 4, 5, 1, 2, 5, 4, 3 };
// chess pieces
Sprite f[32];

//functions

bool inBounds(int x, int y) {
    return (x >= 0 && x < 8) && (y >= 0 && y < 8);
}
std::string toChessNote(Vector2f p)
{
    std::string s = "";
    s += char(p.x / size + 97);
    s += char(7 - p.y / size + 49);
    return s;
}
// sets the initial positions of the pieces

void loadPosition()
{
    int k = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            int n = board[i][j];
            if (!n) continue;
            int x = abs(n) - 1;
            int y = n > 0 ? 1 : 0;
            f[k].setTextureRect(IntRect(size * x, size * y, size, size));
            f[k].setPosition(size * j, size * i);
            k++;
        }

    for (int i = 0; i < position.length(); i += 5)
        move(position.substr(i, 4));
}

// draw blank chess board
void drawBoard(RenderWindow & window) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            auto colour = ((i + j) % 2 == 1) ? Color(118, 150, 86) : Color(238, 238, 210);
            sf::RectangleShape rectangle(Vector2f(size,size));
            rectangle.setFillColor(colour);
            rectangle.setPosition(60 * i, 60 * j);
            window.draw(rectangle);
        }
    }
}
Vector2f toCoord(char a, char b)
{
    int x = int(a) - 97;
    int y = 7 - int(b) + 49;
    return Vector2f(x * size, y * size);
}
void setQueenSprite(Sprite sprite, bool isWhite, Vector2f newPos) {
    Texture pieces;
    pieces.loadFromFile("images/pieces.png");
    sprite.setTexture(pieces);
    int y = isWhite ? 1 : 0;
    sprite.setTextureRect(IntRect(0, size * y, size, size));
    sprite.setPosition(newPos);
    std::cout << "Here";
}
void move(std::string str)
{
    std::cout << str << std::endl;
    Vector2f oldPos = toCoord(str[0], str[1]);
    Vector2f newPos = toCoord(str[2], str[3]);
    int piece = board[int(oldPos.y) / size][int(oldPos.x) / size];
    if (piece == 0)
        return;
    std::cout << "Piece: " << piece << " x y:" << int(oldPos.x) / size << " " << int(oldPos.y) / size << std::endl;
    board[int(oldPos.y) / size][int(oldPos.x) / size] = 0;
    board[int(newPos.y) / size][int(newPos.x) / size] = piece;
    if (piece == KING) {
        whiteKingPos = { int(newPos.y) / size ,int(newPos.x) / size };
        if (oldPos != newPos)
            whiteKingMoved = true;
    }
    else if (piece == -KING) {
        blackKingPos = { int(newPos.y) / size ,int(newPos.x) / size };
        if (oldPos != newPos)
            blackKingMoved = true;
    }
    bool isQueening = false;
    // queening
    if (piece == PAWN && int(newPos.y) / size == 0) {
        board[0][int(newPos.x) / size] = QUEEN;
        isQueening = true;
    }
    if (piece == -PAWN && int(newPos.y) / size == 7) {
        board[7][int(newPos.x) / size] = -QUEEN;
        isQueening = true;
    }
    bool isWhite = piece > 0;
    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == newPos) f[i].setPosition(-100, -100);

    for (int i = 0; i < 32; i++) {
        if (f[i].getPosition() == oldPos) {
            f[i].setPosition(newPos);
        }
    }

    //castling       //if the king didn't move
    if (str == "e1g1") if (position.find("e1") == -1) move("h1f1");
    if (str == "e8g8") if (position.find("e8") == -1) move("h8f8");
    if (str == "e1c1") if (position.find("e1") == -1) move("a1d1");
    if (str == "e8c8") if (position.find("e8") == -1) move("a8d8");
}
// draws a blue oblique square on selected
void drawSelectedPiece(RenderWindow& window) {
    if (selectedPiece.first == -1 || board[selectedPiece.first][selectedPiece.second] == 0)
        return;
    sf::RectangleShape rectangle(Vector2f(size, size));
    rectangle.setFillColor(sf::Color(137, 187, 254, 100));
    rectangle.setOutlineThickness(2.5);
    rectangle.setOutlineColor(sf::Color(137, 187, 254));
    rectangle.setPosition(selectedPiece.second * size, selectedPiece.first* size);
    window.draw(rectangle);
}

void findAvailableMoves(std::pair<int, int> selected, std::unordered_set<std::pair<int, int>, pair_hash> & moves, 
                        int sBoard[][8]) {

    if (selected.first == -1 || sBoard[selected.first][selected.second] == 0) {
        return;
    }
    int piece = abs(sBoard[selected.first][selected.second]);
    bool isWhite = sBoard[selected.first][selected.second] > 0;
    int x, y;
    switch (piece) {
    case PAWN:
        if (isWhite) {
            if (selected.first != 0 && sBoard[selected.first - 1][selected.second] == 0)
                moves.insert({ selected.first - 1 , selected.second });
            if (selected.first == 6 && !moves.empty() && sBoard[selected.first - 2][selected.second] == 0) // havent moved
                moves.insert({ selected.first - 2 , selected.second });
            // taking pieces
            
            if (selected.first != 0)
                if(selected.second-1 >=0 && sBoard[selected.first - 1][selected.second-1] < 0)   //left side
                    moves.insert({ selected.first - 1 , selected.second-1 });
                if(selected.second + 1 < 8 && sBoard[selected.first - 1][selected.second + 1] < 0) // right side
                    moves.insert({ selected.first - 1 , selected.second + 1 });
            // queening and en passent  sometime later
        }
        else {
            if (selected.first != 7 && sBoard[selected.first + 1][selected.second] == 0)
                moves.insert({ selected.first + 1 , selected.second });
            if (selected.first == 1 && !moves.empty() && sBoard[selected.first +2][selected.second] == 0) // havent moved
                moves.insert({ selected.first + 2 , selected.second });
            // taking pieces

            if (selected.first != 7)
                if (selected.second - 1 >= 0 && sBoard[selected.first + 1][selected.second - 1] > 0)   //left side
                    moves.insert({ selected.first + 1 , selected.second - 1 });
                if (selected.second + 1 < 8 && sBoard[selected.first + 1][selected.second + 1] > 0) // right side
                    moves.insert({ selected.first + 1 , selected.second + 1 });
        }
        break;
    case KNIGHT:
        // 8 different moves
        
        x = selected.second + 1;
        y = selected.first + 2;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second + 2;
        y = selected.first + 1;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second - 1;
        y = selected.first + 2;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second + 1;
        y = selected.first - 2;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second - 2;
        y = selected.first + 1;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second + 2;
        y = selected.first - 1;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second - 2;
        y = selected.first - 1;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        x = selected.second - 1;
        y = selected.first - 2;
        if (inBounds(x, y) && (sBoard[y][x] == 0 || (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))) {
            moves.insert({ y, x });
        }
        break;
    case BISHOP: 
        // 4 directions, stops after the first piece encountered
        // up right
        y = selected.first;
        x = selected.second;
        do {
            y++;
            x++;
            if (inBounds(x,y) && (sBoard[y][x] == 0))
                moves.insert({ y, x });
        }while (inBounds(x, y) && sBoard[y][x] == 0);
        if(inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        
        // up left
        y = selected.first;
        x = selected.second;
        do {
            y++;
            x--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        // down right
        y = selected.first;
        x = selected.second;
        do {
            y--;
            x++;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        // down left
        y = selected.first;
        x = selected.second;
        do {
            y--;
            x--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
            
        break;
    case ROOK:
        y = selected.first;
        x = selected.second;
        // up
        do {
            y++;
            if (inBounds(x, y) && (sBoard[y][x] == 0))
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });

        // down
        y = selected.first;
        x = selected.second;
        do {
            y--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        //  right
        y = selected.first;
        x = selected.second;
        do {
            x++;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        // left
        y = selected.first;
        x = selected.second;
        do {
            x--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });

        break;
    case QUEEN:
        // combines bishop and rook logic
        // up right
        y = selected.first;
        x = selected.second;
        do {
            y++;
            x++;
            if (inBounds(x, y) && (sBoard[y][x] == 0))
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });

        // up left
        y = selected.first;
        x = selected.second;
        do {
            y++;
            x--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        // down right
        y = selected.first;
        x = selected.second;
        do {
            y--;
            x++;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        // down left
        y = selected.first;
        x = selected.second;
        do {
            y--;
            x--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        y = selected.first;
        x = selected.second;
        // up
        do {
            y++;
            if (inBounds(x, y) && (sBoard[y][x] == 0))
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });

        // down
        y = selected.first;
        x = selected.second;
        do {
            y--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        //  right
        y = selected.first;
        x = selected.second;
        do {
            x++;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });
        // left
        y = selected.first;
        x = selected.second;
        do {
            x--;
            if (inBounds(x, y) && sBoard[y][x] == 0)
                moves.insert({ y, x });
        } while (inBounds(x, y) && sBoard[y][x] == 0);
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite))
            moves.insert({ y, x });

        break;
    case KING:
        y = selected.first+1;
        x = selected.second;
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first - 1;
        x = selected.second;
        if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first ;
        x = selected.second+1;
                if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first;
        x = selected.second - 1;
                if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first + 1;
        x = selected.second + 1;
                if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first - 1;
        x = selected.second + 1;
                if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first + 1;
        x = selected.second - 1;
                if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        y = selected.first - 1;
        x = selected.second - 1;
                if (inBounds(x, y) && (sBoard[y][x] < 0 && isWhite || sBoard[y][x] > 0 && !isWhite || sBoard[y][x] == 0))
            moves.insert({ y, x });

        // castling
        if (isWhite && !isWhiteCheck && !whiteKingMoved && board[7][5] == 0 && board[7][6] == 0 && board[7][7] == ROOK) {
            moves.insert({ 7, 6 });
        }
        if (isWhite && !isWhiteCheck && !whiteKingMoved && board[7][3] == 0 && board[7][2] == 0 && board[7][1] == 0 && board[7][0] == ROOK) {
            moves.insert({ 7, 2 });
        }
        if (!isWhite && !isBlackCheck && !blackKingMoved && board[0][5] == 0 && board[0][6] == 0 && board[0][7] == -ROOK) {
            moves.insert({ 0, 6 });
        }
        if (!isWhite && !isBlackCheck && !blackKingMoved && board[0][3] == 0 && board[0][2] == 0 && board[0][1] == 0 && board[0][0] == -ROOK) {
            moves.insert({ 0, 2 });
        }
        break;

    }

}

void drawAvailableMoves(RenderWindow& window) {
    for (auto it = availableMoves.begin(); it != availableMoves.end(); it++) {
        auto colour = (board[it->first][it->second] == 0) ?  Color(137, 187, 254, 100) : Color(235, 94, 85, 100);
        sf::RectangleShape rectangle(Vector2f(size, size));
        rectangle.setFillColor(colour);
        rectangle.setPosition(it->second * size, it->first * size);
        window.draw(rectangle);
    }
}
// check for checks + remove available moves if it results in a check
void findLegalMoves(std::pair<int, int> selected, std::unordered_set<std::pair<int, int>, pair_hash> & moves) {
    bool isWhite = board[selected.first][selected.second] > 0;
    bool isKing = abs(board[selected.first][selected.second]) == KING;
    if (!RANDOMMOVE_ON &&((isWhite && !isWhiteTurn) || (!isWhite && isWhiteTurn))) { // not your turn
        moves.clear();
        return;
    }

    std::pair<int, int> originalKingPos;
    if (isKing && isWhite) {
        originalKingPos = whiteKingPos;
    }
    else if (isKing && !isWhite)
        originalKingPos = blackKingPos;

    for (auto it = moves.begin(); it != moves.end(); it++) {
        // first, simulate the moves
        int simulatedBoard[8][8];
        std::copy(&board[0][0], &board[0][0] + 64, &simulatedBoard[0][0]);
        // make move
        int piece = board[selected.first][selected.second];
        simulatedBoard[it->first][it->second] = piece;
        simulatedBoard[selected.first][selected.second] = 0;
        if (isKing && isWhite) {
            whiteKingPos = *it;
        }
        else if (isKing && !isWhite)
            blackKingPos = *it;
        // make a hash that stores every move opponent can take in the next turn
        // then compare if the king's position is in any of them
        std::unordered_set<std::pair<int, int>, pair_hash> enemyNextMoves;
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                // find enemy piece
                if (simulatedBoard[y][x] != 0 && (simulatedBoard[y][x] < 0 && isWhite || simulatedBoard[y][x] > 0 && !isWhite )) {
                    findAvailableMoves(std::make_pair(y, x), enemyNextMoves, simulatedBoard);
                }
            }
        }
        if (isWhite) {
            if (enemyNextMoves.find(whiteKingPos) != enemyNextMoves.end())
                moves.erase(it);
        }
        else {
            if (enemyNextMoves.find(blackKingPos) != enemyNextMoves.end())
                moves.erase(it);
        }
        if (isKing && isWhite) {
            whiteKingPos = originalKingPos ;
        }
        else if (isKing && !isWhite)
            blackKingPos= originalKingPos;
    }
    if (isKing) {
        if (!whiteKingMoved && isWhite && moves.find({ 7, 5 }) == moves.end())
            moves.erase({ 7, 6 });
        if (!whiteKingMoved && isWhite && moves.find({ 7, 3 }) == moves.end() ) {
            moves.erase({ 7, 2 });
        }
        if (!blackKingMoved && !isWhite && moves.find({ 0, 5 }) == moves.end()) {
            moves.erase({ 0, 6 });
        }
        if (!blackKingMoved && !isWhite && moves.find({ 0, 3 }) == moves.end()) {
            moves.erase({ 0, 2 });
        }
    }
}
void findCheck() {
    std::unordered_set<std::pair<int, int>, pair_hash> allLegalWhiteMoves;
    std::unordered_set<std::pair<int, int>, pair_hash> allLegalBlackMoves;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board[y][x] != 0) {
                bool isWhite = board[y][x] > 0;
                std::unordered_set<std::pair<int, int>, pair_hash> moves;
                std::pair<int, int> yx = std::make_pair(y, x);
                findAvailableMoves(yx, moves, board);
                findLegalMoves(yx, moves);
                if (isWhite)
                    allLegalWhiteMoves.insert(moves.begin(), moves.end());
                else
                    allLegalBlackMoves.insert(moves.begin(), moves.end());
            }
        }
    }
    if (allLegalBlackMoves.find(whiteKingPos) != allLegalBlackMoves.end()) {
        isWhiteCheck = true;
        std::cout << "Check" << std::endl;
        // checkmate
        if (allLegalWhiteMoves.empty()) {
            std::cout << "Checkmate, black wins!" << std::endl;
            isGameOver = true;
            return;
        }
    }
    else {
        isWhiteCheck = false;
    }
    if (allLegalWhiteMoves.find(blackKingPos) != allLegalWhiteMoves.end()) {
        std::cout << "Check" << std::endl;
        isBlackCheck = true;
        
        // checkmate
        if (allLegalBlackMoves.empty()) {
            std::cout << "Checkmate, white wins!" << std::endl;
            isGameOver = true;
            return;
        }
    }
    else {
        isBlackCheck = false;
    }
    if (allLegalBlackMoves.empty() || allLegalWhiteMoves.empty()) {
        std::cout << "Stalemate" << std::endl;
        isGameOver = true;
        return;
    }
}
void drawCheck(RenderWindow& window) {
    if (isBlackCheck) {
        auto colour = Color(235, 94, 85, 200);
        sf::RectangleShape rectangle(Vector2f(size, size));
        rectangle.setFillColor(colour);
        rectangle.setPosition(blackKingPos.second * size, blackKingPos.first * size);
        window.draw(rectangle);
    }
    if (isWhiteCheck) {
        auto colour = Color(235, 94, 85, 200);
        sf::RectangleShape rectangle(Vector2f(size, size));
        rectangle.setFillColor(colour);
        rectangle.setPosition(whiteKingPos.second * size, whiteKingPos.first * size);
        window.draw(rectangle);
    }
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(480, 480), "Chess");
    Texture pieces;
    pieces.loadFromFile("images/pieces.png");
    for (int i = 0; i < 32; i++) f[i].setTexture(pieces);

    loadPosition();
   // char* stockfish = "stockfish.exe";
    ConnectToEngine(const_cast<char*>("stockfish.exe"));

    bool isMove = false;
    float dx = 0, dy = 0;
    std::pair<int, int> oldBoardPos, newBoardPos;
    int pieceID;
    Vector2f oldPos, newPos;
    std::string str;
    int n = 0;

    while (window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::BackSpace)
                {
                    if (position.length() > 6) position.erase(position.length() - 6, 5); loadPosition();
                }
            if (event.type == Event::MouseButtonPressed)
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    std::cout << str << std::endl;
                    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
                    std::cout << localPosition.y / size << " "<< localPosition.x / size << std::endl;
                    selectedPiece = { localPosition.y / size , localPosition.x / size };
                    pieceID = board[localPosition.y / size][localPosition.x / size];
                    oldBoardPos = selectedPiece;
                    availableMoves.clear();
                    if (pieceID != 0) {
                        findAvailableMoves(selectedPiece, availableMoves, board);
                        findLegalMoves(selectedPiece, availableMoves);
                    }
                    for (int i = 0; i < 32; i++)
                        if (f[i].getGlobalBounds().contains(pos.x, pos.y))
                        {
                            isMove = true; n = i;
                            dx = pos.x - f[i].getPosition().x;
                            dy = pos.y - f[i].getPosition().y;
                            oldPos = f[i].getPosition();
                        }
                }
            if (event.type == Event::MouseButtonReleased)
                if (event.key.code == Mouse::Left)
                {
                    
                    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
                    newBoardPos = { localPosition.y / size , localPosition.x / size };
                   // board[oldBoardPos.first][oldBoardPos.second] = 0;
                    //board[newBoardPos.first][newBoardPos.second] = pieceID; // place new piece 
                    
                    isMove = false;
                    Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
                    newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));
                    str = toChessNote(oldPos) + toChessNote(newPos);
                    if(board[selectedPiece.first][selectedPiece.second] != 0)
                        if ( RANDOMMOVE_ON||availableMoves.find(newBoardPos) != availableMoves.end()) {
                            move(str);
                            findCheck();
                            if (oldPos != newPos) position += str + " ";
                            f[n].setPosition(newPos);
                        }
                        else {
                            f[n].setPosition(selectedPiece.second * size, selectedPiece.first * size);
                        }
                        
                    if (oldBoardPos != newBoardPos) {
                        availableMoves.clear();
                        isWhiteTurn = !isWhiteTurn;
                    }
                }
            // computer move
            
            if (Keyboard::isKeyPressed(Keyboard::Space))
            {
                str = getNextMove(position);
                oldPos = toCoord(str[0], str[1]);
                newPos = toCoord(str[2], str[3]);

                for (int i = 0; i < 32; i++) if (f[i].getPosition() == oldPos) n = i;

                /////animation///////
                for (int k = 0; k < 50; k++)
                {
                    Vector2f p = newPos - oldPos;
                    f[n].move(p.x / 50, p.y / 50);
                    drawBoard(window);
                    for (int i = 0; i < 32; i++) window.draw(f[i]); window.draw(f[n]);
                    window.display();
                }

                move(str);  position += str + " ";
                f[n].setPosition(newPos);
                findCheck();
                isWhiteTurn = !isWhiteTurn;
            } 
        }
        if (isMove) f[n].setPosition(pos.x - dx, pos.y - dy);

        window.clear();
        drawBoard(window);
        drawSelectedPiece(window);
        drawCheck(window);
        drawAvailableMoves(window);
        for (int i = 0; i < 32; i++) window.draw(f[i]); window.draw(f[n]);
        window.display();
    }

    return 0;
}