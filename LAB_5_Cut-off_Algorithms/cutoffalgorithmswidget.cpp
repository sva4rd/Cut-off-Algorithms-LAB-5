#include "cutoffalgorithmswidget.h"

CutOffAlgorithmsWidget::CutOffAlgorithmsWidget(QWidget *parent): QWidget{parent}
{
    mainWgt = new QCustomPlot();
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(mainWgt);
    mainWgt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWgt->setInteraction(QCP::iRangeDrag,true);
    mainWgt->setInteraction(QCP::iRangeZoom,true);
    GoToOrigin();

    visiblePartPen.setWidth(3);
    visiblePartPen.setColor(Qt::blue);
    cuttingPartPen.setWidth(3);
    cuttingPartPen.setColor(Qt::red);
    cuttingPartPen.setStyle(Qt::DotLine);
    windowPen.setWidth(6);
    windowPen.setColor(Qt::black);

    digitValidator.setPattern("^-?[0-9]*$");
    nValidator.setPattern("^[1-9]([0-9])*$");
    mValidator.setPattern("^[3-9]([0-9])*$");
    currentGraph = -1;
    mainWgt->legend->setVisible(true);
    mainWgt->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);
}

CutOffAlgorithmsWidget::~CutOffAlgorithmsWidget(){}

bool CutOffAlgorithmsWidget::process(QFile file, Type type)
{
    if(!readMainNumbers(file, type))
        return false;

    QString data;
    int i = n;
    while (!file.atEnd())
    {
        data = file.readLine();
        data = data.trimmed();
        figures.push_back(data.split(" "));
        i--;
    }

    if (i != -1 || !checkData(type))
    {
        Clear();
        return false;
    }

    setLegend();
    xMin = qMin(figures[n][0].toInt(), figures[n][2].toInt());
    xMax = qMax(figures[n][0].toInt(), figures[n][2].toInt());
    yMin = qMin(figures[n][1].toInt(), figures[n][3].toInt());
    yMax = qMax(figures[n][1].toInt(), figures[n][3].toInt());

    drawWindow(xMin, yMin, xMax, yMax);

    for (int i = 0; i < n; i++)
    {
        if (type == POLYGON_TYPE)
        {
            for (int j = 0; j < 2*m; j += 2)
                drawSectionBySutherland_Cohen(figures[i][j].toInt(), figures[i][j+1].toInt(),
                        figures[i][j+2].toInt(), figures[i][j+3].toInt());

            drawSectionBySutherland_Cohen(figures[i][0].toInt(), figures[i][1].toInt(),
                    figures[i][2*m].toInt(), figures[i][2*m+1].toInt());
        }
        else
        {
            drawSectionBySutherland_Cohen(figures[i][0].toInt(), figures[i][1].toInt(),
                    figures[i][2].toInt(), figures[i][3].toInt());
        }
    }

    mainWgt->replot();
    return true;
}

void CutOffAlgorithmsWidget::setCode(QVector<int> &code, int x, int y)
{
    code.clear();
    code.resize(4);
    if (x < xMin)
        code[3] = 1;
    else if (x > xMax)
        code[2] = 1;
    if (y < yMin)
        code[1] = 1;
    else if (y > yMax)
        code[0] = 1;
}

void CutOffAlgorithmsWidget::drawSectionBySutherland_Cohen(double x1, double y1, double x2, double y2)
{
    QVector<int> aCode;
    setCode(aCode, x1, y1);
    QVector<int> bCode;
    setCode(bCode, x2, y2);
    double x, y;
    QVector<int> currentCode(4);

    while (aCode != currentCode || bCode != currentCode)    //currentCode всегда из cостоит
    {                                                       //в начале цикла т.е. пока код
        if((aCode[0] == 1 && bCode[0] == 1) || (aCode[1] == 1 && bCode[1] == 1)  // у двух != 0000
                || (aCode[2] == 1 && bCode[2] == 1) || (aCode[3] == 1 && bCode[3] == 1))
        {
            drawCuttingLine(x1, y1, x2, y2);
            return;
        }

        if (aCode != currentCode)//не нулевой
        {
            currentCode = aCode;
            x = x1;
            y = y1;
        }
        else
        {
            currentCode = bCode;
            x = x2;
            y = y2;
        }


        if (currentCode[3] == 1)//левее окна
        {
            y += (y1 - y2) * (xMin - x) / (x1 - x2);
            x = xMin;
        }
        else if (currentCode[2] == 1)//правее окна
        {
            y += (y1 - y2) * (xMax - x) / (x1 - x2);
            x = xMax;
        }
        else if (currentCode[1] == 1)//ниже окна
        {
            x += (x1 - x2) * (yMin - y) / (y1 - y2);
            y = yMin;
        }
        else if (currentCode[0] == 1)//выше окна
        {
            x += (x1 - x2) * (yMax - y) / (y1 - y2);
            y = yMax;
        }


        if (currentCode == aCode)
        {
            drawCuttingLine(x1, y1, x, y);
            x1 = x;
            y1 = y;
            setCode(aCode, x1, y1);
        }
        else
        {
            drawCuttingLine(x2, y2, x, y);
            x2 = x;
            y2 = y;
            setCode(bCode, x2, y2);
        }
        currentCode.clear();
        currentCode.resize(4);
    }
    drawVisibleLine(x1, y1, x2, y2);
}

