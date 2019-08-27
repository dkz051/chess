#include "global.h"

#include <cstring>

QTcpServer *tcpServer = nullptr;
QTcpSocket *tcpSocket = nullptr;

Chessboard::Chessboard() {
	memset(chessboard, 0, sizeof chessboard);
}

void Chessboard::defaultChessboard() {
	chessboard[0][7] = chessboard[7][7] = Piece(RoleType::White, PieceType::Rook);
	chessboard[1][7] = chessboard[6][7] = Piece(RoleType::White, PieceType::Knight);
	chessboard[2][7] = chessboard[5][7] = Piece(RoleType::White, PieceType::Bishop);
	chessboard[3][7] = Piece(RoleType::White, PieceType::Queen);
	chessboard[4][7] = Piece(RoleType::White, PieceType::King);

	chessboard[0][0] = chessboard[7][0] = Piece(RoleType::Black, PieceType::Rook);
	chessboard[1][0] = chessboard[6][0] = Piece(RoleType::Black, PieceType::Knight);
	chessboard[2][0] = chessboard[5][0] = Piece(RoleType::Black, PieceType::Bishop);
	chessboard[3][0] = Piece(RoleType::Black, PieceType::Queen);
	chessboard[4][0] = Piece(RoleType::Black, PieceType::King);

	for (qint32 i = 0; i < size; ++i) {
		chessboard[i][6] = Piece(RoleType::White, PieceType::Pawn);
		chessboard[i][1] = Piece(RoleType::Black, PieceType::Pawn);
		for (qint32 j = 2; j < 6; ++j) {
			chessboard[i][j] = Piece(RoleType::Neither, PieceType::None);
		}
	}
}

Piece *Chessboard::operator[](const qint32 &index) {
	return chessboard[index];
}

const Piece *Chessboard::operator[](const qint32 &index) const {
	return chessboard[index];
}
