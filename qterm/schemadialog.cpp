
#include "schemadialog.h"

schemaDialog::schemaDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : schemaDialogUI( parent, name, modal, fl )
{
	connectSlots();
}


schemaDialog::~schemaDialog()
{
}

void schemaDialog::connectSlots()
{
}
