#include "mainwindow.h"
#include "core/logger.h"
#include "apple_game/apple_game_widget.h"
#include "space_battle/space_battle_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStringList>
#include <QPalette>
#include <QColor>
#include <QFont>
#include <QSettings>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QTranslator>
#include <QApplication>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stackedWidget_(nullptr)
    , menuWidget_(nullptr)
    , titleLabel_(nullptr)
    , subtitleLabel_(nullptr)
    , settingsButton_(nullptr)
    , appleGame_(nullptr)
    , spaceBattleGame_(nullptr)
    , translator_(nullptr)
    , isDarkMode_(false)
{
    LOG_INFO("MainWindow initializing...");
    setWindowTitle(tr("TypingHero - Typing Game Platform"));
    resize(1200, 800);
    setMinimumSize(1000, 700);

    setupUI();
    LOG_INFO("MainWindow initialized successfully");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    stackedWidget_ = new QStackedWidget(this);

    menuWidget_ = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(menuWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(30, 20, 30, 20);
    headerWidget->setStyleSheet("background-color: #1a1a2e;");

    titleLabel_ = new QLabel(tr("TypingHero"));
    titleLabel_->setStyleSheet("QLabel { color: white; font-size: 36px; font-weight: bold; }");
    headerLayout->addWidget(titleLabel_);

    headerLayout->addStretch();

    langButton_ = new QPushButton(tr("Language"));
    langButton_->setStyleSheet(
        "QPushButton { color: white; background-color: #9b59b6; "
        "border: none; border-radius: 20px; "
        "padding: 8px 20px; font-size: 14px; font-weight: bold; }"
    );
    headerLayout->addWidget(langButton_);
    connect(langButton_, &QPushButton::clicked, this, &MainWindow::switchLanguage);

    settingsButton_ = new QPushButton(tr("Settings"));
    settingsButton_->setStyleSheet(
        "QPushButton { color: white; background-color: #45b7d1; "
        "border: none; border-radius: 20px; "
        "padding: 8px 20px; font-size: 14px; font-weight: bold; }"
    );
    headerLayout->addWidget(settingsButton_);

    mainLayout->addWidget(headerWidget);

    QWidget *bannerWidget = new QWidget();
    QVBoxLayout *bannerLayout = new QVBoxLayout(bannerWidget);
    bannerLayout->setContentsMargins(30, 60, 30, 60);
    bannerLayout->setAlignment(Qt::AlignCenter);
    bannerWidget->setStyleSheet("background-color: #f0f0f0;");

    bannerTitle_ = new QLabel(tr("Welcome to TypingHero!"));
    bannerTitle_->setStyleSheet("QLabel { color: #333; font-size: 42px; font-weight: bold; }");
    bannerTitle_->setAlignment(Qt::AlignCenter);
    bannerLayout->addWidget(bannerTitle_);

    subtitleLabel_ = new QLabel(tr("Choose a game to start your typing adventure"));
    subtitleLabel_->setStyleSheet("QLabel { color: #666; font-size: 20px; }");
    subtitleLabel_->setAlignment(Qt::AlignCenter);
    bannerLayout->addWidget(subtitleLabel_);

    mainLayout->addWidget(bannerWidget);

    QWidget *gamesWidget = new QWidget();
    QVBoxLayout *gamesLayout = new QVBoxLayout(gamesWidget);
    gamesLayout->setContentsMargins(80, 40, 80, 40);
    gamesWidget->setStyleSheet("background-color: #ffffff;");

    gamesTitle_ = new QLabel(tr("Select Game"));
    gamesTitle_->setStyleSheet("QLabel { color: #1a1a2e; font-size: 24px; font-weight: bold; margin-bottom: 30px; }");
    gamesLayout->addWidget(gamesTitle_);

    QWidget *gamesGrid = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(gamesGrid);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setHorizontalSpacing(40);
    gridLayout->setVerticalSpacing(40);
    gridLayout->setAlignment(Qt::AlignCenter);

    createGameButtons();

    for (int i = 0; i < gameButtons_.size(); ++i) {
        gridLayout->addWidget(gameButtons_[i], i / 3, i % 3);
    }

    gamesLayout->addWidget(gamesGrid);
    gamesLayout->addStretch();

    mainLayout->addWidget(gamesWidget);

    stackedWidget_->addWidget(menuWidget_);

    appleGame_ = new AppleGameWidget();
    stackedWidget_->addWidget(appleGame_);

    spaceBattleGame_ = new SpaceBattleWidget();
    stackedWidget_->addWidget(spaceBattleGame_);

    connect(settingsButton_, &QPushButton::clicked, this, &MainWindow::showSettingsDialog);

    connect(appleGame_, &AppleGameWidget::backToMenu, this, &MainWindow::onBackToMenu);
    connect(spaceBattleGame_, &SpaceBattleWidget::backToMenu, this, &MainWindow::onBackToMenu);

    setCentralWidget(stackedWidget_);
    stackedWidget_->setCurrentWidget(menuWidget_);
}

void MainWindow::createGameButtons()
{
    buttonTexts_ = {
        tr("Save Apple"),
        tr("Space Battle")
    };

    QStringList buttonIcons = {
        "Apple", "Space"
    };

    QStringList colors = {
        "#ff6b6b",
        "#4ecdc4"
    };

    for (int i = 0; i < buttonTexts_.size(); ++i) {
        QPushButton *btn = new QPushButton();
        btn->setFixedSize(220, 200);
        btn->setStyleSheet(
            QString(
                "QPushButton { "
                "background-color: %1; "
                "border: none; border-radius: 20px; "
                "color: white; font-size: 18px; font-weight: bold; "
                "text-align: center; padding: 25px; "
                "}"
            ).arg(colors[i])
        );
        btn->setText(QString("%1\n\n%2").arg(buttonIcons[i], buttonTexts_[i]));

        int index = i;
        connect(btn, &QPushButton::clicked, this, [this, index]() {
            onGameButtonClicked(index);
        });

        gameButtons_.append(btn);
    }
}

void MainWindow::onGameButtonClicked(int gameIndex)
{
    LOG_INFO(QString("Game button clicked, index: %1").arg(gameIndex));
    
    switch (gameIndex) {
    case 0:
        LOG_INFO("Starting Apple Game");
        stackedWidget_->setCurrentWidget(appleGame_);
        appleGame_->showGame();
        break;
    case 1:
        LOG_INFO("Starting Space Battle Game");
        stackedWidget_->setCurrentWidget(spaceBattleGame_);
        spaceBattleGame_->showGame();
        break;
    default:
        LOG_WARNING(QString("Unknown game index: %1").arg(gameIndex));
        break;
    }
}

void MainWindow::onBackToMenu()
{
    // 确保两个游戏都隐藏并停止音乐
    if (appleGame_) {
        appleGame_->hideGame();
    }
    if (spaceBattleGame_) {
        spaceBattleGame_->hideGame();
    }
    stackedWidget_->setCurrentWidget(menuWidget_);
}

void MainWindow::showSettingsDialog()
{
    QDialog *settingsDialog = new QDialog(this);
    settingsDialog->setWindowTitle(tr("Settings"));
    settingsDialog->setFixedSize(500, 500);
    settingsDialog->setStyleSheet("QDialog { background-color: #f8f9fa; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(settingsDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel(tr("Settings"), settingsDialog);
    titleLabel->setStyleSheet("QLabel { color: #333; font-size: 24px; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(25);

    QGroupBox *langGroup = new QGroupBox(tr("Language"));
    langGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; }");
    QVBoxLayout *langLayout = new QVBoxLayout(langGroup);

    QLabel *langLabel = new QLabel(tr("Select Language:"));
    langLayout->addWidget(langLabel);

    QComboBox *langComboBox = new QComboBox();
    langComboBox->addItem("English", "en_US");
    langComboBox->addItem("中文", "zh_CN");
    langComboBox->setStyleSheet(
        "QComboBox { padding: 6px; border: 1px solid #ddd; border-radius: 4px; }"
        "QComboBox::drop-down { border: none; width: 20px; }"
    );
    langLayout->addWidget(langComboBox);

    mainLayout->addWidget(langGroup);

    QGroupBox *gameGroup = new QGroupBox(tr("Game Settings"));
    gameGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; }");
    QFormLayout *gameLayout = new QFormLayout(gameGroup);

    QSlider *speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(5);
    speedSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 8px; background: #ddd; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: #4ecdc4; border: none; width: 20px; height: 20px; border-radius: 50%; }"
    );
    gameLayout->addRow(tr("Game Speed:"), speedSlider);

    QCheckBox *soundCheckBox = new QCheckBox(tr("Enable Sound"));
    soundCheckBox->setChecked(true);
    gameLayout->addRow(soundCheckBox);

    QCheckBox *aiCheckBox = new QCheckBox(tr("Enable AI Word Generation"));
    aiCheckBox->setChecked(true);
    gameLayout->addRow(aiCheckBox);

    mainLayout->addWidget(gameGroup);

    QGroupBox *appearanceGroup = new QGroupBox(tr("Appearance"));
    appearanceGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; }");
    QFormLayout *appearanceLayout = new QFormLayout(appearanceGroup);

    QCheckBox *darkModeCheckBox = new QCheckBox(tr("Dark Mode"));
    darkModeCheckBox->setChecked(isDarkMode_);
    appearanceLayout->addRow(darkModeCheckBox);

    mainLayout->addWidget(appearanceGroup);

    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setStyleSheet(
        "QPushButton { background-color: #4ecdc4; color: white; border: none; "
        "border-radius: 8px; padding: 10px 30px; font-weight: bold; }"
        "QPushButton:hover { background-color: #44a08d; }"
    );
    buttonLayout->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setStyleSheet(
        "QPushButton { background-color: #6c757d; color: white; border: none; "
        "border-radius: 8px; padding: 10px 30px; font-weight: bold; }"
        "QPushButton:hover { background-color: #5a6268; }"
    );
    buttonLayout->addWidget(cancelButton);

    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, [this, settingsDialog, langComboBox, darkModeCheckBox]() {
        QString lang = langComboBox->currentData().toString();
        
        if (lang == "zh_CN") {
            if (translator_) {
                qApp->removeTranslator(translator_);
                delete translator_;
            }
            translator_ = new QTranslator(this);
            if (translator_->load(":/mui/typing_game_zh_CN.qm")) {
                qApp->installTranslator(translator_);
            }
        } else if (lang == "en_US") {
            if (translator_) {
                qApp->removeTranslator(translator_);
                delete translator_;
            }
            translator_ = new QTranslator(this);
            if (translator_->load(":/mui/typing_game_en_US.qm")) {
                qApp->installTranslator(translator_);
            }
        }
        
        // 应用深色模式
        isDarkMode_ = darkModeCheckBox->isChecked();
        applyDarkMode(isDarkMode_);
        
        retranslateUI();
        settingsDialog->accept();
    });
    connect(cancelButton, &QPushButton::clicked, settingsDialog, &QDialog::reject);

    settingsDialog->exec();
}

