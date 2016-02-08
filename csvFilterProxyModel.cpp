#include "csvFilterProxyModel.h"

#include <QDebug>
#include <QRegExp>

//------------------------------------------------------------------------------
//	FUNCTION: csvFilterProxyModel [ public ]
//------------------------------------------------------------------------------
csvFilterProxyModel::csvFilterProxyModel(QObject* parent /*= (QObject*)0*/)
: QSortFilterProxyModel(parent),
m_filterModel(nullptr),
m_duplicateColumn(0),
m_removeDuplicates(false),
m_removeSingles(false)
{

}

//------------------------------------------------------------------------------
//	FUNCTION: ~csvFilterProxyModel [ public ]
//------------------------------------------------------------------------------
csvFilterProxyModel::~csvFilterProxyModel()
{

}

//------------------------------------------------------------------------------
//	FUNCTION: filterAcceptsRow [ public ]
//------------------------------------------------------------------------------
bool csvFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if (!m_filterModel)
		return true;

	// remove duplicates if enabled
	QString duplicateColumnValue = sourceModel()->data(sourceModel()->index(source_row, m_duplicateColumn)).toString();
	if (m_removeDuplicates && m_duplicateMapping.count(duplicateColumnValue) > 1)
		return false;

	// remove singles if enabled
	if (m_removeSingles && m_duplicateMapping.count(duplicateColumnValue) == 1)
		return false;

	int	 softHitOrder = 1;
	bool hardMatch = true;

	for (auto group : m_filterMapping)
	{
		// a group always has the same hard/soft value for all its members
		bool hardHit = m_filterModel->data(m_filterModel->index(0, group.first().first), Qt::CheckStateRole).toBool();
		bool softMatch = false;

		if (!hardHit)
		{
			++softHitOrder;
		}

		QList<QStringList> filterValues;

		// remove hard hits, tag soft hits
		int filterRow;
		for (filterRow = 2; filterRow < m_filterModel->rowCount(); ++filterRow)
		{
			bool match = true;
			for (auto filterPair : group)
			{
				int& filterCol = filterPair.first;
				int& masterCol = filterPair.second;

				// clear previous soft hits
				sourceModel()->setData(sourceModel()->index(source_row, masterCol), 0, Qt::UserRole);
				sourceModel()->setData(sourceModel()->index(source_row, masterCol), "", Qt::ToolTipRole);

				QString masterColValue = sourceModel()->data(sourceModel()->index(source_row, masterCol, source_parent)).toString();
				QString filterColVal = m_filterModel->data(m_filterModel->index(filterRow, filterCol)).toString();

				match &= ((filterColVal == masterColValue) || masterColValue.trimmed().isEmpty());
			}

			if (match)
				break;
		}
		// if no match
		if (filterRow == m_filterModel->rowCount())
		{
			if (hardHit)
			{
				hardMatch = false;
			}
			else // soft hit
			{
				softMatch = true;
			}
		}

		// if the soft match persisted across the full group, mark each group member
		if (softMatch)
		{
			for (auto filterPair : group)
			{
				int& masterCol = filterPair.second;
				sourceModel()->setData(sourceModel()->index(source_row, masterCol), softHitOrder, Qt::UserRole);
				QString toolTip;
				for (auto filterPair : group)
				{
					int& masterCol = filterPair.second;
					if (!toolTip.isEmpty()) toolTip.append(',');
					toolTip.append(sourceModel()->headerData(masterCol, Qt::Horizontal).toString());
				}
				sourceModel()->setData(sourceModel()->index(source_row, masterCol), toolTip, Qt::ToolTipRole);
			}
		}
	}
	
	return hardMatch;
}

//------------------------------------------------------------------------------
//	FUNCTION: setFilterModel [ public ]
//------------------------------------------------------------------------------
void csvFilterProxyModel::setFilterModel(const csvFilterModel* filterModel)
{
	m_filterModel = filterModel;
	invalidate();
}

