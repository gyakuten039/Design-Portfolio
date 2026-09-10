#include "enemy.h"

Enemy::Enemy()
    : typedWord_()
    , position_(0, 0)
    , waitingForBullet_(false)
{
}

Enemy::Enemy(const QString& word, const QPointF& position)
    : word_(word)
    , typedWord_()
    , position_(position)
    , waitingForBullet_(false)
{
}

QString Enemy::word() const
{
    return word_;
}

void Enemy::setWord(const QString& word)
{
    word_ = word;
}

QString Enemy::typedWord() const
{
    return typedWord_;
}

void Enemy::setType(const QString& typed)
{
    typedWord_ = typed;
}

QPointF Enemy::position() const
{
    return position_;
}

void Enemy::setPosition(const QPointF& position)
{
    position_ = position;
}

QRectF Enemy::boundingRect() const
{
    return QRectF(position_.x() - 40, position_.y() - 30, 80, 60);
}

bool Enemy::isCompleted() const
{
    return typedWord_ == word_;
}

bool Enemy::isWaitingForBullet() const
{
    return waitingForBullet_;
}

void Enemy::setWaitingForBullet(bool waiting)
{
    waitingForBullet_ = waiting;
}

bool Enemy::isOutOfBounds() const
{
    return position_.y() > 700;
}

void Enemy::updatePosition(double speed)
{
    position_.ry() += speed;
}

void Enemy::reset(const QString& word, const QPointF& position)
{
    word_ = word;
    typedWord_.clear();
    position_ = position;
}
