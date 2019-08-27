#ifndef RENDER_H
#define RENDER_H

#include <QPainter>
#include <QIntegerForSize>

#include "global.h"

qint32 getChessboardGridSize(qint32 width, qint32 height);

void renderChessboard(QPainter *canvas, qint32 width, qint32 height, const Chessboard &chessboard);

#endif // RENDER_H
