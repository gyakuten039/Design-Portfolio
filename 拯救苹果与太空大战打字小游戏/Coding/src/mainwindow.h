#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QTranslator>

class AppleGameWidget;
class SpaceBattleWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGameButtonClicked(int gameIndex);
    void onBackToMenu();
    void showSettingsDialog();
    void switchLanguage();

private:
    void setupUI();
    void createGameButtons();
    void retranslateUI();
    void applyDarkMode(bool enabled);

    QStackedWidget *stackedWidget_;
    QWidget *menuWidget_;
    QLabel *titleLabel_;
    QLabel *subtitleLabel_;
    QLabel *bannerTitle_;
    QLabel *gamesTitle_;
    QPushButton *settingsButton_;
    QPushButton *langButton_;
    QList<QPushButton*> gameButtons_;
    QStringList buttonTexts_;

    AppleGameWidget *appleGame_;
    SpaceBattleWidget *spaceBattleGame_;
    QTranslator *translator_;
    bool isDarkMode_;
};

#endif // MAINWINDOW_H
