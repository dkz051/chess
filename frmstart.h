#ifndef FRMSTART_H
#define FRMSTART_H

#include <QTimer>
#include <QDialog>
#include <QtNetwork>

namespace Ui {
	class frmStart;
}

class frmStart : public QDialog {
	Q_OBJECT

public:
	explicit frmStart(QWidget *parent = nullptr);
	~frmStart();

private slots:
	void on_btnStart_clicked();
	bool checkIpPort();

	void onServerConnected();
	void onClientConnected();

	void on_btnCancel_clicked();

	void onConnectionTimeout();
private:
	Ui::frmStart *ui;

	QTcpServer *tcpServer = nullptr;
	QTcpSocket *tcpSocket = nullptr;

	QTimer timer = QTimer(this);
};

#endif // FRMSTART_H
