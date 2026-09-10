#include "apple_game_widget.h"
#include "../core/logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGradient>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QPainterPath>
#include <QDialog>
#include <QSlider>
#include <QGroupBox>
#include <QtMath>

AppleGameWidget::AppleGameWidget(QWidget *parent)
    : QWidget(parent)
    , stackedWidget_(new QStackedWidget(this))
    , gameWidget_(new QWidget(this))
    , menuWidget_(new QWidget(this))
    , gameOverWidget_(new QWidget(this))
    , scoreBoard_(new ScoreBoard(this))
    , wordManager_(new WordManager(this))
    , startButton_(new QPushButton(tr("Start"), this))
    , pauseButton_(new QPushButton(tr("Pause"), this))
    , restartButton_(new QPushButton(tr("Restart"), this))
    , backButton_(new QPushButton(tr("Back"), this))
    , menuButton_(new QPushButton(tr("Back to Menu"), this))
    , titleLabel_(new QLabel(tr("Save Apple"), this))
    , finalScoreLabel_(new QLabel(this))
    , finalAccuracyLabel_(new QLabel(this))
    , updateTimer_(new QTimer(this))
    , spawnTimer_(new QTimer(this))
    , gameTimer_(new QTimer(this))
    , score_(0)
    , missed_(0)
    , combo_(0)
    , totalTyped_(0)
    , correctTyped_(0)
    , elapsed_(0)
    , isRunning_(false)
    , isPaused_(false)
    , fallSpeed_(0.6)
    , bgMusic_(nullptr)
    , musicVolume_(50)
    , spawnInterval_(3500)
    , spawnCount_(1)
{
    bgMusic_ = new AudioPlayer(this);

    wordManager_->setWordLengthRange(3, 10);
    wordManager_->initialize();
    
    connect(wordManager_, &WordManager::wordsReady, this, &AppleGameWidget::onWordsReady);

    setupUI();

    connect(updateTimer_, &QTimer::timeout, this, &AppleGameWidget::updateGame);
    connect(spawnTimer_, &QTimer::timeout, this, &AppleGameWidget::spawnApple);
    connect(gameTimer_, &QTimer::timeout, this, [this]() {
        elapsed_++;
        scoreBoard_->updateTime(elapsed_);
    });

    updateTimer_->start(16);
}

AppleGameWidget::~AppleGameWidget()
{
}

void AppleGameWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    titleLabel_->setStyleSheet("QLabel { color: #333; font-size: 20px; font-weight: bold; }");
    headerLayout->addWidget(titleLabel_);

    headerLayout->addStretch();

    startButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #4CAF50; color: white; border: none; "
        "border-radius: 5px; padding: 8px 20px; font-size: 14px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #45a049; }"
    );
    headerLayout->addWidget(startButton_);

    pauseButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #FF9800; color: white; border: none; "
        "border-radius: 5px; padding: 8px 20px; font-size: 14px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #e68900; }"
    );
    headerLayout->addWidget(pauseButton_);

    QPushButton *settingsButton = new QPushButton(this);
    settingsButton->setFixedSize(35, 35);
    settingsButton->setStyleSheet(
        "QPushButton { "
        "background-color: #607D8B; border: none; "
        "border-radius: 17px; "
        "} "
        "QPushButton:hover { background-color: #546E7A; }"
    );
    settingsButton->setIcon(createGearIcon());
    settingsButton->setIconSize(QSize(25, 25));
    headerLayout->addWidget(settingsButton);
    connect(settingsButton, &QPushButton::clicked, this, &AppleGameWidget::showSettings);

    mainLayout->addWidget(headerWidget);

    QWidget *scoreWidget = new QWidget(this);
    QHBoxLayout *scoreLayout = new QHBoxLayout(scoreWidget);
    scoreLayout->setContentsMargins(20, 5, 20, 5);

    scoreLayout->addWidget(scoreBoard_);

    mainLayout->addWidget(scoreWidget);

    stackedWidget_->addWidget(gameWidget_);
    stackedWidget_->addWidget(menuWidget_);
    stackedWidget_->addWidget(gameOverWidget_);
    stackedWidget_->setCurrentWidget(menuWidget_);

    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget_);
    menuLayout->setAlignment(Qt::AlignCenter);

    menuTitle_ = new QLabel(tr("Save Apple"), menuWidget_);
    menuTitle_->setStyleSheet("QLabel { color: #4CAF50; font-size: 48px; font-weight: bold; }");
    menuTitle_->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(menuTitle_);

    menuSubtitle_ = new QLabel(tr("Type the words on the falling apples!"), menuWidget_);
    menuSubtitle_->setStyleSheet("QLabel { color: #666; font-size: 18px; }");
    menuSubtitle_->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(menuSubtitle_);

    if (wordManager_->isUsingAI()) {
        QLabel *aiLabel = new QLabel(tr("AI Word Generation Enabled"), menuWidget_);
        aiLabel->setStyleSheet("QLabel { color: #2196F3; font-size: 14px; }");
        aiLabel->setAlignment(Qt::AlignCenter);
        menuLayout->addWidget(aiLabel);
    }

    menuLayout->addSpacing(30);

    restartButton_->setFixedSize(200, 50);
    restartButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #4CAF50; color: white; border: none; "
        "border-radius: 25px; font-size: 18px; font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #45a049; }"
    );
    menuLayout->addWidget(restartButton_, 0, Qt::AlignCenter);

    menuLayout->addSpacing(20);

    backButton_->setFixedSize(200, 50);
    backButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #2196F3; color: white; border: none; "
        "border-radius: 25px; font-size: 18px; font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #1976D2; }"
    );
    menuLayout->addWidget(backButton_, 0, Qt::AlignCenter);

    menuLayout->addStretch();

    QVBoxLayout *gameOverLayout = new QVBoxLayout(gameOverWidget_);
    gameOverLayout->setAlignment(Qt::AlignCenter);

    gameOverTitle_ = new QLabel(tr("Game Over"), gameOverWidget_);
    gameOverTitle_->setStyleSheet("QLabel { color: #f44336; font-size: 48px; font-weight: bold; }");
    gameOverTitle_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(gameOverTitle_);

    gameOverLayout->addSpacing(20);

    finalScoreLabel_->setStyleSheet("QLabel { color: #333; font-size: 24px; }");
    finalScoreLabel_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(finalScoreLabel_);

    finalAccuracyLabel_->setStyleSheet("QLabel { color: #333; font-size: 24px; }");
    finalAccuracyLabel_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(finalAccuracyLabel_);

    gameOverLayout->addSpacing(30);

    menuButton_->setFixedSize(200, 50);
    menuButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #4CAF50; color: white; border: none; "
        "border-radius: 25px; font-size: 18px; font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #45a049; }"
    );
    gameOverLayout->addWidget(menuButton_, 0, Qt::AlignCenter);

    gameOverLayout->addStretch();

    mainLayout->addWidget(stackedWidget_);

    connect(startButton_, &QPushButton::clicked, this, &AppleGameWidget::onStartClicked);
    connect(pauseButton_, &QPushButton::clicked, this, &AppleGameWidget::onPauseClicked);
    connect(restartButton_, &QPushButton::clicked, this, &AppleGameWidget::onRestartClicked);
    connect(backButton_, &QPushButton::clicked, this, &AppleGameWidget::onBackToMenuClicked);
    connect(menuButton_, &QPushButton::clicked, this, &AppleGameWidget::onBackToMenuClicked);
}

void AppleGameWidget::showGame()
{
    stackedWidget_->setCurrentWidget(gameWidget_);
    setFocus();
}

void AppleGameWidget::hideGame()
{
    isRunning_ = false;
    isPaused_ = false;
    apples_.clear();
    spawnTimer_->stop();
    gameTimer_->stop();
    stopBackgroundMusic();
    stackedWidget_->setCurrentWidget(menuWidget_);
    update();
}

