//--------------------------------------------------------------------------------------------------
// 
/// @PROJECT	csvFilter
///	@AUTHORS	Nic Holthaus
/// @DATE		2015/11/11
// 
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
// 
// Copyright (c) 2015 Nic Holthaus
// 
//--------------------------------------------------------------------------------------------------

#ifndef csvFilterProxyModel_h__
#define csvFilterProxyModel_h__

//--------------------
//	INCLUDES
//--------------------

#include "csvFilterModel.h"

#include <QSortFilterProxyModel>

//------------------------------------------------------------
//	@class 		csvFilterProxyModel
//------------------------------------------------------------
//	@brief		Filters a csvModel according to a csvFilterModel
//	@details	
//------------------------------------------------------------
class csvFilterProxyModel : public QSortFilterProxyModel
{
public:

	explicit csvFilterProxyModel(QObject* parent = (QObject*)0);
	virtual ~csvFilterProxyModel();
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
	
	void setFilterModel(const csvFilterModel* filterModel);
	void setDuplicateFilterColumn(QString title);
	void setDuplicateFilteringEnabled(bool enabled);
	void setSingleEntryFilteringEnabled(bool enabled);

	void setFilterRegExp(const QRegExp& regExp) = delete;
	void setFilterRegExp(const QString& pattern) = delete;

public slots:

	void invalidate();

protected:

	virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
	
private:

	const csvFilterModel*				m_filterModel;
	QList<QList<QPair<int, int>>>		m_filterMapping;
	QHash<QString, int>					m_duplicateMapping;
	int									m_duplicateColumn;

	bool								m_removeDuplicates;
	bool								m_removeSingles;
};

#endif // csvFilterProxyModel_h__
