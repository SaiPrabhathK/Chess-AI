
#include <iostream>
#include <ctype.h>
#include <string>
#include <ostream>
#include <map>
#include <climits>
#include <algorithm>
#include <iterator>
#include <random>
#include <utility>
#include "parsingfen.h"
#include "putils.h"

using namespace std;

Cell Cell::clone(){
    Cell cell;
    cell.cellIndex = this->cellIndex;
    cell.pieceChar = this->pieceChar;
    cell.pieceType = this->pieceType;
    cell.pieceColor = this->pieceColor;
    return cell;
}
ostream &operator<<(ostream &os, Cell cell) //<< operator overloading to print all the members of the Cell class objects.
{
    os << cell.cellIndex << " " << cell.pieceChar << " " << cell.pieceColor << " " << cell.pieceType << "| ";
    return os;
}

string Cell::get_string()
{
    return to_string(cellIndex) + " " + pieceChar + " " + (pieceColor >= 0 ? colors[pieceColor].substr(0, 2) : to_string(pieceColor)) + " " + +(pieceType != -1 ? " " : "") + to_string(pieceType) + " | ";
}

// move functions
Move::Move(){}
Move::Move(int from, int to)
{
    this->from = from;
    this->to = to;
    this->isCaptured=false;
    this->isPromotion=false;
    this->isCastling=false;
    this->isEnpassant=false;
    this->capturedPieceType=-1;
    this->promotionPieceType=-1;
    this->castlingSide=-1;
    this->enpassantCapturePieceIndex=-1;
    
}
void Move::setPromotionProperties(int promotionPieceType)
{
    this->isPromotion = true;
    this->promotionPieceType = promotionPieceType;
}
void Move::setEnpassantProperties(int enpassantCaptureCell)
{
    this->isEnpassant = true;
    this->enpassantCapturePieceIndex = enpassantCaptureCell;
}
void Move::setCaptureProperties(int capturedPieceType)
{
    this->isCaptured = true;
    this->capturedPieceType = capturedPieceType;
}
string Move::uciFormat()
{
    return GameBoard::getFilesandRanks(from) + GameBoard::getFilesandRanks(to);
}
Move Move::clone()
{
    Move cloneMove(this->from, this->to);
    cloneMove.isCaptured = this->isCaptured;
    cloneMove.isPromotion = this->isPromotion;
    cloneMove.isCastling = this->isCastling;
    cloneMove.isEnpassant = this->isEnpassant;
    cloneMove.capturedPieceType = this->capturedPieceType;
    cloneMove.promotionPieceType = this->promotionPieceType;
    cloneMove.castlingSide = this->castlingSide;
    cloneMove.enpassantCapturePieceIndex = this->enpassantCapturePieceIndex;
    return cloneMove;
}
ostream &operator<<(ostream &os, Move move)
{
    os << move.from << " " << move.to << " " << move.uciFormat();
    return os;
}

// game functions
GameBoard::GameBoard(){

}

bool GameBoard::isPawnInInitialRank(int cellIndex, int color)
{
    int colorToInitialRank[2] = {8, 3};
    if (!isIndexValid(cellIndex))
        return false;
    return ((cellIndex / 10) == colorToInitialRank[color]);
}

bool GameBoard::isPawnInPromotionRank(int cellIndex, int color)
{
    int colorToPromotionRank[2] = {2, 9};
    if (!isIndexValid(cellIndex))
        return false;
    return ((cellIndex / 10) == colorToPromotionRank[color]);
}

GameBoard::GameBoard(vector<Cell> boardCells, int turn, int myCastlings[2], int enemyCastlings[2], int enPassant, int halfMoves, int moves)
{
    this->boardCells = boardCells;

    for (int i = 0; i < boardCells.size(); i++)
    {
        if (turn == boardCells[i].pieceColor)
        {
            myPieces.push_back(boardCells[i]);
        }
        else if ((!turn) == boardCells[i].pieceColor)
        {
            enemyPieces.push_back(boardCells[i]);
        }
    }

    this->turn = turn;
    this->myCastlings[0] = myCastlings[0];
    this->myCastlings[1] = myCastlings[1];
    this->enemyCastlings[0] = enemyCastlings[0];
    this->enemyCastlings[1] = enemyCastlings[1];
    this->enPassant = enPassant;
    this->halfMoves = halfMoves;
    this->moves = moves;
}

