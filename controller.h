#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QVector>
#include <QtNetwork>

#include "global.h"

RoleType opponent(RoleType role);

quint32 cartesianToSequential(const Position &cartesian);
Position sequentialToCartesian(quint32 sequential);

Position algebraicToCartesian(const QString &algebraic);
QString cartesianToAlgebraic(const Position &cartesian);

void sendMessage(QTcpSocket *tcpSocket, const QString &message);

Position getPositionXY(qint32 x, qint32 y, qint32 width, qint32 height, RoleType role);

PositionSet attackRange(Position from, RoleType role, const Chessboard &chessboard);
PositionSet moveRange(Position from, RoleType role, const Chessboard &chessboard);

bool isAttacking(Position from, Position to, RoleType fromRole, const Chessboard &chessboard);
bool isMovePossible(Position from, Position to, RoleType fromRole, const Chessboard &chessboard);

bool isInCheck(RoleType role, const Chessboard &chessboard);
bool isCheckmate(RoleType role, const Chessboard &chessboard);
bool isStalemate(RoleType role, const Chessboard &chessboard);

bool isOutOfPossibleMoves(RoleType role, const Chessboard &chessboard);

void moveChessman(Position from, Position to, Chessboard &chessboard);

#endif // CONTROLLER_H
