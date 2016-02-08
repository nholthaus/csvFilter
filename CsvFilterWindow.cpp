#include "CsvFilterWindow.h"

#include "filterModelDelegate.h"
#include "outputModelDelegate.h"

#include <QAction>
#include <QCompleter>
#include <QDirModel>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFrame>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QMimeData>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTimer>

#include <vector>

//------------------------------------------------------------------------------
//	FUNCTION: CsvFilter [ public ]
//------------------------------------------------------------------------------
CsvFilterWindow::CsvFilterWindow(QWidget* parent /*= (QObject*)0*/)
:	QMainWindow(parent),
	distribution(0,500)
{
	this->setWindowTitle("CSV Filter");
	this->setWindowIcon(QIcon(":/icons/icon"));
	this->setCentralWidget(new QFrame(this));
	this->setAcceptDrops(true);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &CsvFilterWindow::customContextMenuRequested, this, &CsvFilterWindow::customContextMenu);

	m_centralWidgetLayout = new QVBoxLayout(this);
	this->centralWidget()->setLayout(m_centralWidgetLayout);

	this->setStatusBar(new QStatusBar(this));

	setupMasterSpreadsheet();
	setupFilterSpreadsheet();
	setupOutputDock();
	setupFileMenu();

	// restore state
	QSettings settings("Menari Softworks", "csvFilter");
	this->restoreGeometry(settings.value("geometry").toByteArray());
	this->restoreState(settings.value("state").toByteArray());
}

//------------------------------------------------------------------------------
//	FUNCTION: ~CsvFilter [ public ]
//------------------------------------------------------------------------------
CsvFilterWindow::~CsvFilterWindow()
{

}

//------------------------------------------------------------------------------
//	FUNCTION: closeEvent [ public ]
//------------------------------------------------------------------------------
void CsvFilterWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings("Menari Softworks", "csvFilter");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	QMainWindow::closeEvent(e);
}

//------------------------------------------------------------------------------
//	FUNCTION: setupMasterSpreadsheet [ public ]
//------------------------------------------------------------------------------
void CsvFilterWindow::setupMasterSpreadsheet()
{
	m_masterSpreadSheetGroup = new QGroupBox("Master spreadsheet", this);
	m_masterSpreadSheetLayout = new QHBoxLayout(m_masterSpreadSheetGroup);
	m_masterSpeadSheetLineEdit = new QLineEdit(m_masterSpreadSheetGroup);
	m_masterSpreadSheetBrowseButton = new QPushButton("Browse...", m_masterSpreadSheetGroup);
	m_masterSpreadSheetModel = new csvModel(this);
	m_masterDropEventFilter = new QEventFilter([&](QObject* watched, QEvent* event)
	{
		if (event->type() == QEvent::Drop)
		{
			QDropEvent* dropEvent = dynamic_cast<QDropEvent*>(event);
			QLineEdit* edit = dynamic_cast<QLineEdit*>(watched);
			if (edit)
			{
				
				edit->setText(dropEvent->mimeData()->urls().at(0).toLocalFile());
				return true;
			}
		}

		return false;
	});

	m_centralWidgetLayout->addWidget(m_masterSpreadSheetGroup);

	m_masterSpreadSheetGroup->setLayout(m_masterSpreadSheetLayout);

	m_masterSpreadSheetLayout->addWidget(m_masterSpeadSheetLineEdit);
	m_masterSpreadSheetLayout->addWidget(m_masterSpreadSheetBrowseButton);

	m_masterSpreadSheetGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_masterSpeadSheetLineEdit->setCompleter(new QCompleter(new QDirModel(QStringList() << "*.csv", QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name), m_masterSpeadSheetLineEdit));
	m_masterSpeadSheetLineEdit->setPlaceholderText("Path to master spreadsheet...");
	m_masterSpeadSheetLineEdit->setAcceptDrops(true);
	m_masterSpeadSheetLineEdit->installEventFilter(m_masterDropEventFilter);
	connect(m_masterSpreadSheetBrowseButton, &QPushButton::clicked, [&]
	{
		QString filename = QFileDialog::getOpenFileName(this, "Master Spreadsheet",
			QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first(),
			"CSV Spreadsheets (*.csv)");
		m_masterSpeadSheetLineEdit->setText(filename);
	});
	connect(m_masterSpeadSheetLineEdit->completer(), static_cast<void (QCompleter::*)(const QString&)>(&QCompleter::activated), [this]
	{
		QTimer::singleShot(0, [this]
		{
			if (!this->m_masterSpeadSheetLineEdit->text().endsWith('\\', Qt::CaseInsensitive) &&
				!this->m_masterSpeadSheetLineEdit->text().endsWith(".csv"))
			{
				this->m_masterSpeadSheetLineEdit->setText(this->m_masterSpeadSheetLineEdit->text().append('\\')); 
			}
		});
	});
	connect(m_masterSpeadSheetLineEdit, &QLineEdit::textChanged, [&](const QString& text)
	{
		if (text.endsWith(".csv"))
		{
			this->statusBar()->showMessage("Parsing " + text + "...");
			if (m_masterSpreadSheetModel->importFromFile(text))
			{
				m_filterSpreadSheetGroup->setEnabled(true);
				m_openFilterAction->setEnabled(true);
			}
			else
			{
				m_filterSpreadSheetGroup->setEnabled(false);
				m_openFilterAction->setEnabled(false);
			}
			m_outputView->resizeColumnsToContents();
			m_outputView->resizeRowsToContents();
			this->statusBar()->clearMessage();
		}
	});
}

