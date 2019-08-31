#include <QFont>
#include <QDateTime>
#include <QApplication>

#include "frmstart.h"

int main(int argc, char *argv[]) {
	srand(quint32(QDateTime::currentMSecsSinceEpoch()));

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);

	QFont font;
	font.setFamily(font.defaultFamily());
	app.setFont(font);

	frmStart *dlgStart = new frmStart;
	dlgStart->show();

	int result = app.exec();
	delete dlgStart;
	return result;
}