void MainWindow::switchLanguage()
{
    LOG_INFO("Opening language selection dialog");
    QDialog *langDialog = new QDialog(this);
    langDialog->setWindowTitle(tr("Select Language"));
    langDialog->setFixedSize(300, 200);
    langDialog->setStyleSheet("QDialog { background-color: #f5f5f5; }");
    langDialog->setWindowFlags(langDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(langDialog);

    QLabel *titleLabel = new QLabel(tr("Choose Language:"), langDialog);
    titleLabel->setStyleSheet("QLabel { color: #333; font-size: 18px; font-weight: bold; margin-bottom: 20px; }");
    mainLayout->addWidget(titleLabel);

    QPushButton *chineseButton = new QPushButton(tr("Chinese"), langDialog);
    chineseButton->setFixedHeight(40);
    chineseButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #c0392b; }"
    );
    mainLayout->addWidget(chineseButton);

    QPushButton *englishButton = new QPushButton(tr("English"), langDialog);
    englishButton->setFixedHeight(40);
    englishButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #2980b9; }"
    );
    mainLayout->addWidget(englishButton);

    mainLayout->addStretch();

    connect(chineseButton, &QPushButton::clicked, this, [this, langDialog]() {
        LOG_INFO("Switching to Chinese language");
        if (translator_) {
            qApp->removeTranslator(translator_);
            delete translator_;
        }
        translator_ = new QTranslator(this);
        if (translator_->load(":/mui/typing_game_zh_CN.qm")) {
            qApp->installTranslator(translator_);
            LOG_INFO("Chinese translation loaded successfully");
        } else {
            LOG_ERROR("Failed to load Chinese translation");
        }
        langDialog->accept();
        retranslateUI();
    });

    connect(englishButton, &QPushButton::clicked, this, [this, langDialog]() {
        LOG_INFO("Switching to English language");
        if (translator_) {
            qApp->removeTranslator(translator_);
            delete translator_;
        }
        translator_ = new QTranslator(this);
        if (translator_->load(":/mui/typing_game_en_US.qm")) {
            qApp->installTranslator(translator_);
            LOG_INFO("English translation loaded successfully");
        } else {
            LOG_ERROR("Failed to load English translation");
        }
        langDialog->accept();
        retranslateUI();
    });

    langDialog->exec();
    delete langDialog;
}

