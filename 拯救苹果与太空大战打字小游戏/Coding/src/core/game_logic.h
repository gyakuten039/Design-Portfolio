#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QTimer>
#include <QPointF>

class GameLogic : public QObject
{
    Q_OBJECT

public:
    explicit GameLogic(QObject *parent = nullptr);

    void startGame();
    void pauseGame();
    void resumeGame();
    void stopGame();
    void resetGame();

    bool isRunning() const;
    bool isPaused() const;

    int getScore() const;
    int getMissed() const;
    int getCombo() const;
    double getAccuracy() const;
    int getElapsedTime() const;

    virtual void handleKeyPress(const QString& key) = 0;

signals:
    void scoreChanged(int score);
    void missedChanged(int missed);
    void comboChanged(int combo);
    void accuracyChanged(double accuracy);
    void timeChanged(int elapsed);
    void gameOver();
    void gameStateChanged(bool running);

protected:
    QList<QString> wordList_;
    QTimer *spawnTimer_;
    QTimer *gameTimer_;
    QTimer *updateTimer_;

    int score_;
    int missed_;
    int combo_;
    int totalTyped_;
    int correctTyped_;
    int elapsed_;
    bool isRunning_;
    bool isPaused_;

    double spawnInterval_;

    QString getRandomWord();
};

#endif // GAME_LOGIC_H
