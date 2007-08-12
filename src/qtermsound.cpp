#include "qtermsound.h"
// #include <qsound.h>
// #include <qfile.h>
// #include <qmessagebox.h>
//Added by qt3to4:
// #include <Q3CString>
#include <QSound>
#include <QFile>
#include <QMessageBox>
#include <stdlib.h>
/*
#ifndef _NO_ESD_COMPILED
#include <esd.h>
#endif

#ifndef _NO_ARTS_COMPILED
#include <soundserver.h>
using namespace Arts;
#endif*/
namespace QTerm
{
extern void runProgram(const QString&);

Sound::~Sound()
{
}

void
InternalSound::play()
{
	if(QFile::exists(_soundfile))
		QSound::play(_soundfile);
}
/*
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
*/
void
ExternalSound::setPlayer(const QString & playername)
{
	_player = playername;
}

void
ExternalSound::play()
{
	if(QFile::exists(_soundfile)) {
		QString command = _player + ' ' + _soundfile;
		runProgram(command);
	}
}

} // namespace QTerm