string GameBoard::getFilesandRanks(int cellIndex)
{
    char cell[2] = {files[cellIndex % 10], ranks[cellIndex / 10]};
    return string(cell);
}

ostream &operator<<(ostream &os, GameBoard gameBoard) //<< operator overloading to print all the members of the GameBoard class objects.
{
    os << gameBoard.printboard2() << endl;
    os << colors[gameBoard.turn] << endl;
    os << gameBoard.myCastlings[0] << " " << gameBoard.myCastlings[1] << endl;
    os << gameBoard.enemyCastlings[0] << " " << gameBoard.enemyCastlings[1] << endl;
    os << gameBoard.enPassant << endl;
    return os;
}

void GameBoard::printBoard()
{
    string str;
    for (int i = 64; i > 0; i--)
    {
        if (i % 8 == 0 && !str.empty())
        {
            cout << str << endl;
            str = "";
        }
        str = basicBoard[i - 1] + str;
    }
    cout << str << endl;
}

GameBoard GameBoard::parseFen2(string fenString)
{
    vector<string> fenParts = split(fenString, " ");
    vector<Cell> boardCells(120, Cell());
    // board parsing
    vector<string> boardRows = split(fenParts[0], "/");
    for (int eachRow = 7; eachRow >= 0; eachRow--)
    {
        int rowOffSet = (21 + (7 - eachRow) * 10);
        string currentRow = boardRows[eachRow];
        int currentSquareIndex = rowOffSet;
        for (int eachCell = 0; eachCell < currentRow.size(); eachCell++)
        {
            if (isdigit(currentRow[eachCell]))
            {
                int emptyCellCount = currentRow[eachCell] - '0';
                for (int eachRowCell = currentSquareIndex; eachRowCell < (currentSquareIndex + emptyCellCount); eachRowCell++)
                {
                    boardCells[eachRowCell].cellIndex = eachRowCell;
                    boardCells[eachRowCell].pieceChar = '-';
                    boardCells[eachRowCell].pieceColor = -1;
                    boardCells[eachRowCell].pieceType = pieceTypes.at(boardCells[currentSquareIndex].pieceChar);
                }
                currentSquareIndex += emptyCellCount;
                continue;
            }
            // it will be alphabets

            boardCells[currentSquareIndex].cellIndex = currentSquareIndex;
            boardCells[currentSquareIndex].pieceChar = currentRow[eachCell];
            boardCells[currentSquareIndex].pieceColor = currentRow[eachCell] > 96 ? 0 : 1;
            boardCells[currentSquareIndex].pieceType = pieceTypes.at(tolower(currentRow[eachCell]));
            currentSquareIndex++;
        }
    }
    // turn
    int turn = fenParts[1] == "w" ? 1 : 0;
    // castlings
    int myCastlings[2] = {0, 0};
    int enemyCastlings[2] = {0, 0};
    myCastlings[0] = turn ? (fenParts[2].find('K') != -1):(fenParts[2].find('k') != -1);
    myCastlings[1] = turn ? (fenParts[2].find('Q') != -1):(fenParts[2].find('k') != -1);
    enemyCastlings[0] = !turn ? (fenParts[2].find('k') != -1):(fenParts[2].find('K') != -1);
    enemyCastlings[1] = !turn ? (fenParts[2].find('q') != -1): (fenParts[2].find('Q') != -1);
    // enpassant
    int enPassant = fenParts[3] == "-" ? -1 : GameBoard::squareNameToIndex(fenParts[3]);
    // half moves
    int halfMoves = stoi(fenParts[4]);
    int moves = stoi(fenParts[5]);
    return GameBoard(boardCells, turn, myCastlings, enemyCastlings, enPassant, halfMoves, moves);
}

int GameBoard::squareNameToIndex(string cellName)
{
    return (((cellName[0] - 'a') + 1) + ((cellName[1] - '0') + 1) * 10);
}

string GameBoard::printboard2()
{
    string str;
    for (int i = 0; i < 120; i++)
    {
        if (i % 10 == 0)
        {
            str += "\n";
        }
        str = str + boardCells[i].get_string();
    }
    return str;
}

bool GameBoard::isIndexEmpty(int cellIndex)
{
    return this->boardCells[cellIndex].pieceType == 0;
}

