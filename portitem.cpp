#include "portitem.h"
#include "nodeitem.h"
#include <QPainter>
#include <QDebug>

PortItem::PortItem(PortItem::PortTypes type, uint posY, uint num, QColor color, QGraphicsItem *parent) : QGraphicsObject(parent),
    m_posY(posY),
    m_size(QSize(12,12)),
    m_color(color),
    m_number(num),
    m_ellipseRadius(6),
    m_isHovered(false),
    m_portPos(mapToScene(QPointF(0,0))),
    m_type(type)
{
    setAcceptHoverEvents(true);
}

QRectF PortItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

QPainterPath PortItem::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, m_size.width(), m_size.height());
    return path;
}

void PortItem::paint(QPainter *p, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(item);

    p->setRenderHint(QPainter::Antialiasing);

    p->setBrush(QBrush(m_color));

    QPointF portPos = QPointF(m_ellipseRadius,m_size.height()/2);
    QPointF mapPortPos = mapToScene(portPos);

    if(mapPortPos != m_portPos)
    {
        m_portPos = mapPortPos;
        emit positionChanged();
    }

    if(m_isHovered)
        p->setPen(QPen(m_color,1));
    else p->setPen(QPen(QColor::fromRgb(0,0,0,100),1));

    p->drawEllipse(portPos,m_ellipseRadius,m_ellipseRadius);
}

QUuid PortItem::parentUuid()
{
    QUuid result;

    NodeItem *parentNode = NULL;

    if(parentItem())
        parentNode = (NodeItem*)parentItem();

    if(parentNode)
       result = parentNode->uuid();

    return result;
}

void PortItem::resize(qreal w, qreal h)
{
    m_size = QSize(w,h);
    update();
}

void PortItem::setPosY(int posY)
{
    m_posY = posY;
    setPos(pos().x(),(qreal)posY);
    update();
}

void PortItem::setColor(QColor color)
{
    m_color = color;
    update();
}

bool PortItem::isHovered(QPointF point)
{
    bool result = false;

    QPointF s_pos = mapToScene(QPointF(0,0));
    QPointF s_size = mapToScene(QPointF(m_size.width(),m_size.height()));

    if(point.x() >= s_pos.x() &&
       point.y() >= s_pos.y() &&
       point.x() <= s_size.x() &&
       point.y() <= s_size.y())
        result = true;

    if(m_isHovered != result)
    {
        m_isHovered = result;
        update();
    }

    return m_isHovered;
}

void PortItem::setValue(QVariant value)
{
    m_value = value;
    emit valueChanged(value);
}
