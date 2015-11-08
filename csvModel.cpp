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
		headerData = line.trimmed().split(',');
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

	emit importedFromFile();

	return true;
}

//------------------------------------------------------------------------------
//	FUNCTION: filter [ public ]
//------------------------------------------------------------------------------
csvModel* csvModel::filter(csvModel* filter)
{
	csvModel* output = new csvModel(this);
	for (int filterCol = 0; filterCol < filter->columnCount(); ++filterCol)
	{
		QString columnName = filter->headerData(filterCol, Qt::Horizontal, Qt::DisplayRole).toString();
		
		bool hardHit = filter->data(filter->index(1, filterCol), Qt::CheckStateRole).toBool();

		// find the corresponding column in this spreadsheet
		int masterColumn;
		for (masterColumn = 0; masterColumn < this->columnCount(); masterColumn++)
		{
			qDebug() << columnName << "?=" << this->headerData(masterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
			if (this->headerData(masterColumn, Qt::Horizontal, Qt::DisplayRole).toString() == columnName)
			{
				qDebug() << masterColumn << "==" << columnName;
				break;
			}
		}

		for (int filterRow = 2; filterRow < filter->rowCount(); ++filterRow)
		{

		}
	}

	return output;
}

