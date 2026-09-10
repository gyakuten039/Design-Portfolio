#ifndef APPLE_H
#define APPLE_H

#include <QString>
#include <QRectF>
#include <QPointF>

class Apple
{
public:
    Apple();
    Apple(const QString& word, const QPointF& position);

    QString word() const;
    void setWord(const QString& word);

    QString typedWord() const;
    void setType(const QString& typed);

    QPointF position() const;
    void setPosition(const QPointF& position);

    QRectF boundingRect() const;

    bool isCompleted() const;
    bool isFailed() const;
    bool isOutOfBounds() const;

    void updatePosition(double speed);

    void reset(const QString& word, const QPointF& position);

private:
    QString word_;
    QString typedWord_;
    QPointF position_;
};

#endif // APPLE_H