//------------------------------------------------------------------------------
//	FUNCTION: setupFilterSpreadsheet [virtual  protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::setupFilterSpreadsheet()
{
	m_filterSpreadSheetGroup = new QGroupBox("Filter spreadsheet", this);
	m_filterSpreadSheetLayout = new QVBoxLayout(m_filterSpreadSheetGroup);
	m_filterSpreadSheetPathLayout = new QHBoxLayout(m_filterSpreadSheetGroup);
	m_filterSpeadSheetDuplicateLayout = new QHBoxLayout(m_filterSpreadSheetGroup);

	m_filterSpeadSheetLineEdit = new QLineEdit(m_filterSpreadSheetGroup);
	m_filterSpreadSheetBrowseButton = new QPushButton("Browse...", m_filterSpreadSheetGroup);
	m_filterDuplicatesCheckbox = new QCheckBox("Remove Duplicates?", m_filterSpreadSheetGroup);
	m_filterSinglesCheckbox = new QCheckBox("Remove Single Entries?", m_filterSpreadSheetGroup);
	m_filterDuplicatesLabel = new QLabel("Column to filter:", m_filterSpreadSheetGroup);
	m_filterDuplicatesCombobox = new QComboBox(m_filterSpreadSheetGroup);
	m_filterSpreadSheetView = new QTableView(m_filterSpreadSheetGroup);
	m_filterSpreadSheetModel = new csvFilterModel(m_filterSpreadSheetGroup);

	m_centralWidgetLayout->addWidget(m_filterSpreadSheetGroup);

	m_filterSpreadSheetGroup->setLayout(m_filterSpreadSheetLayout);
	m_filterSpreadSheetLayout->addLayout(m_filterSpreadSheetPathLayout);
	m_filterSpreadSheetLayout->addLayout(m_filterSpeadSheetDuplicateLayout);
	m_filterSpreadSheetLayout->addWidget(m_filterSpreadSheetView);

	m_filterSpreadSheetPathLayout->addWidget(m_filterSpeadSheetLineEdit);
	m_filterSpreadSheetPathLayout->addWidget(m_filterSpreadSheetBrowseButton);

	m_filterSpeadSheetDuplicateLayout->addWidget(m_filterDuplicatesCheckbox);
	m_filterSpeadSheetDuplicateLayout->addWidget(m_filterSinglesCheckbox);
	m_filterSpeadSheetDuplicateLayout->addWidget(m_filterDuplicatesLabel);
	m_filterSpeadSheetDuplicateLayout->addWidget(m_filterDuplicatesCombobox);
	m_filterSpeadSheetDuplicateLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding));

	m_filterSpreadSheetGroup->setEnabled(false);
	m_filterSpreadSheetGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_filterSpeadSheetLineEdit->setCompleter(new QCompleter(new QDirModel(QStringList() << "*.csv", QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name), m_filterSpeadSheetLineEdit));
	m_filterSpeadSheetLineEdit->setPlaceholderText("Path to filter spreadsheet...");
	m_filterSpeadSheetLineEdit->setAcceptDrops(true);
	connect(m_filterSpreadSheetBrowseButton, &QPushButton::clicked, [&]
	{
		QString filename = QFileDialog::getOpenFileName(this, "filter Spreadsheet",
			QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first(),
			"CSV Spreadsheets (*.csv)");
		m_filterSpeadSheetLineEdit->setText(filename);
	});
	connect(m_filterSpeadSheetLineEdit->completer(), static_cast<void (QCompleter::*)(const QString&)>(&QCompleter::activated), [this]
	{
		QTimer::singleShot(0, [this]
		{
			if (!this->m_filterSpeadSheetLineEdit->text().endsWith('\\', Qt::CaseInsensitive) &&
				!this->m_filterSpeadSheetLineEdit->text().endsWith(".csv"))
			{
				this->m_filterSpeadSheetLineEdit->setText(this->m_filterSpeadSheetLineEdit->text().append('\\'));
			}
		});
	});	
	connect(m_filterSpeadSheetLineEdit, &QLineEdit::textChanged, [&](const QString& text)
	{
		if (text.endsWith(".csv"))
		{
			this->statusBar()->showMessage("Parsing " + text + "...");
			m_filterSpreadSheetModel->importFromFile(text);
			m_filterSpreadSheetView->resizeColumnsToContents();
			m_filterSpreadSheetView->resizeRowsToContents();
			this->statusBar()->clearMessage();
		}
	});

	connect(m_filterDuplicatesCheckbox, &QCheckBox::stateChanged, [&](int state)
	{
		if (state == Qt::Checked)
		{
			m_outputProxyModel->setDuplicateFilteringEnabled(true);
		}
		else if (state == Qt::Unchecked)
		{
			m_outputProxyModel->setDuplicateFilteringEnabled(false);
		}
	});
	connect(m_filterSinglesCheckbox, &QCheckBox::stateChanged, [&](int state)
	{
		if (state == Qt::Checked)
		{
			m_outputProxyModel->setSingleEntryFilteringEnabled(true);
		}
		else if (state == Qt::Unchecked)
		{
			m_outputProxyModel->setSingleEntryFilteringEnabled(false);
		}
	});
	connect(m_filterDuplicatesCombobox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), [&](const QString& title)
	{
		m_outputProxyModel->setDuplicateFilterColumn(title);
	});
	connect(m_masterSpreadSheetModel, &csvModel::importedFromFile, [&]
	{
		QStringList filters;
		// master column headers
		for (int col = 0; col < m_masterSpreadSheetModel->rowCount(); ++col)
		{
			filters << m_masterSpreadSheetModel->headerData(col, Qt::Horizontal).toString();
		}
		m_filterDuplicatesCombobox->addItems(filters);
		m_filterDuplicatesCombobox->setCurrentIndex(m_filterDuplicatesCombobox->findText("E1765_CODCASO"));
		m_filterDuplicatesCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	});

	connect(m_filterSpreadSheetModel, &csvModel::dataChanged, [&]
	{
		m_outputProxyModel->invalidate();
	});
	connect(m_filterSpreadSheetModel, &csvModel::importedFromFile, [&]
	{
		m_outputProxyModel->setFilterModel(m_filterSpreadSheetModel);
		m_filterSpreadSheetView->setRowHidden(1, true);	// hide the filter group row.
	});

	m_filterSpreadSheetView->setModel(m_filterSpreadSheetModel);
	m_filterSpreadSheetView->setItemDelegate(new filterModelDelegate(this));
	m_filterSpreadSheetView->setEditTriggers(QAbstractItemView::AllEditTriggers);

	// setup header drag/dropping
	m_filterSpreadSheetView->horizontalHeader()->setSectionsMovable(true);
	m_filterSpreadSheetView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	connect(m_filterSpreadSheetView->horizontalHeader(), &QHeaderView::sectionMoved, 
		[&](int index, int oldVisualIndex, int newVisualIndex)
	{
		m_filterSpreadSheetModel->moveColumn(QModelIndex(), index, QModelIndex(), newVisualIndex);
	});

	m_filterSpreadSheetGroup->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_filterSpreadSheetGroup, &QTableView::customContextMenuRequested, this, &CsvFilterWindow::filterContextMenu);
}

