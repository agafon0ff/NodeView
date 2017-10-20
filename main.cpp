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

    PortItem *portIn = NULL;
    PortItem *portOut = NULL;

    for(int i=0;i<3;++i)
    {
        QWidget *widget = new QWidget;
        widget->resize(200,40);

        QLabel *label = new QLabel("Node:",widget);
        label->resize(60,20);
        label->move(10,10);

        QLineEdit *lineEdit = new QLineEdit("12345",widget);
        lineEdit->resize(120,20);
        lineEdit->move(70,10);

        NodeItem *node = view.createNode(widget);
        node->setTitle("Node Item");
        node->setTitleColor(QColor::fromRgb(180,50,50,200));

        portIn = node->createPortIn(20,QColor(Qt::yellow));

        if(portIn && portOut)
            view.createConnection(portOut,portIn);

        portOut = node->createPortOut(20,QColor(Qt::yellow));

        node->setPos(i*350,i*50);
    }

    return a.exec();
}