bool GameBoard::isIndexValid(int cellIndex)
{
    bool isWithInBoard = cellIndex > -1 && cellIndex < 120;
    if (!isWithInBoard)
        return false;
    return this->boardCells[cellIndex].pieceType != -1;
}

bool GameBoard::isIndexOccupied(int cellIndex)
{
    if (!isIndexValid(cellIndex))
        return false;
    return !isIndexEmpty(cellIndex);
}

bool GameBoard::isIndexOccupied(int cellIndex, int color)
{
    if (!isIndexValid(cellIndex))
        return false;
    if (isIndexEmpty(cellIndex))
        return false;
    return this->boardCells[cellIndex].pieceColor == color;
}

bool GameBoard::isIndexOccupied(int cellIndex, int color, int pieceType)
{
    if (!isIndexValid(cellIndex))
        return false;
    if (isIndexEmpty(cellIndex))
        return false;
    Cell cell = this->boardCells[cellIndex];
    if (cell.pieceColor != color)
        return false;
    return cell.pieceType == pieceType;
}

vector<Move> GameBoard::getPawnMovesBasedOnTurn(Cell pawnCell, vector<Move> &availableMoves)
{
    // vector<Move> availableMoves;
    int currentTurn = this->turn;
    int currentCell = pawnCell.cellIndex;
    int pawnColor = pawnCell.pieceColor;
    for (int eachDirection = 0; eachDirection < AVAILABLE_PAWN_DIRECTIONS; eachDirection++)
    {
        int currentDirectionOffset = pawnOffsets[pawnColor][eachDirection];
        int nextCell = currentCell + currentDirectionOffset;
        if (!isIndexValid(nextCell))
            continue;
        Move newMove(currentCell, nextCell);
        if (eachDirection == 0)
        {
            if (!isIndexEmpty(nextCell))
                continue;
            availableMoves.push_back(newMove);
            // promotion
            if (isPawnInPromotionRank(nextCell, pawnColor))
            {
                insertAllChoicesOfPromotions(availableMoves, newMove);
                continue;
            }
            if (!isPawnInInitialRank(currentCell, pawnColor)) continue;
            // double move            
            nextCell = nextCell + currentDirectionOffset;
            if (!isIndexEmpty(nextCell))
                continue;
            Move doubleMove = newMove.clone();
            doubleMove.to = nextCell;
            availableMoves.push_back(doubleMove);
            continue;
        }  
        // enpassant
        int enpassantCaptureCell = nextCell + pawnOffsets[!pawnColor][0];
        int isPossibleToCaptureByEnpassant = nextCell == enPassant && isIndexOccupied(enpassantCaptureCell, !pawnColor, pawnCell.pieceType);
        if (isIndexEmpty(nextCell) && isPossibleToCaptureByEnpassant)
        {
            newMove.setEnpassantProperties(enpassantCaptureCell);
            availableMoves.push_back(newMove);
            continue;
        }
        if (isIndexEmpty(nextCell))
            continue;

        if(isIndexOccupied(nextCell,pawnColor)) continue;
        
        Cell capturedCell = this->boardCells[nextCell];
        newMove.setCaptureProperties(capturedCell.pieceType);
        if (isPawnInPromotionRank(nextCell, pawnColor))
        {
            insertAllChoicesOfPromotions(availableMoves, newMove);
            continue;
        }
        availableMoves.push_back(newMove);
        // capture
    }
    return availableMoves;
}

