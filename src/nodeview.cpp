#include "nodeview.h"
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QDebug>
#include <QScrollBar>
#include <QtMath>
#include <QGraphicsProxyWidget>

NodeView::NodeView(QWidget *parent) : QGraphicsView(parent),
    m_scene(new QGraphicsScene),
    m_scenePos(QPointF(0,0)),
    m_pressPos(QPointF(0,0)),
    m_moveScene(false),
    m_currentScale(1.0),
    m_activeRope(0),
    m_isCheckingColor(true),
    m_isOnlyOneInputConnection(true),
    m_ropeFlexion(100.0)
{
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(QBrush(QColor::fromRgb(80,80,80)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setCacheMode(QGraphicsView::CacheBackground);

    m_scene->setSceneRect(m_scene->itemsBoundingRect());
    setScene(m_scene);
}

NodeView::~NodeView()
{
    delete m_scene;
}

void NodeView::wheelEvent(QWheelEvent *e)
{
    qreal scaleFactor = 1.15;

    if (e->delta() > 0)
    {
        if(m_currentScale < scaleFactor)
        {
            scale(scaleFactor,scaleFactor);
            m_currentScale *= scaleFactor;
        }
    }
    else if(m_currentScale > 0.1)
    {
        scale(1/scaleFactor, 1/scaleFactor);
        m_currentScale /= scaleFactor;
    }

    update();
}

void NodeView::drawBackground(QPainter *painter, const QRectF &r)
{
    QGraphicsView::drawBackground(painter, r);

    QPen pfine(QColor::fromRgb(50,50,50), 0.6);

    painter->setPen(pfine);
    drawGrid(painter,15);

    QPen p(QColor::fromRgb(50,50,50), 2.0);

    painter->setPen(p);
    drawGrid(painter,150);
}

void NodeView::drawGrid(QPainter *painter, double gridStep)
{
    QRect   windowRect = rect();
    QPointF tl = mapToScene(windowRect.topLeft());
    QPointF br = mapToScene(windowRect.bottomRight());

    double left   = qFloor(tl.x() / gridStep - 0.5);
    double right  = qFloor(br.x() / gridStep + 1.0);
    double bottom = qFloor(tl.y() / gridStep - 0.5);
    double top    = qFloor(br.y() / gridStep + 1.0);

    for (int xi = int(left); xi <= int(right); ++xi)
    {
      QLineF line(xi * gridStep, bottom * gridStep,
                  xi * gridStep, top * gridStep );

      painter->drawLine(line);
    }

    for (int yi = int(bottom); yi <= int(top); ++yi)
    {
      QLineF line(left * gridStep, yi * gridStep,
                  right * gridStep, yi * gridStep );
      painter->drawLine(line);
    }
}

void NodeView::mousePressEvent(QMouseEvent *e)
{
    m_scenePos = mapToScene(e->pos());
    m_pressPos = m_scenePos;

    if (e->button() == Qt::MiddleButton)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setInteractive(false);

        QMouseEvent fake(e->type(), e->pos(), Qt::LeftButton, Qt::LeftButton, e->modifiers());
        e = &fake;

        m_moveScene = true;
    }
    else if (e->button() == Qt::LeftButton)
    {
        setDragMode(QGraphicsView::RubberBandDrag);

        checkRopeCreation(mapToScene(e->pos()));
    }
    else if (e->button() == Qt::RightButton)
    {
        NodeItem *node = checkNodeHit(mapToScene(e->pos()));

        if (node)
           emit calledMenuNode(node);
        else emit calledMenuView();
    }

    QGraphicsView::mousePressEvent(e);
    update();
}

void NodeView::mouseMoveEvent(QMouseEvent *e)
{
    m_scenePos = mapToScene(e->pos());
    PortItem *hitPort = checkPortHit(m_scenePos);

    if (m_activeRope)
    {
        if (hitPort)
            m_activeRope->setMousePoint(hitPort->portPos());
        else m_activeRope->setMousePoint(m_scenePos);
    }

    if (m_moveScene)
    {
        QPointF difference = m_pressPos - m_scenePos;
        setSceneRect(sceneRect().translated(difference.x(), difference.y()));
    }

    update();
    QGraphicsView::mouseMoveEvent(e);
}

void NodeView::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        checkNodesSelected();
        checkRopeConnection(mapToScene(e->pos()));
    }
    else if (e->button() == Qt::MiddleButton)
    {
        setDragMode(QGraphicsView::NoDrag);
        setInteractive(true);

        QMouseEvent fake(e->type(), e->pos(), Qt::LeftButton, Qt::LeftButton, e->modifiers());
        e = &fake;
    }

    m_moveScene = false;
    QGraphicsView::mouseReleaseEvent(e);
    update();
}

