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
			this->m_masterSpeadSheetLineEdit->setText(this->m_masterSpeadSheetLineEdit->text() + '\\'); 
			this->m_masterSpeadSheetLineEdit->editingFinished();
			QTimer::singleShot(0, [this]
			{
				this->m_masterSpeadSheetLineEdit->completer()->complete();
			});
		});
	});
}

//------------------------------------------------------------------------------
//	FUNCTION: setupFilterSpreadsheet [virtual  protected ]
//------------------------------------------------------------------------------
void CsvFilter::setupFilterSpreadsheet()
{
	m_filterSpreadSheetGroup = new QGroupBox("Filter", this);
	m_centralWidgetLayout->addWidget(m_filterSpreadSheetGroup);
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
}