vector<Move> GameBoard::getOtherPiecesMovesBasedOnTurn(Cell pieceCell, vector<Move> &availableMoves){
    // vector<Move> availableMoves;
    // int currentTurn = this->turn;
    int pieceColor = pieceCell.pieceColor;
    int pieceType = pieceCell.pieceType;
    for (int eachDirection = 0; eachDirection < AVAILABLE_DIRECTIONS; eachDirection++)
    {
        int currentDirectionOffset = piecesOffsets[pieceType][eachDirection];
        if(currentDirectionOffset == 0) continue;
        int currentCell = pieceCell.cellIndex;
        int nextCell = currentCell + currentDirectionOffset;
        while(1){
            if (!isIndexValid(nextCell))
                break;
            Move newMove(currentCell, nextCell);
            if(isIndexOccupied(nextCell, pieceColor)) break;
            //empty
            if(isIndexEmpty(nextCell)){
                availableMoves.push_back(newMove);
                if(pieceType == pieceTypes.at('k') && (eachDirection == 3 || eachDirection == 4)){
                    insertAllChoicesOfCastling(availableMoves, newMove, eachDirection);
                }
                
            }else if(isIndexOccupied(nextCell, !pieceColor)){
            // capture a piece and stop
                Cell capturedCell = this->boardCells[nextCell];
                if(capturedCell.pieceType != pieceTypes.at('k')){
                    newMove.setCaptureProperties(capturedCell.pieceType);
                    availableMoves.push_back(newMove);
                }
            }
            
            if(pieceType == pieceTypes.at('k') || pieceType == pieceTypes.at('n') || newMove.isCaptured){
                break;
            }
            nextCell += currentDirectionOffset;
        }
    }
    return availableMoves;
}
vector<std::pair<Move, GameBoard>> GameBoard::generateMoves()
{
    vector<Cell> myPieces = this->myPieces;
    vector<Move> availableMoves;
    for (int eachPiece = 0; eachPiece < myPieces.size(); eachPiece++)
    {
        Cell currentPiece = myPieces[eachPiece];
        if (currentPiece.pieceType == pieceTypes.at('p'))
        {
            this->getPawnMovesBasedOnTurn(currentPiece, availableMoves);
        }else{
            this->getOtherPiecesMovesBasedOnTurn(currentPiece,availableMoves);
        }
    }
    std::vector<std::pair<Move,GameBoard>> validMovesandGameBoards;
    for (int eachMove = 0; eachMove < availableMoves.size(); eachMove++)
    {
        Move move = availableMoves[eachMove];
        GameBoard newBoardObj = makeMove(move);
        //cout<<move.uciFormat()<<endl;
        if(!newBoardObj.isKingInCheck()){
           //validMoves.push_back(move);
           validMovesandGameBoards.push_back(std::make_pair(move, newBoardObj));
           //cout<<move<<endl;
        }
    }
    
    return validMovesandGameBoards;
}
// string GameBoard::getRandomMove(){
//     vector<Move> availableMoves = generateMoves();
//     if(availableMoves.size()>0){
//          srand (time(NULL));
// 	int random_index = rand() % (availableMoves.size());
//         // std::random_device rd;     // only used once to initialise (seed) engine
//         // std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
//         // std::uniform_int_distribution<int> uni(0,availableMoves.size()); // guaranteed unbiased
//         // int random_index = uni(rng);
//         cout<<availableMoves[random_index]<<endl;
//         return availableMoves[random_index].uciFormat();
//     }
//     return "";
// }
void GameBoard::insertAllChoicesOfPromotions(vector<Move> &availableMoves, Move promotionMove)
{
    int promotionChoices[4] = {2, 3, 4, 5};
    for (int eachPromotionChoice = 0; eachPromotionChoice < 4; eachPromotionChoice++)
    {
        Move promotionMoveClone = promotionMove.clone();
        promotionMove.setPromotionProperties(promotionChoices[eachPromotionChoice]);
        availableMoves.push_back(promotionMoveClone);
    }
}
void GameBoard::insertAllChoicesOfCastling(vector<Move> &availableMoves,Move move,int direction){
    if(this->isKingInCheck()) return;
    int* availableCastles = myCastlings;
    map<int,int> directionToCastlingIndex = {
        {3, 1},
        {4, 0}
    };
    //cout<<availableCastles[directionToCastlingIndex[direction]];
    if(availableCastles[directionToCastlingIndex[direction]] == 0) return;
    if(!isIndexEmpty(move.to)) return;
    vector<int> piecesAttackingTheSquare = getPiecesAttacksOnSquareIndex(move.to,!turn);
    if(piecesAttackingTheSquare.size()>0) return;
    int pieceType = pieceTypes.at('k');
    int moveNextSquare = move.to + piecesOffsets[pieceType][direction];
    if(!isIndexEmpty(moveNextSquare)) return;
    piecesAttackingTheSquare = getPiecesAttacksOnSquareIndex(move.to,!turn);
    if(piecesAttackingTheSquare.size()>0) return;
    if(direction == 4){
        Move tempMove = move.clone();
        tempMove.isCastling = true;
        tempMove.to = moveNextSquare;
        tempMove.castlingSide = pieceTypes.at('k');
        availableMoves.push_back(tempMove);
        return;
    }
    int move3rdSquare = moveNextSquare + piecesOffsets[pieceType][direction];
    if(!isIndexEmpty(move3rdSquare)) return;
    Move tempMove = move.clone();
    tempMove.isCastling = true;
    tempMove.to = moveNextSquare;
    tempMove.castlingSide = pieceTypes.at('q');
    availableMoves.push_back(tempMove);
}
GameBoard GameBoard::clone(bool switchSide=false){
    
    vector<Cell> boardCells(this->boardCells.size());
    for(int i=0;i<boardCells.size();i++){
        boardCells[i] = this->boardCells[i].clone();
    }
    
    std::vector<Cell> myPieces(this->myPieces.size());
    for(int i=0;i<myPieces.size();i++){
        myPieces[i] = this->myPieces[i].clone();
    }
    
    std::vector<Cell> enemyPieces(this->enemyPieces.size());
    for(int i=0;i<enemyPieces.size();i++){
        enemyPieces[i] = this->enemyPieces[i].clone();
    }
    
    int turn = this->turn;
    int myCastlings[2];
    myCastlings[0] = this->myCastlings[0];
    myCastlings[1] = this->myCastlings[1];
    int enemyCastlings[2];
    enemyCastlings[0] = this->enemyCastlings[0];
    enemyCastlings[1] = this->enemyCastlings[1];
    int enPassant = this->enPassant;
    int halfMoves = this->halfMoves;
    int moves = this->moves;
    
    if(switchSide)
    {
        return GameBoard(boardCells,!turn,enemyCastlings,myCastlings,enPassant,halfMoves,moves);
    }
    return GameBoard(boardCells,turn,myCastlings,enemyCastlings,enPassant,halfMoves,moves);
}