void AppleGameWidget::playBackgroundMusic()
{
    if (bgMusic_) {
        bgMusic_->playMusic("res/music/apple_music.mp3");
        bgMusic_->setVolume(musicVolume_);
    }
}

void AppleGameWidget::pauseBackgroundMusic()
{
    if (bgMusic_) {
        bgMusic_->pauseMusic();
    }
}

void AppleGameWidget::resumeBackgroundMusic()
{
    if (bgMusic_) {
        if (bgMusic_->isPaused()) {
            bgMusic_->resumeMusic();
        } else {
            bgMusic_->playMusic("res/music/apple_music.mp3");
            bgMusic_->setVolume(musicVolume_);
        }
    }
}

void AppleGameWidget::stopBackgroundMusic()
{
    if (bgMusic_) {
        bgMusic_->stopMusic();
    }
}

void AppleGameWidget::keyPressEvent(QKeyEvent *event)
{
    if (isRunning_ && !isPaused_) {
        QString key = event->text().toLower();
        if (!key.isEmpty()) {
            bool foundMatch = false;
            
            for (auto &apple : apples_) {
                if (apple.isCompleted() || apple.isOutOfBounds()) continue;

                QString expected = apple.word();
                QString typed = apple.typedWord();
                QString nextExpected = expected.mid(typed.length(), 1);

                if (nextExpected == key) {
                    typed += key;
                    apple.setType(typed);

                    totalTyped_++;
                    correctTyped_++;

                    if (apple.isCompleted()) {
                        combo_++;
                        int points = 10 + (combo_ > 1 ? combo_ * 2 : 0);
                        score_ += points;
                        scoreBoard_->updateScore(score_);
                        scoreBoard_->updateCombo(combo_);
                    }

                    scoreBoard_->updateAccuracy((double)correctTyped_ / totalTyped_ * 100.0);
                    foundMatch = true;
                    break;
                }
            }
            
            if (!foundMatch) {
                combo_ = 0;
                totalTyped_++;
                scoreBoard_->updateCombo(combo_);
                scoreBoard_->updateAccuracy((double)correctTyped_ / totalTyped_ * 100.0);
            }
        }
    }
    QWidget::keyPressEvent(event);
}

void AppleGameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);

    if (stackedWidget_->currentWidget() == gameWidget_) {
        for (const Apple &apple : apples_) {
            drawApple(painter, apple);
        }
    }
}

void AppleGameWidget::drawApple(QPainter &painter, const Apple &apple)
{
    QPointF pos = apple.position();
    QString word = apple.word();
    QString typed = apple.typedWord();

    QRadialGradient gradient(pos, 35);
    gradient.setColorAt(0, QColor(255, 100, 100));
    gradient.setColorAt(0.7, QColor(200, 50, 50));
    gradient.setColorAt(1, QColor(150, 30, 30));

    painter.setBrush(gradient);
    painter.setPen(QPen(QColor(100, 20, 20), 2));
    painter.drawEllipse(QRectF(pos.x() - 35, pos.y() - 35, 70, 70));

    painter.setPen(QPen(QColor(80, 150, 80), 3));
    painter.drawLine(QPointF(pos.x(), pos.y() - 35), QPointF(pos.x() + 5, pos.y() - 50));

    painter.setBrush(QColor(80, 180, 80));
    QPainterPath leaf;
    leaf.moveTo(pos.x() + 5, pos.y() - 45);
    leaf.quadTo(pos.x() + 20, pos.y() - 55, pos.x() + 15, pos.y() - 40);
    leaf.quadTo(pos.x() + 10, pos.y() - 48, pos.x() + 5, pos.y() - 45);
    painter.drawPath(leaf);

    QFont font("Arial", 12, QFont::Bold);
    painter.setFont(font);

    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(word);
    int textHeight = fm.height();

    QRectF textRect(pos.x() - textWidth / 2 - 5, pos.y() - 15, textWidth + 10, textHeight + 5);

    painter.setBrush(QColor(255, 255, 255, 200));
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.drawRoundedRect(textRect, 5, 5);

    if (!typed.isEmpty()) {
        QString remaining = word.mid(typed.length());

        int typedWidth = fm.horizontalAdvance(typed);

        painter.setPen(QPen(QColor(0, 150, 0), 1));
        painter.drawText(QRectF(pos.x() - textWidth / 2 - 5, pos.y() - 15, typedWidth + 5, textHeight + 5),
                         Qt::AlignCenter, typed);

        painter.setPen(QPen(QColor(50, 50, 50), 1));
        painter.drawText(QRectF(pos.x() - textWidth / 2 + typedWidth - 5, pos.y() - 15,
                                textWidth - typedWidth + 5, textHeight + 5),
                         Qt::AlignLeft, remaining);
    } else {
        painter.setPen(QPen(QColor(50, 50, 50), 1));
        painter.drawText(textRect, Qt::AlignCenter, word);
    }
}

