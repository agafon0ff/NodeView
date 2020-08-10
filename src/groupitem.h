#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QGraphicsItem>
#include <QFont>
#include <QBrush>
#include <QUuid>

#include "nodeitem.h"

class GroupItem : public QGraphicsObject
{
    Q_OBJECT
public:
    GroupItem(QGraphicsItem *parent = Q_NULLPTR);

private:

    QString m_title;
    QUuid m_uuid;
    QSizeF m_size;
    QSizeF m_indentSize;
    QSizeF m_titleSize;
    QFont m_font;
    QColor m_fontColor;
    QColor m_titleColor;
    QColor m_backgroundColor;
    QBrush m_backgroundBrush;
    bool m_isUserBrush;
    bool m_isSelected;
    bool m_isMove;

    QList<NodeItem*> m_nodeList;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

signals:
    void selected(bool state);
    void becameEmpty(GroupItem *group);

public slots:
    bool isHovered(const QPointF &point);

    void addNode(NodeItem *node);
    void setNodes(QList<NodeItem*> list);
    void removeNode(NodeItem *node);
    void clearNodes();

    QList<NodeItem*> nodeList() const {return m_nodeList;}

    void updateSize();

    QUuid uuid(){return m_uuid;}
    void setUuid(const QUuid &uuid){m_uuid = uuid;}

    QString title(){return m_title;}
    void setTitle(const QString &title);
    void clearTitle();

    void setFont(const QFont &font);
    void setFontColor(const QColor &color);
    void setTitleColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setBackgroundBrush(const QBrush &brush);
};

#endif // GROUPITEM_H
