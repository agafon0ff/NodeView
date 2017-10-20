#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
#include <QFont>
#include <QBrush>
#include <QUuid>
#include "portitem.h"

class NodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeItem(QWidget *widget, QGraphicsItem *parent = 0);

private:

    QWidget *m_widget;
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

    QList<PortItem*> m_portList;
    QVariantMap m_parameters;
    QString m_title;


    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
    void selected(bool state);

public slots:
    bool isHovered(QPointF point);
    PortItem *isHoveredPort(QPointF point);

    void updateSize();

    QWidget *widget(){return m_widget;}

    void addPortIn(PortItem *port);
    PortItem *createPortIn(uint posY, QColor color);
    PortItem *createPortIn(uint posY, QColor color, uint num);

    void addPortOut(PortItem *port);
    PortItem *createPortOut(uint posY, QColor color);
    PortItem *createPortOut(uint posY, QColor color, uint num);

    QList<PortItem*> portList(){return m_portList;}
    PortItem* portAt(uint num);

    void removePortAt(uint num);
    void removePort(PortItem *port);

    QUuid uuid(){return m_uuid;}
    void setUuid(QUuid uuid){m_uuid = uuid;}

    QString title(){return m_title;}
    void setTitle(QString title);
    void clearTitle();

    void setFont(QFont font);
    void setFontColor(QColor color);
    void setTitleColor(QColor color);
    void setBackgroundColor(QColor color);
    void setBackgroundBrush(QBrush brush);

    const QVariantMap &parameters(){return m_parameters;}
    void setParameters(QVariantMap parameters){m_parameters = parameters;}
    void setParameter(QString key, QVariant value){m_parameters.insert(key,value);}
};

#endif // NODEITEM_H