void AppleGameWidget::drawBackground(QPainter &painter)
{
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(135, 206, 235));
    gradient.setColorAt(0.5, QColor(176, 226, 255));
    gradient.setColorAt(1, QColor(144, 238, 144));

    painter.fillRect(rect(), gradient);

    painter.setBrush(QColor(100, 180, 100));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, height() - 50, width(), 100);

    painter.setBrush(QColor(80, 160, 80));
    painter.drawEllipse(0, height() - 30, width(), 60);
}

QIcon AppleGameWidget::createGearIcon()
{
    QPixmap pixmap(25, 25);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath gear;
    int centerX = 12, centerY = 12;
    int outerRadius = 10, innerRadius = 5;
    int teeth = 8;
    qreal angleStep = 2 * M_PI / teeth;

    for (int i = 0; i < teeth; ++i) {
        qreal angle = i * angleStep;
        qreal nextAngle = (i + 0.5) * angleStep;
        qreal outerAngle1 = angle - angleStep * 0.15;
        qreal outerAngle2 = angle + angleStep * 0.15;

        if (i == 0) {
            gear.moveTo(centerX + outerRadius * qCos(outerAngle1), 
                       centerY + outerRadius * qSin(outerAngle1));
        }
        gear.lineTo(centerX + outerRadius * qCos(outerAngle2), 
                   centerY + outerRadius * qSin(outerAngle2));
        gear.lineTo(centerX + innerRadius * qCos(nextAngle), 
                   centerY + innerRadius * qSin(nextAngle));
    }
    gear.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawPath(gear);

    painter.setBrush(QColor(96, 125, 139));
    painter.drawEllipse(QPoint(centerX, centerY), 3, 3);

    painter.end();
    return QIcon(pixmap);
}

void AppleGameWidget::spawnApple()
{
    const int maxApplesOnScreen = 12;
    const int appleWidth = 60;
    const int minSpacing = 20;
    
    if (apples_.size() >= maxApplesOnScreen) {
        return;
    }
    
    int countToSpawn = qMin(spawnCount_, maxApplesOnScreen - apples_.size());
    
    for (int i = 0; i < countToSpawn; ++i) {
        QString word = wordManager_->getRandomWord();
        
        qreal x = 50 + QRandomGenerator::global()->bounded(width() - 100);
        
        for (int retry = 0; retry < 10; ++retry) {
            bool overlaps = false;
            qreal minX = x - appleWidth / 2 - minSpacing;
            qreal maxX = x + appleWidth / 2 + minSpacing;
            
            for (const Apple& existing : apples_) {
                qreal existingMinX = existing.position().x() - appleWidth / 2;
                qreal existingMaxX = existing.position().x() + appleWidth / 2;
                
                if (minX < existingMaxX && maxX > existingMinX) {
                    overlaps = true;
                    break;
                }
            }
            
            if (!overlaps) {
                break;
            }
            
            x = 50 + QRandomGenerator::global()->bounded(width() - 100);
        }
        
        Apple apple(word, QPointF(x, -i * 100));
        apples_.append(apple);
    }
}

