#include "audio_player.h"
#include "logger.h"
#include <windows.h>
#include <mmsystem.h>
#include <QFile>
#include <QDir>

#pragma comment(lib, "winmm.lib")

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , isPlaying_(false)
    , isPaused_(false)
{
}

AudioPlayer::~AudioPlayer()
{
    stopMusic();
}

void AudioPlayer::playMusic(const QString &filePath)
{
    LOG_DEBUG(QString("Attempting to play music: %1").arg(filePath));
    
    QFile file(filePath);
    if (!file.exists()) {
        LOG_ERROR(QString("Audio file not found: %1").arg(filePath));
        return;
    }

    QString closeCommand = "close bgmusic";
    mciSendStringA(closeCommand.toUtf8().constData(), NULL, 0, NULL);

    QString openCommand = "open \"" + QDir::toNativeSeparators(filePath) + "\" type mpegvideo alias bgmusic";
    MCIERROR error = mciSendStringA(openCommand.toUtf8().constData(), NULL, 0, NULL);

    if (error != 0) {
        LOG_WARNING(QString("Failed to open audio file as mpegvideo, error code: %1. Trying wav format...").arg(error));
        openCommand = "open \"" + QDir::toNativeSeparators(filePath) + "\" alias bgmusic";
        error = mciSendStringA(openCommand.toUtf8().constData(), NULL, 0, NULL);
    }

    if (error == 0) {
        QString playCommand = "play bgmusic repeat";
        mciSendStringA(playCommand.toUtf8().constData(), NULL, 0, NULL);
        isPlaying_ = true;
        isPaused_ = false;
        currentFile_ = filePath;
        LOG_INFO(QString("Music started playing: %1").arg(filePath));
    } else {
        LOG_ERROR(QString("Failed to play audio file: %1, error code: %2").arg(filePath).arg(error));
    }
}

void AudioPlayer::pauseMusic()
{
    if (isPlaying_ && !isPaused_) {
        LOG_DEBUG(QString("Pausing music: %1").arg(currentFile_));
        QString pauseCommand = "pause bgmusic";
        mciSendStringA(pauseCommand.toUtf8().constData(), NULL, 0, NULL);
        isPaused_ = true;
        LOG_INFO("Music paused");
    }
}

void AudioPlayer::resumeMusic()
{
    if (isPlaying_ && isPaused_) {
        LOG_DEBUG(QString("Resuming music: %1").arg(currentFile_));
        QString resumeCommand = "resume bgmusic";
        mciSendStringA(resumeCommand.toUtf8().constData(), NULL, 0, NULL);
        isPaused_ = false;
        LOG_INFO("Music resumed");
    }
}

void AudioPlayer::stopMusic()
{
    if (isPlaying_) {
        LOG_DEBUG(QString("Stopping music: %1").arg(currentFile_));
        QString stopCommand = "stop bgmusic";
        mciSendStringA(stopCommand.toUtf8().constData(), NULL, 0, NULL);
        QString closeCommand = "close bgmusic";
        mciSendStringA(closeCommand.toUtf8().constData(), NULL, 0, NULL);
        isPlaying_ = false;
        isPaused_ = false;
        currentFile_.clear();
        LOG_INFO("Music stopped");
    }
}

void AudioPlayer::setVolume(int volume)
{
    if (isPlaying_) {
        QString volumeCommand = QString("setaudio bgmusic volume to %1").arg(volume * 10);
        mciSendStringA(volumeCommand.toUtf8().constData(), NULL, 0, NULL);
        LOG_DEBUG(QString("Volume set to: %1").arg(volume));
    }
}

bool AudioPlayer::isPlaying() const
{
    return isPlaying_ && !isPaused_;
}

bool AudioPlayer::isPaused() const
{
    return isPaused_;
}
