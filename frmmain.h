#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QtNetwork>
#include <QByteArray>
#include <QMainWindow>

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

protected:
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::frmMain *ui;

	RoleType role = RoleType::Neither;
	Chessboard chessboard = Chessboard::defaultChessboard();
	Position currentPosition = nullPosition;

	QTcpServer *tcpServer = nullptr;
	QTcpSocket *tcpSocket = nullptr;

	QByteArray dataBuffer;
	bool connected = false;
	RoleType currentRole = RoleType::White;
};

#endif // FRMMAIN_H