void AppleGameWidget::updateGame()
{
    if (!isRunning_ || isPaused_) return;

    for (auto it = apples_.begin(); it != apples_.end();) {
        if (it->isCompleted()) {
            it = apples_.erase(it);
            continue;
        }

        it->updatePosition(fallSpeed_);

        if (it->isOutOfBounds()) {
            missed_++;
            combo_ = 0;
            scoreBoard_->updateMissed(missed_);
            scoreBoard_->updateCombo(combo_);
            it = apples_.erase(it);

            if (missed_ >= 10) {
                onGameOver();
            }
            continue;
        }

        ++it;
    }

    update();
}

void AppleGameWidget::onWordsReady()
{
    LOG_INFO(QString("Words ready for Apple Game. AI enabled: %1").arg(wordManager_->isUsingAI()));
}

void AppleGameWidget::onStartClicked()
{
    if (!isRunning_) {
        score_ = 0;
        missed_ = 0;
        combo_ = 0;
        totalTyped_ = 0;
        correctTyped_ = 0;
        elapsed_ = 0;
        apples_.clear();

        scoreBoard_->updateScore(0);
        scoreBoard_->updateMissed(0);
        scoreBoard_->updateCombo(0);
        scoreBoard_->updateAccuracy(100.0);
        scoreBoard_->updateTime(0);

        isRunning_ = true;
        isPaused_ = false;

        playBackgroundMusic();
        spawnTimer_->start(spawnInterval_);
        gameTimer_->start(1000);

        showGame();
        pauseButton_->setText(tr("Pause"));
    } else if (isPaused_) {
        isPaused_ = false;
        playBackgroundMusic();
        spawnTimer_->start(spawnInterval_);
        gameTimer_->start(1000);
        pauseButton_->setText(tr("Pause"));
    }
}

void AppleGameWidget::onPauseClicked()
{
    if (isRunning_) {
        if (isPaused_) {
            isPaused_ = false;
            resumeBackgroundMusic();
            spawnTimer_->start(spawnInterval_);
            gameTimer_->start(1000);
            pauseButton_->setText(tr("Pause"));
        } else {
            isPaused_ = true;
            pauseBackgroundMusic();
            spawnTimer_->stop();
            gameTimer_->stop();
            pauseButton_->setText(tr("Resume"));
        }
    }
}

void AppleGameWidget::onRestartClicked()
{
    score_ = 0;
    missed_ = 0;
    combo_ = 0;
    totalTyped_ = 0;
    correctTyped_ = 0;
    elapsed_ = 0;
    apples_.clear();

    scoreBoard_->updateScore(0);
    scoreBoard_->updateMissed(0);
    scoreBoard_->updateCombo(0);
    scoreBoard_->updateAccuracy(100.0);
    scoreBoard_->updateTime(0);

    isRunning_ = true;
    isPaused_ = false;

    playBackgroundMusic();
    spawnTimer_->start(spawnInterval_);
    gameTimer_->start(1000);

    showGame();
    pauseButton_->setText(tr("Pause"));
}

void AppleGameWidget::onBackToMenuClicked()
{
    emit backToMenu();
}

void AppleGameWidget::onGameOver()
{
    isRunning_ = false;
    spawnTimer_->stop();
    gameTimer_->stop();

    finalScoreLabel_->setText(tr("Final Score: %1").arg(score_));
    double accuracy = (totalTyped_ > 0) ? (double)correctTyped_ / totalTyped_ * 100.0 : 100.0;
    finalAccuracyLabel_->setText(tr("Accuracy: %1%").arg(accuracy, 0, 'f', 1));
    stackedWidget_->setCurrentWidget(gameOverWidget_);
}

