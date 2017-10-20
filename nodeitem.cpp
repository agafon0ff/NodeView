#include "nodeitem.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsProxyWidget>

NodeItem::NodeItem(QWidget *widget, QGraphicsItem *parent) : QGraphicsObject(parent),
    m_widget(widget),
    m_uuid(QUuid::createUuid()),
    m_size(QSizeF(200,200)),
    m_indentSize(QSizeF(14,2)),
    m_titleSize(QSizeF(0,0)),
    m_font(QFont("Arial",10,QFont::Bold)),
    m_fontColor(QColor(Qt::black)),
    m_titleColor(QColor::fromRgb(120,120,120,210)),
    m_backgroundColor(QColor::fromRgb(190,190,190,210)),
    m_backgroundBrush(QBrush(m_backgroundColor)),
    m_isUserBrush(false),
    m_isSelected(false)
{
    widget->setAttribute(Qt::WA_NoSystemBackground);
    QGraphicsProxyWidget *proxyWidget = new QGraphicsProxyWidget(this);
    proxyWidget->setWidget(widget);

    setZValue(1);
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);

    updateSize();
}

QRectF NodeItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

QPainterPath NodeItem::shape() const
{
    QPainterPath path;
    path.addRect(m_indentSize.width(),m_indentSize.height(),m_size.width()-m_indentSize.width()*2,m_size.height()-m_indentSize.height()*2);
    return path;
}

void NodeItem::paint(QPainter *p, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);

    bool isSelected = false;

    if(item->state & QStyle::State_Selected)isSelected = true;

    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setBrush(QColor::fromRgb(0,0,0,0));
    p->setPen(pen);

    p->setRenderHint(QPainter::Antialiasing);
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::HighQualityAntialiasing);

    QLinearGradient gradient_0(0,m_size.height()/2,m_size.width(),m_size.height()/2);
    gradient_0.setColorAt(0,QColor::fromRgb(0,0,0,0));
    gradient_0.setColorAt(0.1,QColor::fromRgb(0,0,0,130));
    gradient_0.setColorAt(0.9,QColor::fromRgb(0,0,0,130));
    gradient_0.setColorAt(1,QColor::fromRgb(0,0,0,0));

    p->setBrush(gradient_0);
    p->drawRoundedRect(2,1,m_size.width()-4,m_size.height()-2,(qreal)m_size.width()/((qreal)m_size.width()/10.0),(qreal)m_size.height()/((qreal)m_size.height()/10.0));

    if(isSelected)
    {
        pen.setWidth(3);
        pen.setBrush(QColor::fromRgb(200,80,20,255));
        setZValue(2);
    }
    else
    {
        pen.setWidth(2);
        pen.setBrush(QColor::fromRgb(0,0,0,170));
        setZValue(1);
    }

    if(m_isSelected != isSelected)
    {
        m_isSelected = isSelected;
        emit selected(m_isSelected);
    }

    p->setPen(pen);

    if(m_isUserBrush)
        p->setBrush(m_backgroundBrush);
    else
    {
        qreal gradientHeight = ((m_titleSize.height()+m_indentSize.height())*100/m_size.height())*0.01;
        QLinearGradient gradient_1(m_size.width()/2,0,m_size.width()/2,m_size.height());
        gradient_1.setColorAt(0,m_backgroundColor);
        gradient_1.setColorAt(gradientHeight,m_titleColor);
        gradient_1.setColorAt(gradientHeight+0.01,m_backgroundColor);

        p->setBrush(gradient_1);
    }
    p->drawRoundedRect(6,1,m_size.width()-12,m_size.height()-2,(qreal)m_size.width()/((qreal)m_size.width()/10.0),(qreal)m_size.height()/((qreal)m_size.height()/10.0));

    p->setPen(QPen(m_fontColor,2));
    p->setFont(m_font);
    p->drawText(QRect(m_indentSize.width(),m_indentSize.height(),m_size.width()-m_indentSize.width()*2,m_titleSize.height()),Qt::AlignCenter,m_title);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mousePressEvent(e);
    update();
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseMoveEvent(e);
    update();
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseReleaseEvent(e);
    update();
}

QVariant NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionHasChanged)
    {

    }
    else if(change == ItemVisibleHasChanged)
    {

    }

    return QGraphicsItem::itemChange(change, value);
}

