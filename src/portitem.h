#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QUuid>

class PortItem : public QGraphicsObject
{
    Q_OBJECT
public:

    enum PortTypes
    {
        TypeIn,
        TypeOut
    };

    explicit PortItem(PortItem::PortTypes type, uint posY, uint num, QColor color, QGraphicsItem *parent = Q_NULLPTR);

private:

    uint m_posY;
    QSizeF m_size;
    QColor m_color;
    uint m_number;

    qreal m_ellipseRadius;
    bool m_isHovered;
    QPointF m_portPos;
    QVariant m_value;
    PortTypes m_type;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

protected:

signals:
    void positionChanged();
    void valueChanged(const QVariant &value);

public slots:
    QUuid parentUuid();

    QSizeF size(){return m_size;}
    void resize(qreal w, qreal h);

    uint posY(){return m_posY;}
    void setPosY(int posY);

    QColor color(){return m_color;}
    void setColor(const QColor &color);

    bool isHovered(const QPointF &point);

    void calculatePosition();
    QPointF portPos(){return m_portPos;}

    uint number(){return m_number;}
    void setNumber(uint num){m_number = num;}

    QVariant value(){return m_value;}
    void setValue(const QVariant &value);

    PortItem::PortTypes portType(){return m_type;}
    void setPortType(PortItem::PortTypes type){m_type = type;}
};

#endif // PORTITEM_H
