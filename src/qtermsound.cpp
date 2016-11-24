#include "qtermsound.h"
#include <QtCore/QFile>
#include <QtCore/QProcess>

#ifdef PHONON_ENABLED
#ifdef OLD_PHONON
#include <phonon>
#else
#include <Phonon/MediaObject>
#endif
#endif // PHONON_ENABLED

#ifdef QMEDIAPLAYER_ENABLED
#include <QMediaPlayer>
#endif // QMEDIAPLAYER_ENABLED

namespace QTerm
{

Sound::~Sound()
{
}

#ifdef PHONON_ENABLED
void
PhononSound::play()
{
    if (QFile::exists(_soundfile)) {
        Phonon::MediaObject * player = Phonon::createPlayer(Phonon::NotificationCategory, Phonon::MediaSource(_soundfile));
        player->play();
    }
}
#endif // PHONON_ENABLED

#ifdef QMEDIAPLAYER_ENABLED
void
QMediaPlayerSound::play()
{
    if (QFile::exists(_soundfile)) {
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl::fromLocalFile(_soundfile));
        player->play();
    }
}
#endif // QMEDIAPLAYER_ENABLED

void
ExternalSound::setPlayer(const QString & playername)
{
    _player = playername;
}

void
ExternalSound::play()
{
    if (QFile::exists(_soundfile)) {
        QString command = _player + ' ' + _soundfile;
        QProcess::startDetached(command);
    }
}

} // namespace QTerm
