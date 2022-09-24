#ifndef PARSINGFEN_H
#define PARSINGFEN_H
#include <iostream>
#include <ctype.h>
#include <string>
#include <ostream>
#include <map>
#include <vector>
#include <climits>
#include <utility>
#include "putils.h"

// encoding for piece colors as 0 for black and 1 for white.
const std::string colors[2] = {"black", "white"}; // 0 -> black, 1 -> white;

//files on chess board
const std::string files = "-abcdefgh-";

//ranks on chess board
const std::string ranks = "--12345678--";

const int ranksInMemory[8] = {0, 1, 2, 3, 4, 5, 6, 7};

const std::string g_piecesString= "-kqbnrp";

//mapping of piece types to specific integers
const std::map<char, int> pieceTypes = {
    {'-', 0},
    {'k', 1},
    {'q', 2},
    {'b', 3},
    {'n', 4},
    {'r', 5},
    {'p', 6}};

//values of different piece types according to above map to be used by heuristic function.
const int pieceTypeValues[7] = {0, 150, 10, 3, 4, 5, 1};

const int AVAILABLE_DIRECTIONS = 8;

const int AVAILABLE_PIECES_TYPES = 6;

//mapping piece types to the directions they can move in.
const std::map<std::string, std::vector<int>> directionsToPieces = {
    {"NW", std::vector<int>{pieceTypes.at('b'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"N", std::vector<int>{pieceTypes.at('r'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"NE", std::vector<int>{pieceTypes.at('b'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"W", std::vector<int>{pieceTypes.at('r'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"E", std::vector<int>{pieceTypes.at('r'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"SW", std::vector<int>{pieceTypes.at('b'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"S", std::vector<int>{pieceTypes.at('r'), pieceTypes.at('q'), pieceTypes.at('k')}},
    {"SE", std::vector<int>{pieceTypes.at('b'), pieceTypes.at('q'), pieceTypes.at('k')}},
};

//offset of index values adjusted for mailbox notation.
const std::vector<std::vector<int>> piecesOffsets{
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-11, -10, -9, -1, 1, 9, 10, 11},   /* KING */
    {-11, -10, -9, -1, 1, 9, 10, 11},   /* QUEEN */
    {-11, -9, 9, 11, 0, 0, 0, 0},       /* BISHOP */
    {-21, -19, -12, -8, 8, 12, 19, 21}, /* KNIGHT */
    {-10, -1, 1, 10, 0, 0, 0, 0},       /* ROOK */
};

//mapping directions to index values in mailbox notation.
const std::vector<std::pair<std::string, int>> arrAvailableDirections{
    std::make_pair("NW", -11),
    std::make_pair("N", -10),
    std::make_pair("NE", -9),
    std::make_pair("W", -1),
    std::make_pair("E", 1),
    std::make_pair("SW", 9),
    std::make_pair("S", 10),
    std::make_pair("SE", 11),
};

// pawn piece direction offsets.
const int AVAILABLE_PAWN_DIRECTIONS = 3;
const int pawnOffsets[2][AVAILABLE_PAWN_DIRECTIONS]{
    {-10, -11, -9}, /* Black */
    {10, 9, 11}     /* White */
};

//Cell class contains members that store the index value a piece is at, type of piece, color of piece, and char representing piece type in map used above.
class Cell
{
public:
    int cellIndex;
    char pieceChar;
    int pieceColor;
    int pieceType;
    //default constructor
    Cell()
    {
        cellIndex = -1;
        pieceChar = '-';
        pieceType = -1;
        pieceColor = -1;
    }
    //parameterized constructor
    Cell(int cellIndex, char pieceChar, int pieceColor, int pieceType){
        this->cellIndex = cellIndex;
        this->pieceChar = pieceChar;
        this->pieceColor = pieceColor;
        this->pieceType = pieceType;
    }
    static Cell getEmptyCell(int cellIndex){
        Cell cell(cellIndex, '-', -1, 0);
        return cell;
    }
    Cell clone();
    std::string get_string();
};

//Move class contains members that store information that store a piece movement information such as from index values and to index value
//and information about enpassant cell, castling rights, promotion piece types etc.
class Move
{
public:
    int from;
    int to;
    bool isCaptured;
    bool isPromotion;
    bool isCastling;
    bool isEnpassant;
    int capturedPieceType;
    int promotionPieceType;
    int castlingSide;
    int enpassantCapturePieceIndex;
    Move();
    Move(int from, int to);
    void setPromotionProperties(int promotionPieceType);
    void setEnpassantProperties(int enpassantCaptureCell);
    void setCaptureProperties(int capturedPieceType);
    int moveValue();
    //function translates index values into files and ranks.
    std::string uciFormat();
    Move clone();
};

//Gameboard class contains the most important members and member functions.
//boardcells vector of Cell type contains all the chess board in a mailbox notation; parseFen2() functions takes the fenstring and fills the vector.
class GameBoard
{
public:
    //std::string basicBoard = "................................................................";
    std::vector<Cell> boardCells;
    std::vector<Cell> myPieces;
    std::vector<Cell> enemyPieces;
    int turn;
    int myCastlings[2];
    int enemyCastlings[2];
    int enPassant;
    int halfMoves;
    int moves;
    // void parseFen(std::string fenString);
    GameBoard();
    GameBoard(std::vector<Cell> boardCells, int turn, int myCastlings[2], int enemyCastlings[2], int enPassant, int halfMoves, int moves);
    GameBoard static parseFen2(std::string fenString);
    void printBoard();
    std::string printboard2();
    std::string static getFilesandRanks(int cellIndex);
    int static squareNameToIndex(std::string cellName);
    bool isIndexEmpty(int cellIndex);
    bool isIndexOccupied(int cellIndex);
    bool isIndexOccupied(int cellIndex, int color);
    bool isIndexOccupied(int cellIndex, int color, int pieceType);
    bool isIndexValid(int cellIndex);
    bool isPawnInInitialRank(int cellIndex, int color);
    bool isPawnInPromotionRank(int cellIndex, int color);
    void insertAllChoicesOfPromotions(std::vector<Move> &availableMoves, Move promotionMove);
    bool isKingInCheck(int cellIndex);
    std::string getRandomMove();
    std::vector<std::pair<Move, GameBoard>> generateMoves();
    std::vector<Move> getPawnMovesBasedOnTurn(Cell pawnCell, std::vector<Move> &availableMoves);
    std::vector<Move> getOtherPiecesMovesBasedOnTurn(Cell pawnCell, std::vector<Move> &availableMoves);
    std::vector<int> getPiecesAttacksOnSquareIndex(int cellIndex, int opponentColor);
    std::vector<int> getSquaresKnightExistFromGivenSquareIndex(int sourceSquareIndex, int opponentColor);
    GameBoard clone(bool switchSide);
    GameBoard makeMove(Move move);
    bool isKingInCheck();
    void insertAllChoicesOfCastling(std::vector<Move> &availableMoves, Move move,int direction);
    int evaluateState(); //heuristic function.
};

//Algo class contains the algorithms used to play the game.
class Algo
{
    public:
        static std::string timeLimitedIterativeDeepeningABMInMaxWithQuiescenceSearchAndMoveOrdering(std::string orgFenString, double remainingTime); //time limited iterative deepening alpha beta minimax  with quiescence search and move ordering function.
        static Move searchAB(GameBoard initGameBoard, int depth); //search function
        static int quiescentSearch(GameBoard gameState, int alpha, int beta); //quiescence search function
        static int minPlayerAB(GameBoard eachMaxPlayerGameState, int depth, int alphaValue, int betaValue); //min function
        static int maxPlayerAB(GameBoard eachMinPlayerGameState, int depth, int alphaValue, int betaValue); //max function
};

std::ostream &operator<<(std::ostream &os, GameBoard gameBoard);
#endif