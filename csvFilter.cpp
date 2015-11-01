#include "csvFilter.h"

#include <QIcon>
#include <QFile>
#include <QCompleter>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDirModel>
#include <QFrame>
#include <QStatusBar>
#include <QTimer>
#include <QHeaderView>

//------------------------------------------------------------------------------
//	FUNCTION: CsvFilter [ public ]
//------------------------------------------------------------------------------
CsvFilter::CsvFilter(QWidget* parent /*= (QObject*)0*/)
: QMainWindow(parent)
{
	this->setWindowTitle("CSV Filter");
	this->setWindowIcon(QIcon(":/icons/icon"));
	this->setCentralWidget(new QFrame(this));
	
	m_centralWidgetLayout = new QVBoxLayout(this);
	this->centralWidget()->setLayout(m_centralWidgetLayout);

	this->setStatusBar(new QStatusBar(this));

	setupMasterSpreadsheet();
	setupFilterSpreadsheet();
	setupOutputDock();

}

//------------------------------------------------------------------------------
//	FUNCTION: ~CsvFilter [ public ]
//------------------------------------------------------------------------------
CsvFilter::~CsvFilter()
{

}

//------------------------------------------------------------------------------
//	FUNCTION: setupMasterSpreadsheet [ public ]
//------------------------------------------------------------------------------
void CsvFilter::setupMasterSpreadsheet()
{
	m_masterSpreadSheetGroup = new QGroupBox("Master spreadsheet", this);
	m_masterSpreadSheetLayout = new QHBoxLayout(m_masterSpreadSheetGroup);
	m_masterSpeadSheetLineEdit = new QLineEdit(m_masterSpreadSheetGroup);
	m_masterSpreadSheetBrowseButton = new QPushButton("Browse...", m_masterSpreadSheetGroup);
	m_masterSpreadSheetModel = new csvModel(this);

	m_centralWidgetLayout->addWidget(m_masterSpreadSheetGroup);

	m_masterSpreadSheetGroup->setLayout(m_masterSpreadSheetLayout);

	m_masterSpreadSheetLayout->addWidget(m_masterSpeadSheetLineEdit);
	m_masterSpreadSheetLayout->addWidget(m_masterSpreadSheetBrowseButton);

	m_masterSpreadSheetGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_masterSpeadSheetLineEdit->setCompleter(new QCompleter(new QDirModel(QStringList() << "*.csv", QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name), m_masterSpeadSheetLineEdit));
	m_masterSpeadSheetLineEdit->setPlaceholderText("Path to master spreadsheet...");
	m_masterSpeadSheetLineEdit->setAcceptDrops(true);
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
			this->statusBar()->clearMessage();
		}
	});
}

//------------------------------------------------------------------------------
//	FUNCTION: setupFilterSpreadsheet [virtual  protected ]
//------------------------------------------------------------------------------
void CsvFilter::setupFilterSpreadsheet()
{
	m_filterSpreadSheetGroup = new QGroupBox("filter spreadsheet", this);
	m_filterSpreadSheetLayout = new QVBoxLayout(m_filterSpreadSheetGroup);
	m_filterSpreadSheetPathLayout = new QHBoxLayout(m_filterSpreadSheetGroup);

	m_filterSpeadSheetLineEdit = new QLineEdit(m_filterSpreadSheetGroup);
	m_filterSpreadSheetBrowseButton = new QPushButton("Browse...", m_filterSpreadSheetGroup);
	m_filterSpreadSheetView = new QTableView(m_filterSpreadSheetGroup);
	m_filterSpreadSheetModel = new csvModel(m_filterSpreadSheetGroup);

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
			this->statusBar()->clearMessage();
		}
	});
	connect(m_filterSpreadSheetModel, &csvModel::dataChanged, [&]
	{
		this->statusBar()->showMessage("changed", 1);
	});

	m_filterSpreadSheetView->setModel(m_filterSpreadSheetModel);
	m_filterSpreadSheetView->horizontalHeader()->setSectionsMovable(true);
	m_filterSpreadSheetView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

//------------------------------------------------------------------------------
//	FUNCTION: setupOutputDock [ virtual protected ]
//------------------------------------------------------------------------------
void CsvFilter::setupOutputDock()
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
QSize CsvFilter::sizeHint() const
{
	return QSize(1000, 800);
}
