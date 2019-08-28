#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QVector>

#include "global.h"

Position getPositionXY(qint32 x, qint32 y, qint32 width, qint32 height, RoleType role);
QVector<Position> attackRange(Position from, RoleType role, const Chessboard &chessboard);
QVector<Position> moveRange(Position from, RoleType role, const Chessboard &chessboard);
bool isAttacking(Position from, Position to, RoleType fromRole, const Chessboard &chessboard);
bool isMovePossible(Position from, Position to, RoleType fromRole, const Chessboard &chessboard);

#endif // CONTROLLER_H