GameBoard GameBoard::makeMove(Move move){
    GameBoard newGameBoard = this->clone();
    Cell fromCell = boardCells[move.from];
    Cell toCell = boardCells[move.to];
    newGameBoard.boardCells[move.from] = Cell::getEmptyCell(move.from);
    newGameBoard.boardCells[move.to] = Cell(move.to, fromCell.pieceChar,fromCell.pieceColor,fromCell.pieceType);
    if(move.isEnpassant){
        newGameBoard.boardCells[move.enpassantCapturePieceIndex] = Cell::getEmptyCell(move.from);
    } else if(move.isPromotion){
        newGameBoard.boardCells[move.to].pieceType = move.promotionPieceType;
    } else if(move.isCastling){
        map<int,vector<int>> castlingSideToRookSquareIndex = {
            {pieceTypes.at('k'), vector<int>{28,98}},
            {pieceTypes.at('q'), vector<int>{21,91}}
        };
        map<int,vector<int>> castlingSideToNewRookPosition = {
            {pieceTypes.at('k'), vector<int>{26,96}},
            {pieceTypes.at('q'), vector<int>{24,94}}
        };
        newGameBoard.boardCells[castlingSideToRookSquareIndex[move.castlingSide][turn]] = Cell::getEmptyCell(castlingSideToRookSquareIndex[move.castlingSide][turn]);
        char rookSymbol = g_piecesString[pieceTypes.at('r')]+turn*-32;
        newGameBoard.boardCells[castlingSideToNewRookPosition[move.castlingSide][turn]] = Cell(
               castlingSideToNewRookPosition[move.castlingSide][turn],
                rookSymbol,
                turn,
                pieceTypes.at('r')
                );
        newGameBoard.myCastlings[0] = 0;
        newGameBoard.myCastlings[1] = 0;
    }

    for (int i = 0; i < newGameBoard.boardCells.size(); i++)
    {
        if (turn == boardCells[i].pieceColor)
        {
            newGameBoard.myPieces.push_back(boardCells[i]);
        }
        else if ((!turn) == boardCells[i].pieceColor)
        {
            newGameBoard.enemyPieces.push_back(boardCells[i]);
        }
    }
    return newGameBoard;
}

