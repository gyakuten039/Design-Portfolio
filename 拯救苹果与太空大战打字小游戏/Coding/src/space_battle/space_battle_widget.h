#ifndef SPACE_BATTLE_WIDGET_H
#define SPACE_BATTLE_WIDGET_H

#include "../core/score_board.h"
#include "../core/audio_player.h"
#include "../../skills/word_manager.h"
#include "enemy.h"
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QList>
#include <QPointF>
#include <QIcon>

struct Bullet {
    QPointF position;
    int targetIndex;  // 目标敌机索引
};

struct Explosion {
    QPointF position;
    int life;  // 爆炸生命值，用于淡出效果
};

class SpaceBattleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpaceBattleWidget(QWidget *parent = nullptr);
    ~SpaceBattleWidget();

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
    void drawEnemy(QPainter &painter, const Enemy &enemy);
    void drawBackground(QPainter &painper);
    void drawSpaceship(QPainter &painter);
    void drawStars(QPainter &painter);
    void drawExplosion(QPainter &painter, const QPointF& pos, int life);
    void drawBullet(QPainter &painter, const Bullet& bullet);
    void drawHealthBar(QPainter &painter);
    void spawnEnemy();
    void shootBullet(int targetIndex);
    void checkCollisions();
    void updateBullets();
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
    QLabel *destroyedLabel_;
    
    QLabel *menuTitle_;
    QLabel *menuSubtitle_;
    QLabel *gameOverTitle_;

    QTimer *updateTimer_;
    QTimer *spawnTimer_;
    QTimer *gameTimer_;

    QList<Enemy> enemies_;
    QList<Explosion> explosions_;
    QList<Bullet> bullets_;

    int score_;
    int missed_;
    int combo_;
    int totalTyped_;
    int correctTyped_;
    int elapsed_;
    int destroyedCount_;
    bool isRunning_;
    bool isPaused_;

    double fallSpeed_;
    int playerHealth_;
    int maxHealth_;
    int shipX_;
    int shipSpeed_;
    int shipDirection_;
    AudioPlayer *bgMusic_;
    int musicVolume_;
    int spawnInterval_;
    int spawnCount_;
};

#endif // SPACE_BATTLE_WIDGET_H
