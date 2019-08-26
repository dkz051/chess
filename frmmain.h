#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>

namespace Ui {
	class frmMain;
}

class frmMain : public QMainWindow
{
	Q_OBJECT

public:
	explicit frmMain(QWidget *parent = nullptr);
	~frmMain();

private:
	Ui::frmMain *ui;
};

#endif // FRMMAIN_H