//
vector<int> GameBoard::getPiecesAttacksOnSquareIndex(int cellIndex, int opponentColor) {
  string pawnAttackDirectionFromSquarePerspective[2][2] = {
   {"SW", "SE"}, {"NE", "NW"}
  };
  vector<int> arrPiecesAttackingTheSquareIndex;
  for (int eachDirection = 0; eachDirection < arrAvailableDirections.size(); eachDirection++) {
    std::pair<string,int> directionSelected = arrAvailableDirections[eachDirection];
    string pawnAttacks[2];
    pawnAttacks[0] = pawnAttackDirectionFromSquarePerspective[opponentColor][0];
    pawnAttacks[1] = pawnAttackDirectionFromSquarePerspective[opponentColor][1];
    bool isPawnAttackDirection = ((pawnAttacks[0] == directionSelected.first) || (pawnAttacks[1] == directionSelected.first));
    vector<int> piecesHavingSelectedDirection = directionsToPieces.at(directionSelected.first);
    int sourceSquareIndex = cellIndex;
    int targetSquareIndex =
      sourceSquareIndex + directionSelected.second;
    int hops = 0;
    vector<int> skipPieceTypes;
    skipPieceTypes.push_back(pieceTypes.at('n'));
    if (!isPawnAttackDirection) {
      skipPieceTypes.push_back(pieceTypes.at('p'));
    }
    while (1) {
      if (!isIndexValid(targetSquareIndex)) {
        break;
      }
      if (isIndexOccupied(targetSquareIndex,!opponentColor)) {
        break;
      }
      if (isIndexEmpty(targetSquareIndex)) {
        sourceSquareIndex = targetSquareIndex;
        targetSquareIndex = targetSquareIndex + directionSelected.second;
        hops++;
        continue;
      }
      Cell cell = boardCells[targetSquareIndex];
      int pieceType = cell.pieceType;
      bool canSkipPiece = find(skipPieceTypes.begin(),skipPieceTypes.end(), pieceType) != skipPieceTypes.end();
      if (canSkipPiece) {
        break;
      }
      if (pieceType == pieceTypes.at('p') && hops == 0 && isPawnAttackDirection) {
        arrPiecesAttackingTheSquareIndex.push_back(pieceType);
      } else if (find(piecesHavingSelectedDirection.begin(), piecesHavingSelectedDirection.end(),pieceType) != (piecesHavingSelectedDirection.end())) {
        if (pieceType == pieceTypes.at('k') && hops > 0) break;
        arrPiecesAttackingTheSquareIndex.push_back(pieceType);
      }
      break;
    }
  }
  vector<int> squaresKnightExist = getSquaresKnightExistFromGivenSquareIndex(cellIndex, opponentColor); 
  if (squaresKnightExist.size() > 0) {
    arrPiecesAttackingTheSquareIndex.push_back(pieceTypes.at('n'));
  }  
  return arrPiecesAttackingTheSquareIndex;
}

vector<int> GameBoard::getSquaresKnightExistFromGivenSquareIndex(int sourceSquareIndex, int opponentColor) {
  auto knightDirections = piecesOffsets[pieceTypes.at('n')];
   vector<int>  squaresKnightExist;
  for (int eachDirection = 0; eachDirection < knightDirections.size(); eachDirection++) {
    int targetSquareIndex = sourceSquareIndex + knightDirections[eachDirection];
    if (!isIndexValid(sourceSquareIndex)) continue;
    if (!isIndexOccupied(targetSquareIndex, opponentColor, pieceTypes.at('n'))) continue;
    Cell cell = boardCells[targetSquareIndex];
    if (cell.pieceType == pieceTypes.at('n')) {
      squaresKnightExist.push_back(targetSquareIndex);
    }
  }
  return squaresKnightExist;
}

bool GameBoard::isKingInCheck(){
   Cell kingPosition;
    for(int i=0;i<boardCells.size();i++){
        Cell cell = boardCells[i];
        if(cell.pieceType == pieceTypes.at('k') && cell.pieceColor == turn){
            kingPosition = cell;
            break;
        }
    }
    vector<int> attackingPieces = getPiecesAttacksOnSquareIndex(kingPosition.cellIndex, !turn);
    return attackingPieces.size()>0;
}

int Algo::minPlayer(GameBoard eachMaxPlayerGameState, int depth)
{
    if(depth == 0)
    {
        return eachMaxPlayerGameState.evaluateState();
    }
    
    int bestMinValue = INT_MAX;
    std::vector<std::pair<Move, GameBoard>> minPlayerMovesWithTheirGameStates = eachMaxPlayerGameState.generateMoves();
    for ( int eachMinPlayerPair; eachMinPlayerPair < minPlayerMovesWithTheirGameStates.size(); eachMinPlayerPair++)
    {
        int value = Algo::maxPlayer(minPlayerMovesWithTheirGameStates[eachMinPlayerPair].second.clone(true), depth - 1);
        if(bestMinValue > value)
        {
            bestMinValue = value;
        }
    }
    return bestMinValue;

}

