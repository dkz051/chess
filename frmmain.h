#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QtNetwork>
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

protected:
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::frmMain *ui;

	Chessboard chessboard;
};

#endif // FRMMAIN_H
