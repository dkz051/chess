#include "global.h"

#include <cstring>

Chessboard::Chessboard() {
	memset(chessboard, 0, sizeof chessboard);
}

Chessboard Chessboard::defaultChessboard() {
	Chessboard ans;

	ans[0][7] = ans[7][7] = Chessman(RoleType::White, ChessmanType::Rook);
	ans[1][7] = ans[6][7] = Chessman(RoleType::White, ChessmanType::Knight);
	ans[2][7] = ans[5][7] = Chessman(RoleType::White, ChessmanType::Bishop);
	ans[3][7] = Chessman(RoleType::White, ChessmanType::Queen);
	ans[4][7] = Chessman(RoleType::White, ChessmanType::King);

	ans[0][0] = ans[7][0] = Chessman(RoleType::Black, ChessmanType::Rook);
	ans[1][0] = ans[6][0] = Chessman(RoleType::Black, ChessmanType::Knight);
	ans[2][0] = ans[5][0] = Chessman(RoleType::Black, ChessmanType::Bishop);
	ans[3][0] = Chessman(RoleType::Black, ChessmanType::Queen);
	ans[4][0] = Chessman(RoleType::Black, ChessmanType::King);

	for (qint32 i = 0; i < ranks; ++i) {
		ans[i][1] = Chessman(RoleType::Black, ChessmanType::Pawn);
		for (qint32 j = 2; j < 6; ++j) {
			ans[i][j] = Chessman(RoleType::Neither, ChessmanType::None);
		}
		ans[i][6] = Chessman(RoleType::White, ChessmanType::Pawn);
	}

	return ans;
}

Chessman *Chessboard::operator[](const qint32 &index) {
	return chessboard[index];
}

const Chessman *Chessboard::operator[](const qint32 &index) const {
	return chessboard[index];
}

Chessman &Chessboard::operator[](const Position &position) {
	return chessboard[position.first][position.second];
}

const Chessman &Chessboard::operator[](const Position &position) const {
	return chessboard[position.first][position.second];
}
