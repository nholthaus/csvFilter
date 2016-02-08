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

#ifndef filterModelDelegate_h__
#define filterModelDelegate_h__

//--------------------
//	INCLUDES
//--------------------
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPen>

#include <array>

//------------------------------------------------------------
//	@class 		filterModelDelegate
//------------------------------------------------------------
//	@brief		delegate for painting filter models
//	@details	
//------------------------------------------------------------
class filterModelDelegate : public QStyledItemDelegate
{
public:

	explicit filterModelDelegate(QObject* parent = (QObject *)0) 
		: QStyledItemDelegate(parent)
	{
			colors[0] = QColor("#E2FBFB");		
			colors[1] = QColor("#E4FDE4");
			colors[2] = QColor("#F6FEE5");
			colors[3] = QColor("#FFF7E6");
			colors[4] = QColor("#FFEDE6");
			colors[5] = QColor("#FFE6E6");
			colors[6] = QColor("#FDE4F2");
 			colors[7] = QColor("#F3E4FB");
 			colors[8] = QColor("#EBE5FC");
 			colors[9] = QColor("#E5ECFB");
			colors[10] = QColor("#C9F3F3");
 			colors[11] = QColor("#CEF9CE");
 			colors[12] = QColor("#EFFDD2");
 			colors[13] = QColor("#FEFFD3");
 			colors[14] = QColor("#FFF0D3");
 			colors[15] = QColor("#FFE7D3");
 			colors[16] = QColor("#FFD4D3");
 			colors[17] = QColor("#FACFE5");
 			colors[18] = QColor("#E0CFF6");
 			colors[19] = QColor("#D0DBF5");
	};

	virtual ~filterModelDelegate() {};

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		const QAbstractItemModel* model = index.model();
		int filterGroup = model->data(model->index(1, index.column()), Qt::DisplayRole).toInt();
		if (filterGroup > 0)
		{
			painter->save();

			QPen pen;
			pen.setStyle(Qt::NoPen);
			painter->setPen(pen);

			painter->setBrush(colors.at((filterGroup - 1) % colors.size()));
			painter->drawRect(option.rect);
			painter->restore();
		}

		QStyledItemDelegate::paint(painter, option, index);
	}


protected:

	
	
private:

	std::array<QColor, 20>		colors;

};

#endif // filterModelDelegate_h__