void NodeView::checkRopeCreation(const QPointF &point)
{
    NodeItem *hitNode = checkNodeHit(point);

    if (!hitNode)
        return;

    PortItem *hitPort = hitNode->isHoveredPort(point);

    if (!hitPort)
        return;

    createRopeHitPort(hitPort);
}

void NodeView::checkRopeConnection(const QPointF &point)
{
    if (!m_activeRope)
        return;

    NodeItem *hitNode = checkNodeHit(point);

    if (!hitNode)
    {
        removeActiveRope();
        return;
    }

    PortItem *port = hitNode->isHoveredPort(point);

    if (port)
        createConnectionToPort(port);
    else removeActiveRope();
}

void NodeView::checkNodesSelected()
{
    QList<NodeItem*> selectedNodeList;

    foreach (NodeItem *node, m_nodeList)
        if (node->isSelected())
            selectedNodeList.append(node);

    if (m_selectedNodes != selectedNodeList)
    {
        m_selectedNodes = selectedNodeList;
        emit selectionsChanged();
    }
}

NodeItem *NodeView::checkNodeHit(const QPointF &point)
{
    NodeItem *hitNode = Q_NULLPTR;

    foreach (NodeItem *node, m_nodeList)
        if (node->isHovered(point))
        {
            hitNode = node;
            break;
        }

    return hitNode;
}

PortItem *NodeView::checkPortHit(const QPointF &point)
{
    PortItem *hitPort = Q_NULLPTR;

    NodeItem *hitNode = checkNodeHit(point);

    if (hitNode)
        hitPort = hitNode->isHoveredPort(point);

    return hitPort;
}

void NodeView::createRopeHitPort(PortItem *port)
{
    if (!port)
        return;

    setDragMode(QGraphicsView::NoDrag);

    if (port->portType() == PortItem::TypeIn)
    {
        if (isPortFree(port))
        {
            m_activeRope = new RopeItem(0,port);
            m_activeRope->setFlexion(m_ropeFlexion);
            m_scene->addItem(m_activeRope);
            m_ropeList.append(m_activeRope);
        }
        else
        {
            m_activeRope = getRopeWithPort(port);
            m_activeRope->removePortIn();
            m_activeRope->setMousePoint(m_scenePos);
        }
    }
    else
    {
        m_activeRope = new RopeItem(port,0);
        m_activeRope->setFlexion(m_ropeFlexion);
        m_scene->addItem(m_activeRope);
        m_ropeList.append(m_activeRope);
    }
}

void NodeView::createConnectionToPort(PortItem *port)
{
    if (!m_activeRope)
        return;

    bool colorPermission = false;
    bool severalConnectionPermission = false;

    if (m_isCheckingColor && m_activeRope->color() == port->color())
        colorPermission = true;
    else if (!m_isCheckingColor)
        colorPermission = true;

    if (m_isOnlyOneInputConnection && isPortFree(port))
        severalConnectionPermission = true;
    else if (!m_isOnlyOneInputConnection)
        severalConnectionPermission = true;

    if (m_activeRope->portIn() &&
       !m_activeRope->portOut() &&
       port->portType() == PortItem::TypeOut &&
       colorPermission)
    {
        m_activeRope->setPortOut(port);
    }

    if (m_activeRope->portOut() &&
       !m_activeRope->portIn() &&
       port->portType() == PortItem::TypeIn &&
       colorPermission &&
       severalConnectionPermission)
    {
        m_activeRope->setPortIn(port);
    }

    if (m_activeRope->isConnected())
        m_activeRope = Q_NULLPTR;
    else removeActiveRope();
}

