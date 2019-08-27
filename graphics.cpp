#include "render.h"

#include "global.h"

qint32 getChessboardGridSize(qint32 width, qint32 height) {
	return std::min(width / columns, height / rows);
}

void renderChessboard(QPainter *canvas, qint32 width, qint32 height, const Chessboard &chessboard) {
	canvas->save();
	qint32 gridSize = getChessboardGridSize(width, height);
	canvas->translate((width - gridSize * columns) / 2, (height - gridSize * rows) / 2);

	for (qint32 i = 0; i < columns; ++i) {
		for (qint32 j = 0; j < rows; ++j) {
			canvas->setPen(Qt::NoPen);
			canvas->setBrush(((i + j) % 2 == 0 ? colorLight : colorDark));
			canvas->drawRect(QRect(i * gridSize, j * gridSize, gridSize, gridSize));
			//canvas->drawImage()
		}
	}

	canvas->restore();
}
