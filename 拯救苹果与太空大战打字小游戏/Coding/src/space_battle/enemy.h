#ifndef ENEMY_H
#define ENEMY_H

#include <QString>
#include <QRectF>
#include <QPointF>

class Enemy
{
public:
    Enemy();
    Enemy(const QString& word, const QPointF& position);

    QString word() const;
    void setWord(const QString& word);

    QString typedWord() const;
    void setType(const QString& typed);

    QPointF position() const;
    void setPosition(const QPointF& position);

    QRectF boundingRect() const;

    bool isCompleted() const;
    bool isWaitingForBullet() const;
    void setWaitingForBullet(bool waiting);
    bool isOutOfBounds() const;

    void updatePosition(double speed);

    void reset(const QString& word, const QPointF& position);

private:
    QString word_;
    QString typedWord_;
    QPointF position_;
    bool waitingForBullet_;
};

#endif // ENEMY_H
