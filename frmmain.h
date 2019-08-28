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
	void setRole(RoleType role);

private slots:
	void dataArrival();

protected:
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::frmMain *ui;

	RoleType role = RoleType::Neither;
	Chessboard chessboard = Chessboard::defaultChessboard();
	Position currentPosition = nullPosition;

	QTcpServer *tcpServer = nullptr;
	QTcpSocket *tcpSocket = nullptr;

	QByteArray buffer;
};

#endif // FRMMAIN_H
