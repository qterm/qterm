#ifndef FSSHLOGIN_H
#define FSSHLOGIN_H
#include "qtermsshloginui.h"

class fSSHLogin : public fSSHLoginUI
{
    Q_OBJECT

public:
    fSSHLogin(QString * username, QString * password,  QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~fSSHLogin();
private:
    QString * strUserName;
    QString * strPassword;
private slots:
    void accept();
};

#endif // FSSHLOGIN_H
