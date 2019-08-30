#include "frmmain.h"

#include <QPainter>
#include <QPalette>
#include <QFileDialog>
#include <QMouseEvent>

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

	QPixmap pixmap = QPixmap(":/img/chess.jpg").scaled(this->size());
	QPalette palette(this->palette());
	palette.setBrush(QPalette::Window, QBrush(pixmap));
	this->setPalette(palette);
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

	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onDataArrival()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void frmMain::setGame(RoleType role) {
	this->role = role;
	connected = true;
	gameFinished = false;
	ticksLeft = timeoutMove * milli;
	lastTick = QDateTime::currentMSecsSinceEpoch();
	ui->lblThisRole->setText(role == RoleType::White ? tr("White") : tr("Black"));
	ui->lblRole->setText(role == currentRole ? tr("You") : tr("Opponent"));
	if (role == currentRole) {
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
			if (role == currentRole) {
				renderMoves(g, p->width(), p->height(), role, currentPosition, chessboard);
			}
			renderChessmen(g, p->width(), p->height(), role, chessboard);
			delete g;
			return true;
		} else if (e->type() == QEvent::MouseButtonPress) {
			if (role != currentRole) {
				currentPosition = nullPosition;
				return false;
			}
			auto ev = static_cast<QMouseEvent *>(e);
			Position position = getPositionXY(ev->x(), ev->y(), p->width(), p->height(), role);
			if (isOutOfRange(position)) {
				currentPosition = nullPosition;
			} else if (position == currentPosition) {
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

				bool captureFlag = (chessboard[position].second == ChessmanType::King);

				if (chessboard[currentPosition].second == ChessmanType::King && squareEuclideanDistance(currentPosition, position) == 4) {
					// Castling
					moveChessman(currentPosition, position, chessboard);
					Position rook = position;
					if (rook.first == 6) {
						rook.first = 7;
					} else if (rook.first == 2) {
						rook.first = 0;
					}
					moveChessman(rook, midpoint(currentPosition, position), chessboard);
					sendMessage(tcpSocket, QString("castle %1 %2 %3 %4 %5 %6;")
											.arg(currentPosition.first)
											.arg(currentPosition.second)
											.arg(rook.first)
											.arg(rook.second)
											.arg(position.first)
											.arg(position.second)
					);
				} else {
					moveChessman(currentPosition, position, chessboard);
					sendMessage(tcpSocket, QString("move %1 %2 %3 %4;").arg(currentPosition.first).arg(currentPosition.second).arg(position.first).arg(position.second));

					if (promoteTo != ChessmanType::None) {
						sendMessage(tcpSocket, QString("promote %1 %2 %3;").arg(position.first).arg(position.second).arg(qint32(promoteTo)));

						chessboard[position] = Chessman(role, promoteTo);
					}
				}
				currentRole = opponent(currentRole);
				ui->lblRole->setText(role == currentRole ? tr("You") : tr("Opponent"));

				currentPosition = nullPosition;

				if (captureFlag) {
					sendMessage(tcpSocket, "captured;");
					gameWon(tr("You've successfully captured your opponent's king!"));
				}

				if (isStalemate(currentRole, chessboard)) {
					sendMessage(tcpSocket, "stalemate;");
					gameDraw(tr("Stalemate!"));
				}

				if (isCheckmate(currentRole, chessboard)) {
					sendMessage(tcpSocket, "checkmate;");
					gameWon(tr("Checkmate!"));
				}
			} else {
				currentPosition = nullPosition;
			}
			p->update();
			return true;
		}
	}
	return false;
}

