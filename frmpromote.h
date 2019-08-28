#ifndef FRMPROMOTE_H
#define FRMPROMOTE_H

#include <QDialog>
#include <QCloseEvent>

#include "global.h"

namespace Ui {
	class frmPromote;
}

class frmPromote : public QDialog
{
	Q_OBJECT

public:
	explicit frmPromote(QWidget *parent = nullptr);
	~frmPromote();

public slots:
	void setRole(RoleType role);

protected:
	bool eventFilter(QObject *o, QEvent *e);
	void closeEvent(QCloseEvent *e);

private:
	Ui::frmPromote *ui;

	RoleType role;
};

#endif // FRMPROMOTE_H
