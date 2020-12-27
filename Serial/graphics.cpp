#include "graphics.h"
#include "ui_graphics.h"

graphics::graphics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphics)
{
    ui->setupUi(this);



    // create graph and assign data to it:
    ui->graphPlot->addGraph();
    ui->graphPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->graphPlot->xAxis->setLabel("x");
    ui->graphPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->graphPlot->xAxis->setRange(0, 100);
    ui->graphPlot->yAxis->setRange(0, 1.5);
    ui->graphPlot->replot();
}

graphics::~graphics()
{
    delete ui;
}

void graphics::addPoint(double _x, double _y)
{
    x.append(_x);
    y.append(_y);
    ui->graphPlot->graph(0)->setData(x, y);
    ui->graphPlot->replot();

}
