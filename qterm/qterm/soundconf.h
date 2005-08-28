#ifndef FSOUNDCONF_H
#define FSOUNDCONF_H

#include "soundconfui.h"
class QTermSound;

class fSoundConf : public fSoundConfUI
{
    Q_OBJECT

public:
    fSoundConf( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~fSoundConf();
    void loadSetting();
    void saveSetting();

public slots:
    void onSelectFile();
    void onSelectProg();
    void onPlayMethod( int id );
    void onTestPlay();
protected slots:
    void accept();

private:
    QTermSound * m_pSound;
};

#endif // FSOUNDCONF_H
