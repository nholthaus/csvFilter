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

#ifndef CsvFilterWindow_h__
#define CsvFilterWindow_h__

//--------------------
//	INCLUDES
//--------------------

// Qt
#include <QCheckBox>
#include <QComboBox>
#include <QMainWindow>
#include <QGroupBox>
#include <QLabel>
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
#include <random>

// local
#include "csvModel.h"
#include "csvFilterModel.h"
#include "QEventFilter.h"
#include "csvFilterProxyModel.h"

//------------------------------------------------------------
//	@class 		
//------------------------------------------------------------
//	@brief		Main Window of the csvFilter program
//	@details	
//------------------------------------------------------------
class CsvFilterWindow : public QMainWindow
{
public:

	explicit CsvFilterWindow(QWidget* parent = (QWidget*)0);
	virtual ~CsvFilterWindow();
	
protected:

	virtual bool exportToFile(QAbstractItemModel* model, QString csvFilePath);
	virtual void setupMasterSpreadsheet();
	virtual void setupFilterSpreadsheet();
	virtual void setupOutputDock();
	virtual void setupFileMenu();
	virtual QSize sizeHint() const override;
	virtual void customContextMenu();
	virtual void mousePressEvent(QMouseEvent *) override;
	virtual void populateColumnsCombobox();

	void filterContextMenu(const QPoint& pos);
	virtual void closeEvent(QCloseEvent *) override;

private:

	// central widget
	QVBoxLayout*						m_centralWidgetLayout;

	// master spread sheet widgets
	QGroupBox*							m_masterSpreadSheetGroup;
	QHBoxLayout*						m_masterSpreadSheetLayout;
	QLineEdit*							m_masterSpeadSheetLineEdit;
	QPushButton*						m_masterSpreadSheetBrowseButton;
	csvModel*							m_masterSpreadSheetModel;
	QEventFilter*						m_masterDropEventFilter;

	// filter spreadsheet widgets
	QGroupBox*							m_filterSpreadSheetGroup;
	QVBoxLayout*						m_filterSpreadSheetLayout;
	QHBoxLayout*						m_filterSpreadSheetPathLayout;
	QHBoxLayout*						m_filterSpeadSheetDuplicateLayout;
	QCheckBox*							m_filterDuplicatesCheckbox;
	QCheckBox*							m_filterSinglesCheckbox;
	QLabel*								m_filterDuplicatesLabel;
	QComboBox*							m_filterDuplicatesCombobox;
	QLineEdit*							m_filterSpeadSheetLineEdit;
	QPushButton*						m_filterSpreadSheetBrowseButton;
	QTableView*							m_filterSpreadSheetView;
	csvFilterModel*						m_filterSpreadSheetModel;
	QEventFilter*						m_filterDropEventFilter;

	// output spreadsheet widgets
	QDockWidget*						m_outputDock;
	QGroupBox*							m_outputGroup;
	QTableView*							m_outputView;
	csvFilterProxyModel*				m_outputProxyModel;

	// menus
	QMenu*								m_fileMenu;
	QAction*							m_saveFilterAction;
	QAction*							m_saveOutputAction;
	QAction*							m_openFilterAction;
	QAction*							m_openMasterAction;

	// random numbers for toasty
	std::default_random_engine			generator;
	std::uniform_int_distribution<int>	distribution;

};
#endif // CsvFilterWindow_h__