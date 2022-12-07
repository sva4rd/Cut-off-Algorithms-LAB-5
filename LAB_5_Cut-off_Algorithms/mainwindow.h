#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cutoffalgorithmswidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *mainWgt;
    QToolBar *fileTool;
    QPushButton *toOrigin;
    QPushButton *clearData;

    CutOffAlgorithmsWidget *algorithmWgt;

    QString openPath;

    void createActions();
    void openFileAndProcess(CutOffAlgorithmsWidget::Type type);
private slots:
    void OpenFileWithSections();
    void OpenFileWithPolygons();
    void GetFormat();
    void Clear();
    void GoToOrigin();
};
#endif // MAINWINDOW_H