void NodeView::removeActiveRope()
{
    if (!m_activeRope)
        return;

    m_activeRope->removePortIn();
    m_activeRope->removePortOut();

    m_ropeList.removeOne(m_activeRope);
    m_scene->removeItem(m_activeRope);
    m_activeRope = Q_NULLPTR;
}

RopeItem *NodeView::getRopeWithPort(PortItem *port)
{
    RopeItem *result = Q_NULLPTR;

    foreach (RopeItem *rope, m_ropeList)
    {
        if (rope->portIn() == port)
        {
            result = rope;
            break;
        }
        else if (rope->portOut() == port)
        {
            result = rope;
            break;
        }
    }

    return result;
}

void NodeView::addNode(NodeItem *node)
{
    node->setPos(m_scenePos);
    m_scene->addItem(node);
    m_nodeList.append(node);
}

NodeItem *NodeView::createNode(QWidget *widget)
{
    NodeItem *node = new NodeItem(widget);
    addNode(node);

    return node;
}

void NodeView::removeNode(NodeItem *node)
{
    foreach (PortItem *port, node->portList())
    {
        QList<RopeItem*> repesWithNode;

        foreach (RopeItem *rope, m_ropeList)
        {
            if (rope->portIn() == port)
                repesWithNode.append(rope);
            else if (rope->portOut() == port)
                repesWithNode.append(rope);
        }

        foreach (RopeItem *rope, repesWithNode)
        {
            rope->removePortIn();
            rope->removePortOut();

            m_ropeList.removeOne(rope);
            m_scene->removeItem(rope);
        }
    }

    foreach(GroupItem *group, m_groupList)
        group->removeNode(node);

    m_nodeList.removeOne(node);
    m_scene->removeItem(node);
}

NodeItem *NodeView::nodeAt(const QUuid &uuid)
{
    NodeItem *result = Q_NULLPTR;

    foreach (NodeItem *node, m_nodeList)
    {
        if (node->uuid() == uuid)
        {
            result = node;
            break;
        }
    }

    return result;
}

void NodeView::addGroup(GroupItem *group)
{
    group->setPos(m_scenePos);
    m_scene->addItem(group);
    m_groupList.append(group);
}

GroupItem *NodeView::createGroup(QList<NodeItem *> list)
{
    Q_UNUSED(list);

    GroupItem *group = new GroupItem;
    group->setNodes(list);
    addGroup(group);

    return group;
}

bool NodeView::createConnection(PortItem *portOut, PortItem *portIn)
{
    bool result = false;

    if (isPortFree(portIn))
    {
        RopeItem *rope = new RopeItem(portOut,portIn);
        rope->setFlexion(m_ropeFlexion);
        m_scene->addItem(rope);
        m_ropeList.append(rope);
        result = true;
    }

    return result;
}

void NodeView::removePortConnections(PortItem *port)
{
    QList<RopeItem*> ropesToRemove;
    foreach (RopeItem *rope, m_ropeList)
        if (rope->portIn() == port || rope->portOut() == port)
            ropesToRemove.append(rope);

    foreach (RopeItem *rope, ropesToRemove)
    {
        m_ropeList.removeOne(rope);
        m_scene->removeItem(rope);
    }
}

bool NodeView::isPortFree(PortItem *port)
{
    bool result = true;
    foreach (RopeItem *rope, m_ropeList)
    {
        if (rope->portIn() == port || rope->portOut() == port)
        {
            result = false;
            break;
        }
    }
    return result;
}

void NodeView::setCheckingColor(bool state)
{
    m_isCheckingColor = state;
    update();
}

void NodeView::setOnlyOneInputConnection(bool state)
{
    m_isOnlyOneInputConnection = state;
    update();
}

void NodeView::setRopeFlexion(qreal value)
{
    m_ropeFlexion = value;
    update();
}

