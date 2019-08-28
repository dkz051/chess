#include "graphics.h"

#include "global.h"

qint32 getChessboardGridSize(qint32 width, qint32 height) {
	return qint32(std::min(width * 0.9 / ranks, height * 0.9 / ranks));
}

Position getPositionXY(qint32 x, qint32 y, qint32 width, qint32 height) {
	qint32 gridSize = getChessboardGridSize(width, height);
	return Position((x - (width - gridSize * ranks) / 2) / gridSize, (y - (height - gridSize * ranks) / 2) / gridSize);
}

void renderChessboard(QPainter *canvas, qint32 width, qint32 height, RoleType role, const Chessboard &chessboard) {
	if (role == RoleType::Neither) {
		return;
	}

	canvas->save();
	qint32 gridSize = getChessboardGridSize(width, height);
	canvas->translate((width - gridSize * ranks) / 2, (height - gridSize * ranks) / 2);

	for (qint32 i = 0; i < ranks; ++i) {
		for (qint32 j = 0; j < ranks; ++j) {
			canvas->setPen(Qt::NoPen);
			canvas->setBrush(((i + j) % 2 == 0 ? colorLight : colorDark));

			QRect rect;

			if (role == RoleType::White) {
				rect = QRect(i * gridSize, j * gridSize, gridSize, gridSize);
			} else if (role == RoleType::Black) {
				rect = QRect((ranks - 1 - i) * gridSize, (ranks - 1 - j) * gridSize, gridSize, gridSize);
			}

			canvas->drawRect(rect);
			canvas->drawImage(rect, pieceToImage[chessboard[i][j]]);
		}
	}

	canvas->setPen(Qt::black);
	for (qint32 i = 0; i < ranks; ++i) {
		canvas->drawText(QRect(-gridSize, i * gridSize, gridSize, gridSize), Qt::AlignVCenter | Qt::AlignRight, QString("%1").arg(role == RoleType::White ? ranks - i : i + 1));
		canvas->drawText(QRect(i * gridSize, ranks * gridSize, gridSize, gridSize), Qt::AlignHCenter | Qt::AlignTop, QString("%1").arg(QString(role == RoleType::White ? 'a' + i : 'a' + ranks - 1 - i)));
	}

	canvas->restore();
}
