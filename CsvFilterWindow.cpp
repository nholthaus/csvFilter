#include "CsvFilterWindow.h"

#include "filterModelDelegate.h"

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
#include <QMediaPlayer>
#include <QMenu>
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
			}
			else
			{
				m_filterSpreadSheetGroup->setEnabled(false);
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

	m_filterSpeadSheetLineEdit = new QLineEdit(m_filterSpreadSheetGroup);
	m_filterSpreadSheetBrowseButton = new QPushButton("Browse...", m_filterSpreadSheetGroup);
	m_filterSpreadSheetView = new QTableView(m_filterSpreadSheetGroup);
	m_filterSpreadSheetModel = new csvFilterModel(m_filterSpreadSheetGroup);

	m_centralWidgetLayout->addWidget(m_filterSpreadSheetGroup);

	m_filterSpreadSheetGroup->setLayout(m_filterSpreadSheetLayout);
	m_filterSpreadSheetLayout->addLayout(m_filterSpreadSheetPathLayout);
	m_filterSpreadSheetLayout->addWidget(m_filterSpreadSheetView);

	m_filterSpreadSheetPathLayout->addWidget(m_filterSpeadSheetLineEdit);
	m_filterSpreadSheetPathLayout->addWidget(m_filterSpreadSheetBrowseButton);

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
	connect(m_filterSpreadSheetModel, &csvModel::dataChanged, [&]
	{
		m_masterSpreadSheetModel->filter(m_filterSpreadSheetModel);
	});
	connect(m_filterSpreadSheetModel, &csvModel::importedFromFile, [&]
	{
		m_masterSpreadSheetModel->filter(m_filterSpreadSheetModel);
		m_filterSpreadSheetView->setRowHidden(1, true);	// hide the filter group row.
	});

	m_filterSpreadSheetView->setModel(m_filterSpreadSheetModel);
	m_filterSpreadSheetView->horizontalHeader()->setSectionsMovable(true);
	m_filterSpreadSheetView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	m_filterSpreadSheetView->setItemDelegate(new filterModelDelegate(this));
	m_filterSpreadSheetGroup->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_filterSpreadSheetGroup, &QTableView::customContextMenuRequested, this, &CsvFilterWindow::filterContextMenu);
}

//------------------------------------------------------------------------------
//	FUNCTION: setupOutputDock [ virtual protected ]
//------------------------------------------------------------------------------
void CsvFilterWindow::setupOutputDock()
{
	m_outputDock = new QDockWidget("Output", this);
	m_outputDock->setAllowedAreas(Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

	m_outputView = new QTableView(m_outputDock);
	m_outputDock->setWidget(m_outputView);

	m_outputView->setModel(m_masterSpreadSheetModel);
	m_outputView->horizontalHeader()->setSectionsMovable(true);
	m_outputView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	m_outputView->setSortingEnabled(true);
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
	if (m_filterSpreadSheetView->selectionModel()->hasSelection())
	{
		QMenu menu;
		bool addGroupAction = false;
		bool addUngroupAction = false;
		QList<int> selectedColumns;

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
			menu.exec(this->cursor().pos());
		}
	}
}
