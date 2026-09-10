#ifndef AI_WORD_GENERATOR_H
#define AI_WORD_GENERATOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>

class AIWordGenerator : public QObject
{
    Q_OBJECT

public:
    enum class AIProvider {
        ZhipuAI,
        BaiduERNIE,
        AliQWen
    };

    explicit AIWordGenerator(QObject *parent = nullptr);
    ~AIWordGenerator();

    void setProvider(AIProvider provider);
    void generateWords(int count, int minLength, int maxLength);
    bool isAvailable() const;
    QString getLastError() const;
    QString getCurrentProviderName() const;

signals:
    void wordsGenerated(const QStringList &words);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void loadApiKeyFromEnvironment();
    QString buildPrompt(int count, int minLength, int maxLength);
    QStringList parseZhipuResponse(const QByteArray &response);
    QStringList parseBaiduResponse(const QByteArray &response);
    void callZhipuAPI(const QString &prompt);
    void callBaiduAPI(const QString &prompt);

    QNetworkAccessManager *networkManager_;
    QString apiKey_;
    QString apiUrl_;
    QString lastError_;
    bool isAvailable_;
    AIProvider currentProvider_;

    static const QString ZHIPU_API_URL;
    static const QString BAIDU_API_URL;
};

#endif // AI_WORD_GENERATOR_H
