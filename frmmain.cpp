#include "frmmain.h"

#include <QPainter>
#include <QMessageBox>
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

void frmMain::setGame(RoleType role) {
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
			if (role != currentRole) {
				currentPosition = nullPosition;
				return false;
			}
			auto ev = static_cast<QMouseEvent *>(e);
			Position position = getPositionXY(ev->x(), ev->y(), p->width(), p->height(), role);
			if (position.first < 0 || position.first >= ranks || position.second < 0 || position.second >= ranks) {
				currentPosition = nullPosition;
			} else if (chessboard[position].first == role) {
				currentPosition = position;
			} else if (isMovePossible(currentPosition, position, role, chessboard)) {
				bool winFlag = (chessboard[position].second == ChessmanType::King);

				moveChessman(currentPosition, position, chessboard);
				sendMessage(tcpSocket, QString("move %1 %2 %3 %4;").arg(currentPosition.first).arg(currentPosition.second).arg(position.first).arg(position.second));

				assert(currentRole != RoleType::Neither);
				currentRole = (currentRole == RoleType::White ? RoleType::Black : RoleType::White);

				if (winFlag) {
					sendMessage(tcpSocket, "captured;");
					QMessageBox::information(this, tr("You Won!"), tr("You've successfully captured your opponent's king!"));
					this->close();
					return true;
				}
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

		ui->txtCommands->append(QString::fromUtf8(arrays[i]));

		assert(tokens.size() > 0);
		if (tokens[0] == "role") {
			assert(tokens.size() == 2);
			assert(tokens[1] == "white" || tokens[1] == "black");
			setGame(tokens[1] == "white" ? RoleType::White : RoleType::Black);
		} else if (tokens[0] == "move") {
			assert(tokens.size() == 5);
			moveChessman(Position(tokens[1].toInt(), tokens[2].toInt()), Position(tokens[3].toInt(), tokens[4].toInt()), chessboard);
			assert(currentRole != RoleType::Neither);
			currentRole = (currentRole == RoleType::White ? RoleType::Black : RoleType::White);
			this->update();
		} else if (tokens[0] == "captured") {
			QMessageBox::warning(this, tr("Lost!"), tr("Congratulations -- your king has been captured!"));
			this->close();
		} else if (tokens[0] == "resign") {
			QMessageBox::information(this, tr("You Won!"), tr("Your opponent has resigned. Congratulations!"));
			this->close();
		}
	}
}

void frmMain::on_btnResign_clicked() {
	if (QMessageBox::question(this, tr("Confirm?"), tr("Are you sure you want to resign?")) == QMessageBox::Yes) {
		sendMessage(tcpSocket, "resign;");
		this->close();
	}
}