bool NodeItem::isHovered(QPointF point)
{
    bool result = false;

    QPointF s_pos = mapToScene(QPointF(-6,0));
    QPointF s_size = mapToScene(QPointF(m_size.width()+6,m_size.height()));

    if(point.x() >= s_pos.x() &&
       point.y() >= s_pos.y() &&
       point.x() <= s_size.x() &&
       point.y() <= s_size.y())
        result = true;

    return result;
}

PortItem *NodeItem::isHoveredPort(QPointF point)
{
    PortItem *result = NULL;

    foreach (PortItem *port, m_portList)
    {
        if(port->isHovered(point))
        {
            result = port;
            break;
        }
    }

    return result;
}

void NodeItem::updateSize()
{
    m_size = QSizeF(m_widget->width()+m_indentSize.width()*2,m_widget->height()+m_indentSize.height()*2+m_titleSize.height());
    m_widget->move(m_indentSize.width(),m_indentSize.height()+m_titleSize.height());

    foreach(PortItem *port,m_portList)
    {
        if(port->portType() == PortItem::TypeIn)
            port->setPos(0,(port->posY()+m_indentSize.height()+m_titleSize.height())-port->size().height()/2);
        else port->setPos(m_size.width()-port->size().width(),(port->posY()+m_indentSize.height()+m_titleSize.height())-port->size().height()/2);
    }

    update();
}

void NodeItem::addPortIn(PortItem *port)
{
    port->setParentItem(this);
    m_portList.append(port);

    updateSize();
}

PortItem *NodeItem::createPortIn(uint posY, QColor color)
{
    PortItem *port = new PortItem(PortItem::TypeIn,posY,m_portList.count(),color,this);
    m_portList.append(port);

    updateSize();

    return port;
}

PortItem *NodeItem::createPortIn(uint posY, QColor color, uint num)
{
    PortItem *port = new PortItem(PortItem::TypeIn,posY,m_portList.count(),color,this);
    port->setNumber(num);
    m_portList.append(port);

    updateSize();

    return port;
}

void NodeItem::addPortOut(PortItem *port)
{
    port->setParentItem(this);
    m_portList.append(port);

    updateSize();
}

PortItem *NodeItem::createPortOut(uint posY, QColor color)
{
    PortItem *port = new PortItem(PortItem::TypeOut,posY,m_portList.count(),color,this);
    m_portList.append(port);

    updateSize();

    return port;
}

PortItem *NodeItem::createPortOut(uint posY, QColor color, uint num)
{
    PortItem *port = new PortItem(PortItem::TypeOut,posY,m_portList.count(),color,this);
    port->setNumber(num);
    m_portList.append(port);

    updateSize();

    return port;
}

PortItem *NodeItem::portAt(uint num)
{
    PortItem *result = NULL;

    foreach (PortItem *port, m_portList)
    {
        if(port->number() == num)
        {
            result = port;
            break;
        }
    }

    return result;
}

void NodeItem::removePortAt(uint num)
{
    removePort(portAt(num));
}
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "nodeview.h"
void NodeItem::removePort(PortItem *port)
{
    if(!port)
        return;

    if(!m_portList.contains(port))
        return;

    if(scene()->views().count() > 0)
    {
        if(scene()->views().at(0)->inherits("NodeView"))
        {
            NodeView *nView = (NodeView*)scene()->views().at(0);
            nView->removePortConnections(port);

            m_portList.removeOne(port);
            scene()->removeItem(port);
        }
    }
}

void NodeItem::setTitle(QString title)
{
    m_title = title;

    QFontMetrics fm(m_font);
    m_titleSize = QSizeF(fm.width(m_title),fm.height());

    updateSize();
}

void NodeItem::clearTitle()
{
    m_title.clear();
    m_titleSize = QSizeF(0,0);
    updateSize();
}

void NodeItem::setFont(QFont font)
{
    m_font = font;
    update();
}

void NodeItem::setFontColor(QColor color)
{
    m_fontColor = color;
    update();
}

void NodeItem::setTitleColor(QColor color)
{
    m_titleColor = color;
    update();
}

void NodeItem::setBackgroundColor(QColor color)
{
    m_backgroundColor = color;
    update();
}

void NodeItem::setBackgroundBrush(QBrush brush)
{
    m_isUserBrush = true;
    m_backgroundBrush = brush;
    update();
}