//------------------------------------------------------------------------------
//	FUNCTION: setupOutputDock [ virtual protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::setupOutputDock()
{
	m_outputDock = new QDockWidget("Output", this);
	m_outputDock->setObjectName("m_outputDock");
	m_outputDock->setAllowedAreas(Qt::BottomDockWidgetArea);
	m_outputDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	this->addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

	m_outputView = new QTableView(m_outputDock);
	m_outputDock->setWidget(m_outputView);

	m_outputProxyModel = new csvFilterProxyModel(this);

	m_outputProxyModel->setSourceModel(m_masterSpreadSheetModel);

	m_outputView->setModel(m_outputProxyModel);
	m_outputView->horizontalHeader()->setSectionsMovable(true);
	m_outputView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	m_outputView->setItemDelegate(new outputModelDelegate(m_outputView));
	m_outputView->setSortingEnabled(true);
	m_outputView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//------------------------------------------------------------------------------
//	FUNCTION: setupFileMenu [virtual  protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::setupFileMenu()
{
	m_fileMenu = new QMenu("File", this);
	m_openFilterAction = new QAction("Open filter spreadsheet...", m_fileMenu);
	m_openMasterAction = new QAction("Open master spreadsheet...", m_fileMenu);
	m_saveFilterAction = new QAction("Save filter file...", m_fileMenu);
	m_saveOutputAction = new QAction("Save output file...", m_fileMenu);

	m_openFilterAction->setEnabled(false);

	connect(m_openFilterAction, &QAction::triggered, m_filterSpreadSheetBrowseButton, &QPushButton::click);
	connect(m_openMasterAction, &QAction::triggered, m_masterSpreadSheetBrowseButton, &QPushButton::click);
	connect(m_saveFilterAction, &QAction::triggered, [&]
	{
		QString filePath = QFileDialog::getSaveFileName(this, 
			"Save Filter File", 
			m_filterSpreadSheetModel->file(),
			"CSV Spreadsheets (*.csv)");
		
		if (!filePath.isEmpty())
		{
			exportToFile(m_filterSpreadSheetModel, filePath);
		}
	});
	connect(m_saveOutputAction, &QAction::triggered, [&]
	{
		QFileInfo info(m_masterSpreadSheetModel->file());
		QString filePath = QFileDialog::getSaveFileName(this,
			"Save Output File",
			info.dir().absolutePath() + '\\' + "output.csv",
			"CSV Spreadsheets (*.csv)");

		if (!filePath.isEmpty())
		{
			exportToFile(m_outputProxyModel, filePath);
		}
	});

	m_fileMenu->addAction(m_openMasterAction);
	m_fileMenu->addAction(m_openFilterAction);
	m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_saveFilterAction);
	m_fileMenu->addAction(m_saveOutputAction);

	this->menuBar()->addMenu(m_fileMenu);
}

