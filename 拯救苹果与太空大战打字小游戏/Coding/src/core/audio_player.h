#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <QObject>
#include <QString>

class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void playMusic(const QString &filePath);
    void pauseMusic();
    void resumeMusic();
    void stopMusic();
    void setVolume(int volume);
    bool isPlaying() const;
    bool isPaused() const;

private:
    QString currentFile_;
    bool isPlaying_;
    bool isPaused_;
};

#endif // AUDIO_PLAYER_H
