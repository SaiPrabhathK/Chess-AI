#ifndef PARSINGFEN_H
#define PARSINGFEN_H
#include <iostream>
#include <ctype.h>
#include <string>
#include <ostream>
#include <map>
#include <vector>
#include "putils.h"


const std::string colors[2] = {"black", "white"}; // 0 -> black, 1 -> white;

const std::string files = "-abcdefgh-";

const std::string ranks = "--12345678--";

const int ranksInMemory[8] = {0, 1, 2, 3, 4, 5, 6, 7};

const std::string g_piecesString= "-kqbnrp";

const std::map<char, int> pieceTypes = {
    {'-', 0},
    {'k', 1},
    {'q', 2},
    {'b', 3},
    {'n', 4},
    {'r', 5},
    {'p', 6}};

const int AVAILABLE_DIRECTIONS = 8;

const int AVAILABLE_PIECES_TYPES = 6;
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
const std::vector<std::vector<int>> piecesOffsets{
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-11, -10, -9, -1, 1, 9, 10, 11},   /* KING */
    {-11, -10, -9, -1, 1, 9, 10, 11},   /* QUEEN */
    {-11, -9, 9, 11, 0, 0, 0, 0},       /* BISHOP */
    {-21, -19, -12, -8, 8, 12, 19, 21}, /* KNIGHT */
    {-10, -1, 1, 10, 0, 0, 0, 0},       /* ROOK */
};
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
const int AVAILABLE_PAWN_DIRECTIONS = 3;
const int pawnOffsets[2][AVAILABLE_PAWN_DIRECTIONS]{
    {-10, -11, -9}, /* Black */
    {10, 9, 11}     /* White */
};

class Cell
{
public:
    int cellIndex;
    char pieceChar;
    int pieceColor;
    int pieceType;
    Cell()
    {
        cellIndex = -1;
        pieceChar = '-';
        pieceType = -1;
        pieceColor = -1;
    }
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
    Move(int from, int to);
    void setPromotionProperties(int promotionPieceType);
    void setEnpassantProperties(int enpassantCaptureCell);
    void setCaptureProperties(int capturedPieceType);
    std::string uciFormat();
    Move clone();
};

class GameBoard
{
public:
    std::string basicBoard = "................................................................";
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
    std::vector<Move> generateMoves();
    std::vector<Move> getPawnMovesBasedOnTurn(Cell pawnCell, std::vector<Move> &availableMoves);
    std::vector<Move> getOtherPiecesMovesBasedOnTurn(Cell pawnCell, std::vector<Move> &availableMoves);
    std::vector<int> getPiecesAttacksOnSquareIndex(int cellIndex, int opponentColor);
    std::vector<int> getSquaresKnightExistFromGivenSquareIndex(int sourceSquareIndex, int opponentColor);
    GameBoard clone();
    GameBoard makeMove(Move move);
    bool isKingInCheck();
    void insertAllChoicesOfCastling(std::vector<Move> &availableMoves, Move move,int direction);
};
std::ostream &operator<<(std::ostream &os, GameBoard gameBoard);
#endif