int Algo::maxPlayer(GameBoard eachMinPlayerGameState, int depth)
{
    if(depth == 0)
    {
        return eachMinPlayerGameState.evaluateState();
    }
    
    int bestMaxValue = INT_MIN;
    std::vector<std::pair<Move, GameBoard>> maxPlayerMovesWithTheirGameStates = eachMinPlayerGameState.generateMoves();
    for ( int eachMaxPlayerPair; eachMaxPlayerPair < maxPlayerMovesWithTheirGameStates.size(); eachMaxPlayerPair++)
    {
        int value = Algo::minPlayer(maxPlayerMovesWithTheirGameStates[eachMaxPlayerPair].second.clone(true), depth - 1);
        if(bestMaxValue < value)
        {
            bestMaxValue = value;
        }
    }
    return bestMaxValue; 

}

int GameBoard::evaluateState()
{
    int maxValue = INT_MIN;
    int minValue = INT_MAX;
    for (int eachCellInState = 0; eachCellInState < this->myPieces.size(); eachCellInState++)
    {
        int currentPieceTypeValue = pieceTypeValues[this->myPieces[eachCellInState].pieceType] + (eachCellInState<=this->enemyPieces.size())?this->enemyPieces[eachCellInState].cellIndex:20;
        maxValue += maxValue + currentPieceTypeValue;
    }
    for (int eachCellInState = 0; eachCellInState < this->enemyPieces.size(); eachCellInState++)
    {
        int currentPieceTypeValue = pieceTypeValues[this->enemyPieces[eachCellInState].pieceType] + (eachCellInState<=this->myPieces.size())?this->myPieces[eachCellInState].cellIndex:10;
        minValue += minValue + currentPieceTypeValue;
    }
    Cell myKingPosition, enemyKingPosition;
    int flag=0;
    for(int i=0;i<this->boardCells.size();i++){
        Cell cell = this->boardCells[i];
        if(cell.pieceType == pieceTypes.at('k') && cell.pieceColor == turn){
            myKingPosition = cell;
            flag++;
        }
        if(cell.pieceType == pieceTypes.at('k') && cell.pieceColor == !turn){
            enemyKingPosition = cell;
            flag++;
        }
        if(flag==2)
            break;
    }
    // for(int i=0;i<this->boardCells.size();i++){
    //     Cell cell = this->boardCells[i];
    //     if(cell.pieceType == pieceTypes.at('k') && cell.pieceColor == !turn){
    //         enemyKingPosition = cell;
    //         break;
    //     }
    // }
    vector<int> mykingSafety = this -> getPiecesAttacksOnSquareIndex(myKingPosition.cellIndex, !this -> turn);
    vector<int> enemyKingSafety = this ->getPiecesAttacksOnSquareIndex(enemyKingPosition.cellIndex, this -> turn);
    return maxValue - minValue + (mykingSafety.size() * -2200) + (enemyKingSafety.size() * 2150);
}

std::string Algo::iterativeDeepening(string orgFenString)
{   
    GameBoard newGameBoard = GameBoard::parseFen2(orgFenString); 
    int maxDepth = 4;
    int depth = 1;
    Move newMove;
    while(depth < maxDepth + 1)
    {
        newMove = Algo::maxChoice(newGameBoard, depth);
        depth++;
    }
    return newMove.uciFormat();
}

Move Algo::maxChoice(GameBoard &initGameBoard, int depth)
{
    int bestMaxValue = INT_MIN;
    Move bestMove;
    std::vector<std::pair<Move, GameBoard>> movesWithTheirGameStates = initGameBoard.generateMoves();
    for( int eachPair = 0; eachPair < movesWithTheirGameStates.size(); eachPair++)
    {
        int value = Algo::minPlayer(movesWithTheirGameStates[eachPair].second.clone(true), depth - 1);
        if(bestMaxValue < value)
        {
            bestMaxValue = value;
            bestMove = movesWithTheirGameStates[eachPair].first;
        }
    }
    return bestMove;
}
