#ifndef CUTOFFALGORITHMSWIDGET_H
#define CUTOFFALGORITHMSWIDGET_H

#include <QtWidgets>
#include "qcustomplot.h"

class CutOffAlgorithmsWidget : public QWidget
{
    Q_OBJECT
public:
    CutOffAlgorithmsWidget(QWidget *parent = nullptr);
    ~CutOffAlgorithmsWidget();

    enum Type
    {
        SECTION_TYPE,
        POLYGON_TYPE
    };

    bool process(QFile file, Type type);
    void Clear();
    void GoToOrigin();

private:
    QCustomPlot *mainWgt;
    QRegularExpression digitValidator;
    QRegularExpression nValidator;
    QRegularExpression mValidator;
    QPen visiblePartPen;
    QPen cuttingPartPen;
    QPen windowPen;

    QVector <QStringList> figures;
    int n;
    int m;
    int currentGraph;
    int xMin;
    int xMax;
    int yMin;
    int yMax;

    void setCode(QVector <int> &code, int x, int y);
    void drawSectionBySutherland_Cohen(double x1, double y1, double x2, double y2);
    void drawVisibleLine(double x1, double y1, double x2, double y2);
    void drawCuttingLine(double x1, double y1, double x2, double y2);
    void drawWindow(int x1, int y1, int x2, int y2);

    void setLegend();
    bool checkData(Type type);
    bool checkWindow();
    bool readMainNumbers(QFile &file, Type type);
};

#endif // CUTOFFALGORITHMSWIDGET_H
