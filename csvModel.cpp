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
//	FUNCTION: file [ public ]
//------------------------------------------------------------------------------
QString csvModel::file() const
{
	return m_file;
}
