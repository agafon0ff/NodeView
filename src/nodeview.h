#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>
#include "nodeitem.h"
#include "ropeitem.h"

class NodeView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NodeView(QWidget *parent = Q_NULLPTR);
    ~NodeView();

private:
    QGraphicsScene *m_scene;
    QPointF m_scenePos;
    QPointF m_pressPos;
    bool m_moveScene;

    RopeItem *m_activeRope;
    QList<NodeItem*> m_nodeList;
    QList<RopeItem*> m_ropeList;

    QList<NodeItem*> m_selectedNodes;

    bool m_isCheckingColor;
    bool m_isOnlyOneInputConnection;

    void wheelEvent(QWheelEvent *event);
    void drawBackground(QPainter *painter, const QRectF &r);
    void drawGrid(QPainter *painter, double gridStep);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void checkRopeCreation(const QPointF &point);
    void checkRopeConnection(const QPointF &point);
    void checkNodesSelected();

    NodeItem *checkNodeHit(const QPointF &point);
    PortItem *checkPortHit(const QPointF &point);

    void createRopeHitPort(PortItem *port);
    void createConnectionToPort(PortItem *port);

    void removeActiveRope();

    bool isPortFree(PortItem *port);
    RopeItem *getRopeWithPort(PortItem *port);

signals:
    void selectionsChanged();
    void calledMenuNode(NodeItem *node);
    void calledMenuView();

public slots:
    void addNode(NodeItem *node);
    NodeItem *createNode(QWidget *widget);
    void removeNode(NodeItem *node);
    NodeItem *nodeAt(const QUuid &uuid);

    bool createConnection(PortItem *portOut, PortItem *portIn);
    void removePortConnections(PortItem *port);

    QList<NodeItem*> nodeList(){return m_nodeList;}
    QList<RopeItem*> ropeList(){return m_ropeList;}
    QList<NodeItem*> selectedNodeList(){return m_selectedNodes;}

    void setCheckingColor(bool state = true);
    void setOnlyOneInputConnection(bool state = true);
};

#endif // NODEVIEW_H