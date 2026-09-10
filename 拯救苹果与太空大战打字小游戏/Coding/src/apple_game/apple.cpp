#include "apple.h"

Apple::Apple()
    : typedWord_()
    , position_(0, 0)
{
}

Apple::Apple(const QString& word, const QPointF& position)
    : word_(word)
    , typedWord_()
    , position_(position)
{
}

QString Apple::word() const
{
    return word_;
}

void Apple::setWord(const QString& word)
{
    word_ = word;
}

QString Apple::typedWord() const
{
    return typedWord_;
}

void Apple::setType(const QString& typed)
{
    typedWord_ = typed;
}

QPointF Apple::position() const
{
    return position_;
}

void Apple::setPosition(const QPointF& position)
{
    position_ = position;
}

QRectF Apple::boundingRect() const
{
    return QRectF(position_.x() - 40, position_.y() - 30, 80, 60);
}

bool Apple::isCompleted() const
{
    return typedWord_ == word_;
}

bool Apple::isFailed() const
{
    return !word_.startsWith(typedWord_);
}

bool Apple::isOutOfBounds() const
{
    return position_.y() > 700;
}

void Apple::updatePosition(double speed)
{
    position_.ry() += speed;
}

void Apple::reset(const QString& word, const QPointF& position)
{
    word_ = word;
    typedWord_.clear();
    position_ = position;
}
