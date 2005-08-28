#include "qtermsound.h"
#include <qsound.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <stdlib.h>
#ifndef _NO_ESD_COMPILED
#include <esd.h>
#endif

#ifndef _NO_ARTS_COMPILED
#include <soundserver.h>
using namespace Arts;
#endif
extern void runProgram(const QCString&);

QTermSound::~QTermSound()
{
}

void
QTermInternalSound::play()
{
	if(QFile::exists(_soundfile))
		QSound::play(_soundfile);
}

#ifndef _NO_ARTS_COMPILED
void
QTermArtsSound::play()
{
	Dispatcher dispatcher;
	SimpleSoundServer server;
	server = Arts::Reference("global:Arts_SimpleSoundServer");

	if (server.isNull()){
		qWarning("Cannot connect to the sound server, check if you do have a Arts system installed\n");
		return;
	}

	if(QFile::exists(_soundfile))
		server.play(_soundfile.ascii());
}
#endif

#ifndef _NO_ESD_COMPILED
void
QTermEsdSound::play()
{
	int fd = esd_open_sound(NULL);
	if (fd >= 0 && QFile::exists(_soundfile) ) {
		esd_play_file(NULL, _soundfile.ascii(), 0);
		esd_close(fd);
	}else
	qWarning("Cannot open Esd driver, Check if you do have a Esd system installed\n");
}
#endif

void
QTermExternalSound::setPlayer(const QString & playername)
{
	_player = playername;
}

void
QTermExternalSound::play()
{
	if(QFile::exists(_soundfile)) {
		QString command = _player + ' ' + _soundfile;
		runProgram(command.local8Bit());
	}
}
