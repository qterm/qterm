#include "qtermsshlogin.h"
#include <qstring.h>
#include <qlineedit.h>
#include <qbutton.h>

/* 
 *  Constructs a fSSHLogin which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
fSSHLogin::fSSHLogin( QString * username, QString * password, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : fSSHLoginUI( parent, name, modal, fl )
{
	strUserName = username;
	strPassword = password;
	lePassword->setEchoMode(QLineEdit::Password);
	leUserName->setText(*username);
	lePassword->setText(*password);
	leUserName->setFocus();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
fSSHLogin::~fSSHLogin()
{
    // no need to delete child widgets, Qt does it all for us
}

void fSSHLogin::accept()
{
	*strUserName = leUserName->text();
	*strPassword  = lePassword->text();
	fSSHLoginUI::accept();
}
#include <qtermsshlogin.moc>
