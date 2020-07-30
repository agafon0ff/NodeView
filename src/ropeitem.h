#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsItem>
#include "portitem.h"

class RopeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RopeItem(PortItem *portOut, PortItem *portIn, QGraphicsItem *parent = Q_NULLPTR);

private:

    PortItem *m_portOut;
    PortItem *m_portIn;

    QPointF m_pointOut;
    QPointF m_pointIn;

    QPointF m_mousePoint;

    QSize m_size;
    QColor m_color;

    bool m_isConnected;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    void createPortsConnection();
    void disconnectFromPorts();

signals:

public slots:
    QColor color(){return m_color;}
    void setColor(QColor color);

    PortItem *portOut(){return m_portOut;}
    void setPortOut(PortItem *port);
    void removePortOut();

    PortItem *portIn(){return m_portIn;}
    void setPortIn(PortItem *port);
    void removePortIn();

    void setMousePoint(QPointF point);
    void calculateRope();

    bool isConnected(){return m_isConnected;}
};

#endif // CONNECTION_H
