#ifndef WORD_MANAGER_H
#define WORD_MANAGER_H

#include <QObject>
#include <QStringList>
#include <QTimer>

class AIWordGenerator;

class WordManager : public QObject
{
    Q_OBJECT

public:
    explicit WordManager(QObject *parent = nullptr);
    ~WordManager();

    void initialize();
    QString getRandomWord();
    QStringList getWordList() const;
    bool isUsingAI() const;

    void setUseAI(bool use);
    void setWordLengthRange(int min, int max);
    void refreshWords(int count = 50);

signals:
    void wordsReady();
    void errorOccurred(const QString &error);

private slots:
    void onWordsGenerated(const QStringList &words);
    void onAIError(const QString &error);
    void onFallbackTimer();

private:
    AIWordGenerator *aiGenerator_;
    QStringList wordList_;
    QStringList fallbackWords_;
    bool useAI_;
    int minLength_;
    int maxLength_;
    bool isReady_;

    void loadFallbackWords();
    void ensureMinimumWords();
};

#endif // WORD_MANAGER_H
