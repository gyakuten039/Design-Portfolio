#include "space_battle_widget.h"
#include "../core/logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGradient>
#include <QRandomGenerator>
#include <QPainterPath>
#include <QDialog>
#include <QSlider>
#include <QGroupBox>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QtMath>

SpaceBattleWidget::SpaceBattleWidget(QWidget *parent)
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
    , titleLabel_(new QLabel(tr("Space Battle"), this))
    , finalScoreLabel_(new QLabel(this))
    , finalAccuracyLabel_(new QLabel(this))
    , destroyedLabel_(new QLabel(this))
    , menuTitle_(nullptr)
    , menuSubtitle_(nullptr)
    , gameOverTitle_(nullptr)
    , updateTimer_(new QTimer(this))
    , spawnTimer_(new QTimer(this))
    , gameTimer_(new QTimer(this))
    , score_(0)
    , missed_(0)
    , combo_(0)
    , totalTyped_(0)
    , correctTyped_(0)
    , elapsed_(0)
    , destroyedCount_(0)
    , isRunning_(false)
    , isPaused_(false)
    , fallSpeed_(0.5)
    , playerHealth_(100)
    , maxHealth_(100)
    , shipX_(0)
    , shipSpeed_(8)
    , shipDirection_(1)
    , bgMusic_(nullptr)
    , musicVolume_(50)
    , spawnInterval_(3000)
    , spawnCount_(1)
{
    bgMusic_ = new AudioPlayer(this);

    wordManager_->setWordLengthRange(4, 12);
    wordManager_->initialize();
    
    connect(wordManager_, &WordManager::wordsReady, this, &SpaceBattleWidget::onWordsReady);

    setupUI();

    connect(updateTimer_, &QTimer::timeout, this, &SpaceBattleWidget::updateGame);
    connect(spawnTimer_, &QTimer::timeout, this, &SpaceBattleWidget::spawnEnemy);
    connect(gameTimer_, &QTimer::timeout, this, [this]() {
        elapsed_++;
        scoreBoard_->updateTime(elapsed_);
    });

    updateTimer_->start(16);
}

SpaceBattleWidget::~SpaceBattleWidget()
{
}

