#ifndef RENDER_H
#define RENDER_H

#include <QImage>
#include <QPainter>
#include <QIntegerForSize>

#include "global.h"

qint32 getChessboardGridSize(qint32 width, qint32 height);
Position getPositionXY(qint32 x, qint32 y, qint32 width, qint32 height);

void renderChessboard(QPainter *canvas, qint32 width, qint32 height, RoleType role, const Chessboard &chessboard);
void renderAttacks(QPainter *canvas, qint32 width, qint32 height, Position position, const Chessboard &chessboard);

#endif // RENDER_H
