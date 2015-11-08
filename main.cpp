#include <QApplication>

#include "CsvFilterWindow.h"


int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resources);

	QApplication app(argc, argv);

	CsvFilterWindow csvFilter;

	csvFilter.show();
	app.exec();
}