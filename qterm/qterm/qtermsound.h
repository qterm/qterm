#include "qterm.h"
#include <qstring.h>
#include <qobject.h>

class QTermSound : public QObject
{
protected:
	QString _soundfile;
public:
	QTermSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:QObject(parent, name),
		 _soundfile(filename)
	{
	}
	~QTermSound();
	virtual void play() = 0;
};

// Play sound using qsound.
class QTermInternalSound : public QTermSound
{
public:
	QTermInternalSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:QTermSound(filename, parent, name)
	{
	}
	void play();
};

#ifndef _NO_ARTS_COMPILED
// Play sound using arts.
class QTermArtsSound : public QTermSound
{
public:
	QTermArtsSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:QTermSound(filename, parent, name)
	{
	}
	void play();
};
#endif

#ifndef _NO_ESD_COMPILED
// Play sound using esd.
class QTermEsdSound : public QTermSound
{
public:
	QTermEsdSound(const QString & filename, QObject * parent = 0, const char * name = 0)
		:QTermSound(filename, parent, name)
	{
	}
	void play();
};
#endif

class QTermExternalSound : public QTermSound
{
private:
	QString _player;
public:
	QTermExternalSound(const QString & playername, QString & filename, QObject * parent = 0, const char * name = 0)
		:QTermSound(filename, parent, name),
		 _player(playername)
	{
	}
	void play();
	void setPlayer(const QString & playername);
};
