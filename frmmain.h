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
	void setGame(RoleType role);
	void setNetWork(QTcpServer *server, QTcpSocket *socket);

private slots:
	void onTimeout();
	void dataArrival();

	void gameWon(const QString &prompt);
	void gameLost(const QString &prompt);
	void gameDraw(const QString &prompt);

	void closeAllDialogs();
	void setCountdown(qint64 ticks);

	void on_btnExit_clicked();
	void on_btnLoad_clicked();
	void on_btnSave_clicked();
	void on_btnResign_clicked();
	void on_btnProposeDraw_clicked();

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
	RoleType currentRole = firstRole;
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
		tr("Are you sure you are loading another game?\nThis needs your opponent's acceptance."),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox loadAcceptConfirm = QMessageBox(
		QMessageBox::Question,
		tr("Confirm?"),
		tr("Your opponent has proposed loading another game.\nAccept?\nIf you choose [yes], you will have to wait for your opponent loading game file."),
		QMessageBox::Yes | QMessageBox::No,
		this
	);

	QMessageBox loadRefused = QMessageBox(
		QMessageBox::Warning,
		tr("Rejected"),
		tr("Your opponent has rejected loading another game.\nThe current game continues."),
		QMessageBox::Ok,
		this
	);
};

#endif // FRMMAIN_H
