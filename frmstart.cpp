#include "frmstart.h"

#include <QRegExp>
#include <QMessageBox>
#include <QHostAddress>

#include "globals.h"
#include "frmmain.h"
#include "ui_frmstart.h"

frmStart::frmStart(QWidget *parent) : QDialog(parent), ui(new Ui::frmStart) {
	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->window()->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

frmStart::~frmStart() {
	delete ui;
}

bool frmStart::checkIpPort() {
	QRegExp	regexIp(R"(^((2[0-4]\d|25[0-5]|[01]?\d\d?)\.){3}(2[0-4]\d|25[0-5]|[01]?\d\d?)$)");
	QRegExp regexPort(R"(^(\d?\d?\d?\d)|([0-5]\d\d\d\d)|(6[0-4]\d\d\d)|(65[0-4]\d\d)|(655[0-2]\d)|(6553[0-5])$)");
	if (!regexIp.exactMatch(ui->txtServerIp->text())) {
		QMessageBox::warning(this, tr("Error"), tr("Illegal IP address."));
		return false;
	} else if (!regexPort.exactMatch(ui->txtPort->text())) {
		QMessageBox::warning(this, tr("Error"), tr("Illegal port."));
		return false;
	} else {
		//QMessageBox::information(this, tr("Okay!"), tr("Correct IP address and correct port."));
		return true;
	}
}

void frmStart::on_btnStart_clicked() {
	if (ui->txtServerIp->text().isEmpty()) {
		ui->txtServerIp->setText(ui->txtServerIp->placeholderText());
	}

	if (ui->txtPort->text().isEmpty()) {
		ui->txtPort->setText(ui->txtPort->placeholderText());
	}

	if (!checkIpPort()) {
		return;
	}

	ui->btnCancel->setEnabled(true);
	ui->btnStart->setEnabled(false);

	ui->btnStart->setText(tr("Connecting..."));

	ui->txtServerIp->setEnabled(false);
	ui->txtPort->setEnabled(false);

	if (ui->optServer->isChecked()) {
		if (tcpServer == nullptr) {
			tcpServer = new QTcpServer;
			connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onServerConnected()));
		}
		tcpServer->listen(QHostAddress(ui->txtServerIp->text()), quint16(ui->txtPort->text().toInt()));
	} else {
		if (tcpSocket == nullptr) {
			tcpSocket = new QTcpSocket;
			connect(tcpSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
		}
		tcpSocket->connectToHost(QHostAddress(ui->txtServerIp->text()), quint16(ui->txtPort->text().toInt()));
	}
}

void frmStart::on_btnCancel_clicked() {
	if (ui->optServer->isChecked()) {
		if (tcpSocket != nullptr) {
			tcpSocket->abort();
		}
		tcpServer->close();
	} else {
		tcpSocket->abort();
	}

	ui->btnCancel->setEnabled(false);
	ui->btnStart->setEnabled(true);

	ui->btnStart->setText(tr("S&tart"));

	ui->txtServerIp->setEnabled(true);
	ui->txtPort->setEnabled(true);
}

void frmStart::onServerConnected() {
	tcpSocket = tcpServer->nextPendingConnection();
	tcpServer->pauseAccepting();

	frmMain *dlgMain = new frmMain;
	dlgMain->show();

	this->close();
}

void frmStart::onClientConnected() {
	frmMain *dlgMain = new frmMain;
	dlgMain->show();

	this->close();
}
