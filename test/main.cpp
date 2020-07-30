#include "nodeview.h"
#include "nodeitem.h"
#include <QApplication>

#include <QLineEdit>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NodeView view;
    view.resize(1000,900);
    view.show();

    PortItem *portIn = Q_NULLPTR;
    PortItem *portOut = Q_NULLPTR;

    for (int i=0; i<5; ++i)
    {
        QWidget *widget = new QWidget;
        widget->resize(150,30);

        QLabel *label = new QLabel("Node:",widget);
        label->resize(60,20);
        label->move(2,5);

        QLineEdit *lineEdit = new QLineEdit("12345",widget);
        lineEdit->resize(105,20);
        lineEdit->move(40,5);

        NodeItem *node = view.createNode(widget);
        node->setTitle("Node Item " + QString::number(i + 1));
        node->setTitleColor(QColor::fromRgb(160,160,160,200));
        node->setBackgroundColor(QColor::fromRgb(190,190,190,200));

        portIn = node->createPortIn(15, QColor(Qt::yellow));

        if (portIn && portOut)
            view.createConnection(portOut, portIn);

        portOut = node->createPortOut(15, QColor(Qt::yellow));

        node->setPos(i*250,i*50);
    }

    return a.exec();
}