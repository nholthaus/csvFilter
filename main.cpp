#include <QApplication>

#include "csvFilter.h"


int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resources);

	QApplication app(argc, argv);

	CsvFilter csvFilter;

	csvFilter.show();
	app.exec();
}