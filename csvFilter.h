//--------------------------------------------------------------------------------------------------
// 
/// @PROJECT	csvFilter
///	@AUTHORS	Nic Holthaus
/// @DATE		2015/10/31
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
#include <QMainWindow>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileSystemModel>
#include <QDockWidget>
#include <QTableView>
#include <QDebug>

// std
#include <memory>

// local
#include "csvModel.h"

//------------------------------------------------------------
//	@class 		
//------------------------------------------------------------
//	@brief		Main Window of the csvFilter program
//	@details	
//------------------------------------------------------------
class CsvFilter : public QMainWindow
{
public:

	explicit CsvFilter(QWidget* parent = (QWidget*)0);
	virtual ~CsvFilter();
	
protected:

	virtual void setupMasterSpreadsheet();
	virtual void setupFilterSpreadsheet();
	virtual void setupOutputDock();

private:

	// central widget
	QVBoxLayout*			m_centralWidgetLayout;

	// master spread sheet widgets
	QGroupBox*				m_masterSpreadSheetGroup;
	QHBoxLayout*			m_masterSpreadSheetLayout;
	QLineEdit*				m_masterSpeadSheetLineEdit;
	QPushButton*			m_masterSpreadSheetBrowseButton;
	csvModel*				m_masterSpreadSheetModel;

	QGroupBox*				m_filterSpreadSheetGroup;
	QGroupBox*				m_outputGroup;

	QDockWidget*			m_outputDock;
	QTableView*				m_outputView;

};