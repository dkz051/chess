#include "frmmain.h"

#include <QPainter>
#include <QPaintEvent>

#include "frmstart.h"
#include "graphics.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::frmMain) {
	ui->setupUi(this);

	ui->canvas->installEventFilter(this);

	chessboard.defaultChessboard();
}

frmMain::~frmMain() {
	delete ui;
}

bool frmMain::eventFilter(QObject *o, QEvent *e) {
	if (o == ui->canvas) {
		auto p = static_cast<QWidget *>(o);
		if (e->type() == QEvent::Paint) {
			auto ev = static_cast<QPaintEvent *>(e);
			QPainter *g = new QPainter(p);
			g->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
			renderChessboard(g, p->width(), p->height(), chessboard);
			delete g;
		}
	}
	return false;
}
