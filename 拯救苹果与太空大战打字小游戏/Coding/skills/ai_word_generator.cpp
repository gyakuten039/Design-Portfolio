#include "ai_word_generator.h"
#include <QProcessEnvironment>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

const QString AIWordGenerator::ZHIPU_API_URL = "https://open.bigmodel.cn/api/paas/v4/chat/completions";
const QString AIWordGenerator::BAIDU_API_URL = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/completions";

AIWordGenerator::AIWordGenerator(QObject *parent)
    : QObject(parent)
    , networkManager_(new QNetworkAccessManager(this))
    , apiUrl_(ZHIPU_API_URL)
    , isAvailable_(false)
    , currentProvider_(AIProvider::ZhipuAI)
{
    loadApiKeyFromEnvironment();
}

AIWordGenerator::~AIWordGenerator()
{
}

void AIWordGenerator::setProvider(AIProvider provider)
{
    currentProvider_ = provider;
    if (provider == AIProvider::ZhipuAI) {
        apiUrl_ = ZHIPU_API_URL;
    } else if (provider == AIProvider::BaiduERNIE) {
        apiUrl_ = BAIDU_API_URL;
    }
}

void AIWordGenerator::loadApiKeyFromEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    
    apiKey_ = env.value("ZHIPU_API_KEY", "");
    
    if (apiKey_.isEmpty()) {
        apiKey_ = env.value("BAIDU_API_KEY", "");
        if (!apiKey_.isEmpty()) {
            currentProvider_ = AIProvider::BaiduERNIE;
            apiUrl_ = BAIDU_API_URL;
        }
    }
    
    if (apiKey_.isEmpty()) {
        apiKey_ = env.value("ALI_API_KEY", "");
        if (!apiKey_.isEmpty()) {
            currentProvider_ = AIProvider::AliQWen;
        }
    }
    
    if (apiKey_.isEmpty()) {
        apiKey_ = env.value("AI_API_KEY", "");
    }
    
    isAvailable_ = !apiKey_.isEmpty();
    
    if (!isAvailable_) {
        lastError_ = tr("API key not found. Please set ZHIPU_API_KEY, BAIDU_API_KEY, or AI_API_KEY environment variable.");
        qWarning() << lastError_;
    }
}

bool AIWordGenerator::isAvailable() const
{
    return isAvailable_;
}

QString AIWordGenerator::getLastError() const
{
    return lastError_;
}

QString AIWordGenerator::getCurrentProviderName() const
{
    switch (currentProvider_) {
    case AIProvider::ZhipuAI: return "智谱AI (ChatGLM)";
    case AIProvider::BaiduERNIE: return "百度文心一言";
    case AIProvider::AliQWen: return "阿里通义千问";
    default: return "Unknown";
    }
}

void AIWordGenerator::generateWords(int count, int minLength, int maxLength)
{
    if (!isAvailable_) {
        emit errorOccurred(lastError_);
        return;
    }

    QString prompt = buildPrompt(count, minLength, maxLength);

    if (currentProvider_ == AIProvider::ZhipuAI) {
        callZhipuAPI(prompt);
    } else if (currentProvider_ == AIProvider::BaiduERNIE) {
        callBaiduAPI(prompt);
    }
}

QString AIWordGenerator::buildPrompt(int count, int minLength, int maxLength)
{
    return QString(
        "Generate exactly %1 random English words for a typing game. "
        "Each word should be between %2 and %3 characters long. "
        "Words should be simple and easy to type. "
        "IMPORTANT: Return ONLY the words, one word per line, no numbers, no bullets, no explanations."
    ).arg(count).arg(minLength).arg(maxLength);
}

void AIWordGenerator::callZhipuAPI(const QString &prompt)
{
    QNetworkRequest request;
    request.setUrl(QUrl(apiUrl_));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey_).toUtf8());

    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;

    QJsonArray messages;
    messages.append(message);

    QJsonObject json;
    json["model"] = "glm-4";
    json["messages"] = messages;
    json["temperature"] = 0.8;
    json["max_tokens"] = 500;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = networkManager_->post(request, data);
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished(QNetworkReply*)));
}

void AIWordGenerator::callBaiduAPI(const QString &prompt)
{
    QNetworkRequest request;
    request.setUrl(QUrl(apiUrl_));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;

    QJsonArray messages;
    messages.append(message);

    QJsonObject json;
    json["messages"] = messages;
    json["temperature"] = 0.8;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = networkManager_->post(request, data);
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished(QNetworkReply*)));
}

void AIWordGenerator::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        lastError_ = tr("Network error: %1").arg(reply->errorString());
        emit errorOccurred(lastError_);
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QStringList words;
    
    if (currentProvider_ == AIProvider::ZhipuAI) {
        words = parseZhipuResponse(response);
    } else {
        words = parseBaiduResponse(response);
    }
    
    if (words.isEmpty()) {
        lastError_ = tr("Failed to parse AI response");
        qWarning() << "Response:" << response;
        emit errorOccurred(lastError_);
    } else {
        emit wordsGenerated(words);
    }
    
    reply->deleteLater();
}

QStringList AIWordGenerator::parseZhipuResponse(const QByteArray &response)
{
    QStringList words;
    
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (!doc.isObject()) {
        return words;
    }

    QJsonObject obj = doc.object();
    QJsonArray choices = obj.value("choices").toArray();
    
    if (choices.isEmpty()) {
        return words;
    }

    QJsonObject choice = choices.first().toObject();
    QJsonObject message = choice.value("message").toObject();
    QString content = message.value("content").toString();

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    for (QString line : lines) {
        line = line.trimmed();
        line.remove(QRegularExpression("^\\d+[.)]\\s*"));
        line.remove(QRegularExpression("^[-*•]\\s*"));
        
        if (!line.isEmpty() && line.length() >= 2 && line.length() <= 15) {
            line = line.toLower();
            if (line.contains(QRegularExpression("^[a-z]+$"))) {
                words.append(line);
            }
        }
    }

    return words;
}

QStringList AIWordGenerator::parseBaiduResponse(const QByteArray &response)
{
    QStringList words;
    
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (!doc.isObject()) {
        return words;
    }

    QJsonObject obj = doc.object();
    QJsonArray choices = obj.value("choices").toArray();
    
    if (choices.isEmpty()) {
        return words;
    }

    QJsonObject choice = choices.first().toObject();
    QString content = choice.value("message").toObject().value("content").toString();

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    for (QString line : lines) {
        line = line.trimmed();
        line.remove(QRegularExpression("^\\d+[.)]\\s*"));
        line.remove(QRegularExpression("^[-*•]\\s*"));
        
        if (!line.isEmpty() && line.length() >= 2 && line.length() <= 15) {
            line = line.toLower();
            if (line.contains(QRegularExpression("^[a-z]+$"))) {
                words.append(line);
            }
        }
    }

    return words;
}
