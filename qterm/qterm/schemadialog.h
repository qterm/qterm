#ifndef SCHEMADIALOG_H
#define SCHEMADIALOG_H

#include "schemadialogui.h"

class schemaDialog : public schemaDialogUI
{ 
	Q_OBJECT

public:
    schemaDialog( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~schemaDialog();

protected:
	void connectSlots();
};

#endif // SCHEMADIALOG_H
