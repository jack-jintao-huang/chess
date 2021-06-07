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

// chess piece size
int size = 60;
std::string position = "";

std::pair<int, int> selectedPiece = { -1, -1 };
std::unordered_set<std::pair<int, int>, pair_hash> availableMoves;
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
void move(std::string str)
{
    std::cout << str << std::endl;
    Vector2f oldPos = toCoord(str[0], str[1]);
    Vector2f newPos = toCoord(str[2], str[3]);

    int piece = board[int(oldPos.y) / size][int(oldPos.x) / size];
    std::cout << "Piece: " << piece << " x y:" << int(oldPos.x) / size << " " << int(oldPos.y) / size << std::endl;
    board[int(oldPos.y) / size][int(oldPos.x) / size] = 0;
    board[int(newPos.y) / size][int(newPos.x) / size] = piece;

    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == newPos) f[i].setPosition(-100, -100);

    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == oldPos) {
           
            f[i].setPosition(newPos);
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

void findAvailableMoves() {
    availableMoves.clear();
    if (selectedPiece.first == -1 || board[selectedPiece.first][selectedPiece.second] == 0) {
        return;
    }
    int piece = abs(board[selectedPiece.first][selectedPiece.second]);
    bool isWhite = board[selectedPiece.first][selectedPiece.second] > 0;
    switch (piece) {
    case PAWN:
        if (isWhite) {
            if (selectedPiece.first != 0 && board[selectedPiece.first - 1][selectedPiece.second] == 0)
                availableMoves.insert({ selectedPiece.first - 1 , selectedPiece.second });
            if (selectedPiece.first == 6 && !availableMoves.empty() && board[selectedPiece.first - 2][selectedPiece.second] == 0) // havent moved
                availableMoves.insert({ selectedPiece.first - 2 , selectedPiece.second });
            // taking pieces
            
            if (selectedPiece.first != 0)
                if(selectedPiece.second-1 >=0 && board[selectedPiece.first - 1][selectedPiece.second-1] < 0)   //left side
                    availableMoves.insert({ selectedPiece.first - 1 , selectedPiece.second-1 });
                if(selectedPiece.second + 1 < 8 && board[selectedPiece.first - 1][selectedPiece.second + 1] < 0) // right side
                    availableMoves.insert({ selectedPiece.first - 1 , selectedPiece.second + 1 });
            // queening and en passent  sometime later
        }
        else {
            if (selectedPiece.first != 7 && board[selectedPiece.first + 1][selectedPiece.second] == 0)
                availableMoves.insert({ selectedPiece.first + 1 , selectedPiece.second });
            if (selectedPiece.first == 1 && !availableMoves.empty() && board[selectedPiece.first +2][selectedPiece.second] == 0) // havent moved
                availableMoves.insert({ selectedPiece.first + 2 , selectedPiece.second });
            // taking pieces

            if (selectedPiece.first != 7)
                if (selectedPiece.second - 1 >= 0 && board[selectedPiece.first + 1][selectedPiece.second - 1] > 0)   //left side
                    availableMoves.insert({ selectedPiece.first + 1 , selectedPiece.second - 1 });
                if (selectedPiece.second + 1 < 8 && board[selectedPiece.first + 1][selectedPiece.second + 1] > 0) // right side
                    availableMoves.insert({ selectedPiece.first + 1 , selectedPiece.second + 1 });
        }
        break;
    case KNIGHT:
        // 8 different moves
        int x, y;
        x = selectedPiece.second + 1;
        y = selectedPiece.first + 2;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second + 2;
        y = selectedPiece.first + 1;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second - 1;
        y = selectedPiece.first + 2;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second + 1;
        y = selectedPiece.first - 2;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second - 2;
        y = selectedPiece.first + 1;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second + 2;
        y = selectedPiece.first - 1;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second - 2;
        y = selectedPiece.first - 1;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        x = selectedPiece.second - 1;
        y = selectedPiece.first - 2;
        if (inBounds(x, y) && (board[y][x] == 0 || (board[y][x] < 0 && isWhite || board[y][x] > 0 && !isWhite))) {
            availableMoves.insert({ y, x });
        }
        break;
    case BISHOP:

        break;
    case ROOK:

        break;
    case QUEEN:

        break;
    case KING:

        break;
    }
    if (!availableMoves.empty())
        std::cout << availableMoves.size() << " Moves Found! \n";
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
                    
                    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
                    std::cout << localPosition.y / size << " "<< localPosition.x / size << std::endl;
                    selectedPiece = { localPosition.y / size , localPosition.x / size };
                    pieceID = board[localPosition.y / size][localPosition.x / size];
                    oldBoardPos = selectedPiece;
                    findAvailableMoves();
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
                    if(oldBoardPos != newBoardPos)
                        availableMoves.clear(); 
                    isMove = false;
                    
                    Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
                    newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));
                    str = toChessNote(oldPos) + toChessNote(newPos);
                    move(str);
                    if (oldPos != newPos) position += str + " ";
                    f[n].setPosition(newPos);
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
            } 
        }
        if (isMove) f[n].setPosition(pos.x - dx, pos.y - dy);

        window.clear();
        drawBoard(window);
        drawSelectedPiece(window);
        drawAvailableMoves(window);
        for (int i = 0; i < 32; i++) window.draw(f[i]); window.draw(f[n]);
        window.display();
    }

    return 0;
}