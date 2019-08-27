#ifndef FRMSTART_H
#define FRMSTART_H

#include <QDialog>

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

private:
	Ui::frmStart *ui;
};

#endif // FRMSTART_H
