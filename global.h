#ifndef GLOBALS_H
#define GLOBALS_H

#include <utility>

#include <QMap>
#include <QColor>
#include <QImage>
#include <QtNetwork>
#include <QIntegerForSize>

enum RoleType { White = 7, Black = 0, Neither = 1 };
enum ChessmanType { King = 4, Queen = 10, Rook = 5, Bishop = 3, Knight = 2, Pawn = 1, None = 99 };
//enum CommandType { Role = 0, Move, Castling, Draw, Surrender, Message, Exit };

typedef std::pair<RoleType, ChessmanType> Chessman;
typedef std::pair<qint32, qint32> Position;

const qint32 ranks = 8;
const qint32 baseRankWhite = 0;
const qint32 baseRankBlack = 7;
const qint32 pawnStartRankWhite = 6;
const qint32 pawnStartRankBlack = 1;

const Chessman nullChessman(RoleType::Neither, ChessmanType::None);

const qint32 dir8X[8] = {0, 0, 1, -1, -1, -1, 1, 1};
const qint32 dir8Y[8] = {-1, 1, 0, 0, -1, 1, -1, 1};
const qint32 dirKnightX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
const qint32 dirKnightY[8] = {1, 2, 2, 1, -1, -2, -2, -1};
const qint32 dirPawnAttackX[2] = {-1, 1};
const qint32 dirPawnAttackYWhite[2] = {-1, -1};
const qint32 dirPawnAttackYBlack[2] = {1, 1};
const qint32 dirPawnMoveX[2] = {0, 0};
const qint32 dirPawnMoveYWhite[2] = {-1, -2};
const qint32 dirPawnMoveYBlack[2] = {1, 2};

const QColor colorDark(181, 135, 99, 255);
const QColor colorLight(240, 218, 181, 255);

const QColor colorMoveTarget(255, 0, 0, 192);

const qint32 timeoutMove = 150;
const qint32 timeoutConnection = 15;

const qint32 milli = 1000;

const RoleType firstRole = RoleType::White;

const QMap<Chessman, QImage> chessmanToImage({
	std::make_pair(Chessman(RoleType::White, ChessmanType::King), QImage("://img/white_king.png")),
	std::make_pair(Chessman(RoleType::White, ChessmanType::Queen), QImage("://img/white_queen.png")),
	std::make_pair(Chessman(RoleType::White, ChessmanType::Rook), QImage("://img/white_rook.png")),
	std::make_pair(Chessman(RoleType::White, ChessmanType::Bishop), QImage("://img/white_bishop.png")),
	std::make_pair(Chessman(RoleType::White, ChessmanType::Knight), QImage("://img/white_knight.png")),
	std::make_pair(Chessman(RoleType::White, ChessmanType::Pawn), QImage("://img/white_pawn.png")),
	std::make_pair(Chessman(RoleType::White, ChessmanType::None), QImage("://img/none.png")),

	std::make_pair(Chessman(RoleType::Black, ChessmanType::King), QImage("://img/black_king.png")),
	std::make_pair(Chessman(RoleType::Black, ChessmanType::Queen), QImage("://img/black_queen.png")),
	std::make_pair(Chessman(RoleType::Black, ChessmanType::Rook), QImage("://img/black_rook.png")),
	std::make_pair(Chessman(RoleType::Black, ChessmanType::Bishop), QImage("://img/black_bishop.png")),
	std::make_pair(Chessman(RoleType::Black, ChessmanType::Knight), QImage("://img/black_knight.png")),
	std::make_pair(Chessman(RoleType::Black, ChessmanType::Pawn), QImage("://img/black_pawn.png")),
	std::make_pair(Chessman(RoleType::Black, ChessmanType::None), QImage("://img/none.png")),

	std::make_pair(nullChessman, QImage("://img/none.png"))
});

const Position nullPosition(-1, -1);

struct Chessboard {
	Chessman chessboard[ranks][ranks];

	Chessboard();
	void defaultChessboard();
	void nullChessboard();

	Chessman *operator[](const qint32 &index);
	const Chessman *operator[](const qint32 &index) const;
	Chessman &operator[](const Position &position);
	const Chessman &operator[](const Position &position) const;

	QString serialize() const;
	void deserialize(const QString &string);
	bool loadLocalFile(const QString &path, RoleType &firstRole);
	bool saveLocalFile(const QString &path, const RoleType &firstRole) const;
};

#endif // GLOBALS_H
