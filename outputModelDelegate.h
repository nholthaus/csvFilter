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

#ifndef outputModelDelegate_h__
#define outputModelDelegate_h__

//--------------------
//	INCLUDES
//--------------------

#include <QPen>
#include <QPainter>
#include <QStyledItemDelegate>

//------------------------------------------------------------
//	@class 		outputModelDelegate
//------------------------------------------------------------
//	@brief		used to color soft hits from the output filter
//	@details	
//------------------------------------------------------------
class outputModelDelegate : public QStyledItemDelegate
{
public:

	explicit outputModelDelegate(QObject* parent = (QObject*)0) : QStyledItemDelegate(parent) {}
	virtual ~outputModelDelegate() {};
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		const QAbstractItemModel* model = index.model();
		int softHitOder = model->data(index, Qt::UserRole).toInt();
		if (softHitOder > 0)
		{
			painter->save();

			QPen pen;
			pen.setStyle(Qt::NoPen);
			painter->setPen(pen);

			painter->setBrush(QColor("#FFADAD"));
			painter->drawRect(option.rect);
			painter->restore();
		}

		QStyledItemDelegate::paint(painter, option, index);
	}


protected:

	
	
private:



};

#endif // outputModelDelegate_h__
