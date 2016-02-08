#include "csvFilterModel.h"

#include <QtAlgorithms>
#include <QDebug>

//------------------------------------------------------------------------------
//	FUNCTION: csvFilterModel [ public ]
//------------------------------------------------------------------------------
csvFilterModel::csvFilterModel(QObject* parent /*= (QObject*)0*/) 
: csvModel(parent)
{

}

//------------------------------------------------------------------------------
//	FUNCTION: ~csvFilterModel [ ]
//------------------------------------------------------------------------------
csvFilterModel::~csvFilterModel()
{

}

//------------------------------------------------------------------------------
//	FUNCTION: importFromFile [ public ]
//------------------------------------------------------------------------------
bool csvFilterModel::importFromFile(QString csvFilePath)
{
	this->beginResetModel();

	this->blockSignals(true);

	if (!csvModel::importFromFile(csvFilePath))
	{
		return false;
	}

	// set check state for row 0
	for (int col = 0; col < this->columnCount(); col++)
	{
		QStandardItem* item = this->item(0, col);
		item->setCheckable(true);
		QString value = this->data(indexFromItem(item), Qt::DisplayRole).toString();

		Qt::CheckState state;
		if (value.isEmpty() || value.compare("0") == 0 || value.compare("false", Qt::CaseInsensitive) == 0)
		{
			state = Qt::Unchecked;
		} 
		else
		{
			state = Qt::Checked;
		}
		item->setCheckState(state);
		item->setData("Hard", Qt::DisplayRole);
		item->setFlags(item->flags() ^ Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
	}

	this->blockSignals(false);
	this->endResetModel();

	emit importedFromFile();

	return true;
}

//------------------------------------------------------------------------------
//	FUNCTION: group [ ]
//------------------------------------------------------------------------------
void csvFilterModel::group(QList<int> columns)
{
	qSort(columns);

	ungroup(columns);

	// get list of groups
	std::set<size_t> groups;
	for (int col = 0; col != columnCount(); ++col)
	{
		groups.insert(this->data(index(1, col)).toULongLong());
	}

	// find the first hole
	size_t nextGroup = 1;
	while (groups.count(nextGroup) == 1)
	{
		++nextGroup;
	}

	for (auto colItr = columns.begin(); colItr != columns.end(); ++colItr)
	{
		this->setData(index(1, *colItr), nextGroup);
		this->setData(index(0, *colItr), false, Qt::CheckStateRole);		// default new groups to soft hits
	}

	this->dataChanged(index(0, *columns.begin()), index(this->rowCount() - 1, *columns.end()));
}

//------------------------------------------------------------------------------
//	FUNCTION: ungroup [ ]
//------------------------------------------------------------------------------
void csvFilterModel::ungroup(QList<int> columns)
{
	qSort(columns);

	for (auto colItr = columns.begin(); colItr != columns.end(); ++colItr)
	{
		this->setData(index(1, *colItr), 0);
	}

	// delete groups of 1
	QHash<size_t, int> groups;
	for (int col = 0; col != columnCount(); ++col)
	{
		// insert all columns into a group hash
		groups.insertMulti(this->data(index(1, col)).toULongLong(), col);
	}
	for (auto itr = groups.begin(); itr != groups.end(); ++itr)
	{
		if (itr.key() != 0 && groups.count(itr.key()) == 1)
		{
			// remove this group
			this->setData(index(1, itr.value()), 0);
		}
	}

	this->dataChanged(index(0, *columns.begin()), index(this->rowCount() - 1, *columns.end()));
}

//------------------------------------------------------------------------------
//	FUNCTION: setData [virtual  public ]
//------------------------------------------------------------------------------
bool csvFilterModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	// if hit state changes, sync it across the entire filter group
	if (index.row() == 0)
	{
		for (int col = 0; col != columnCount(); ++col)
		{
			// if the groups match
			size_t group = data(this->index(1, index.column())).toInt();
			if (group != 0 && group == data(this->index(1,col)).toInt())
			{
				QStandardItemModel::setData(this->index(0, col), value, role);
			}
		}
	}

	return QStandardItemModel::setData(index, value, role);
}

//------------------------------------------------------------------------------
//	FUNCTION: addFilter [virtual  public ]
//------------------------------------------------------------------------------
void csvFilterModel::addFilter(const QString& filter)
{
	this->beginInsertColumns(QModelIndex(), columnCount(), columnCount());

	QList<QStandardItem*> items;
	
	QStandardItem* hit = new QStandardItem();
	hit->setCheckable(true);
	hit->setCheckState(Qt::Unchecked);
	hit->setData("Hard", Qt::DisplayRole);
	items.push_back(hit);
	
	QStandardItem* group = new QStandardItem();
	group->setData(0, Qt::DisplayRole);
	items.push_back(group);

	this->appendColumn(items);
	this->setHeaderData(columnCount() - 1, Qt::Horizontal, filter);

	this->endInsertColumns();
}

//------------------------------------------------------------------------------
//	FUNCTION: moveColumn [virtual  public ]
//------------------------------------------------------------------------------
bool csvFilterModel::moveColumn(const QModelIndex &sourceParent, int sourceColumn, const QModelIndex &destinationParent, int destinationChild)
{
	QString header = headerData(sourceColumn, Qt::Horizontal).toString();
	auto columnData = this->takeColumn(sourceColumn);
	this->insertColumn(destinationChild, columnData);
	setHeaderData(destinationChild, Qt::Horizontal, header);

	emit dataChanged(index(0, qMin(sourceColumn, destinationChild)), index(rowCount() - 1, qMax(sourceColumn, destinationChild)));

	return true;
}
