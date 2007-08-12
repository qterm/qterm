#include "qtermsshlogin.h"
// #include <qstring.h>
// #include <qlineedit.h>
// #include <q3button.h>
#include <QString>
#include <QLineEdit>

namespace QTerm
{
/* 
 *  Constructs a fSSHLogin which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SSHLogin::SSHLogin( QString * username, QString * password, QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	ui.setupUi(this);
	strUserName = username;
	strPassword = password;
	ui.lePassword->setEchoMode(QLineEdit::Password);
	ui.leUserName->setText(*username);
	ui.lePassword->setText(*password);
	ui.leUserName->setFocus();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SSHLogin::~SSHLogin()
{
    // no need to delete child widgets, Qt does it all for us
}

void SSHLogin::accept()
{
	*strUserName = ui.leUserName->text();
	*strPassword  = ui.lePassword->text();
	QDialog::accept();
}

} // namespace QTerm

#include <qtermsshlogin.moc>