void AppleGameWidget::showSettings()
{
    QDialog *settingsDialog = new QDialog(this);
    settingsDialog->setWindowTitle(tr("Settings"));
    settingsDialog->setFixedSize(450, 470);
    settingsDialog->setStyleSheet("QDialog { background-color: #f5f5f5; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(settingsDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel(tr("Game Settings"), settingsDialog);
    titleLabel->setStyleSheet("QLabel { color: #333; font-size: 24px; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(25);

    QGroupBox *gameGroup = new QGroupBox(tr("Game Settings"), settingsDialog);
    gameGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; margin-top: 10px; }");
    QVBoxLayout *gameLayout = new QVBoxLayout(gameGroup);

    QLabel *speedLabel = new QLabel(tr("Fall Speed:"), settingsDialog);
    gameLayout->addWidget(speedLabel);

    QSlider *speedSlider = new QSlider(Qt::Horizontal, settingsDialog);
    speedSlider->setRange(5, 30);
    speedSlider->setValue(static_cast<int>(fallSpeed_ * 10));
    speedSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 10px; background: #ddd; border-radius: 5px; }"
        "QSlider::handle:horizontal { background: #4CAF50; border: none; width: 20px; height: 20px; margin: -5px 0; border-radius: 50%; }"
    );

    QLabel *speedValueLabel = new QLabel(QString::number(fallSpeed_, 'f', 1), settingsDialog);
    speedValueLabel->setAlignment(Qt::AlignRight);
    gameLayout->addWidget(speedSlider);
    gameLayout->addWidget(speedValueLabel);

    connect(speedSlider, &QSlider::valueChanged, this, [this, speedValueLabel](int value) {
        fallSpeed_ = value / 10.0;
        speedValueLabel->setText(QString::number(fallSpeed_, 'f', 1));
    });

    gameLayout->addSpacing(10);

    QLabel *countLabel = new QLabel(tr("Spawn Count:"), settingsDialog);
    gameLayout->addWidget(countLabel);

    QSlider *countSlider = new QSlider(Qt::Horizontal, settingsDialog);
    countSlider->setRange(1, 5);
    countSlider->setValue(spawnCount_);
    countSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 10px; background: #ddd; border-radius: 5px; }"
        "QSlider::handle:horizontal { background: #FF9800; border: none; width: 20px; height: 20px; margin: -5px 0; border-radius: 50%; }"
    );

    QLabel *countValueLabel = new QLabel(QString::number(spawnCount_), settingsDialog);
    countValueLabel->setAlignment(Qt::AlignRight);
    gameLayout->addWidget(countSlider);
    gameLayout->addWidget(countValueLabel);

    connect(countSlider, &QSlider::valueChanged, this, [this, countValueLabel](int value) {
        spawnCount_ = value;
        countValueLabel->setText(QString::number(value));
    });

    gameLayout->addSpacing(10);

    QLabel *intervalLabel = new QLabel(tr("Spawn Interval (ms):"), settingsDialog);
    gameLayout->addWidget(intervalLabel);

    QSlider *intervalSlider = new QSlider(Qt::Horizontal, settingsDialog);
    intervalSlider->setRange(500, 3000);
    intervalSlider->setValue(spawnInterval_);
    intervalSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 10px; background: #ddd; border-radius: 5px; }"
        "QSlider::handle:horizontal { background: #9C27B0; border: none; width: 20px; height: 20px; margin: -5px 0; border-radius: 50%; }"
    );

    QLabel *intervalValueLabel = new QLabel(QString::number(spawnInterval_) + "ms", settingsDialog);
    intervalValueLabel->setAlignment(Qt::AlignRight);
    gameLayout->addWidget(intervalSlider);
    gameLayout->addWidget(intervalValueLabel);

    connect(intervalSlider, &QSlider::valueChanged, this, [this, intervalValueLabel](int value) {
        spawnInterval_ = value;
        intervalValueLabel->setText(QString::number(value) + "ms");
        if (isRunning_ && !isPaused_) {
            spawnTimer_->setInterval(value);
        }
    });

    mainLayout->addWidget(gameGroup);

    QGroupBox *audioGroup = new QGroupBox(tr("Audio Settings"), settingsDialog);
    audioGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; margin-top: 10px; }");
    QVBoxLayout *audioLayout = new QVBoxLayout(audioGroup);

    QLabel *volumeLabel = new QLabel(tr("Music Volume:"), settingsDialog);
    audioLayout->addWidget(volumeLabel);

    QSlider *volumeSlider = new QSlider(Qt::Horizontal, settingsDialog);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(musicVolume_);
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 10px; background: #ddd; border-radius: 5px; }"
        "QSlider::handle:horizontal { background: #2196F3; border: none; width: 20px; height: 20px; margin: -5px 0; border-radius: 50%; }"
    );

    QLabel *volumeValueLabel = new QLabel(QString::number(musicVolume_) + "%", settingsDialog);
    volumeValueLabel->setAlignment(Qt::AlignRight);
    audioLayout->addWidget(volumeSlider);
    audioLayout->addWidget(volumeValueLabel);

    connect(volumeSlider, &QSlider::valueChanged, this, [this, volumeValueLabel](int value) {
        musicVolume_ = value;
        volumeValueLabel->setText(QString::number(value) + "%");
        if (bgMusic_) {
            bgMusic_->setVolume(value);
        }
    });

    mainLayout->addWidget(audioGroup);

    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *quitButton = new QPushButton(tr("Quit Game"), settingsDialog);
    quitButton->setFixedHeight(40);
    quitButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border: none; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #d32f2f; }"
    );
    buttonLayout->addWidget(quitButton);

    QPushButton *okButton = new QPushButton(tr("OK"), settingsDialog);
    okButton->setFixedHeight(40);
    okButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; border: none; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    buttonLayout->addWidget(okButton);

    mainLayout->addLayout(buttonLayout);

    bool quittingGame = false;
    connect(quitButton, &QPushButton::clicked, this, [this, settingsDialog, &quittingGame]() {
        quittingGame = true;
        settingsDialog->accept();
        hideGame();
    });

    connect(okButton, &QPushButton::clicked, settingsDialog, &QDialog::accept);

    bool wasGamePaused = isPaused_;
    bool wasMusicPlaying = bgMusic_ && (bgMusic_->isPlaying() || bgMusic_->isPaused());

    if (!wasGamePaused) {
        isPaused_ = true;
        pauseBackgroundMusic();
        spawnTimer_->stop();
        gameTimer_->stop();
    }

    settingsDialog->exec();

    if (!wasGamePaused && !quittingGame) {
        isPaused_ = false;
        if (wasMusicPlaying) {
            resumeBackgroundMusic();
        }
        spawnTimer_->start(spawnInterval_);
        gameTimer_->start(1000);
    }

    delete settingsDialog;
}

void AppleGameWidget::retranslateUI()
{
    startButton_->setText(tr("Start"));
    pauseButton_->setText(isPaused_ ? tr("Resume") : tr("Pause"));
    restartButton_->setText(tr("Restart"));
    backButton_->setText(tr("Back"));
    menuButton_->setText(tr("Back to Menu"));
    titleLabel_->setText(tr("Save Apple"));
    
    if (menuTitle_) {
        menuTitle_->setText(tr("Save Apple"));
    }
    if (menuSubtitle_) {
        menuSubtitle_->setText(tr("Type the words on the falling apples!"));
    }
    if (gameOverTitle_) {
        gameOverTitle_->setText(tr("Game Over"));
    }
    
    if (isRunning_) {
        finalScoreLabel_->setText(tr("Final Score: %1").arg(score_));
        double accuracy = (totalTyped_ > 0) ? (double)correctTyped_ / totalTyped_ * 100.0 : 100.0;
        finalAccuracyLabel_->setText(tr("Accuracy: %1%").arg(accuracy, 0, 'f', 1));
    }
}
