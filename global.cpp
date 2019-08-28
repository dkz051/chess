#include "global.h"

#include <cstring>

//QTcpServer *tcpServer = nullptr;
//QTcpSocket *tcpSocket = nullptr;

Chessboard::Chessboard() {
	memset(chessboard, 0, sizeof chessboard);
}

Chessboard Chessboard::defaultChessboard() {
	Chessboard ans;

	ans[0][7] = ans[7][7] = Piece(RoleType::White, PieceType::Rook);
	ans[1][7] = ans[6][7] = Piece(RoleType::White, PieceType::Knight);
	ans[2][7] = ans[5][7] = Piece(RoleType::White, PieceType::Bishop);
	ans[3][7] = Piece(RoleType::White, PieceType::Queen);
	ans[4][7] = Piece(RoleType::White, PieceType::King);

	ans[0][0] = ans[7][0] = Piece(RoleType::Black, PieceType::Rook);
	ans[1][0] = ans[6][0] = Piece(RoleType::Black, PieceType::Knight);
	ans[2][0] = ans[5][0] = Piece(RoleType::Black, PieceType::Bishop);
	ans[3][0] = Piece(RoleType::Black, PieceType::Queen);
	ans[4][0] = Piece(RoleType::Black, PieceType::King);

	for (qint32 i = 0; i < ranks; ++i) {
		ans[i][1] = Piece(RoleType::Black, PieceType::Pawn);
		for (qint32 j = 2; j < 6; ++j) {
			ans[i][j] = Piece(RoleType::Neither, PieceType::None);
		}
		ans[i][6] = Piece(RoleType::White, PieceType::Pawn);
	}

	return ans;
}

Piece *Chessboard::operator[](const qint32 &index) {
	return chessboard[index];
}

const Piece *Chessboard::operator[](const qint32 &index) const {
	return chessboard[index];
}