void SpaceBattleWidget::setupUI()
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
        "background-color: #607D8B; border: none; border-radius: 17px; "
        "} "
        "QPushButton:hover { background-color: #546E7A; }"
    );
    // 绘制齿轮图标
    settingsButton->setIcon(createGearIcon());
    settingsButton->setIconSize(QSize(25, 25));
    headerLayout->addWidget(settingsButton);
    connect(settingsButton, &QPushButton::clicked, this, &SpaceBattleWidget::showSettings);

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

    menuTitle_ = new QLabel(tr("Space Battle"), menuWidget_);
    menuTitle_->setStyleSheet("QLabel { color: #2196F3; font-size: 48px; font-weight: bold; }");
    menuTitle_->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(menuTitle_);

    menuSubtitle_ = new QLabel(tr("Type the words to destroy enemy spaceships!"), menuWidget_);
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
        "background-color: #2196F3; color: white; border: none; "
        "border-radius: 25px; font-size: 18px; font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #1976D2; }"
    );
    menuLayout->addWidget(restartButton_, 0, Qt::AlignCenter);

    menuLayout->addSpacing(20);

    backButton_->setFixedSize(200, 50);
    backButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #607D8B; color: white; border: none; "
        "border-radius: 25px; font-size: 18px; font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #546E7A; }"
    );
    menuLayout->addWidget(backButton_, 0, Qt::AlignCenter);

    menuLayout->addStretch();

    QVBoxLayout *gameOverLayout = new QVBoxLayout(gameOverWidget_);
    gameOverLayout->setAlignment(Qt::AlignCenter);

    gameOverTitle_ = new QLabel(tr("Mission Complete"), gameOverWidget_);
    gameOverTitle_->setStyleSheet("QLabel { color: #FF9800; font-size: 48px; font-weight: bold; }");
    gameOverTitle_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(gameOverTitle_);

    gameOverLayout->addSpacing(20);

    finalScoreLabel_->setStyleSheet("QLabel { color: #333; font-size: 24px; }");
    finalScoreLabel_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(finalScoreLabel_);

    finalAccuracyLabel_->setStyleSheet("QLabel { color: #333; font-size: 24px; }");
    finalAccuracyLabel_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(finalAccuracyLabel_);

    destroyedLabel_->setStyleSheet("QLabel { color: #333; font-size: 24px; }");
    destroyedLabel_->setAlignment(Qt::AlignCenter);
    gameOverLayout->addWidget(destroyedLabel_);

    gameOverLayout->addSpacing(30);

    menuButton_->setFixedSize(200, 50);
    menuButton_->setStyleSheet(
        "QPushButton { "
        "background-color: #2196F3; color: white; border: none; "
        "border-radius: 25px; font-size: 18px; font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #1976D2; }"
    );
    gameOverLayout->addWidget(menuButton_, 0, Qt::AlignCenter);

    gameOverLayout->addStretch();

    mainLayout->addWidget(stackedWidget_);

    connect(startButton_, &QPushButton::clicked, this, &SpaceBattleWidget::onStartClicked);
    connect(pauseButton_, &QPushButton::clicked, this, &SpaceBattleWidget::onPauseClicked);
    connect(restartButton_, &QPushButton::clicked, this, &SpaceBattleWidget::onRestartClicked);
    connect(backButton_, &QPushButton::clicked, this, &SpaceBattleWidget::onBackToMenuClicked);
    connect(menuButton_, &QPushButton::clicked, this, &SpaceBattleWidget::onBackToMenuClicked);
}

void SpaceBattleWidget::showGame()
{
    stackedWidget_->setCurrentWidget(gameWidget_);
    setFocus();
}

void SpaceBattleWidget::hideGame()
{
    isRunning_ = false;
    isPaused_ = false;
    enemies_.clear();
    bullets_.clear();
    explosions_.clear();
    spawnTimer_->stop();
    gameTimer_->stop();
    stopBackgroundMusic();
    stackedWidget_->setCurrentWidget(menuWidget_);
    update();
}

void SpaceBattleWidget::playBackgroundMusic()
{
    if (bgMusic_) {
        bgMusic_->playMusic("res/music/space_music.mp3");
        bgMusic_->setVolume(musicVolume_);
    }
}

void SpaceBattleWidget::pauseBackgroundMusic()
{
    if (bgMusic_) {
        bgMusic_->pauseMusic();
    }
}

void SpaceBattleWidget::resumeBackgroundMusic()
{
    if (bgMusic_) {
        if (bgMusic_->isPaused()) {
            bgMusic_->resumeMusic();
        } else {
            bgMusic_->playMusic("res/music/space_music.mp3");
            bgMusic_->setVolume(musicVolume_);
        }
    }
}

void SpaceBattleWidget::stopBackgroundMusic()
{
    if (bgMusic_) {
        bgMusic_->stopMusic();
    }
}

