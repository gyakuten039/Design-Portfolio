#include "score_board.h"
#include <QString>
#include <QFont>

ScoreBoard::ScoreBoard(QWidget *parent)
    : QWidget(parent)
    , scoreLabel_(nullptr)
    , missedLabel_(nullptr)
    , comboLabel_(nullptr)
    , accuracyLabel_(nullptr)
    , timeLabel_(nullptr)
    , scoreValue_(nullptr)
    , missedValue_(nullptr)
    , comboValue_(nullptr)
    , accuracyValue_(nullptr)
    , timeValue_(nullptr)
{
    setupUI();
}

void ScoreBoard::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(20);

    QFrame *separator1 = new QFrame(this);
    separator1->setFrameShape(QFrame::VLine);
    separator1->setStyleSheet("color: #ccc;");
    mainLayout->addWidget(separator1);

    scoreLabel_ = new QLabel(tr("Score: "), this);
    scoreLabel_->setStyleSheet("QLabel { color: #333; font-size: 16px; font-weight: bold; }");
    mainLayout->addWidget(scoreLabel_);

    scoreValue_ = new QLabel("0", this);
    scoreValue_->setStyleSheet("QLabel { color: #4CAF50; font-size: 18px; font-weight: bold; }");
    mainLayout->addWidget(scoreValue_);

    QFrame *separator2 = new QFrame(this);
    separator2->setFrameShape(QFrame::VLine);
    separator2->setStyleSheet("color: #ccc;");
    mainLayout->addWidget(separator2);

    missedLabel_ = new QLabel(tr("Missed: "), this);
    missedLabel_->setStyleSheet("QLabel { color: #333; font-size: 16px; font-weight: bold; }");
    mainLayout->addWidget(missedLabel_);

    missedValue_ = new QLabel("0", this);
    missedValue_->setStyleSheet("QLabel { color: #f44336; font-size: 18px; font-weight: bold; }");
    mainLayout->addWidget(missedValue_);

    QFrame *separator3 = new QFrame(this);
    separator3->setFrameShape(QFrame::VLine);
    separator3->setStyleSheet("color: #ccc;");
    mainLayout->addWidget(separator3);

    comboLabel_ = new QLabel(tr("Combo: "), this);
    comboLabel_->setStyleSheet("QLabel { color: #333; font-size: 16px; font-weight: bold; }");
    mainLayout->addWidget(comboLabel_);

    comboValue_ = new QLabel("0", this);
    comboValue_->setStyleSheet("QLabel { color: #FF9800; font-size: 18px; font-weight: bold; }");
    mainLayout->addWidget(comboValue_);

    QFrame *separator4 = new QFrame(this);
    separator4->setFrameShape(QFrame::VLine);
    separator4->setStyleSheet("color: #ccc;");
    mainLayout->addWidget(separator4);

    accuracyLabel_ = new QLabel(tr("Accuracy: "), this);
    accuracyLabel_->setStyleSheet("QLabel { color: #333; font-size: 16px; font-weight: bold; }");
    mainLayout->addWidget(accuracyLabel_);

    accuracyValue_ = new QLabel("100%", this);
    accuracyValue_->setStyleSheet("QLabel { color: #2196F3; font-size: 18px; font-weight: bold; }");
    mainLayout->addWidget(accuracyValue_);

    QFrame *separator5 = new QFrame(this);
    separator5->setFrameShape(QFrame::VLine);
    separator5->setStyleSheet("color: #ccc;");
    mainLayout->addWidget(separator5);

    timeLabel_ = new QLabel(tr("Time: "), this);
    timeLabel_->setStyleSheet("QLabel { color: #333; font-size: 16px; font-weight: bold; }");
    mainLayout->addWidget(timeLabel_);

    timeValue_ = new QLabel("00:00", this);
    timeValue_->setStyleSheet("QLabel { color: #9C27B0; font-size: 18px; font-weight: bold; }");
    mainLayout->addWidget(timeValue_);
}

void ScoreBoard::updateScore(int score)
{
    scoreValue_->setText(QString::number(score));
}

void ScoreBoard::updateMissed(int missed)
{
    missedValue_->setText(QString::number(missed));
}

void ScoreBoard::updateCombo(int combo)
{
    comboValue_->setText(QString::number(combo));
}

void ScoreBoard::updateAccuracy(double accuracy)
{
    accuracyValue_->setText(QString("%1%").arg(accuracy, 0, 'f', 1));
}

void ScoreBoard::updateTime(int elapsed)
{
    int minutes = elapsed / 60;
    int seconds = elapsed % 60;
    timeValue_->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}

void ScoreBoard::onScoreChanged(int score)
{
    updateScore(score);
}

void ScoreBoard::onMissedChanged(int missed)
{
    updateMissed(missed);
}

void ScoreBoard::onComboChanged(int combo)
{
    updateCombo(combo);
}

void ScoreBoard::onAccuracyChanged(double accuracy)
{
    updateAccuracy(accuracy);
}

void ScoreBoard::onTimeChanged(int elapsed)
{
    updateTime(elapsed);
}
