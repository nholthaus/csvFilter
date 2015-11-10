#include "csvModel.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>

#include <exception>
#include <set>

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
	qDebug() << "importing";
	this->beginResetModel();

	bool signalsWereBlocked = this->signalsBlocked();
	this->blockSignals(true);

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

	m_file = csvFilePath;
	QTextStream in(&csvFile);
	QString data(in.readAll());
	csvFile.close();

	QStringList lines = data.split('\n');

	if (lines.size() < 2)
		return false;

	// the first line of the file is the header
	QStringList headerData(lines.at(0).trimmed().split(','));

	// the rest of the lines are model data
	for (int i = 1; i < lines.size(); i++)
	{
		QStringList values(lines.at(i).split(','));
		QList<QStandardItem*> items;

		for (int i = 0; i < values.size(); ++i)
		{
			QStandardItem* item = new QStandardItem;
			item->setData(values.at(i), Qt::DisplayRole);
			item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
			items.push_back(item);
		}

		this->appendRow(items);
	}

	csvFile.close();

	this->blockSignals(signalsWereBlocked);
	this->endResetModel();

	// set the header labels
	this->setHorizontalHeaderLabels(headerData);

	emit importedFromFile();

	return true;
}

//------------------------------------------------------------------------------
//	FUNCTION: exportToFile [virtual  public ]
//------------------------------------------------------------------------------
bool csvModel::exportToFile(QString csvFilePath)
{
	bool ok;

	QFile file(csvFilePath);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return false;
	}

	QTextStream out(&file);
	// write the header data
	for (int col = 0; col < columnCount(); col++)
	{
		if (col != 0) out << ',';
		out << this->headerData(col, Qt::Horizontal).toString();
	}
	out << '\n';

	// write the row data
	for (int row = 0; row < rowCount(); row++)
	{
		for (int col = 0; col < columnCount(); col++)
		{
			if (col != 0) out << ',';
			if (!this->data(index(row, col), Qt::CheckStateRole).isNull())
			{
				out << this->data(index(row, col), Qt::CheckStateRole).toString();
			}
			else
			{
				out << this->data(index(row, col), Qt::DisplayRole).toString().trimmed();
			}		
		}
		out << '\n';
	}

	file.close();

	return true;
}

//------------------------------------------------------------------------------
//	FUNCTION: filter [ public ]
//------------------------------------------------------------------------------
csvModel* csvModel::filter(csvModel* filter)
{
	csvModel* output = new csvModel;

	// Make a hash of all the groups in the filter
	QMultiHash<size_t, int> groups;
	std::set<int> unusedFilterColumns;

	for (int filterCol = 0; filterCol < filter->columnCount(); ++filterCol)
	{
		unusedFilterColumns.insert(filterCol);

		size_t group = filter->data(filter->index(1, filterCol)).toULongLong();
		groups.insert(group, filterCol);
	}

	// filter in column order, but each group aside from 0 should be filtered using logical 'and' with all group members
	for (auto itr = unusedFilterColumns.begin(); itr != unusedFilterColumns.end(); ++itr)
	{
		int							filterCol		= *itr;
		QString						columnName		= filter->headerData(filterCol, Qt::Horizontal, Qt::DisplayRole).toString();
		size_t						group			= filter->data(filter->index(1, filterCol)).toULongLong();
		size_t						numInGroup		= (group == 0 ? 1 : groups.count(group));
		bool						hardHit			= filter->data(filter->index(1, filterCol), Qt::CheckStateRole).toBool();
		QList<QStringList>			filterValues;
		QList<QString>				filterColumnNames;

		// make a list of filter values, i.e. all the things in the filter column
		auto filterColumns = groups.values(group);
		qSort(filterColumns);

		if (group != 0)
		{
			// for each filter in the group
			for (auto groupColumnItr = filterColumns.begin(); groupColumnItr != filterColumns.end(); ++groupColumnItr)
			{
				int column = *groupColumnItr;

				filterValues.push_back(QStringList());
				filterColumnNames.push_back(filter->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString());

				// erase other columns that are part of this group so we don't check them twice
				if(groupColumnItr != filterColumns.begin()) unusedFilterColumns.erase(column);

				// add all the filter values
				for (int filterRow = 2; filterRow < filter->rowCount(); ++filterRow)
				{
					QString value = filter->data(filter->index(filterRow, filterCol)).toString();
					if (!value.isEmpty()) filterValues.back() << value;
				}
			}
			qDebug() << "filters:" << filterColumnNames << filterColumns;
		}	
		else
		{
			filterColumnNames.push_back(columnName);
			qDebug() << "filters:" << filterColumnNames << filterCol;
		}

		// find the corresponding column in this spreadsheet
		QList<int> masterColumns;
		QList<QString> masterColumnNames;
		for (int i = 0; i < filterColumnNames.size(); ++i)
		{
			for (int masterColumn = 0; masterColumn < this->columnCount(); masterColumn++)
			{
				QString masterColunName = this->headerData(masterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
				if (masterColunName == filterColumnNames.at(i))
				{
					masterColumns.push_back(masterColumn);
					masterColumnNames.push_back(masterColunName);
					break;
				}
			}
		}

		qDebug() << "master:" << masterColumnNames << masterColumns;
		
		//  
	}

	return output;
}

//------------------------------------------------------------------------------
//	FUNCTION: file [ public ]
//------------------------------------------------------------------------------
QString csvModel::file() const
{
	return m_file;
}
