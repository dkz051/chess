#ifndef GLOBALS_H
#define GLOBALS_H

#include <utility>

#include <QColor>
#include <QtNetwork>
#include <QIntegerForSize>

enum RoleType { White = 0, Black };
enum PieceType { King = 0, Queen, Rook, Bishop, Knight, Pawn };
enum CommandType { Role = 0, Move, Castling, Draw, Surrender, Message, Exit };

const qint32 rows = 8;
const qint32 columns = 8;

const qint32 dir4X[4] = {0, 0, 1, -1};
const qint32 dir4Y[4] = {-1, 1, 0, 0};
const qint32 dir8X[8] = {0, 0, 1, -1, -1, -1, 1, 1};
const qint32 dir8Y[8] = {-1, 1, 0, 0, -1, 1, -1, 1};
const qint32 dirKnightX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
const qint32 dirKnightY[8] = {1, 2, 2, 1, -1, -2, -2, -1};

const QColor colorDark(181, 135, 99, 255);
const QColor colorLight(240, 218, 181, 255);

const QString roleName[] = {"white", "black"};
const QString pieceName[] = {"king", "queen", "rook", "bishop", "knight", "pawn"};

typedef std::pair<RoleType, PieceType> Piece;

struct Chessboard {
	Piece chessboard[columns][rows];

	Chessboard();
	Piece *operator[](const qint32 &index);
};

extern QTcpServer *tcpServer;
extern QTcpSocket *tcpSocket;

#endif // GLOBALS_H
