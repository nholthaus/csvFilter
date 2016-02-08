//--------------------------------------------------------------------------------------------------
// 
/// @PROJECT	csvFilter
///	@AUTHORS	Nic Holthaus
/// @DATE		2015/11/08
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

#ifndef csvFilterModel_h__
#define csvFilterModel_h__

//--------------------
//	INCLUDES
//--------------------
#include "csvModel.h"

#include <set>

//------------------------------------------------------------
//	@class 		csvFilterModel
//------------------------------------------------------------
//	@brief		model with handling for the special rows that
//				filter csv's contain.
//	@details	
//------------------------------------------------------------
class csvFilterModel : public csvModel
{
public:

	explicit csvFilterModel(QObject* parent = (QObject*)0);
	virtual ~csvFilterModel();
	virtual bool importFromFile(QString csvFilePath) override;

	virtual void group(QList<int> columns);
	virtual void ungroup(QList<int> columns);

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual bool moveColumn(const QModelIndex &sourceParent, int sourceColumn, const QModelIndex &destinationParent, int destinationChild);

	virtual void addFilter(const QString& filter);

protected:

	
	
private:


};
#endif // csvFilterModel_h__
