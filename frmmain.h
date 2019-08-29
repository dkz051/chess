#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QTimer>
#include <QtNetwork>
#include <QByteArray>
#include <QMainWindow>
#include <QMessageBox>

#include "global.h"

namespace Ui {
	class frmMain;
}

class frmMain : public QMainWindow {
	Q_OBJECT

public:
	explicit frmMain(QWidget *parent = nullptr);
	~frmMain();

public slots:
	void setNetWork(QTcpServer *server, QTcpSocket *socket);
	void setGame(RoleType role);

private slots:
	void dataArrival();

	void on_btnResign_clicked();

	void onTimeout();

	void gameWon(const QString &prompt);
	void gameLost(const QString &prompt);
	void gameDrawn(const QString &prompt);

	void on_btnExit_clicked();
	void closeAllDialogs();

	void on_btnProposeDraw_clicked();

	void on_btnLoad_clicked();

protected:
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::frmMain *ui;

	RoleType role = RoleType::Neither;
	Chessboard chessboard;
	Position currentPosition = nullPosition;

	QTcpServer *tcpServer = nullptr;
	QTcpSocket *tcpSocket = nullptr;

	bool connected = false;
	QByteArray dataBuffer;
	RoleType currentRole = RoleType::White;
	QTimer timer = QTimer(this);

	qint64 ticksLeft, lastTick;

	QMessageBox resignConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Are you sure you want to resign?"),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox exitConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Are you sure you want to exit?\nThe game will end immediately if you exit!"),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox drawProposalConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Are you sure you are proposing a draw?\nThis needs your opponent's acceptance."),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox drawAcceptanceConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Your opponent has proposed a draw.\nAccept?"),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox drawRejected = QMessageBox(
		QMessageBox::Warning,
		tr("Draw Proposal Rejected"),
		tr("Your opponent has rejected your draw proposal.\nThe game continues."),
		QMessageBox::Ok,
		this
	);

	QMessageBox loadConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Are you sure you are loading another endgame?\nThis needs your opponent's acceptance."),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox loadAcceptConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Your opponent has proposed loading another endgame.\nAccept?\nIf you choose [yes], you will have to wait for your opponent loading endgame file."),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox loadRefused = QMessageBox(
		QMessageBox::Warning,
		tr("Rejected"),
		tr("Your opponent has rejected loading another endgame.\nThe current game continues."),
		QMessageBox::Ok,
		this
	);
};

#endif // FRMMAIN_H
