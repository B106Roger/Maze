#include "MazeWidget.h"
#include <QtWidgets/QApplication>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MazeWidget w;
	w.show();
	return a.exec();
}
