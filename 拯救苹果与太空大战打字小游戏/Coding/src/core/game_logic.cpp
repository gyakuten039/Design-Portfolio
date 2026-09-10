#include "game_logic.h"
#include <QRandomGenerator>

GameLogic::GameLogic(QObject *parent)
    : QObject(parent)
    , spawnTimer_(new QTimer(this))
    , gameTimer_(new QTimer(this))
    , updateTimer_(new QTimer(this))
    , score_(0)
    , missed_(0)
    , combo_(0)
    , totalTyped_(0)
    , correctTyped_(0)
    , elapsed_(0)
    , isRunning_(false)
    , isPaused_(false)
    , spawnInterval_(2000)
{
    wordList_ << "apple" << "banana" << "cherry" << "date" << "elderberry"
              << "fig" << "grape" << "honeydew" << "kiwi" << "lemon"
              << "mango" << "nectarine" << "orange" << "peach" << "quince"
              << "raspberry" << "strawberry" << "tangerine" << "ugli" << "vanilla"
              << "watermelon" << "xigua" << "yellow" << "zucchini"
              << "code" << "debug" << "test" << "build" << "run"
              << "qt" << "cpp" << "java" << "python" << "rust"
              << "hello" << "world" << "game" << "type" << "fast"
              << "speed" << "score" << "combo" << "level" << "star"
              << "space" << "battle" << "enemy" << "shoot" << "laser"
              << "rocket" << "planet" << "galaxy" << "comet" << "asteroid";

    connect(gameTimer_, &QTimer::timeout, this, [this]() {
        elapsed_++;
        emit timeChanged(elapsed_);
    });
}

void GameLogic::startGame()
{
    resetGame();
    isRunning_ = true;
    isPaused_ = false;

    spawnTimer_->start(spawnInterval_);
    updateTimer_->start(16);
    gameTimer_->start(1000);

    emit gameStateChanged(true);
}

void GameLogic::pauseGame()
{
    if (isRunning_ && !isPaused_) {
        isPaused_ = true;
        spawnTimer_->stop();
        updateTimer_->stop();
        gameTimer_->stop();
    }
}

void GameLogic::resumeGame()
{
    if (isRunning_ && isPaused_) {
        isPaused_ = false;
        spawnTimer_->start(spawnInterval_);
        updateTimer_->start(16);
        gameTimer_->start(1000);
    }
}

void GameLogic::stopGame()
{
    isRunning_ = false;
    isPaused_ = false;

    spawnTimer_->stop();
    updateTimer_->stop();
    gameTimer_->stop();

    emit gameStateChanged(false);
    emit gameOver();
}

void GameLogic::resetGame()
{
    score_ = 0;
    missed_ = 0;
    combo_ = 0;
    totalTyped_ = 0;
    correctTyped_ = 0;
    elapsed_ = 0;
    isRunning_ = false;
    isPaused_ = false;

    emit scoreChanged(0);
    emit missedChanged(0);
    emit comboChanged(0);
    emit accuracyChanged(100.0);
    emit timeChanged(0);
}

bool GameLogic::isRunning() const
{
    return isRunning_;
}

bool GameLogic::isPaused() const
{
    return isPaused_;
}

int GameLogic::getScore() const
{
    return score_;
}

int GameLogic::getMissed() const
{
    return missed_;
}

int GameLogic::getCombo() const
{
    return combo_;
}

double GameLogic::getAccuracy() const
{
    if (totalTyped_ == 0) return 100.0;
    return (double)correctTyped_ / totalTyped_ * 100.0;
}

int GameLogic::getElapsedTime() const
{
    return elapsed_;
}

QString GameLogic::getRandomWord()
{
    int index = QRandomGenerator::global()->bounded(wordList_.size());
    return wordList_[index];
}
