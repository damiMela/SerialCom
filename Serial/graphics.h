#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QWidget>

namespace Ui {
class graphics;
}

class graphics : public QWidget
{
    Q_OBJECT

public:
    explicit graphics(QWidget *parent = nullptr);
    ~graphics();
    void addPoint(double _x, double _y);

private:
    Ui::graphics *ui;
    QVector<double> x,y; // initialize with entries 0..100
    int x_count;
    int y_count;
};

#endif // GRAPHICS_H
