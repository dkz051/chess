#include "graphics.h"

#include "global.h"

qint32 getChessboardGridSize(qint32 width, qint32 height) {
	return qint32(std::min(width * 0.9 / size, height * 0.9 / size));
}

void renderChessboard(QPainter *canvas, qint32 width, qint32 height, const Chessboard &chessboard) {
	canvas->save();
	qint32 gridSize = getChessboardGridSize(width, height);
	canvas->translate((width - gridSize * size) / 2, (height - gridSize * size) / 2);

	for (qint32 i = 0; i < size; ++i) {
		for (qint32 j = 0; j < size; ++j) {
			canvas->setPen(Qt::NoPen);
			canvas->setBrush(((i + j) % 2 == 0 ? colorLight : colorDark));
			canvas->drawRect(QRect(i * gridSize, j * gridSize, gridSize, gridSize));
			canvas->drawImage(QRect(i * gridSize, j * gridSize, gridSize, gridSize), pieceToImage[chessboard[i][j]]);
		}
	}

	canvas->setPen(Qt::black);
	for (qint32 i = 0; i < size; ++i) {
		canvas->drawText(QRect(-gridSize, i * gridSize, gridSize, gridSize), Qt::AlignVCenter | Qt::AlignRight, QString("%1").arg(size - i));
		canvas->drawText(QRect(i * gridSize, size * gridSize, gridSize, gridSize), Qt::AlignHCenter | Qt::AlignTop, QString("%1").arg(QString('a' + i)));
	}

	canvas->restore();
}
