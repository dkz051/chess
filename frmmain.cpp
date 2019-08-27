#include "frmmain.h"

#include <QPainter>
#include <QPaintEvent>

#include "frmstart.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::frmMain) {
	ui->setupUi(this);

	ui->canvas->installEventFilter(this);
}

frmMain::~frmMain() {
	delete ui;
}

bool frmMain::eventFilter(QObject *o, QEvent *e) {
	if (o == ui->canvas) {
		auto p = static_cast<QWidget *>(o);
		if (e->type() == QEvent::Paint) {
			auto ev = static_cast<QPaintEvent *>(e);
			QPainter *g = new QPainter(ui->canvas);
			g->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
			//renderChecker();
			delete g;
		}
	}
	return false;
}