void MainWindow::applyDarkMode(bool enabled)
{
    LOG_INFO(QString("Applying dark mode: %1").arg(enabled));
    if (enabled) {
        // 深色模式样式
        menuWidget_->setStyleSheet(
            "QWidget { background-color: #1a1a2e; }"
            "QLabel { color: #ffffff; }"
        );
        
        // 更新banner区域
        QWidget *bannerWidget = subtitleLabel_->parentWidget();
        if (bannerWidget) {
            bannerWidget->setStyleSheet("background-color: #16213e;");
        }
        bannerTitle_->setStyleSheet("QLabel { color: #ffffff; font-size: 42px; font-weight: bold; }");
        subtitleLabel_->setStyleSheet("QLabel { color: #a0a0a0; font-size: 20px; }");
        
        // 更新游戏选择区域
        QWidget *gamesWidget = gamesTitle_->parentWidget();
        if (gamesWidget) {
            gamesWidget->setStyleSheet("background-color: #0f3460;");
        }
        gamesTitle_->setStyleSheet("QLabel { color: #ffffff; font-size: 24px; font-weight: bold; margin-bottom: 30px; }");
        LOG_INFO("Dark mode applied successfully");
    } else {
        // 浅色模式样式（恢复默认）
        menuWidget_->setStyleSheet("");
        
        // 恢复banner区域
        QWidget *bannerWidget = subtitleLabel_->parentWidget();
        if (bannerWidget) {
            bannerWidget->setStyleSheet("background-color: #f0f0f0;");
        }
        bannerTitle_->setStyleSheet("QLabel { color: #333; font-size: 42px; font-weight: bold; }");
        subtitleLabel_->setStyleSheet("QLabel { color: #666; font-size: 20px; }");
        
        // 恢复游戏选择区域
        QWidget *gamesWidget = gamesTitle_->parentWidget();
        if (gamesWidget) {
            gamesWidget->setStyleSheet("background-color: #ffffff;");
        }
        gamesTitle_->setStyleSheet("QLabel { color: #1a1a2e; font-size: 24px; font-weight: bold; margin-bottom: 30px; }");
        LOG_INFO("Light mode applied successfully");
    }
}

void MainWindow::retranslateUI()
{
    setWindowTitle(tr("TypingHero - Typing Game Platform"));
    titleLabel_->setText(tr("TypingHero"));
    langButton_->setText(tr("Language"));
    settingsButton_->setText(tr("Settings"));
    bannerTitle_->setText(tr("Welcome to TypingHero!"));
    subtitleLabel_->setText(tr("Choose a game to start your typing adventure"));
    gamesTitle_->setText(tr("Select Game"));
    
    buttonTexts_ = {
        tr("Save Apple"),
        tr("Space Battle")
    };
    
    QStringList buttonIcons = {
        "Apple", "Space"
    };
    
    for (int i = 0; i < gameButtons_.size(); ++i) {
        gameButtons_[i]->setText(QString("%1\n\n%2").arg(buttonIcons[i], buttonTexts_[i]));
    }
    
    if (appleGame_) {
        appleGame_->retranslateUI();
    }
    if (spaceBattleGame_) {
        spaceBattleGame_->retranslateUI();
    }
}
