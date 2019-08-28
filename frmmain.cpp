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

	timer.setInterval(10);
	timer.stop();

	connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
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
	ticksLeft = timeoutMove * milli;
	lastTick = QDateTime::currentMSecsSinceEpoch();
	if (role == firstRole) {
		timer.start();
	} else {
		timer.stop();
	}
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
				timer.stop();

				bool winFlag = (chessboard[position].second == ChessmanType::King);

				moveChessman(currentPosition, position, chessboard);
				sendMessage(tcpSocket, QString("move %1 %2 %3 %4;").arg(currentPosition.first).arg(currentPosition.second).arg(position.first).arg(position.second));

				assert(currentRole != RoleType::Neither);
				currentRole = (currentRole == RoleType::White ? RoleType::Black : RoleType::White);

				if (winFlag) {
					sendMessage(tcpSocket, "captured;");
					gameWon(tr("You've successfully captured your opponent's king!"));
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
			timer.stop();

			moveChessman(Position(tokens[1].toInt(), tokens[2].toInt()), Position(tokens[3].toInt(), tokens[4].toInt()), chessboard);
			assert(currentRole != RoleType::Neither);
			currentRole = (currentRole == RoleType::White ? RoleType::Black : RoleType::White);

			ticksLeft = timeoutMove * milli;
			lastTick = QDateTime::currentMSecsSinceEpoch();
			timer.start();
			this->update();
		} else if (tokens[0] == "captured") {
			assert(tokens.size() == 1);

			gameLost(tr("Congratulations -- your king has been captured!"));
			this->close();
		} else if (tokens[0] == "resign") {
			assert(tokens.size() == 1);

			gameWon(tr("Your opponent has resigned. Congratulations!"));
			this->close();
		} else if (tokens[0] == "countdown") {
			assert(tokens.size() == 2);

			qint64 ticks = tokens[1].toInt();
			ui->lblCountdown->setText(QString("<html><head/><body><p><span style=\"font-size:16pt;\">%1</span><span style=\"font-size:10pt;\">.%2</span></p></body></html>").arg(ticks / milli).arg((ticks % milli) / 10, 2, 10, QChar('0')));
		} else if (tokens[0] == "timeout") {
			assert(tokens.size() == 1);

			gameWon(tr("Your opponent's clock has timed out. You've won!"));
			this->close();
		}
	}
}

void frmMain::on_btnResign_clicked() {
	if (resignConfirm.exec() == QMessageBox::Yes) {
		sendMessage(tcpSocket, "resign;");
		this->close();
	}
}

void frmMain::onTimeout() {
	qint64 thisTick = QDateTime::currentMSecsSinceEpoch();
	ticksLeft = std::max(qint64(0), ticksLeft - (thisTick - lastTick));
	lastTick = thisTick;
	sendMessage(tcpSocket, QString("countdown %1;").arg(ticksLeft));
	ui->lblCountdown->setText(QString("<html><head/><body><p><span style=\"font-size:16pt;\">%1</span><span style=\"font-size:10pt;\">.%2</span></p></body></html>").arg(ticksLeft / milli).arg((ticksLeft % milli) / 10, 2, 10, QChar('0')));
	if (ticksLeft == 0) {
		sendMessage(tcpSocket, "timeout;");
		timer.stop();
		gameLost(tr("The clock has timed out. You've lost!"));
		this->close();
	}
}

void frmMain::gameWon(const QString &prompt) {
	resignConfirm.close();
	QMessageBox::information(this, tr("You Won!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->show();
	this->close();
}

void frmMain::gameLost(const QString &prompt) {
	resignConfirm.close();
	QMessageBox::warning(this, tr("You Lost!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->show();
	this->close();
}

void frmMain::gameDrawn(const QString &prompt) {
	resignConfirm.close();
	QMessageBox::information(this, tr("Drawn!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->show();
	this->close();
}
