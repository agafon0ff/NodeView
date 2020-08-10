#include "groupitem.h"
#include "nodeitem.h"

#include <QtMath>
#include <QDebug>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

GroupItem::GroupItem(QGraphicsItem *parent) : QGraphicsObject(parent),
    m_uuid(QUuid::createUuid()),
    m_size(QSizeF(200,200)),
    m_indentSize(QSizeF(10,10)),
    m_titleSize(QSizeF(0,0)),
    m_font(QFont("Calibri",10,QFont::Medium)),
    m_fontColor(QColor(Qt::black)),
    m_titleColor(QColor::fromRgb(207, 244, 241, 150)),
    m_backgroundColor(QColor::fromRgb(207, 244, 241, 100)),
    m_backgroundBrush(QBrush(m_backgroundColor)),
    m_isUserBrush(false),
    m_isSelected(false),
    m_isMove(false)
{
    setZValue(-1);
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);

    updateSize();
}

QRectF GroupItem::boundingRect() const
{
    return QRectF(-m_indentSize.width(), -m_indentSize.height(),
                  m_size.width() + m_indentSize.width() * 2,
                  m_size.height() + m_indentSize.height() * 2);
}

QPainterPath GroupItem::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, m_size.width(), m_size.height());

    return path;
}

void GroupItem::paint(QPainter *p, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);

    bool isSelected = false;

    if (item->state & QStyle::State_Selected) isSelected = true;

    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setBrush(QColor::fromRgb(0,0,0,0));
    p->setPen(pen);

    p->setRenderHints(QPainter::Antialiasing |
                      QPainter::SmoothPixmapTransform |
                      QPainter::HighQualityAntialiasing);

    QLinearGradient gradient_0(0,m_size.height()/2,m_size.width(),m_size.height()/2);
    gradient_0.setColorAt(0,QColor::fromRgb(0,0,0,0));
    gradient_0.setColorAt(0.1,QColor::fromRgb(0,0,0,130));
    gradient_0.setColorAt(0.9,QColor::fromRgb(0,0,0,130));
    gradient_0.setColorAt(1,QColor::fromRgb(0,0,0,0));

    p->setBrush(gradient_0);
    p->drawRoundedRect(2,1,m_size.width()-4,m_size.height()-2,
                       (qreal)m_size.width()/((qreal)m_size.width()/5.0),
                       (qreal)m_size.height()/((qreal)m_size.height()/5.0));

    if (isSelected)
    {
        pen.setWidth(2);
        pen.setBrush(QColor::fromRgb(200,80,20,255));
    }
    else
    {
        pen.setWidth(1);
        pen.setBrush(QColor::fromRgb(0,0,0,170));
    }

    if (m_isSelected != isSelected)
    {
        m_isSelected = isSelected;
        emit selected(m_isSelected);
    }

    p->setPen(pen);

    if (m_isUserBrush)
        p->setBrush(m_backgroundBrush);
    else
    {
        qreal gradientHeight = ((m_titleSize.height()) * 100 / m_size.height()) * 0.01;
        QLinearGradient gradient_1(m_size.width() / 2, 0, m_size.width() / 2, m_size.height());
        gradient_1.setColorAt(0, m_backgroundColor);
        gradient_1.setColorAt(gradientHeight, m_titleColor);
        gradient_1.setColorAt(gradientHeight + 0.01, m_backgroundColor);

        p->setBrush(gradient_1);
    }
    p->drawRoundedRect(6, 1, m_size.width() - 12, m_size.height() - 2,
                       (qreal)m_size.width() / ((qreal)m_size.width() / 5.0),
                       (qreal)m_size.height() / ((qreal)m_size.height() / 5.0));

    p->setPen(QPen(m_fontColor,2));
    p->setFont(m_font);
    p->drawText(QRect(0, 0,m_size.width(),m_titleSize.height()), Qt::AlignCenter, m_title);
}


void GroupItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mousePressEvent(e);

    foreach (NodeItem *node, m_nodeList)
        node->setSelected(true);

    update();
}

void GroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    m_isMove = true;
    QGraphicsItem::mouseMoveEvent(e);
    update();
}

void GroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    m_isMove = false;
    QGraphicsItem::mouseReleaseEvent(e);

    foreach (NodeItem *node, m_nodeList)
        node->setSelected(false);

    update();
}

bool GroupItem::isHovered(const QPointF &point)
{
    bool result = false;

    QPointF s_pos = mapToScene(QPointF(-6,0));
    QPointF s_size = mapToScene(QPointF(m_size.width()+6,m_size.height()));

    if (point.x() >= s_pos.x() &&
       point.y() >= s_pos.y() &&
       point.x() <= s_size.x() &&
       point.y() <= s_size.y())
        result = true;

    return result;
}

void GroupItem::addNode(NodeItem *node)
{
    if (m_nodeList.contains(node)) return;

    connect(node, &NodeItem::positionChanged, this, &GroupItem::updateSize);

    m_nodeList.append(node);
    updateSize();
}

void GroupItem::setNodes(QList<NodeItem*> list)
{
    m_nodeList.clear();
    m_nodeList = list;

    foreach (NodeItem *node, m_nodeList)
        connect(node, &NodeItem::positionChanged, this, &GroupItem::updateSize);

    updateSize();
}

void GroupItem::removeNode(NodeItem *node)
{
    if (!m_nodeList.contains(node)) return;

    m_nodeList.removeOne(node);
    updateSize();

    if(m_nodeList.isEmpty())
        emit becameEmpty(this);
}

void GroupItem::clearNodes()
{
    m_nodeList.clear();
    updateSize();
}

void GroupItem::updateSize()
{
    if (m_isMove) return;
    if (m_nodeList.count() < 1) return;

    NodeItem *firstNode = m_nodeList.at(0);
    qreal left = firstNode->x();
    qreal top = firstNode->y();
    qreal right = firstNode->x() + firstNode->width();
    qreal bottom = firstNode->y() + firstNode->height();

    foreach (NodeItem *node, m_nodeList)
    {
        if (node->x() < left) left = node->x();
        if (node->y() < top) top = node->y();

        if(node->x() + node->width() > right)
            right = node->x() + node->width();

        if(node->y() + node->height() > bottom)
            bottom = node->y() + node->height();
    }

    setPos(left - m_indentSize.width(),
           top - m_indentSize.height() - m_titleSize.height());

    qreal width = qAbs(right - left);
    qreal height = qAbs(bottom - top);
    m_size = QSizeF(width + m_indentSize.width() * 2,
                    height + m_indentSize.height() * 2 + m_titleSize.height());

    prepareGeometryChange();
    update();
}

void GroupItem::setTitle(const QString &title)
{
    m_title = title;

    QFontMetrics fm(m_font);
#if QT_VERSION > 0x050906
    m_titleSize = QSizeF(fm.horizontalAdvance(m_title),fm.height() + 2);
#else
    m_titleSize = QSizeF(fm.width(m_title),fm.height() + 2);
#endif

    updateSize();
}

void GroupItem::clearTitle()
{
    m_title.clear();
    m_titleSize = QSizeF(0,0);
    updateSize();
}

void GroupItem::setFont(const QFont &font)
{
    m_font = font;
    update();
}

void GroupItem::setFontColor(const QColor &color)
{
    m_fontColor = color;
    update();
}

void GroupItem::setTitleColor(const QColor &color)
{
    m_titleColor = color;
    update();
}

void GroupItem::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

void GroupItem::setBackgroundBrush(const QBrush &brush)
{
    m_isUserBrush = true;
    m_backgroundBrush = brush;
    update();
}
