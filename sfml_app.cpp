#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_set>

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
std::unordered_set<std::pair<int, int>, pair_hash> avaiableMoves;
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

    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == newPos) f[i].setPosition(-100, -100);

    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == oldPos) f[i].setPosition(newPos);

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
    avaiableMoves.clear();
    if (selectedPiece.first == -1 || board[selectedPiece.first][selectedPiece.second] == 0) {
        return;
    }
    int piece = abs(board[selectedPiece.first][selectedPiece.second]);
    bool isWhite = board[selectedPiece.first][selectedPiece.second] > 0;
    switch (piece) {
    case PAWN:
        if (isWhite) {
            if (selectedPiece.first != 0 && board[selectedPiece.first - 1][selectedPiece.second] == 0)
                avaiableMoves.insert({ selectedPiece.first - 1 , selectedPiece.second });
            if (selectedPiece.first == 6 && !avaiableMoves.empty() && board[selectedPiece.first - 2][selectedPiece.second] == 0) // havent moved
                avaiableMoves.insert({ selectedPiece.first - 2 , selectedPiece.second });
            // taking pieces
            
            if (selectedPiece.first != 0)
                if(selectedPiece.second-1 >=0 && board[selectedPiece.first - 1][selectedPiece.second-1] < 0)   //left side
                    avaiableMoves.insert({ selectedPiece.first - 1 , selectedPiece.second-1 });
                else if(selectedPiece.second + 1 < 8 && board[selectedPiece.first - 1][selectedPiece.second + 1] < 0) // right side
                    avaiableMoves.insert({ selectedPiece.first - 1 , selectedPiece.second + 1 });
            // queening and en passent  sometime later
        }
        else {
            if (selectedPiece.first != 7 && board[selectedPiece.first + 1][selectedPiece.second] == 0)
                avaiableMoves.insert({ selectedPiece.first + 1 , selectedPiece.second });
            if (selectedPiece.first == 1 && !avaiableMoves.empty() && board[selectedPiece.first +2][selectedPiece.second] == 0) // havent moved
                avaiableMoves.insert({ selectedPiece.first + 2 , selectedPiece.second });
            // taking pieces

            if (selectedPiece.first != 7)
                if (selectedPiece.second - 1 >= 0 && board[selectedPiece.first + 1][selectedPiece.second - 1] > 0)   //left side
                    avaiableMoves.insert({ selectedPiece.first + 1 , selectedPiece.second - 1 });
                else if (selectedPiece.second + 1 < 8 && board[selectedPiece.first + 1][selectedPiece.second + 1] > 0) // right side
                    avaiableMoves.insert({ selectedPiece.first + 1 , selectedPiece.second + 1 });
        }
        break;
    case KNIGHT:

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
    if (!avaiableMoves.empty())
        std::cout << avaiableMoves.size() << " Moves Found! \n";
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(480, 480), "Chess");
    Texture pieces;
    pieces.loadFromFile("images/pieces.png");
    for (int i = 0; i < 32; i++) f[i].setTexture(pieces);
    loadPosition();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                sf::Vector2i localPosition = sf::Mouse::getPosition(window);
                //std::cout  << char('a' + (localPosition.x / size)) << " "<<(8-(localPosition.y /size)) << std::endl;
                selectedPiece = { localPosition.y / size , localPosition.x / size };
                std::cout << board[localPosition.y / size ][localPosition.x / size] << std::endl;
                findAvailableMoves();
            }

        }

        window.clear();
        drawBoard(window);
        drawSelectedPiece(window);
        for (int i = 0; i < 32; i++) window.draw(f[i]); window.draw(f[1]);
        window.display();
    }

    return 0;
}