#include <QApplication>
#include <QMainWindow>

#include "MainWindow.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	QMainWindow program;
	MainWindow* window = new MainWindow(&program);
	program.setWindowTitle("spotlight");
	program.setCentralWidget(window);
	program.show();
	return app.exec();
}