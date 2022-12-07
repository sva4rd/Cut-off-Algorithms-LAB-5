#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    setWindowTitle("Cut-off Algorithms");
    setMinimumSize(700, 500);

    QFont font;
    font.setBold(true);
    setFont(font);

    createActions();
    mainWgt = new QWidget;
    setCentralWidget(mainWgt);

    algorithmWgt = new CutOffAlgorithmsWidget;
    toOrigin = new QPushButton("To Origin");
    clearData = new QPushButton("Clear");

    connect(toOrigin, &QPushButton::clicked, this, &MainWindow::GoToOrigin);
    connect(clearData, &QPushButton::clicked, this, &MainWindow::Clear);

    QGridLayout *mainLayout = new QGridLayout(mainWgt);
    mainLayout->addWidget(algorithmWgt, 0, 0, 1, 6);
    mainLayout->addWidget(toOrigin, 1, 4);
    mainLayout->addWidget(clearData, 1, 5);

    openPath = "D:/";
}

MainWindow::~MainWindow(){}

void MainWindow::createActions()
{
    QAction *openSectionFile = new QAction (QIcon (":/section.png"), "Open file with sections", this);
    openSectionFile->setShortcut(QKeySequence("Ctrl+S"));

    QAction *openPolygonFile = new QAction (QIcon (":/polygon.png"), "Open file with polygons", this);
    openPolygonFile->setShortcut(QKeySequence("Ctrl+P"));

    QAction *formatInfo = new QAction (QIcon (":/formatInfo.png"), "File format", this);
    formatInfo->setShortcut(QKeySequence("Ctrl+I"));

    fileTool = addToolBar ("File");
    fileTool->addAction(openSectionFile);
    fileTool->addAction(openPolygonFile);
    fileTool->addAction(formatInfo);

    fileTool-> setAllowedAreas (Qt::TopToolBarArea | Qt::LeftToolBarArea);
    fileTool->setMovable(false);

    connect(openSectionFile, SIGNAL(triggered()), this, SLOT(OpenFileWithSections()));
    connect(openPolygonFile, SIGNAL(triggered()), this, SLOT(OpenFileWithPolygons()));
    connect(formatInfo, SIGNAL(triggered()), this, SLOT(GetFormat()));
}

void MainWindow::openFileAndProcess(CutOffAlgorithmsWidget::Type type)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", openPath,
                                                    "Data Files(*.txt *.dat)");
    if (!fileName.isEmpty())
    {
        QFileInfo fileInfo(fileName);
        openPath = fileInfo.path();
        if(!algorithmWgt->process(QFile(fileName), type))
            QMessageBox::warning(this, "Error", "Wrong file");
    }
}

void MainWindow::OpenFileWithSections()
{
    openFileAndProcess(CutOffAlgorithmsWidget::SECTION_TYPE);
}

void MainWindow::OpenFileWithPolygons()
{
    openFileAndProcess(CutOffAlgorithmsWidget::POLYGON_TYPE);
}

void MainWindow::GetFormat()
{
    QMessageBox::information(this, "File format",
                             "SECTION FILE:\n"
                             "n - number of segments\n"
                             "X1_1 Y1_1 X2_1 Y2_1\n"
                             "X1_2 Y1_2 X2_2 Y2_2\n"
                             "...\n"
                             "X1_n Y1_n X2_n Y2_n - coordinates of the segments\n"
                             "Xmin Ymin Xmax Ymax - coordinates of the cutting off rectangular window\n"
                             "\n"
                             "POLYGON FILE:\n"
                             "n - number of polygons\n"
                             "m - number of polygon vertices\n"
                             "X1_1 Y1_1 X2_1 Y2_1 ... Xm_1 Ym_1 X1_1 Y1_1\n"
                             "X1_2 Y1_2 X2_2 Y2_2 ... Xm_2 Ym_2 X1_2 Y1_2\n"
                             "...\n"
                             "X1_n Y1_n X2_n Y2_n ... Xm_n Ym_n X1_n Y1_n - coordinates of the polygons\n"
                             "Xmin Ymin Xmax Ymax - coordinates of the cutting off rectangular window");
}

void MainWindow::Clear()
{
    algorithmWgt->Clear();
}

void MainWindow::GoToOrigin()
{
    algorithmWgt->GoToOrigin();
}

