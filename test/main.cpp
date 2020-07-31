#include "nodeview.h"
#include "nodeitem.h"
#include <QApplication>

#include <QLineEdit>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NodeView view;
    view.resize(1000, 900);
    view.setRopeFlexion(50);
    view.show();

    PortItem *portIn = Q_NULLPTR;

    QWidget *widget = new QWidget;
    widget->resize(150, 30);

    QLabel *label = new QLabel("Node:",widget);
    label->resize(60, 20);
    label->move(2, 5);

    QLineEdit *lineEdit = new QLineEdit("12345",widget);
    lineEdit->resize(105, 20);
    lineEdit->move(40, 5);

    NodeItem *node = view.createNode(widget);
    node->setTitle("Node Item");
    node->setTitleColor(QColor::fromRgb(160, 160, 160, 200));
    node->setBackgroundColor(QColor::fromRgb(190, 190, 190, 200));
    PortItem *portOut = node->createPortOut(15, QColor(Qt::cyan));
    node->setPos(-250, 90);

    for (int i=0; i<5; ++i)
    {
        QWidget *widget = new QWidget;
        widget->resize(150, 30);

        QLabel *label = new QLabel("Node:",widget);
        label->resize(60, 20);
        label->move(2, 5);

        QLineEdit *lineEdit = new QLineEdit("12345",widget);
        lineEdit->resize(105, 20);
        lineEdit->move(40, 5);

        NodeItem *node = view.createNode(widget);
        node->setTitle("Node Item " + QString::number(i + 1));
        node->setTitleColor(QColor::fromRgb(160, 160, 160, 200));
        node->setBackgroundColor(QColor::fromRgb(190, 190, 190, 200));
        node->setPos(i * 20, i * 60);

        portIn = node->createPortIn(15, QColor(Qt::cyan));
        view.createConnection(portOut, portIn);
    }

    GroupItem *group = view.createGroup(view.nodeList());
    group->setTitle("Group item");
    group->removeNode(node);

    return a.exec();
}
