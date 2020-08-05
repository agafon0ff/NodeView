#include "ropeitem.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>

qreal distance(const QPointF &a, const QPointF &b)
{
    qreal dx = a.x() - b.x();
    qreal dy = a.y() - b.y();
    return qSqrt(dx * dx + dy * dy);
}

RopeItem::RopeItem(PortItem *portOut, PortItem *portIn, QGraphicsItem *parent)
: QGraphicsObject(parent),
    m_portOut(portOut),
    m_portIn(portIn),
    m_size(QSize(20,20)),
    m_color(QColor(Qt::white)),
    m_isConnected(false),
    m_flexion(100.0)
{
    setZValue(0);

    if (m_portIn)
        m_color = m_portIn->color();

    if (m_portOut)
        m_color = m_portOut->color();

    calculateRope();
    createPortsConnection();
}

QRectF RopeItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

QPainterPath RopeItem::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, m_size.width(), m_size.height());
    return path;
}

void RopeItem::paint(QPainter *p, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(item);

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    p->setRenderHints(QPainter::Antialiasing |
                      QPainter::SmoothPixmapTransform |
                      QPainter::HighQualityAntialiasing);

    pen.setBrush(Qt::black);
    pen.setWidth(4);
    p->setPen(pen);

    QPainterPath path;

    path.moveTo(m_pointIn);

    qreal flexion = distance(m_pointOut, m_pointIn) / 2;
    if (flexion > m_flexion) flexion = m_flexion;

    path.cubicTo(m_pointIn.x() - flexion, m_pointIn.y(),
                 m_pointOut.x() + flexion, m_pointOut.y(),
                 m_pointOut.x(), m_pointOut.y());

    p->drawPath(path);

    pen.setBrush(m_color);
    pen.setWidth(3);
    p->setPen(pen);
    p->drawPath(path);
}

void RopeItem::createPortsConnection()
{
    if (m_portIn && m_portOut && !m_isConnected)
    {
        connect(m_portIn,SIGNAL(positionChanged()),this,SLOT(calculateRope()));
        connect(m_portOut,SIGNAL(positionChanged()),this,SLOT(calculateRope()));
        connect(m_portOut,SIGNAL(valueChanged(QVariant)),m_portIn,SLOT(setValue(QVariant)));
        m_portIn->setValue(m_portOut->value());
        m_isConnected = true;
    }
}

void RopeItem::disconnectFromPorts()
{
    if (m_portIn && m_portOut && m_isConnected)
    {
        disconnect(m_portIn,SIGNAL(positionChanged()),this,SLOT(calculateRope()));
        disconnect(m_portOut,SIGNAL(positionChanged()),this,SLOT(calculateRope()));
        disconnect(m_portOut,SIGNAL(valueChanged(QVariant)),m_portIn,SLOT(setValue(QVariant)));
        m_portIn->setValue(QVariant());
        m_isConnected = false;
    }
}

void RopeItem::setColor(QColor color)
{
    m_color = color;
    update();
}

void RopeItem::setFlexion(qreal value)
{
    m_flexion = value;
    update();
}

void RopeItem::setPortOut(PortItem *port)
{
    m_portOut = port;
    calculateRope();
    createPortsConnection();
}

void RopeItem::removePortOut()
{
    disconnectFromPorts();
    m_portOut = Q_NULLPTR;
    calculateRope();
}

void RopeItem::setPortIn(PortItem *port)
{
    m_portIn = port;
    calculateRope();
    createPortsConnection();
}

void RopeItem::removePortIn()
{
    disconnectFromPorts();
    m_portIn = Q_NULLPTR;
    calculateRope();
}

void RopeItem::setMousePoint(QPointF point)
{
    m_mousePoint = point;
    calculateRope();
}

void RopeItem::calculateRope()
{
    QPointF portPosOut = m_mousePoint;
    QPointF portPosIn = m_mousePoint;

    if (m_portOut)
    {
        portPosOut = m_portOut->portPos();
        if (portPosIn == QPointF(0,0))
            portPosIn = portPosOut;
    }

    if (m_portIn)
    {
        portPosIn = m_portIn->portPos();
        if (portPosOut == QPointF(0,0))
            portPosOut = portPosIn;
    }

    qreal addition = 40;
    qreal x_pos = qMin(portPosOut.x(), portPosIn.x()) - addition;
    qreal y_pos = qMin(portPosOut.y(), portPosIn.y()) - addition;
    qreal w_size = qMax(portPosOut.x(), portPosIn.x()) - x_pos + addition;
    qreal h_size = qMax(portPosOut.y(), portPosIn.y()) - y_pos + addition;

    setPos(x_pos ,y_pos);
    m_size = QSize(w_size, h_size);

    qreal pointOutX = portPosOut.x() - x_pos;
    qreal pointOutY = portPosOut.y() - y_pos;

    qreal pointInX = portPosIn.x() - x_pos;
    qreal pointInY = portPosIn.y() - y_pos;

    m_pointOut = QPointF(pointOutX,pointOutY);
    m_pointIn = QPointF(pointInX,pointInY);

    prepareGeometryChange();
    update();
}