void SpaceBattleWidget::keyPressEvent(QKeyEvent *event)
{
    if (isRunning_ && !isPaused_) {
        QString key = event->text().toLower();
        if (!key.isEmpty()) {
            bool foundMatch = false;
            for (int i = 0; i < enemies_.size(); i++) {
                Enemy &enemy = enemies_[i];
                if (enemy.isCompleted() || enemy.isOutOfBounds() || enemy.isWaitingForBullet()) continue;

                QString expected = enemy.word();
                QString typed = enemy.typedWord();
                QString nextExpected = expected.mid(typed.length(), 1);

                if (nextExpected == key) {
                    typed += key;
                    enemy.setType(typed);

                    totalTyped_++;
                    correctTyped_++;

                    if (enemy.isCompleted()) {
                        enemy.setWaitingForBullet(true);
                        
                        combo_++;
                        int points = 100 + (combo_ > 1 ? combo_ * 20 : 0);
                        score_ += points;
                        scoreBoard_->updateScore(score_);
                        scoreBoard_->updateCombo(combo_);
                        shootBullet(i);
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

void SpaceBattleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawStars(painter);

    if (stackedWidget_->currentWidget() == gameWidget_) {
        for (const Explosion& exp : explosions_) {
            drawExplosion(painter, exp.position, exp.life);
        }

        for (const Enemy &enemy : enemies_) {
            drawEnemy(painter, enemy);
        }

        for (const Bullet& bullet : bullets_) {
            drawBullet(painter, bullet);
        }

        drawSpaceship(painter);
        drawHealthBar(painter);
    }
}

void SpaceBattleWidget::drawEnemy(QPainter &painter, const Enemy &enemy)
{
    QPointF pos = enemy.position();
    QString word = enemy.word();
    QString typed = enemy.typedWord();

    painter.save();

    QPolygonF ship;
    ship << QPointF(pos.x(), pos.y() - 30)
         << QPointF(pos.x() + 40, pos.y() + 25)
         << QPointF(pos.x() + 20, pos.y() + 10)
         << QPointF(pos.x() - 20, pos.y() + 10)
         << QPointF(pos.x() - 40, pos.y() + 25);

    QLinearGradient shipGradient(pos.x(), pos.y() - 30, pos.x(), pos.y() + 25);
    shipGradient.setColorAt(0, QColor(255, 100, 100));
    shipGradient.setColorAt(1, QColor(150, 30, 30));

    painter.setBrush(shipGradient);
    painter.setPen(QPen(QColor(255, 200, 200), 2));
    painter.drawPolygon(ship);

    QPainterPath cockpit;
    cockpit.addEllipse(pos.x() - 10, pos.y() - 5, 20, 15);
    painter.setBrush(QColor(100, 200, 255));
    painter.drawPath(cockpit);

    QFont font("Arial", 12, QFont::Bold);
    painter.setFont(font);

    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(word);
    int textHeight = fm.height();

    QRectF textRect(pos.x() - textWidth / 2 - 5, pos.y() + 30, textWidth + 10, textHeight + 5);

    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(QPen(QColor(100, 100, 100), 1));
    painter.drawRoundedRect(textRect, 5, 5);

    if (!typed.isEmpty()) {
        QString remaining = word.mid(typed.length());

        int typedWidth = fm.horizontalAdvance(typed);

        painter.setPen(QPen(QColor(0, 255, 0), 1));
        painter.drawText(QRectF(pos.x() - textWidth / 2 - 5, pos.y() + 30, typedWidth + 5, textHeight + 5),
                         Qt::AlignCenter, typed);

        painter.setPen(QPen(QColor(255, 255, 255), 1));
        painter.drawText(QRectF(pos.x() - textWidth / 2 + typedWidth - 5, pos.y() + 30,
                                textWidth - typedWidth + 5, textHeight + 5),
                         Qt::AlignLeft, remaining);
    } else {
        painter.setPen(QPen(QColor(255, 255, 255), 1));
        painter.drawText(textRect, Qt::AlignCenter, word);
    }

    painter.restore();
}

void SpaceBattleWidget::drawBackground(QPainter &painter)
{
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(10, 10, 30));
    gradient.setColorAt(0.5, QColor(20, 20, 60));
    gradient.setColorAt(1, QColor(5, 5, 20));

    painter.fillRect(rect(), gradient);
}

void SpaceBattleWidget::drawStars(QPainter &painter)
{
    painter.save();
    painter.setPen(QColor(255, 255, 255));

    for (int i = 0; i < 100; i++) {
        int x = (i * 37) % width();
        int y = (i * 53 + elapsed_ * 2) % height();
        int size = (i % 3) + 1;
        painter.drawPoint(x, y);
    }

    painter.restore();
}

void SpaceBattleWidget::drawSpaceship(QPainter &painter)
{
    if (shipX_ == 0) {
        shipX_ = width() / 2;
    }

    QPointF shipPos(shipX_, height() - 60);

    painter.save();

    QPolygonF ship;
    ship << QPointF(shipPos.x(), shipPos.y() - 35)
         << QPointF(shipPos.x() + 25, shipPos.y())
         << QPointF(shipPos.x() + 15, shipPos.y() - 5)
         << QPointF(shipPos.x() - 15, shipPos.y() - 5)
         << QPointF(shipPos.x() - 25, shipPos.y());

    QLinearGradient shipGradient(shipPos.x(), shipPos.y() - 35, shipPos.x(), shipPos.y());
    shipGradient.setColorAt(0, QColor(100, 180, 255));
    shipGradient.setColorAt(1, QColor(50, 100, 200));

    painter.setBrush(shipGradient);
    painter.setPen(QPen(QColor(150, 220, 255), 2));
    painter.drawPolygon(ship);

    QPainterPath cockpit;
    cockpit.addEllipse(shipPos.x() - 8, shipPos.y() - 15, 16, 20);
    painter.setBrush(QColor(200, 230, 255));
    painter.drawPath(cockpit);

    QPointF flamePos(shipPos.x(), shipPos.y() + 5);
    QLinearGradient flameGradient(flamePos.x(), flamePos.y(), flamePos.x(), flamePos.y() + 15);
    flameGradient.setColorAt(0, QColor(255, 200, 50));
    flameGradient.setColorAt(1, QColor(255, 50, 0));

    painter.setBrush(flameGradient);
    painter.setPen(Qt::NoPen);
    QPolygonF flame;
    flame << QPointF(flamePos.x() - 8, flamePos.y())
          << QPointF(flamePos.x(), flamePos.y() + 10 + (QRandomGenerator::global()->bounded(5)))
          << QPointF(flamePos.x() + 8, flamePos.y());
    painter.drawPolygon(flame);

    painter.restore();
}

void SpaceBattleWidget::drawBullet(QPainter &painter, const Bullet& bullet)
{
    painter.save();
    QRadialGradient bulletGradient(bullet.position, 5);
    bulletGradient.setColorAt(0, QColor(255, 255, 200));
    bulletGradient.setColorAt(1, QColor(255, 200, 50));

    painter.setBrush(bulletGradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(bullet.position, 5, 5);
    painter.restore();
}

void SpaceBattleWidget::drawHealthBar(QPainter &painter)
{
    int barWidth = 200;
    int barHeight = 20;
    int x = 20;
    int y = height() - 50;

    QRectF bgRect(x, y, barWidth, barHeight);
    painter.setBrush(QColor(100, 100, 100));
    painter.setPen(Qt::NoPen);
    painter.drawRect(bgRect);

    double healthRatio = (double)playerHealth_ / maxHealth_;
    QRectF healthRect(x, y, barWidth * healthRatio, barHeight);

    QColor healthColor;
    if (healthRatio > 0.6) {
        healthColor = QColor(76, 175, 80);
    } else if (healthRatio > 0.3) {
        healthColor = QColor(255, 152, 0);
    } else {
        healthColor = QColor(244, 67, 54);
    }

    painter.setBrush(healthColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(healthRect);

    painter.setPen(QPen(QColor(255, 255, 255), 1));
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(QRectF(x, y, barWidth, barHeight), Qt::AlignCenter,
                     QString("HP: %1/%2").arg(playerHealth_).arg(maxHealth_));
}

void SpaceBattleWidget::shootBullet(int targetIndex)
{
    if (shipX_ == 0) {
        shipX_ = width() / 2;
    }
    
    Bullet bullet;
    bullet.position = QPointF(shipX_, height() - 95);
    bullet.targetIndex = targetIndex;
    
    bullets_.append(bullet);
}

void SpaceBattleWidget::checkCollisions()
{
    if (shipX_ == 0) {
        shipX_ = width() / 2;
    }
    QPointF shipPos(shipX_, height() - 60);

    for (auto it = enemies_.begin(); it != enemies_.end();) {
        QPointF enemyPos = it->position();

        bool collision = false;
        if (enemyPos.y() + 25 >= shipPos.y() - 35 &&
            enemyPos.y() - 25 <= shipPos.y() + 10 &&
            enemyPos.x() + 40 >= shipPos.x() - 25 &&
            enemyPos.x() - 40 <= shipPos.x() + 25) {
            collision = true;
        }

        if (collision) {
            playerHealth_ -= 20;
            Explosion exp;
            exp.position = enemyPos;
            exp.life = 30;  // 约0.5秒的爆炸效果
            explosions_.append(exp);
            it = enemies_.erase(it);

            if (playerHealth_ <= 0) {
                playerHealth_ = 0;
                onGameOver();
                return;
            }
            continue;
        }

        ++it;
    }
}

void SpaceBattleWidget::updateBullets()
{
    for (auto it = bullets_.begin(); it != bullets_.end();) {
        int targetIndex = it->targetIndex;
        
        if (targetIndex < 0 || targetIndex >= enemies_.size()) {
            it = bullets_.erase(it);
            continue;
        }
        
        Enemy& targetEnemy = enemies_[targetIndex];
        
        if (!targetEnemy.isWaitingForBullet()) {
            it = bullets_.erase(it);
            continue;
        }
        
        QPointF targetPos = targetEnemy.position();
        qreal dx = targetPos.x() - it->position.x();
        qreal dy = targetPos.y() - it->position.y();
        qreal distance = qSqrt(dx * dx + dy * dy);
        
        if (distance > 5) {
            it->position.rx() += (dx / distance) * 15;
            it->position.ry() += (dy / distance) * 15;
        }
        
        if (distance <= 25) {
            Explosion exp;
            exp.position = targetPos;
            exp.life = 30;  // 约0.5秒的爆炸效果
            explosions_.append(exp);
            enemies_.removeAt(targetIndex);
            destroyedCount_++;
            it = bullets_.erase(it);
            continue;
        }

        ++it;
    }
}

void SpaceBattleWidget::drawExplosion(QPainter &painter, const QPointF& pos, int life)
{
    painter.save();

    // 根据生命值计算透明度和大小
    qreal alpha = life / 30.0;  // 生命值从30到0，透明度从1到0
    qreal scale = 0.5 + (life / 30.0) * 0.5;  // 大小从0.5倍到1倍

    QRadialGradient explosionGradient(pos, 40 * scale);
    explosionGradient.setColorAt(0, QColor(255, 255, 200, 255 * alpha));
    explosionGradient.setColorAt(0.3, QColor(255, 200, 50, 200 * alpha));
    explosionGradient.setColorAt(0.6, QColor(255, 100, 0, 150 * alpha));
    explosionGradient.setColorAt(1, QColor(255, 0, 0, 0));

    painter.setBrush(explosionGradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(pos, 40 * scale, 40 * scale);

    painter.restore();
}

QIcon SpaceBattleWidget::createGearIcon()
{
    QPixmap pixmap(25, 25);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制齿轮
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

    // 绘制中心孔
    painter.setBrush(QColor(96, 125, 139));
    painter.drawEllipse(QPoint(centerX, centerY), 3, 3);

    painter.end();
    return QIcon(pixmap);
}

void SpaceBattleWidget::spawnEnemy()
{
    const int maxEnemiesOnScreen = 8;
    const int enemyWidth = 70;
    const int minSpacing = 25;
    
    if (enemies_.size() >= maxEnemiesOnScreen) {
        return;
    }
    
    int countToSpawn = qMin(spawnCount_, maxEnemiesOnScreen - enemies_.size());
    
    for (int i = 0; i < countToSpawn; ++i) {
        QString word = wordManager_->getRandomWord();
        
        qreal x = 50 + QRandomGenerator::global()->bounded(width() - 120);
        
        for (int retry = 0; retry < 10; ++retry) {
            bool overlaps = false;
            qreal minX = x - enemyWidth / 2 - minSpacing;
            qreal maxX = x + enemyWidth / 2 + minSpacing;
            
            for (const Enemy& existing : enemies_) {
                qreal existingMinX = existing.position().x() - enemyWidth / 2;
                qreal existingMaxX = existing.position().x() + enemyWidth / 2;
                
                if (minX < existingMaxX && maxX > existingMinX) {
                    overlaps = true;
                    break;
                }
            }
            
            if (!overlaps) {
                break;
            }
            
            x = 50 + QRandomGenerator::global()->bounded(width() - 120);
        }
        
        Enemy enemy(word, QPointF(x, -50 - i * 120));
        enemies_.append(enemy);
    }
}

void SpaceBattleWidget::onWordsReady()
{
    LOG_INFO(QString("Words ready for Space Battle. AI enabled: %1").arg(wordManager_->isUsingAI()));
}

void SpaceBattleWidget::updateGame()
{
    if (!isRunning_ || isPaused_) return;

    if (shipX_ == 0) {
        shipX_ = width() / 2;
    }
    
    shipX_ += shipSpeed_ * shipDirection_;
    if (shipX_ >= width() - 25) {
        shipX_ = width() - 25;
        shipDirection_ = -1;
    } else if (shipX_ <= 25) {
        shipX_ = 25;
        shipDirection_ = 1;
    }

    checkCollisions();
    updateBullets();

    // 敌机更新逻辑
    for (auto it = enemies_.begin(); it != enemies_.end();) {
        if (it->isCompleted() && !it->isWaitingForBullet()) {
            it = enemies_.erase(it);
            continue;
        }

        it->updatePosition(fallSpeed_);

        if (it->isOutOfBounds()) {
            missed_++;
            combo_ = 0;
            scoreBoard_->updateMissed(missed_);
            scoreBoard_->updateCombo(combo_);
            it = enemies_.erase(it);
            continue;
        }

        ++it;
    }

    // 爆炸效果淡出
    for (auto it = explosions_.begin(); it != explosions_.end();) {
        it->life--;
        if (it->life <= 0) {
            it = explosions_.erase(it);
        } else {
            ++it;
        }
    }

    update();
}

void SpaceBattleWidget::onStartClicked()
{
    if (!isRunning_) {
        score_ = 0;
        missed_ = 0;
        combo_ = 0;
        totalTyped_ = 0;
        correctTyped_ = 0;
        elapsed_ = 0;
        destroyedCount_ = 0;
        enemies_.clear();
        explosions_.clear();
        bullets_.clear();
        shipDirection_ = 1;
        playerHealth_ = maxHealth_;
        shipX_ = width() / 2;

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

void SpaceBattleWidget::onPauseClicked()
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

void SpaceBattleWidget::onRestartClicked()
{
    score_ = 0;
    missed_ = 0;
    combo_ = 0;
    totalTyped_ = 0;
    correctTyped_ = 0;
    elapsed_ = 0;
    destroyedCount_ = 0;
    enemies_.clear();
    explosions_.clear();
    bullets_.clear();
    playerHealth_ = maxHealth_;
    shipX_ = width() / 2;

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

void SpaceBattleWidget::onBackToMenuClicked()
{
    emit backToMenu();
}

void SpaceBattleWidget::onGameOver()
{
    isRunning_ = false;
    spawnTimer_->stop();
    gameTimer_->stop();

    finalScoreLabel_->setText(tr("Final Score: %1").arg(score_));
    double accuracy = (totalTyped_ > 0) ? (double)correctTyped_ / totalTyped_ * 100.0 : 100.0;
    finalAccuracyLabel_->setText(tr("Accuracy: %1%").arg(accuracy, 0, 'f', 1));
    destroyedLabel_->setText(tr("Enemies Destroyed: %1").arg(destroyedCount_));
    stackedWidget_->setCurrentWidget(gameOverWidget_);
}

void SpaceBattleWidget::showSettings()
{
    QDialog *settingsDialog = new QDialog(this);
    settingsDialog->setWindowTitle(tr("Settings"));
    settingsDialog->setFixedSize(450, 550);
    settingsDialog->setStyleSheet("QDialog { background-color: #f5f5f5; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(settingsDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel(tr("Game Settings"), settingsDialog);
    titleLabel->setStyleSheet("QLabel { color: #333; font-size: 24px; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(25);

    // 游戏设置
    QGroupBox *gameGroup = new QGroupBox(tr("Game Settings"), settingsDialog);
    gameGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; margin-top: 10px; }");
    QVBoxLayout *gameLayout = new QVBoxLayout(gameGroup);

    // 下落速度
    QLabel *speedLabel = new QLabel(tr("Fall Speed:"), settingsDialog);
    gameLayout->addWidget(speedLabel);

    QSlider *speedSlider = new QSlider(Qt::Horizontal, settingsDialog);
    speedSlider->setRange(5, 30); // 0.5 到 3.0
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

    // 飞机速度
    QLabel *shipSpeedLabel = new QLabel(tr("Ship Speed:"), settingsDialog);
    gameLayout->addWidget(shipSpeedLabel);

    QSlider *shipSpeedSlider = new QSlider(Qt::Horizontal, settingsDialog);
    shipSpeedSlider->setRange(1, 10);
    shipSpeedSlider->setValue(shipSpeed_);
    shipSpeedSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 10px; background: #ddd; border-radius: 5px; }"
        "QSlider::handle:horizontal { background: #00BCD4; border: none; width: 20px; height: 20px; margin: -5px 0; border-radius: 50%; }"
    );

    QLabel *shipSpeedValueLabel = new QLabel(QString::number(shipSpeed_), settingsDialog);
    shipSpeedValueLabel->setAlignment(Qt::AlignRight);
    gameLayout->addWidget(shipSpeedSlider);
    gameLayout->addWidget(shipSpeedValueLabel);

    connect(shipSpeedSlider, &QSlider::valueChanged, this, [this, shipSpeedValueLabel](int value) {
        shipSpeed_ = value;
        shipSpeedValueLabel->setText(QString::number(value));
    });

    gameLayout->addSpacing(10);

    // 每次生成数量
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

    // 生成间隔
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

    // 音频设置
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
        "QPushButton { background-color: #2196F3; color: white; border: none; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1976D2; }"
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

void SpaceBattleWidget::retranslateUI()
{
    startButton_->setText(tr("Start"));
    pauseButton_->setText(isPaused_ ? tr("Resume") : tr("Pause"));
    restartButton_->setText(tr("Restart"));
    backButton_->setText(tr("Back"));
    menuButton_->setText(tr("Back to Menu"));
    titleLabel_->setText(tr("Space Battle"));
    
    if (menuTitle_) {
        menuTitle_->setText(tr("Space Battle"));
    }
    if (menuSubtitle_) {
        menuSubtitle_->setText(tr("Type the words to destroy enemy spaceships!"));
    }
    if (gameOverTitle_) {
        gameOverTitle_->setText(tr("Mission Complete"));
    }
    
    if (isRunning_) {
        finalScoreLabel_->setText(tr("Final Score: %1").arg(score_));
        double accuracy = (totalTyped_ > 0) ? (double)correctTyped_ / totalTyped_ * 100.0 : 100.0;
        finalAccuracyLabel_->setText(tr("Accuracy: %1%").arg(accuracy, 0, 'f', 1));
        destroyedLabel_->setText(tr("Enemies Destroyed: %1").arg(destroyedCount_));
    }
}
