#ifndef GLOBALS_H
#define GLOBALS_H

#include <utility>

#include <QMap>
#include <QColor>
#include <QImage>
#include <QtNetwork>
#include <QIntegerForSize>

enum RoleType { White = 7, Black = 0, Neither = 1 };
enum PieceType { King = 0, Queen, Rook, Bishop, Knight, Pawn, None };
enum CommandType { Role = 0, Move, Castling, Draw, Surrender, Message, Exit };

typedef std::pair<RoleType, PieceType> Piece;
typedef std::pair<qint32, qint32> Position;

const qint32 ranks = 8;

const qint32 dir4X[4] = {0, 0, 1, -1};
const qint32 dir4Y[4] = {-1, 1, 0, 0};
const qint32 dir8X[8] = {0, 0, 1, -1, -1, -1, 1, 1};
const qint32 dir8Y[8] = {-1, 1, 0, 0, -1, 1, -1, 1};
const qint32 dirKnightX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
const qint32 dirKnightY[8] = {1, 2, 2, 1, -1, -2, -2, -1};

const QColor colorDark(181, 135, 99, 255);
const QColor colorLight(240, 218, 181, 255);

const QMap<Piece, QImage> pieceToImage({
	std::make_pair(Piece(RoleType::White, PieceType::King), QImage("://img/white_king.png")),
	std::make_pair(Piece(RoleType::White, PieceType::Queen), QImage("://img/white_queen.png")),
	std::make_pair(Piece(RoleType::White, PieceType::Rook), QImage("://img/white_rook.png")),
	std::make_pair(Piece(RoleType::White, PieceType::Bishop), QImage("://img/white_bishop.png")),
	std::make_pair(Piece(RoleType::White, PieceType::Knight), QImage("://img/white_knight.png")),
	std::make_pair(Piece(RoleType::White, PieceType::Pawn), QImage("://img/white_pawn.png")),
	std::make_pair(Piece(RoleType::White, PieceType::None), QImage("://img/none.png")),

	std::make_pair(Piece(RoleType::Black, PieceType::King), QImage("://img/black_king.png")),
	std::make_pair(Piece(RoleType::Black, PieceType::Queen), QImage("://img/black_queen.png")),
	std::make_pair(Piece(RoleType::Black, PieceType::Rook), QImage("://img/black_rook.png")),
	std::make_pair(Piece(RoleType::Black, PieceType::Bishop), QImage("://img/black_bishop.png")),
	std::make_pair(Piece(RoleType::Black, PieceType::Knight), QImage("://img/black_knight.png")),
	std::make_pair(Piece(RoleType::Black, PieceType::Pawn), QImage("://img/black_pawn.png")),
	std::make_pair(Piece(RoleType::Black, PieceType::None), QImage("://img/none.png")),

	std::make_pair(Piece(RoleType::Neither, PieceType::None), QImage("://img/none.png"))
});

const Position nullPosition(-1, -1);

struct Chessboard {
	Piece chessboard[ranks][ranks];

	Chessboard();
	static Chessboard defaultChessboard();
	Piece *operator[](const qint32 &index);
	const Piece *operator[](const qint32 &index) const;
	//QString toForsythEdwards() const;
	//static Chessboard fromForsythEdwards(const QString &fen);
};

//extern QTcpServer *tcpServer;
//extern QTcpSocket *tcpSocket;

#endif // GLOBALS_H
