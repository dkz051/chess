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

protected:
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::frmMain *ui;

	RoleType role = RoleType::Neither;
	Chessboard chessboard = Chessboard::defaultChessboard();
	Position currentPosition = nullPosition;

	QTcpServer *tcpServer = nullptr;
	QTcpSocket *tcpSocket = nullptr;

	bool connected = false;
	QByteArray dataBuffer;
	RoleType currentRole = RoleType::White;
	QTimer timer = QTimer(this);

	qint64 ticksLeft, lastTick;

	QMessageBox resignConfirm = QMessageBox(QMessageBox::Question, tr("Confirm?"), tr("Are you sure you want to resign?"), QMessageBox::Yes | QMessageBox::No, this);
};

#endif // FRMMAIN_H