void frmMain::onDataArrival() {
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
			setGame(tokens[1] == "white" ? RoleType::White : RoleType::Black);
		} else if (tokens[0] == "move") {
			assert(tokens.size() == 5);
			timer.stop();

			moveChessman(Position(tokens[1].toInt(), tokens[2].toInt()), Position(tokens[3].toInt(), tokens[4].toInt()), chessboard);

			currentRole = opponent(currentRole);
			ui->lblRole->setText(role == currentRole ? tr("You") : tr("Opponent"));

			ticksLeft = timeoutMove * milli;
			lastTick = QDateTime::currentMSecsSinceEpoch();
			timer.start();
		} else if (tokens[0] == "captured") {
			assert(tokens.size() == 1);

			gameLost(tr("Congratulations -- your king has been captured!"));
		} else if (tokens[0] == "resign") {
			assert(tokens.size() == 1);

			gameWon(tr("Your opponent has resigned. Congratulations!"));
		} else if (tokens[0] == "countdown") {
			assert(tokens.size() == 2);
			setCountdown(tokens[1].toLongLong());
		} else if (tokens[0] == "timeout") {
			assert(tokens.size() == 1);

			gameWon(tr("Your opponent's clock has timed out. You've won!"));
		} else if (tokens[0] == "promote") {
			assert(tokens.size() == 4);
			chessboard[tokens[1].toInt()][tokens[2].toInt()] = Chessman(opponent(role), ChessmanType(tokens[3].toInt()));
		} else if (tokens[0] == "exit") {
			assert(tokens.size() == 1);
			gameWon(tr("Your opponent has exited the game. You've won!"));
		} else if (tokens[0] == "propose") {
			assert(tokens.size() == 1);
			if (drawAcceptanceConfirm.exec() == QMessageBox::Yes) {
				sendMessage(tcpSocket, "accept;");
				gameDraw(tr("You have agreed the draw proposal."));
			} else {
				sendMessage(tcpSocket, "reject;");
			}
		} else if (tokens[0] == "accept") {
			assert(tokens.size() == 1);
			gameDraw(tr("Your opponent has agreed your draw proposal."));
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
			QFileDialog dlgFile(this, tr("Select Game File"), "./", tr("All Files (*.*)"));
			dlgFile.setFileMode(QFileDialog::ExistingFile);
			while (true) {
				if (dlgFile.exec()) {
					QStringList files = dlgFile.selectedFiles();
					assert(files.size() >= 1);
					if (chessboard.loadLocalFile(files[0], currentRole)) {
						sendMessage(tcpSocket, QString("game %1 %2;").arg(chessboard.serialize()).arg(qint32(currentRole)));
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
		} else if (tokens[0] == "game") {
			assert(tokens.size() == 3);
			chessboard.deserialize(tokens[1]);
			currentRole = RoleType(tokens[2].toInt());
		} else if (tokens[0] == "stalemate") {
			assert(tokens.size() == 1);
			gameDraw(tr("Stalemate!"));
		} else if (tokens[0] == "checkmate") {
			assert(tokens.size() == 1);
			gameLost(tr("Checkmate!"));
		} else if (tokens[0] == "castle") {
			assert(tokens.size() == 7);
			Position king(tokens[1].toInt(), tokens[2].toInt()),
					 rook(tokens[3].toInt(), tokens[4].toInt()),
					 kingTarget(tokens[5].toInt(), tokens[6].toInt()),
					 rookTarget = midpoint(king, kingTarget);
			moveChessman(king, kingTarget, chessboard);
			moveChessman(rook, rookTarget, chessboard);
			currentRole = opponent(currentRole);
			ui->lblRole->setText(role == currentRole ? tr("You") : tr("Opponent"));

			ticksLeft = timeoutMove * milli;
			lastTick = QDateTime::currentMSecsSinceEpoch();
			timer.start();
		}
		ui->canvas->update();
	}
}

void frmMain::on_btnResign_clicked() {
	if (resignConfirm.exec() == QMessageBox::Yes) {
		sendMessage(tcpSocket, "resign;");
		gameLost(tr("You have confirmed resign."));
	}
}

void frmMain::onTimeout() {
	qint64 thisTick = QDateTime::currentMSecsSinceEpoch();
	ticksLeft = std::max(qint64(0), ticksLeft - (thisTick - lastTick));
	lastTick = thisTick;
	sendMessage(tcpSocket, QString("countdown %1;").arg(ticksLeft));

	if (role != currentRole) {
		currentPosition = nullPosition;
	}

	setCountdown(ticksLeft);
	if (ticksLeft == 0) {
		sendMessage(tcpSocket, "timeout;");
		timer.stop();
		gameLost(tr("The clock has timed out. You've lost!"));
		this->close();
	}
}

void frmMain::gameWon(const QString &prompt) {
	timer.stop();
	gameFinished = true;
	closeAllDialogs();
	QMessageBox::information(this, tr("Won!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->setAttribute(Qt::WA_DeleteOnClose);
	dlgStart->show();
	this->close();
}

void frmMain::gameLost(const QString &prompt) {
	timer.stop();
	gameFinished = true;
	closeAllDialogs();
	QMessageBox::warning(this, tr("Lost!"), prompt);

	frmStart *dlgStart = new frmStart;
	dlgStart->setAttribute(Qt::WA_DeleteOnClose);
	dlgStart->show();
	this->close();
}

void frmMain::gameDraw(const QString &prompt) {
	timer.stop();
	gameFinished = true;
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

void frmMain::on_btnSave_clicked() {
	QFileDialog dlgFile(this, tr("Choose Save Location"), "./", tr("All Files (*.*)"));
	dlgFile.setFileMode(QFileDialog::AnyFile);
	if (dlgFile.exec()) {
		QStringList files = dlgFile.selectedFiles();
		assert(files.size() >= 1);
		chessboard.saveLocalFile(files[0], currentRole);
	}
}

void frmMain::setCountdown(qint64 ticks) {
	qint64 minutes = (ticks / milli) / second;
	qint64 seconds = (ticks / milli) % second;
	qint64 centiseconds = (ticks % milli) / (milli / centi);

	if (minutes > 0) {
		ui->lblCountdown->setText(QString("<html><head/><body><p><span style=\"font-size:16pt;\">%1:%2</span><span style=\"font-size:10pt;\">.%3</span></p></body></html>").arg(minutes).arg(seconds, 2, 10, QChar('0')).arg(centiseconds, 2, 10, QChar('0')));
	} else {
		ui->lblCountdown->setText(QString("<html><head/><body><p><span style=\"font-size:16pt;\">%1</span><span style=\"font-size:10pt;\">.%2</span></p></body></html>").arg(seconds).arg(centiseconds, 2, 10, QChar('0')));
	}
}

void frmMain::onDisconnected() {
	timer.stop();
	if (!gameFinished) {
		gameWon(tr("Your opponent is now disconnected.\nYou've won!"));
	}
}
