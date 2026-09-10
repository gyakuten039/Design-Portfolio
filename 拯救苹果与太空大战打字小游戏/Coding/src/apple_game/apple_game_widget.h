#ifndef APPLE_GAME_WIDGET_H
#define APPLE_GAME_WIDGET_H

#include "../core/score_board.h"
#include "../core/audio_player.h"
#include "../../skills/word_manager.h"
#include "apple.h"
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QList>
#include <QIcon>

class AppleGameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppleGameWidget(QWidget *parent = nullptr);
    ~AppleGameWidget();

    void showGame();
    void hideGame();
    void retranslateUI();

signals:
    void backToMenu();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onStartClicked();
    void onPauseClicked();
    void onRestartClicked();
    void onBackToMenuClicked();
    void onGameOver();
    void updateGame();
    void onWordsReady();
    void showSettings();

private:
    void setupUI();
    void drawApple(QPainter &painter, const Apple &apple);
    void drawBackground(QPainter &painter);
    void spawnApple();
    void playBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();
    void stopBackgroundMusic();
    QIcon createGearIcon();

    QStackedWidget *stackedWidget_;
    QWidget *gameWidget_;
    QWidget *menuWidget_;
    QWidget *gameOverWidget_;

    ScoreBoard *scoreBoard_;
    WordManager *wordManager_;

    QPushButton *startButton_;
    QPushButton *pauseButton_;
    QPushButton *restartButton_;
    QPushButton *backButton_;
    QPushButton *menuButton_;

    QLabel *titleLabel_;
    QLabel *finalScoreLabel_;
    QLabel *finalAccuracyLabel_;
    
    QLabel *menuTitle_;
    QLabel *menuSubtitle_;
    QLabel *gameOverTitle_;

    QTimer *updateTimer_;
    QTimer *spawnTimer_;
    QTimer *gameTimer_;

    QList<Apple> apples_;

    int score_;
    int missed_;
    int combo_;
    int totalTyped_;
    int correctTyped_;
    int elapsed_;
    bool isRunning_;
    bool isPaused_;

    double fallSpeed_;
    AudioPlayer *bgMusic_;
    int musicVolume_;
    int spawnInterval_;
    int spawnCount_;
};

#endif // APPLE_GAME_WIDGET_H
