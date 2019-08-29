#include "controller.h"

#include "graphics.h"

RoleType opponent(RoleType role) {
	assert(role != RoleType::Neither);
	return role == RoleType::White ? RoleType::Black : RoleType::White;
}

quint32 cartesianToSequential(const Position &cartesian) {
	return quint32(cartesian.first * ranks + cartesian.second);
}

Position sequentialToCartesian(quint32 sequential) {
	return Position(sequential / ranks, sequential % ranks);
}

Position algebraicToCartesian(const QString &algebraic) {
	assert(algebraic.length() == 2);
	return Position(algebraic[0].toLatin1() - 'a', ranks - 1 - (algebraic[1].toLatin1() - '0'));
}

QString cartesianToAlgebraic(const Position &cartesian) {
	return QString(QChar(cartesian.first + 'a')) + QChar(ranks - cartesian.second + '0');
}

void sendMessage(QTcpSocket *tcpSocket, const QString &message) {
	tcpSocket->write(message.toUtf8());
	tcpSocket->flush();
}

Position getPositionXY(qint32 x, qint32 y, qint32 width, qint32 height, RoleType role) {
	qint32 gridSize = getChessboardGridSize(width, height);
	if (role == RoleType::White) {
		return Position((x - (width - gridSize * ranks) / 2) / gridSize, (y - (height - gridSize * ranks) / 2) / gridSize);
	} else {
		return Position(ranks - 1 - (x - (width - gridSize * ranks) / 2) / gridSize, ranks - 1 - (y - (height - gridSize * ranks) / 2) / gridSize);
	}
}

PositionSet attackRange(Position from, RoleType role, const Chessboard &chessboard) {
	assert(role != RoleType::Neither);

	PositionSet ans;
	ans.reset();
	switch (ChessmanType type = chessboard[from].second; type) {
		case ChessmanType::King:
		case ChessmanType::Knight: {
			const auto dirX = (type == ChessmanType::King ? dir8X : dirKnightX);
			const auto dirY = (type == ChessmanType::King ? dir8Y : dirKnightY);
			for (qint32 k = 0; k < 8; ++k) {
				qint32 x = from.first + dirX[k], y = from.second + dirY[k];
				if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].first != role) {
					ans.set(cartesianToSequential(Position(x, y)));
				}
			}
			break;
		}
		case ChessmanType::Queen:
		case ChessmanType::Rook:
		case ChessmanType::Bishop: {
			qint32 begin = 0, end = 8;
			if (type == ChessmanType::Rook) {
				end = 4;
			} else if (type == ChessmanType::Bishop) {
				begin = 4;
			}
			for (qint32 k = begin; k < end; ++k) {
				qint32 x = from.first, y = from.second;
				while (true) {
					x += dir8X[k];
					y += dir8Y[k];
					if (x >= 0 && x < ranks && y >= 0 && y < ranks) {
						if (chessboard[x][y].first != role) {
							ans.set(cartesianToSequential(Position(x, y)));
						}
						if (chessboard[x][y].second != ChessmanType::None) {
							break;
						}
					} else {
						break;
					}
				}
			}
			break;
		}
		case ChessmanType::Pawn: {
			auto dirPawnAttackY = (role == RoleType::White ? dirPawnAttackYWhite : dirPawnAttackYBlack);
			for (qint32 k = 0; k < 2; ++k) {
				qint32 x = from.first + dirPawnAttackX[k], y = from.second + dirPawnAttackY[k];
				if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].first != role && chessboard[x][y].first != RoleType::Neither) {
					ans.set(cartesianToSequential(Position(x, y)));
				}
			}
			break;
		}
		case ChessmanType::None: {
			break;
		}
	}
	return ans;
}

PositionSet moveRange(Position from, RoleType role, const Chessboard &chessboard) {
	PositionSet ans = attackRange(from, role, chessboard);
	switch (ChessmanType type = chessboard[from].second; type) {
		case ChessmanType::King:
		case ChessmanType::Knight:
		case ChessmanType::Queen:
		case ChessmanType::Rook:
		case ChessmanType::Bishop: {
			break;
		}
		case ChessmanType::Pawn: {
			auto dirPawnMoveY = (role == RoleType::White ? dirPawnMoveYWhite : dirPawnMoveYBlack);
			qint32 startRank = (role == RoleType::White ? pawnStartRankWhite : pawnStartRankBlack);
			for (qint32 k = 0; k < (from.second == startRank ? 2 : 1); ++k) {
				qint32 x = from.first + dirPawnMoveX[k], y = from.second + dirPawnMoveY[k];
				if (x >= 0 && x < ranks && y >= 0 && y < ranks && chessboard[x][y].second == ChessmanType::None) {
					ans.set(cartesianToSequential(Position(x, y)));
				} else {
					break;
				}
			}
			break;
		}
		case ChessmanType::None: {
			break;
		}
	}
	return ans;
}

bool isAttacking(Position from, Position to, RoleType fromRole, const Chessboard &chessboard) {
	return attackRange(from, fromRole, chessboard).test(cartesianToSequential(to));
}

bool isMovePossible(Position from, Position to, RoleType fromRole, const Chessboard &chessboard) {
	return moveRange(from, fromRole, chessboard).test(cartesianToSequential(to));
}

void moveChessman(Position from, Position to, Chessboard &chessboard) {
	// Assuming that this move is legal
	chessboard[to] = chessboard[from];
	chessboard[from] = nullChessman;
}

bool isInCheck(RoleType role, const Chessboard &chessboard) {
	qint32 kingX, kingY;
	for (kingX = 0; kingX < ranks; ++kingX) {
		for (kingY = 0; kingY < ranks; ++kingY) {
			if (chessboard[kingX][kingY] == Chessman(role, ChessmanType::King)) {
				break;
			}
		}
	}
	assert(kingX < ranks);

	for (qint32 x = 0; x < ranks; ++x) {
		for (qint32 y = 0; y < ranks; ++y) {
			if (chessboard[x][y].first != role) {
				if (isAttacking(Position(x, y), Position(kingX, kingY), opponent(role), chessboard)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool isCheckmate(RoleType role, const Chessboard &chessboard) {
	return isInCheck(role, chessboard) && isOutOfPossibleMoves(role, chessboard);
}

bool isStalemate(RoleType role, const Chessboard &chessboard) {
	return !isInCheck(role, chessboard) && isOutOfPossibleMoves(role, chessboard);
}

bool isOutOfPossibleMoves(RoleType role, const Chessboard &chessboard) {
	for (qint32 x = 0; x < ranks; ++x) {
		for (qint32 y = 0; y < ranks; ++y) {
			if (chessboard[x][y].first == role) {
				PositionSet moves = moveRange(Position(x, y), role, chessboard);
				for (quint32 i = 0; i < squares; ++i) {
					if (moves.test(i)) {
						Chessboard moved = chessboard;
						moveChessman(Position(x, y), sequentialToCartesian(i), moved);
						if (!isInCheck(role, moved)) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}
