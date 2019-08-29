#include "global.h"
#include "controller.h"

#include <cstring>

#include <QFile>
#include <QTextStream>

Chessboard::Chessboard() {
	memset(chessboard, 0, sizeof chessboard);
}

void Chessboard::defaultChessboard() {
	chessboard[0][7] = chessboard[7][7] = Chessman(RoleType::White, ChessmanType::Rook);
	chessboard[1][7] = chessboard[6][7] = Chessman(RoleType::White, ChessmanType::Knight);
	chessboard[2][7] = chessboard[5][7] = Chessman(RoleType::White, ChessmanType::Bishop);
	chessboard[3][7] = Chessman(RoleType::White, ChessmanType::Queen);
	chessboard[4][7] = Chessman(RoleType::White, ChessmanType::King);

	chessboard[0][0] = chessboard[7][0] = Chessman(RoleType::Black, ChessmanType::Rook);
	chessboard[1][0] = chessboard[6][0] = Chessman(RoleType::Black, ChessmanType::Knight);
	chessboard[2][0] = chessboard[5][0] = Chessman(RoleType::Black, ChessmanType::Bishop);
	chessboard[3][0] = Chessman(RoleType::Black, ChessmanType::Queen);
	chessboard[4][0] = Chessman(RoleType::Black, ChessmanType::King);

	for (qint32 i = 0; i < ranks; ++i) {
		chessboard[i][1] = Chessman(RoleType::Black, ChessmanType::Pawn);
		for (qint32 j = 2; j < 6; ++j) {
			chessboard[i][j] = Chessman(RoleType::Neither, ChessmanType::None);
		}
		chessboard[i][6] = Chessman(RoleType::White, ChessmanType::Pawn);
	}
}

void Chessboard::nullChessboard() {
	for (qint32 i = 0; i < ranks; ++i) {
		for (qint32 j = 0; j < ranks; ++j) {
			chessboard[i][j] = nullChessman;
		}
	}
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

bool Chessboard::loadLocalFile(const QString &path, RoleType &firstRole) {
	QFile file(path);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}
	QTextStream stream(&file);

	nullChessboard();

	firstRole = RoleType::Neither;
	RoleType currentRole = RoleType::Neither;
	while (!stream.atEnd()) {
		QStringList line = stream.readLine().split(" ");
		assert(line.size() > 0);
		if (line[0] == "white") {
			if (firstRole == RoleType::Neither) {
				firstRole = RoleType::White;
			}
			currentRole = RoleType::White;
		} else if (line[0] == "black") {
			if (firstRole == RoleType::Neither) {
				firstRole = RoleType::Black;
			}
			currentRole = RoleType::Black;
		} else {
			assert(line.size() >= 2 && line.size() == line[1].toInt() + 2);
			ChessmanType chessmanType = ChessmanType::None;
			if (line[0] == "king") {
				chessmanType = ChessmanType::King;
			} else if (line[0] == "queen") {
				chessmanType = ChessmanType::Queen;
			} else if (line[0] == "rook") {
				chessmanType = ChessmanType::Rook;
			} else if (line[0] == "bishop") {
				chessmanType = ChessmanType::Bishop;
			} else if (line[0] == "knight") {
				chessmanType = ChessmanType::Knight;
			} else if (line[0] == "pawn") {
				chessmanType = ChessmanType::Pawn;
			}
			assert(chessmanType != ChessmanType::None);
			for (qint32 i = 2, j = line[1].toInt(); i < j + 2; ++i) {
				(*this)[algebraicToCartesian(line[i])] = Chessman(currentRole, chessmanType);
			}
		}
	}

	file.close();
	return true;
}

