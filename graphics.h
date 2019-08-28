#ifndef RENDER_H
#define RENDER_H

#include <QImage>
#include <QPainter>
#include <QIntegerForSize>

#include "global.h"

qint32 getChessboardGridSize(qint32 width, qint32 height);

void renderMove(QPainter *canvas, qint32 width, qint32 height, RoleType role, Position position, const Chessboard &chessboard);
void renderChessboard(QPainter *canvas, qint32 width, qint32 height, RoleType role);
void renderPieces(QPainter *canvas, qint32 width, qint32 height, RoleType role, const Chessboard &chessboard);
void renderMoves(QPainter *canvas, qint32 width, qint32 height, RoleType role, Position position, const Chessboard &chessboard);

#endif // RENDER_H
