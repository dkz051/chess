#include "global.h"

#include <cstring>

QTcpServer *tcpServer = nullptr;
QTcpSocket *tcpSocket = nullptr;

Board::Board() {
	memset(board, 0, sizeof board);
}

Piece *Board::operator[](const qint32 &index) {
	return board[index];
}
