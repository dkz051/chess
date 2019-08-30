#include "frmstart.h"

#include <QRegExp>
#include <QByteArray>
#include <QFileDialog>
#include <QMessageBox>
#include <QHostAddress>

#include "global.h"
#include "frmmain.h"
#include "controller.h"
#include "ui_frmstart.h"

frmStart::frmStart(QWidget *parent) : QDialog(parent), ui(new Ui::frmStart) {
	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->window()->layout()->setSizeConstraint(QLayout::SetFixedSize);

	timer.setInterval(timeoutConnection * milli);
	connect(&timer, SIGNAL(timeout()), this, SLOT(onConnectionTimeout()));
}

frmStart::~frmStart() {
	delete ui;
	delete tcpServer;
	delete tcpSocket;
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
			tcpServer = new QTcpServer(this);
			connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onServerConnected()));
		}
		if (ui->chkAnyIp->isChecked()) {
			tcpServer->listen(QHostAddress::AnyIPv4, quint16(ui->txtPort->text().toInt()));
		} else {
			tcpServer->listen(QHostAddress(ui->txtServerIp->text()), quint16(ui->txtPort->text().toInt()));
		}
	} else {
		if (tcpSocket == nullptr) {
			tcpSocket = new QTcpSocket(this);
			connect(tcpSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
		}
		tcpSocket->connectToHost(QHostAddress(ui->txtServerIp->text()), quint16(ui->txtPort->text().toInt()));
	}

	timer.start();
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

	delete tcpServer;
	tcpServer = nullptr;

	delete tcpSocket;
	tcpSocket = nullptr;

	ui->btnCancel->setEnabled(false);
	ui->btnStart->setEnabled(true);

	ui->btnStart->setText(tr("S&tart"));

	ui->txtServerIp->setEnabled(true);
	ui->txtPort->setEnabled(true);
}

void frmStart::onServerConnected() {
	timer.stop();

	tcpSocket = tcpServer->nextPendingConnection();
	tcpServer->pauseAccepting();
	RoleType role = (rand() % 2 == 0) ? RoleType::White : RoleType::Black;

	sendMessage(tcpSocket, QString("role %1;").arg(role == RoleType::White ? "black" : "white"));

	frmMain *dlgMain = new frmMain;
	dlgMain->setAttribute(Qt::WA_DeleteOnClose);
	dlgMain->setWindowTitle("Chess Server");
	dlgMain->setNetWork(tcpServer, tcpSocket);
	dlgMain->setGame(role);
	dlgMain->show();
	tcpServer = nullptr;
	tcpSocket = nullptr;
	this->close();
}

void frmStart::onClientConnected() {
	timer.stop();

	frmMain *dlgMain = new frmMain;
	dlgMain->setAttribute(Qt::WA_DeleteOnClose);
	dlgMain->setWindowTitle("Chess Client");
	dlgMain->setNetWork(tcpServer, tcpSocket);
	dlgMain->show();
	tcpServer = nullptr;
	tcpSocket = nullptr;
	this->close();
}

void frmStart::onConnectionTimeout() {
	timer.stop();
	on_btnCancel_clicked();
	QMessageBox::warning(this, tr("Cannot Start Game"), tr("Connection timed out.\nPlease check your system network settings or IP/port configuration."));
}

void frmStart::on_chkAnyIp_clicked(bool checked) {
	if (checked) {
		ui->txtServerIp->setEnabled(false);
	} else {
		ui->txtServerIp->setEnabled(true);
	}
}

void frmStart::on_optClient_clicked(bool checked) {
	if (checked) {
		ui->txtServerIp->setEnabled(true);
		ui->chkAnyIp->setEnabled(false);
	}
}

void frmStart::on_optServer_clicked(bool checked) {
	if (checked) {
		ui->chkAnyIp->setEnabled(true);
		ui->txtServerIp->setEnabled(!ui->chkAnyIp->isChecked());
	}
}