//------------------------------------------------------------------------------
//	FUNCTION: sizeHint [ public ]
//------------------------------------------------------------------------------
QSize CsvFilterWindow::sizeHint() const
{
	return QSize(1000, 800);
}

//------------------------------------------------------------------------------
//	FUNCTION: customContextMenu [virtual  protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::customContextMenu()
{
	QMenu* bernese = new QMenu;
	bernese->setAttribute(Qt::WA_TranslucentBackground);
	bernese->setWindowFlags(bernese->windowFlags() | Qt::FramelessWindowHint);
	bernese->setWindowFlags(bernese->windowFlags() ^ Qt::Popup);
	bernese->setStyleSheet("QMenu{background-image: url(:/images/bernese); background-color: transparent; border: none;}");
	QPixmap image(":/images/bernese");
	bernese->setMinimumSize(image.size());

	auto player = new QMediaPlayer();
	player->setMedia(QUrl("qrc:/sounds/toasty"));
	player->setVolume(100);
	player->play();
	connect(player, &QMediaPlayer::durationChanged, [&, bernese](qint64 duration)
	{
		QTimer::singleShot(duration, [bernese]
		{
			bernese->close();
		});
	});
	
	QPoint point = this->geometry().bottomRight();
	point.setX(point.x() - bernese->minimumSize().width());
	point.setY(point.y() - bernese->minimumSize().height() - this->statusBar()->height());
	bernese->exec(point);
}

//------------------------------------------------------------------------------
//	FUNCTION: mousePressEvent [ public ]
//------------------------------------------------------------------------------
void CsvFilterWindow::mousePressEvent(QMouseEvent *e)
{
	// toasty randomly, every 500 mouse presses or so
	int dice_roll = distribution(generator);  // generates number in the range 1..6
	if (dice_roll == 23)
	{
		customContextMenu();
	}
	else
	{
		QMainWindow::mousePressEvent(e);
	}
}

