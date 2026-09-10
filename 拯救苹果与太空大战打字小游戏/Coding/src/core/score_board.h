#ifndef SCORE_BOARD_H
#define SCORE_BOARD_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>

class ScoreBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreBoard(QWidget *parent = nullptr);

    void updateScore(int score);
    void updateMissed(int missed);
    void updateCombo(int combo);
    void updateAccuracy(double accuracy);
    void updateTime(int elapsed);

public slots:
    void onScoreChanged(int score);
    void onMissedChanged(int missed);
    void onComboChanged(int combo);
    void onAccuracyChanged(double accuracy);
    void onTimeChanged(int elapsed);

private:
    void setupUI();

    QLabel *scoreLabel_;
    QLabel *missedLabel_;
    QLabel *comboLabel_;
    QLabel *accuracyLabel_;
    QLabel *timeLabel_;
    QLabel *scoreValue_;
    QLabel *missedValue_;
    QLabel *comboValue_;
    QLabel *accuracyValue_;
    QLabel *timeValue_;
};

#endif // SCORE_BOARD_H