bool Chessboard::saveLocalFile(const QString &path, const RoleType &firstRole) const {
	QFile file(path);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		return false;
	}
	QTextStream stream(&file);

	assert(firstRole != RoleType::Neither);

	for (RoleType role = firstRole; true; ) {
		if (role == RoleType::White) {
			stream << "white\n";
		} else {
			stream << "black\n";
		}

		auto writeChessman = [&](Chessman chessman, QString string) -> void {
			QVector<Position> positions;
			for (qint32 i = 0; i < ranks; ++i) {
				for (qint32 j = 0; j < ranks; ++j) {
					if (chessboard[i][j] == chessman) {
						positions.push_back(Position(i, j));
					}
				}
			}
			stream << string << ' ' << positions.size();
			for (qint32 i = 0; i < positions.size(); ++i) {
				stream << ' ' << cartesianToAlgebraic(positions[i]);
			}
			stream << '\n';
		};

		writeChessman(Chessman(role, ChessmanType::King), "king");
		writeChessman(Chessman(role, ChessmanType::Queen), "queen");
		writeChessman(Chessman(role, ChessmanType::Rook), "rook");
		writeChessman(Chessman(role, ChessmanType::Bishop), "bishop");
		writeChessman(Chessman(role, ChessmanType::Knight), "knight");
		writeChessman(Chessman(role, ChessmanType::Pawn), "pawn");

		if (role == firstRole) {
			role = opponent(role);
		} else {
			break;
		}
	}

	file.close();
	return true;
}

QString Chessboard::serialize() const {
	QString ans = "";
	for (qint32 i = 0; i < ranks; ++i) {
		for (qint32 j = 0; j < ranks; ++j) {
			RoleType role = chessboard[j][i].first;
			ChessmanType type = chessboard[j][i].second;
			if (role == RoleType::White && type == ChessmanType::King) {
				ans += 'K';
			} else if (role == RoleType::White && type == ChessmanType::Queen) {
				ans += 'Q';
			} else if (role == RoleType::White && type == ChessmanType::Rook) {
				ans += 'R';
			} else if (role == RoleType::White && type == ChessmanType::Bishop) {
				ans += 'B';
			} else if (role == RoleType::White && type == ChessmanType::Knight) {
				ans += 'N';
			} else if (role == RoleType::White && type == ChessmanType::Pawn) {
				ans += 'P';
			} else if (role == RoleType::Black && type == ChessmanType::King) {
				ans += 'k';
			} else if (role == RoleType::Black && type == ChessmanType::Queen) {
				ans += 'q';
			} else if (role == RoleType::Black && type == ChessmanType::Rook) {
				ans += 'r';
			} else if (role == RoleType::Black && type == ChessmanType::Bishop) {
				ans += 'b';
			} else if (role == RoleType::Black && type == ChessmanType::Knight) {
				ans += 'n';
			} else if (role == RoleType::Black && type == ChessmanType::Pawn) {
				ans += 'p';
			} else {
				ans += '.';
			}
		}
	}
	return ans;
}

void Chessboard::deserialize(const QString &string) {
	for (qint32 i = 0, k = 0; i < ranks; ++i) {
		for (qint32 j = 0; j < ranks; ++j) {
			if (string[k] == '.') {
				chessboard[j][i] = nullChessman;
			} else if (string[k] == 'K') {
				chessboard[j][i] = Chessman(RoleType::White, ChessmanType::King);
			} else if (string[k] == 'Q') {
				chessboard[j][i] = Chessman(RoleType::White, ChessmanType::Queen);
			} else if (string[k] == 'R') {
				chessboard[j][i] = Chessman(RoleType::White, ChessmanType::Rook);
			} else if (string[k] == 'B') {
				chessboard[j][i] = Chessman(RoleType::White, ChessmanType::Bishop);
			} else if (string[k] == 'N') {
				chessboard[j][i] = Chessman(RoleType::White, ChessmanType::Knight);
			} else if (string[k] == 'P') {
				chessboard[j][i] = Chessman(RoleType::White, ChessmanType::Pawn);
			} else if (string[k] == 'k') {
				chessboard[j][i] = Chessman(RoleType::Black, ChessmanType::King);
			} else if (string[k] == 'q') {
				chessboard[j][i] = Chessman(RoleType::Black, ChessmanType::Queen);
			} else if (string[k] == 'r') {
				chessboard[j][i] = Chessman(RoleType::Black, ChessmanType::Rook);
			} else if (string[k] == 'b') {
				chessboard[j][i] = Chessman(RoleType::Black, ChessmanType::Bishop);
			} else if (string[k] == 'n') {
				chessboard[j][i] = Chessman(RoleType::Black, ChessmanType::Knight);
			} else if (string[k] == 'p') {
				chessboard[j][i] = Chessman(RoleType::Black, ChessmanType::Pawn);
			} else {
				assert(false);
			}
			++k;
		}
	}
}
