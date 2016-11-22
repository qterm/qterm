#include "qterm.h"
#include <QObject>
#include <QString>

namespace QTerm
{
class Sound : public QObject
{
protected:
    QString _soundfile;
public:
    Sound(const QString & filename, QObject * parent = 0, const char * name = 0)
            : QObject(parent),
            _soundfile(filename) {
    }
    ~Sound();
    virtual void play() = 0;
};

#ifdef PHONON_ENABLED
class PhononSound : public Sound
{
public:
    PhononSound(const QString & filename, QObject * parent = 0, const char * name = 0)
            : Sound(filename, parent, name) {
    }
    void play();
};
#endif // PHONON_ENABLED

#ifdef QMEDIAPLAYER_ENABLED
class QMediaPlayerSound : public Sound
{
public:
    QMediaPlayerSound(const QString & filename, QObject * parent = 0, const char * name = 0)
            : Sound(filename, parent, name) {
    }
    void play();
};
#endif // QMEDIAPLAYER_ENABLED

class ExternalSound : public Sound
{
private:
    QString _player;
public:
    ExternalSound(const QString & playername, const QString & filename, QObject * parent = 0, const char * name = 0)
            : Sound(filename, parent, name),
            _player(playername) {
    }
    void play();
    void setPlayer(const QString & playername);
};

} // namespace QTerm

