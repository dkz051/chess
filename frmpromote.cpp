#include "frmpromote.h"
#include "ui_frmpromote.h"

#include <QPainter>

frmPromote::frmPromote(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmPromote) {
	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->window()->layout()->setSizeConstraint(QLayout::SetFixedSize);

	ui->lblQueen->installEventFilter(this);
	ui->lblRook->installEventFilter(this);
	ui->lblBishop->installEventFilter(this);
	ui->lblKnight->installEventFilter(this);
}

frmPromote::~frmPromote() {
	delete ui;
}

void frmPromote::setRole(RoleType role) {
	this->role = role;
	ui->lblQueen->update();
	ui->lblRook->update();
	ui->lblBishop->update();
	ui->lblKnight->update();
}

bool frmPromote::eventFilter(QObject *o, QEvent *e) {
	if (e->type() == QEvent::Paint) {
		auto p = dynamic_cast<QLabel *>(o);
		if (p != nullptr) {
			QPainter *g = new QPainter(p);
			if (p == ui->lblQueen) {
				g->drawImage(QRect(0, 0, p->width(), p->height()), chessmanToImage[Chessman(role, ChessmanType::Queen)]);
			} else if (p == ui->lblRook) {
				g->drawImage(QRect(0, 0, p->width(), p->height()), chessmanToImage[Chessman(role, ChessmanType::Rook)]);
			} else if (p == ui->lblBishop) {
				g->drawImage(QRect(0, 0, p->width(), p->height()), chessmanToImage[Chessman(role, ChessmanType::Bishop)]);
			} else if (p == ui->lblKnight) {
				g->drawImage(QRect(0, 0, p->width(), p->height()), chessmanToImage[Chessman(role, ChessmanType::Knight)]);
			} else {
				return false;
			}
			return true;
		}
	} else if (e->type() == QEvent::MouseButtonRelease) {
		if (o == ui->lblQueen) {
			this->done(ChessmanType::Queen);
		} else if (o == ui->lblRook) {
			this->done(ChessmanType::Rook);
		} else if (o == ui->lblBishop) {
			this->done(ChessmanType::Bishop);
		} else if (o == ui->lblKnight) {
			this->done(ChessmanType::Knight);
		} else {
			return false;
		}
		return true;
	}
	return false;
}

void frmPromote::closeEvent(QCloseEvent *) {
	this->done(ChessmanType::None);
}