void CutOffAlgorithmsWidget::drawVisibleLine(double x1, double y1, double x2, double y2)
{
    mainWgt->addGraph();
    mainWgt->graph(++currentGraph)->addData(x1, y1);
    mainWgt->graph(currentGraph)->addData(x2, y2);
    mainWgt->graph(currentGraph)->setPen(visiblePartPen);
}

void CutOffAlgorithmsWidget::drawCuttingLine(double x1, double y1, double x2, double y2)
{
    mainWgt->addGraph();
    mainWgt->graph(++currentGraph)->addData(x1, y1);
    mainWgt->graph(currentGraph)->addData(x2, y2);
    mainWgt->graph(currentGraph)->setPen(cuttingPartPen);
}

void CutOffAlgorithmsWidget::drawWindow(int x1, int y1, int x2, int y2)
{
    mainWgt->addGraph();
    mainWgt->graph(++currentGraph)->addData(x1, y1);
    mainWgt->graph(currentGraph)->addData(x2, y1);
    mainWgt->graph(currentGraph)->setPen(windowPen);

    mainWgt->addGraph();
    mainWgt->graph(++currentGraph)->addData(x2, y1);
    mainWgt->graph(currentGraph)->addData(x2, y2);
    mainWgt->graph(currentGraph)->setPen(windowPen);

    mainWgt->addGraph();
    mainWgt->graph(++currentGraph)->addData(x2, y2);
    mainWgt->graph(currentGraph)->addData(x1, y2);
    mainWgt->graph(currentGraph)->setPen(windowPen);

    mainWgt->addGraph();
    mainWgt->graph(++currentGraph)->addData(x1, y2);
    mainWgt->graph(currentGraph)->addData(x1, y1);
    mainWgt->graph(currentGraph)->setPen(windowPen);
}

void CutOffAlgorithmsWidget::setLegend()
{
    mainWgt->addGraph();
    mainWgt->graph(0)->setPen(visiblePartPen);
    mainWgt->graph(0)->setName("Visible");
    mainWgt->addGraph();
    mainWgt->graph(1)->setPen(cuttingPartPen);
    mainWgt->graph(1)->setName("Cutting");
    mainWgt->addGraph();
    mainWgt->graph(2)->setName("Window");
    mainWgt->graph(2)->setPen(windowPen);
    mainWgt->setAutoAddPlottableToLegend(false);
    currentGraph = 2;
}

bool CutOffAlgorithmsWidget::checkData(Type type)
{
    QSet <QString> identityCheck;
    if (!checkWindow())
        return false;
    for (int i = 0; i < n; i++)
    {
        if ((type == POLYGON_TYPE && figures[i].size() != 2*m + 2) ||
                (type == SECTION_TYPE && figures[i].size() != 2*m))
            return false;
        for (int j = 0; j < 2*m; j++)
        {
            if (!digitValidator.match(figures[i][j]).hasMatch())
                return false;
            if (j % 2 == 1)
                identityCheck.insert(figures[i][j-1] + figures[i][j]);
        }

        if (type == SECTION_TYPE && identityCheck.size() != 2)
            return false;
        else if (type == POLYGON_TYPE && (identityCheck.size() != m ||
                                          figures[i][2*m] + figures[i][2*m+1] !=
                                          figures[i][0] + figures[i][1]))
            return false;
        identityCheck.clear();
    }

    return true;
}

bool CutOffAlgorithmsWidget::checkWindow()
{
    if (figures[n].size() == 4)
    {
        if (digitValidator.match(figures[n][0]).hasMatch() &&
                digitValidator.match(figures[n][1]).hasMatch() &&
                digitValidator.match(figures[n][2]).hasMatch() &&
                digitValidator.match(figures[n][3]).hasMatch() &&
                figures[n][0] + figures[n][1] != figures[n][2] + figures[n][3])
            return true;
    }
    return false;
}

bool CutOffAlgorithmsWidget::readMainNumbers(QFile &file, Type type)
{
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (file.atEnd())
        return false;

    QString s = file.readLine();
    s = s.left(s.size()-2);
    if (nValidator.match(s).hasMatch())
        n = s.toInt();
    else
        return false;

    if (type == SECTION_TYPE)
        m = 2;
    else
    {
        if (file.atEnd())
            return false;
        s = file.readLine();
        s = s.left(s.size()-2);
        if (mValidator.match(s).hasMatch())
            m = s.toInt();
        else
            return false;
    }
    Clear();
    return true;
}

void CutOffAlgorithmsWidget::Clear()
{
    mainWgt->clearGraphs();
    currentGraph = -1;
    mainWgt->setAutoAddPlottableToLegend(true);
    figures.clear();
    mainWgt->replot();
}

void CutOffAlgorithmsWidget::GoToOrigin()
{
    mainWgt->xAxis->setRange(-500, 500);
    mainWgt->yAxis->setRange(-500, 500);
    mainWgt->replot();
}
