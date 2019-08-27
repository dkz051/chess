#ifndef GLOBALS_H
#define GLOBALS_H

#include <QtNetwork>
#include <QIntegerForSize>

enum Pieces { King, Queen, Rook, Bishop, Knight, Pawn };
enum CommandType { Role, Move, Castling, Draw, Surrender, Message, Exit };

extern QTcpServer *tcpServer;
extern QTcpSocket *tcpSocket;

extern const qint32 columns;
extern const qint32 dir4X[4];
extern const qint32 dir4Y[4];
extern const qint32 dir8X[8];
extern const qint32 dir8Y[8];
extern const qint32 dirKnightX[8];
extern const qint32 dirKnightY[8];

#endif // GLOBALS_H