//------------------------------------------------------------------------------
//	FUNCTION: invalidate [ public ]
//------------------------------------------------------------------------------
void csvFilterProxyModel::invalidate()
{
	m_filterMapping.clear();

	if (!m_filterModel)
		return;

	// Make a hash of all the groups in the filter
	QMultiHash<size_t, int> groups;
	std::set<int> unusedFilterColumns;

	for (int filterCol = 0; filterCol < m_filterModel->columnCount(); ++filterCol)
	{
		unusedFilterColumns.insert(filterCol);

		size_t group = m_filterModel->data(m_filterModel->index(1, filterCol)).toULongLong();
		groups.insert(group, filterCol);
	}

	// filter in column order, but each group aside from 0 should be filtered using logical 'and' with all group members
	for (auto itr = unusedFilterColumns.begin(); itr != unusedFilterColumns.end(); ++itr)
	{
		int							filterCol = *itr;
		QString						columnName = m_filterModel->headerData(filterCol, Qt::Horizontal, Qt::DisplayRole).toString();
		size_t						group = m_filterModel->data(m_filterModel->index(1, filterCol)).toULongLong();
		size_t						numInGroup = (group == 0 ? 1 : groups.count(group));
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

				filterColumnNames.push_back(m_filterModel->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString());

				// erase other columns that are part of this group so we don't check them twice
				if (groupColumnItr != filterColumns.begin()) 
					unusedFilterColumns.erase(column);				
			}
		}
		else
		{
			filterColumnNames.push_back(columnName);
			filterColumns.clear();
			filterColumns.push_back(filterCol);
		}

		// find the corresponding column in this spreadsheet
		QList<int> masterColumns;
		for (int i = 0; i < filterColumnNames.size(); ++i)
		{
			for (int masterColumn = 0; masterColumn < sourceModel()->columnCount(); masterColumn++)
			{
				QString masterColunName = sourceModel()->headerData(masterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
				if (masterColunName == filterColumnNames.at(i))
				{
					masterColumns.push_back(masterColumn);
					break;
				}
			}
		}

		// add the mappings to the mapping table
		QList<QPair<int, int>> filterMapping;
		for (int i = 0; i < filterColumns.size(); ++i)
		{
			filterMapping.push_back(QPair<int, int>(filterColumns.at(i), masterColumns.at(i)));
		}
		m_filterMapping.push_back(filterMapping);
	}

	QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
//	FUNCTION: lessThan [ public ]
//------------------------------------------------------------------------------
bool csvFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
	QString leftVal = source_left.model()->data(source_left, Qt::DisplayRole).toString();
	QString rightVal = source_left.model()->data(source_right, Qt::DisplayRole).toString();

	// if they are all digits, do a numerical sort
	QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
	if (re.exactMatch(leftVal) && re.exactMatch(rightVal))
	{
		return (leftVal.toInt() < rightVal.toInt());
	}
	else
	{
		return QSortFilterProxyModel::lessThan(source_left, source_right);
	}
}

//------------------------------------------------------------------------------
//	FUNCTION: setDuplicateFilterColumn [ ]
//------------------------------------------------------------------------------
void csvFilterProxyModel::setDuplicateFilterColumn(QString title)
{
	QStringList titles;
	// master column headers
	for (int col = 0; col < sourceModel()->rowCount(); ++col)
	{
		if (sourceModel()->headerData(col, Qt::Horizontal).toString() == title)
		{
			m_duplicateColumn = col;
			break;;
		}
	}

	// create the duplicate mapping
	QHash<QString, int> duplicates;
	for (auto row = 0; row < sourceModel()->rowCount(); ++row)
	{
		m_duplicateMapping.insertMulti(sourceModel()->data(sourceModel()->index(row, m_duplicateColumn)).toString(), row);
	}

	invalidate();
}

//------------------------------------------------------------------------------
//	FUNCTION: setDuplicateFilteringEnabled [ ]
//------------------------------------------------------------------------------
void csvFilterProxyModel::setDuplicateFilteringEnabled(bool enabled)
{
	if (!m_removeDuplicates == enabled)
	{
		m_removeDuplicates = enabled;
		invalidate();
	}
}

//------------------------------------------------------------------------------
//	FUNCTION: setSingleEntryFilteringEnabled [ public ]
//------------------------------------------------------------------------------
void csvFilterProxyModel::setSingleEntryFilteringEnabled(bool enabled)
{
	if (!m_removeSingles == enabled)
	{
		m_removeSingles = enabled;
		invalidate();
	}
}
