#ifndef RENDER_H
#define RENDER_H

#include <QImage>
#include <QPainter>
#include <QIntegerForSize>

#include "global.h"

void initializeGraphics();

qint32 getChessboardGridSize(qint32 width, qint32 height);

void renderChessboard(QPainter *canvas, qint32 width, qint32 height, const Chessboard &chessboard);

#endif // RENDER_H
