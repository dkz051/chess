#include "controller.h"

#include "graphics.h"

Position getPositionXY(qint32 x, qint32 y, qint32 width, qint32 height, RoleType role) {
	qint32 gridSize = getChessboardGridSize(width, height);
	if (role == RoleType::White) {
		return Position((x - (width - gridSize * ranks) / 2) / gridSize, (y - (height - gridSize * ranks) / 2) / gridSize);
	} else {
		return Position(ranks - 1 - (x - (width - gridSize * ranks) / 2) / gridSize, ranks - 1 - (y - (height - gridSize * ranks) / 2) / gridSize);
	}
}

QVector<Position> attackRange(Position from, RoleType role, const Chessboard &chessboard) {
	assert(role != RoleType::Neither);

	QVector<Position> ans;
	switch (PieceType type = chessboard[from.first][from.second].second; type) {
		case PieceType::King:
		case PieceType::Knight: {
			const auto dirX = (type == PieceType::King ? dir8X : dirKnightX);
			const auto dirY = (type == PieceType::King ? dir8Y : dirKnightY);
			for (qint32 k = 0; k < 8; ++k) {
				qint32 x = from.first + dirX[k], y = from.second + dirY[k];
				if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].second == PieceType::None) {
					ans.push_back(Position(x, y));
				}
			}
			break;
		}
		case PieceType::Queen:
		case PieceType::Rook:
		case PieceType::Bishop: {
			qint32 begin = 0, end = 8;
			if (type == PieceType::Rook) {
				end = 4;
			} else if (type == PieceType::Bishop) {
				begin = 4;
			}
			for (qint32 k = begin; k < end; ++k) {
				qint32 x = from.first, y = from.second;
				while (true) {
					x += dir8X[k];
					y += dir8Y[k];
					if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].second == PieceType::None) {
						ans.push_back(Position(x, y));
					} else {
						break;
					}
				}
			}
			break;
		}
		case PieceType::Pawn: {
			auto dirPawnAttackY = (role == RoleType::White ? dirPawnAttackYWhite : dirPawnAttackYBlack);
			for (qint32 k = 0; k < 2; ++k) {
				qint32 x = from.first + dirPawnAttackX[k], y = from.second + dirPawnAttackY[k];
				if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].second == PieceType::None) {
					ans.push_back(Position(x, y));
				}
			}
			break;
		}
		case PieceType::None: {
			break;
		}
	}
	return ans;
}

QVector<Position> moveRange(Position from, RoleType role, const Chessboard &chessboard) {
	QVector<Position> ans = attackRange(from, role, chessboard);
	switch (PieceType type = chessboard[from.first][from.second].second; type) {
		case PieceType::King:
		case PieceType::Knight:
		case PieceType::Queen:
		case PieceType::Rook:
		case PieceType::Bishop: {
			break;
		}
		case PieceType::Pawn: {
			auto dirPawnMoveY = (role == RoleType::White ? dirPawnMoveYWhite : dirPawnMoveYBlack);
			qint32 startRank = (role == RoleType::White ? pawnStartRankWhite : pawnStartRankBlack);
			for (qint32 k = 0; k < (from.second == startRank ? 2 : 1); ++k) {
				qint32 x = from.first + dirPawnMoveX[k], y = from.second + dirPawnMoveY[k];
				if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].second == PieceType::None) {
					ans.push_back(Position(x, y));
				}
			}
			break;
		}
		case PieceType::None: {
			break;
		}
	}
	return ans;
}

bool isAttacking(Position from, Position to, RoleType fromRole, const Chessboard &chessboard) {
	return attackRange(from, fromRole, chessboard).indexOf(to) != -1;
}

bool isMovePossible(Position from, Position to, RoleType fromRole, const Chessboard &chessboard) {
	return moveRange(from, fromRole, chessboard).indexOf(to) != -1;
}
