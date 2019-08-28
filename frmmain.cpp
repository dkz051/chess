#include "frmmain.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>

#include "frmstart.h"
#include "graphics.h"
#include "controller.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain) {
	ui->setupUi(this);
	ui->canvas->installEventFilter(this);
}

frmMain::~frmMain() {
	delete ui;
}

void frmMain::setNetWork(QTcpServer *server, QTcpSocket *socket) {
	tcpServer = server;
	tcpSocket = socket;

	if (tcpServer != nullptr) {
		tcpServer->setParent(this);
	}

	assert(tcpSocket != nullptr);
	tcpSocket->setParent(this);

	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataArrival()));
}

void frmMain::setRole(RoleType role) {
	this->role = role;
	connected = true;
	this->update();
}

bool frmMain::eventFilter(QObject *o, QEvent *e) {
	if (!connected) {
		return false;
	}
	if (o == ui->canvas) {
		auto p = static_cast<QWidget *>(o);
		if (e->type() == QEvent::Paint) {
			QPainter *g = new QPainter(p);
			g->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
			renderChessboard(g, p->width(), p->height(), role);
			renderMoves(g, p->width(), p->height(), role, currentPosition, chessboard);
			renderPieces(g, p->width(), p->height(), role, chessboard);
			delete g;
			return true;
		} else if (e->type() == QEvent::MouseButtonPress) {
			auto ev = static_cast<QMouseEvent *>(e);
			Position position = getPositionXY(ev->x(), ev->y(), p->width(), p->height(), role);
			if (position.first < 0 || position.first >= ranks || position.second < 0 || position.second >= ranks) {
				currentPosition = nullPosition;
			} else if (chessboard[position.first][position.second].first == role) {
				currentPosition = position;
			} else if (isMovePossible(currentPosition, position, role, chessboard)) {
				// move;
			} else {
				currentPosition = nullPosition;
			}
			this->update();
			return true;
		}
	}
	return false;
}

void frmMain::dataArrival() {
	dataBuffer.append(tcpSocket->readAll());

	QList<QByteArray> arrays = dataBuffer.split(';');
	dataBuffer = arrays.back();
	arrays.pop_back();

	for (qint32 i = 0; i < arrays.size(); ++i) {
		QList<QString> tokens = QString::fromUtf8(arrays[i]).split(' ');
		assert(tokens.size() > 0);
		if (tokens[0] == "role") {
			assert(tokens.size() == 2);
			assert(tokens[1] == "white" || tokens[1] == "black");
			setRole(tokens[1] == "white" ? RoleType::White : RoleType::Black);
		} else if (tokens[0] == "hello") {
			tcpSocket->write(QString("role %1;").arg(role == RoleType::White ? "black" : "white").toUtf8());
		}
	}
}
