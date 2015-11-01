#include "csvModel.h"

#include <QFileInfo>
#include <QDebug>

#include <exception>

//------------------------------------------------------------------------------
//	FUNCTION: cvsModel [ public ]
//------------------------------------------------------------------------------
csvModel::csvModel(QObject* parent /*= (QObject*)0*/) : QStandardItemModel(parent)
{
	
}

//------------------------------------------------------------------------------
//	FUNCTION: ~csvModel [virtual  public ]
//------------------------------------------------------------------------------
csvModel::~csvModel()
{

}

//------------------------------------------------------------------------------
//	FUNCTION: importFromFile [ public ]
//------------------------------------------------------------------------------
bool csvModel::importFromFile(QString csvFilePath)
{
	this->beginResetModel();

	this->clear();

	QFile csvFile(csvFilePath);
	QFileInfo csvFileInfo(csvFile);

	if (csvFileInfo.completeSuffix() != "csv")
	{
		return false;
	}

	if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	// the first line of the file is the header
	QStringList headerData;
	if (!csvFile.atEnd())
	{
		QString line = csvFile.readLine();
		headerData = line.split(',');
	}

	// the rest of the lines are model data
	while (!csvFile.atEnd())
	{
		QString line = csvFile.readLine();
		QStringList values = line.split(',');
		QList<QStandardItem*> items;

		for (int i = 0; i < values.size(); ++i)
		{
			QStandardItem* item = new QStandardItem;
			item->setData(values.at(i), Qt::DisplayRole);
			items.push_back(item);
		}

		this->appendRow(items);
	}

	csvFile.close();

	this->endResetModel();

	// set the header labels
	this->setHorizontalHeaderLabels(headerData);

	return true;
}

