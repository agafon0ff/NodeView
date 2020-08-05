#include "portitem.h"
#include "nodeitem.h"
#include <QPainter>
#include <QDebug>

PortItem::PortItem(PortItem::PortTypes type, int posY, uint num, QColor color, QGraphicsItem *parent) : QGraphicsObject(parent),
    m_posY(posY),
    m_size(QSize(12,12)),
    m_color(color),
    m_number(num),
    m_ellipseRadius(5),
    m_isHovered(false),
    m_portPos(mapToScene(QPointF(0,0))),
    m_type(type)
{
    setAcceptHoverEvents(true);
    setFlags(ItemIsSelectable);
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

    if (m_isHovered)
        p->setPen(QPen(m_color,1));
    else p->setPen(QPen(QColor::fromRgb(0,0,0,100),1));

    QPointF portPos = QPointF(m_ellipseRadius,m_size.height()/2);
    p->drawEllipse(portPos,m_ellipseRadius,m_ellipseRadius);
    calculatePosition();
}

QUuid PortItem::parentUuid()
{
    QUuid result;

    NodeItem *parentNode = Q_NULLPTR;

    if (parentItem())
        parentNode = (NodeItem*)parentItem();

    if (parentNode)
       result = parentNode->uuid();

    return result;
}

void PortItem::resize(qreal w, qreal h)
{
    m_size = QSize(w,h);

    prepareGeometryChange();
    update();
}

void PortItem::setPosY(int posY)
{
    m_posY = posY;
    setPos(pos().x(),(qreal)posY);
    update();
}

void PortItem::setColor(const QColor &color)
{
    m_color = color;
    update();
}

bool PortItem::isHovered(const QPointF &point)
{
    bool result = false;

    QPointF s_pos = mapToScene(QPointF(0,0));
    QPointF s_size = mapToScene(QPointF(m_size.width(),m_size.height()));

    if (point.x() >= s_pos.x() &&
       point.y() >= s_pos.y() &&
       point.x() <= s_size.x() &&
       point.y() <= s_size.y())
        result = true;

    if (m_isHovered != result)
    {
        m_isHovered = result;
        update();
    }

    return m_isHovered;
}

void PortItem::calculatePosition()
{
    QPointF portPos = QPointF(m_ellipseRadius, m_size.height()/2);
    QPointF mapPortPos = mapToScene(portPos);

    if (mapPortPos != m_portPos)
    {
        m_portPos = mapPortPos;
        emit positionChanged();
    }
}

void PortItem::setValue(const QVariant &value)
{
    m_value = value;
    emit valueChanged(value);
}
