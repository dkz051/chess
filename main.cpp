#include <QApplication>

#include <QFont>

#include "frmstart.h"

int main(int argc, char *argv[]) {
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);
	frmStart *dlgStart = new frmStart;
	dlgStart->show();

	int result = app.exec();
	delete dlgStart;
	return result;
}