//------------------------------------------------------------------------------
//	FUNCTION: filterContextMenu [ protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::filterContextMenu(const QPoint& pos)
{

	QMenu menu;
	bool addGroupAction = false;
	bool addUngroupAction = false;
	QList<int> selectedColumns;

	if (m_filterSpreadSheetView->selectionModel()->hasSelection())
	{
		for (int col = 0; col < m_filterSpreadSheetModel->columnCount(); ++col)
		{
			if (m_filterSpreadSheetView->selectionModel()->columnIntersectsSelection(col, QModelIndex()))
			{
				selectedColumns.push_back(col);
			}
		}

		for (int col = 0; col < m_filterSpreadSheetModel->columnCount(); ++col)
		{
			if (m_filterSpreadSheetView->selectionModel()->columnIntersectsSelection(col, QModelIndex()))
			{
				int group = m_filterSpreadSheetModel->data(m_filterSpreadSheetModel->index(1, col)).toInt();
				if (selectedColumns.size() > 1)
				{
					addGroupAction = true;
				}
				if (group > 0)
				{
					addUngroupAction = true;
				}
			}
		}

		if (addGroupAction)
		{
			QAction* groupAction = new QAction("group", &menu);
			menu.addAction(groupAction);
			connect(groupAction, &QAction::triggered, [&, selectedColumns]
			{
				m_filterSpreadSheetModel->group(selectedColumns);
				m_filterSpreadSheetView->clearSelection();
			});
		}

		if (addUngroupAction)
		{
			QAction* ungroupAction = new QAction("ungroup", &menu);
			menu.addAction(ungroupAction);
			connect(ungroupAction, &QAction::triggered, [&, selectedColumns]
			{
				m_filterSpreadSheetModel->ungroup(selectedColumns);
				m_filterSpreadSheetView->clearSelection();
			});
		}

		if (addGroupAction || addUngroupAction)
		{
			menu.addSeparator();
		}
	}

	QAction* addFilterAction = new QAction("add filter...", &menu);
	menu.addAction(addFilterAction);
	connect(addFilterAction, &QAction::triggered, [&]
	{
		QStringList existingFilters;
		QStringList filters;

		// filter columns headers
		for (int col = 0; col < m_filterSpreadSheetModel->rowCount(); ++col)
		{
			existingFilters << m_filterSpreadSheetModel->headerData(col, Qt::Horizontal).toString();
		}

		// master column headers
		for (int col = 0; col < m_masterSpreadSheetModel->rowCount(); ++col)
		{
			QString filter = m_masterSpreadSheetModel->headerData(col, Qt::Horizontal).toString();
			if (!existingFilters.contains(filter))
			{
				filters << filter;
			}
		}

		bool ok;
		QString newFilter = QInputDialog::getItem(this, "Add Filter", "Select filter:", filters, 0, false, &ok);
		if (ok)
		{
			m_filterSpreadSheetModel->addFilter(newFilter);
		}

		m_outputProxyModel->invalidate();
	});

	menu.exec(this->cursor().pos());
}

//------------------------------------------------------------------------------
//	FUNCTION: exportToFile [virtual  protected ]
//------------------------------------------------------------------------------
bool CsvFilterWindow::exportToFile(QAbstractItemModel* model, QString csvFilePath)
{
	QFile file(csvFilePath);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return false;
	}

	QTextStream out(&file);
	// write the header data
	for (int col = 0; col < model->columnCount(); col++)
	{
		if (col != 0) out << ',';
		out << model->headerData(col, Qt::Horizontal).toString();
	}
	out << '\n';

	// write the row data
	for (int row = 0; row < model->rowCount(); row++)
	{
		for (int col = 0; col < model->columnCount(); col++)
		{
			if (col != 0) out << ',';
			if (!model->data(model->index(row, col), Qt::CheckStateRole).isNull())
			{
				out << model->data(model->index(row, col), Qt::CheckStateRole).toString();
			}
			else
			{
				out << model->data(model->index(row, col), Qt::DisplayRole).toString().trimmed();
			}
		}
		out << '\n';
	}

	file.close();

	return true;
}

//------------------------------------------------------------------------------
//	FUNCTION: populateColumnsCombobox [virtual  protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::populateColumnsCombobox()
{

}
