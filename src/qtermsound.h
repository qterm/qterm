#include "qterm.h"
// #include <qstring.h>
// #include <qobject.h>
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
		:QObject(parent),
		 _soundfile(filename)
	{
	}
	~Sound();
	virtual void play() = 0;
};

// Play sound using qsound.
class InternalSound : public Sound
{
public:
	InternalSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:Sound(filename, parent, name)
	{
	}
	void play();
};
/*
#ifndef _NO_ARTS_COMPILED
// Play sound using arts.
class QTermArtsSound : public Sound
{
public:
	QTermArtsSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:Sound(filename, parent, name)
	{
	}
	void play();
};
#endif

#ifndef _NO_ESD_COMPILED
// Play sound using esd.
class QTermEsdSound : public Sound
{
public:
	QTermEsdSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:Sound(filename, parent, name)
	{
	}
	void play();
};
#endif
*/
class ExternalSound : public Sound
{
private:
	QString _player;
public:
	ExternalSound(const QString & playername, const QString & filename, QObject * parent = 0, const char * name = 0)
		:Sound(filename, parent, name),
		 _player(playername)
	{
	}
	void play();
	void setPlayer(const QString & playername);
};

} // namespace QTerm

