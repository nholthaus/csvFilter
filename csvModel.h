//--------------------------------------------------------------------------------------------------
// 
/// @PROJECT	cvsFilter
///	@AUTHORS	Nic Holthaus
/// @DATE		2015/11/01
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

//--------------------
//	INCLUDES
//--------------------

// Qt
#include <QObject>
#include <QStandardItemModel>
#include <QString>

//------------------------------------------------------------
//	@class 		
//------------------------------------------------------------
//	@brief		standard item model for comma separated value data.
//	@details	
//------------------------------------------------------------
class csvModel : public QStandardItemModel
{
	Q_OBJECT

public:

	explicit csvModel(QObject* parent = (QObject*)0);
	virtual ~csvModel();
	
	bool importFromFile(QString csvFilePath);

protected:

	
	
private:



};
