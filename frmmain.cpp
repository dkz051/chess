#include "frmmain.h"

#include <QPainter>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPaintEvent>

#include "frmstart.h"
#include "graphics.h"
#include "controller.h"
#include "frmpromote.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain) {
	ui->setupUi(this);
	ui->canvas->installEventFilter(this);

	timer.setInterval(10);
	timer.stop();

	chessboard.defaultChessboard();
	connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

frmMain::~frmMain() {
	delete ui;
	delete tcpServer;
	delete tcpSocket;
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
				ChessmanType promoteTo = ChessmanType::None;

				if (chessboard[currentPosition].second == ChessmanType::Pawn) {
					if (position.second == (role == RoleType::White ? baseRankWhite : baseRankBlack)) {
						frmPromote *dlgPromote = new frmPromote(this);
						dlgPromote->setAttribute(Qt::WA_DeleteOnClose);
						dlgPromote->setRole(role);
						promoteTo = ChessmanType(dlgPromote->exec());
						if (promoteTo == ChessmanType::None) {
							return false;
						}
					}
				}

				timer.stop();

				bool winFlag = (chessboard[position].second == ChessmanType::King);

				moveChessman(currentPosition, position, chessboard);
				sendMessage(tcpSocket, QString("move %1 %2 %3 %4;").arg(currentPosition.first).arg(currentPosition.second).arg(position.first).arg(position.second));

				if (promoteTo != ChessmanType::None) {
					sendMessage(tcpSocket, QString("promote %1 %2 %3;").arg(position.first).arg(position.second).arg(qint32(promoteTo)));

					chessboard[position] = Chessman(role, promoteTo);
				}

				assert(currentRole != RoleType::Neither);
				currentRole = (currentRole == RoleType::White ? RoleType::Black : RoleType::White);

				currentPosition = nullPosition;

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
		} else if (tokens[0] == "promote") {
			assert(tokens.size() == 4);
			chessboard[tokens[1].toInt()][tokens[2].toInt()] = Chessman(role == RoleType::White ? RoleType::Black : RoleType::White, ChessmanType(tokens[3].toInt()));
		} else if (tokens[0] == "exit") {
			assert(tokens.size() == 1);
			gameWon(tr("Your opponent has exited the game. You've won!"));
			this->close();
		} else if (tokens[0] == "propose") {
			assert(tokens.size() == 1);
			if (drawAcceptanceConfirm.exec() == QMessageBox::Yes) {
				sendMessage(tcpSocket, "accept;");
				gameDrawn(tr("You have agreed the draw proposal."));
			} else {
				sendMessage(tcpSocket, "reject;");
			}
		} else if (tokens[0] == "accept") {
			assert(tokens.size() == 1);
			gameDrawn(tr("Your opponent has agreed your draw proposal."));
		} else if (tokens[0] == "reject") {
			assert(tokens.size() == 1);
			drawRejected.exec();
		} else if (tokens[0] == "load") {
			assert(tokens.size() == 1);
			if (loadAcceptConfirm.exec() == QMessageBox::Yes) {
				sendMessage(tcpSocket, "allow;");
				timer.stop();
			} else {
				sendMessage(tcpSocket, "refuse;");
			}
		} else if (tokens[0] == "refuse") {
			assert(tokens.size() == 1);
			loadRefused.exec();
		} else if (tokens[0] == "allow") {
			assert(tokens.size() == 1);
			timer.stop();
			QFileDialog dlgFile(this, tr("Select Endgame File"), "./", tr("All Files (*.*)"));
			dlgFile.setFileMode(QFileDialog::ExistingFile);
			while (true) {
				if (dlgFile.exec()) {
					QStringList files = dlgFile.selectedFiles();
					assert(files.size() == 1);
					if (chessboard.loadLocalFile(files[0], currentRole)) {
						sendMessage(tcpSocket, QString("endgame %1 %2;").arg(chessboard.serialize()).arg(qint32(currentRole)));
						role = (rand() % 2 == 0 ? RoleType::White : RoleType::Black);
						sendMessage(tcpSocket, QString("role %1;").arg(role == RoleType::White ? "black" : "white"));
						break;
					}
				}
			}
			if (role == currentRole) {
				ticksLeft = timeoutMove * milli;
				timer.start();
			}
			this->update();
		} else if (tokens[0] == "endgame") {
			assert(tokens.size() == 3);
			chessboard.deserialize(tokens[1]);
			setGame(RoleType(tokens[2].toInt()));
			this->update();
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
	timer.stop();
	closeAllDialogs();
	QMessageBox::information(this, tr("You Won!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->setAttribute(Qt::WA_DeleteOnClose);
	dlgStart->show();
	this->close();
}

void frmMain::gameLost(const QString &prompt) {
	timer.stop();
	closeAllDialogs();
	QMessageBox::warning(this, tr("You Lost!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->setAttribute(Qt::WA_DeleteOnClose);
	dlgStart->show();
	this->close();
}

void frmMain::gameDrawn(const QString &prompt) {
	timer.stop();
	closeAllDialogs();
	QMessageBox::information(this, tr("Draw!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->setAttribute(Qt::WA_DeleteOnClose);
	dlgStart->show();
	this->close();
}

void frmMain::on_btnExit_clicked() {
	if (exitConfirm.exec() == QMessageBox::Yes) {
		sendMessage(tcpSocket, "exit;");
		this->close();
	}
}

void frmMain::closeAllDialogs() {
	resignConfirm.close();
	exitConfirm.close();
	drawProposalConfirm.close();
	drawAcceptanceConfirm.close();
	drawRejected.close();
	loadConfirm.close();
	loadRefused.close();
	loadAcceptConfirm.close();
}

void frmMain::on_btnProposeDraw_clicked() {
	if (drawProposalConfirm.exec() == QMessageBox::Yes) {
		sendMessage(tcpSocket, "propose;");
	}
}

void frmMain::on_btnLoad_clicked() {
	if (loadConfirm.exec() == QMessageBox::Yes) {
		sendMessage(tcpSocket, "load;");
	}
}
