
#include "schemadialog.h"
#include "qtermconfig.h"

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qwmatrix.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <stdio.h>

extern QString pathLib;

schemaDialog::schemaDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : schemaDialogUI( parent, name, modal, fl )
{
	nLastItem = -1;
	bModified = false;

	alphaSlider->setMinValue(0);
	alphaSlider->setMaxValue(100);
	alphaSlider->setLineStep(1);
	alphaSlider->setPageStep(10);

	bgButtonGroup->setRadioButtonExclusive(true);

	connectSlots();
	loadList();
}


schemaDialog::~schemaDialog()
{
}

void schemaDialog::setSchema(const QString& strSchemaFile)
{
	if(!QFile::exists(strSchemaFile))
		return;
	int  n = fileList.findIndex(strSchemaFile);
	nameListBox->setCurrentItem(n);
}

QString schemaDialog::getSchema()
{
	return strCurrentSchema;
}

void schemaDialog::connectSlots()
{
	connect( saveButton, SIGNAL(clicked()), this, SLOT(saveSchema()));
	connect( removeButton, SIGNAL(clicked()), this, SLOT(removeSchema()));
	connect( okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect( cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));


	connect( clr0Button, SIGNAL(clicked()), this, SLOT(clr0Clicked()) );
	connect( clr1Button, SIGNAL(clicked()), this, SLOT(clr1Clicked()) );
	connect( clr2Button, SIGNAL(clicked()), this, SLOT(clr2Clicked()) );
	connect( clr3Button, SIGNAL(clicked()), this, SLOT(clr3Clicked()) );
	connect( clr4Button, SIGNAL(clicked()), this, SLOT(clr4Clicked()) );
	connect( clr5Button, SIGNAL(clicked()), this, SLOT(clr5Clicked()) );
	connect( clr6Button, SIGNAL(clicked()), this, SLOT(clr6Clicked()) );
	connect( clr7Button, SIGNAL(clicked()), this, SLOT(clr7Clicked()) );
	connect( clr8Button, SIGNAL(clicked()), this, SLOT(clr8Clicked()) );
	connect( clr9Button, SIGNAL(clicked()), this, SLOT(clr9Clicked()) );
	connect( clr10Button, SIGNAL(clicked()), this, SLOT(clr10Clicked()) );
	connect( clr11Button, SIGNAL(clicked()), this, SLOT(clr11Clicked()) );
	connect( clr12Button, SIGNAL(clicked()), this, SLOT(clr12Clicked()) );
	connect( clr13Button, SIGNAL(clicked()), this, SLOT(clr13Clicked()) );
	connect( clr14Button, SIGNAL(clicked()), this, SLOT(clr14Clicked()) );
	connect( clr15Button, SIGNAL(clicked()), this, SLOT(clr15Clicked()) );

	connect( nameListBox, SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(nameChanged(QListBoxItem*)) );

	connect( bgButtonGroup, SIGNAL(clicked(int)), this, SLOT(bgType(int)));
	connect( typeComboBox, SIGNAL(activated(int)), this, SLOT(imageType(int)));
	connect( chooseButton, SIGNAL(clicked()), this, SLOT(chooseImage()));
	connect( fadeButton, SIGNAL(clicked()), this, SLOT(fadeClicked()));
	connect( alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaChanged(int)));

	connect( titleLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
	connect( imageLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
}

void schemaDialog::loadList()
{
	QDir dir;
	dir.setNameFilter("*.schema");
	
	dir.setPath(QDir::homeDirPath()+"/.qterm/schema");
	const QFileInfoList *lstFile = dir.entryInfoList();
	QFileInfoListIterator it(*lstFile);
	QFileInfo *fi;
	while( (fi = it.current())!=0 )
	{
		QTermConfig *pConf = new QTermConfig(fi->absFilePath());
		nameListBox->insertItem(pConf->getItemValue("schema","title"));
		delete pConf;
		fileList.append(fi->absFilePath());
		++it;
	}
	
	dir.setPath( pathLib+"schema" );

	lstFile = dir.entryInfoList();
	QFileInfoListIterator it2(*lstFile);
	while( (fi = it2.current())!=0 )
	{
		QTermConfig *pConf = new QTermConfig(fi->absFilePath());
		nameListBox->insertItem(pConf->getItemValue("schema","title"));
		delete pConf;
		fileList.append(fi->absFilePath());
		++it2;
	}
	
}

void schemaDialog::loadSchema(const QString& strSchemaFile)
{
	QTermConfig *pConf = new QTermConfig(strSchemaFile);

	strCurrentSchema = strSchemaFile;

	title = pConf->getItemValue("schema","title");
	pxmBg = pConf->getItemValue("image", "name");
	QString strTmp = pConf->getItemValue("image","type");
	type = strTmp.toInt();
	fade.setNamedColor(pConf->getItemValue("image","fade"));
	strTmp = pConf->getItemValue("image", "alpha");
	alpha = strTmp.toFloat();
	
	clr0.setNamedColor(pConf->getItemValue("color","color0"));
	clr1.setNamedColor(pConf->getItemValue("color","color1"));
	clr2.setNamedColor(pConf->getItemValue("color","color2"));
	clr3.setNamedColor(pConf->getItemValue("color","color3"));
	clr4.setNamedColor(pConf->getItemValue("color","color4"));
	clr5.setNamedColor(pConf->getItemValue("color","color5"));
	clr6.setNamedColor(pConf->getItemValue("color","color6"));
	clr7.setNamedColor(pConf->getItemValue("color","color7"));
	clr8.setNamedColor(pConf->getItemValue("color","color8"));
	clr9.setNamedColor(pConf->getItemValue("color","color9"));
	clr10.setNamedColor(pConf->getItemValue("color","color10"));
	clr11.setNamedColor(pConf->getItemValue("color","color11"));
	clr12.setNamedColor(pConf->getItemValue("color","color12"));
	clr13.setNamedColor(pConf->getItemValue("color","color13"));
	clr14.setNamedColor(pConf->getItemValue("color","color14"));
	clr15.setNamedColor(pConf->getItemValue("color","color15"));

	delete pConf;

	updateView();
	
}

void schemaDialog::saveNumSchema(int n)
{
	QStringList::Iterator it = fileList.begin();
	while(n--)
		it++;
	
	title = titleLineEdit->text();
	pxmBg = imageLineEdit->text();
	// save it to $HOME/.qterm/schema/ with filename=title
	QFileInfo fi(*it);
	QString strSchemaFile = QDir::homeDirPath()+"/.qterm/schema/"+title+".schema";

	// create a new schema if title changed
	if(strSchemaFile!=strCurrentSchema)
	{
		nameListBox->insertItem(title);
		fileList.append(strSchemaFile);
	}

	QTermConfig *pConf = new QTermConfig(strCurrentSchema);
	
	strCurrentSchema = strSchemaFile;
	

	pConf->setItemValue("schema","title",title);

	pConf->setItemValue("image", "name",pxmBg);

	QString strTmp;
	strTmp.setNum(type);
	pConf->setItemValue("image","type",strTmp);

	pConf->setItemValue("image","fade",fade.name());
		
	strTmp.setNum(alpha);
	pConf->setItemValue("image", "alpha", strTmp);
	
	pConf->setItemValue("color","color0",clr0.name());
	pConf->setItemValue("color","color1",clr1.name());
	pConf->setItemValue("color","color2",clr2.name());
	pConf->setItemValue("color","color3",clr3.name());
	pConf->setItemValue("color","color4",clr4.name());
	pConf->setItemValue("color","color5",clr5.name());
	pConf->setItemValue("color","color6",clr6.name());
	pConf->setItemValue("color","color7",clr7.name());
	pConf->setItemValue("color","color8",clr8.name());
	pConf->setItemValue("color","color9",clr9.name());
	pConf->setItemValue("color","color10",clr10.name());
	pConf->setItemValue("color","color11",clr11.name());
	pConf->setItemValue("color","color12",clr12.name());
	pConf->setItemValue("color","color13",clr13.name());
	pConf->setItemValue("color","color14",clr14.name());
	pConf->setItemValue("color","color15",clr15.name());

	pConf->save(strSchemaFile);

	delete pConf;

	bModified = false;

}

void schemaDialog::updateView()
{
	// title
	titleLineEdit->setText( title );
	// color pane
	clr0Button->setPaletteBackgroundColor(clr0);
	clr1Button->setPaletteBackgroundColor(clr1);
	clr2Button->setPaletteBackgroundColor(clr2);
	clr3Button->setPaletteBackgroundColor(clr3);
	clr4Button->setPaletteBackgroundColor(clr4);
	clr5Button->setPaletteBackgroundColor(clr5);
	clr6Button->setPaletteBackgroundColor(clr6);
	clr7Button->setPaletteBackgroundColor(clr7);
	clr8Button->setPaletteBackgroundColor(clr8);
	clr9Button->setPaletteBackgroundColor(clr9);
	clr10Button->setPaletteBackgroundColor(clr10);
	clr11Button->setPaletteBackgroundColor(clr11);
	clr12Button->setPaletteBackgroundColor(clr12);
	clr13Button->setPaletteBackgroundColor(clr13);
	clr14Button->setPaletteBackgroundColor(clr14);
	clr15Button->setPaletteBackgroundColor(clr15);
	// bg type
	switch(type)
	{
		case 0: // none
			bgButtonGroup->setButton(2);
			bgType(2);
			break;
		case 1: // transparent
			bgButtonGroup->setButton(3);
			bgType(3);
			break;
		case 2: // tile
			bgButtonGroup->setButton(1);
			bgType(1);
			break;
		case 3: // center
			bgButtonGroup->setButton(1);
			bgType(1);
			break;
		case 4: // stretch
			bgButtonGroup->setButton(1);
			bgType(1);
			break;
		default:
			bgButtonGroup->setButton(2);
			break;
	}
	// image type
	
	// image file
	imageLineEdit->setText(pxmBg);
	// fade color
	fadeButton->setPaletteBackgroundColor(fade);
	// alpha
	alphaSlider->setValue(alpha*100);
	
	// load from file, nothing changed
	bModified = false;
}

void schemaDialog::updateBgPreview()
{
	imagePixmapLabel->setPaletteBackgroundColor(clr0);
	imagePixmapLabel->clear();
	if(!QFile::exists(pxmBg) || type ==0)
		return;
	
	QPixmap pixmap(pxmBg);
	QImage img = pixmap.convertToImage();
 	img = fadeColor(img, alpha, fade);
	pixmap.convertFromImage(img);
/*
	switch(type)
	{
		case 2:	// tile
			if( !pixmap.isNull() )
			{
				imagePixmapLabel->setPixmap( pixmap );
				break;
			}
		case 3:	// center
			if( !pixmap.isNull() )
			{
				QPixmap pxm;
				pxm.resize(size());
				pxm.fill( clr0 );
				bitBlt( &pxm, 
				( size().width() - pixmap.width() ) / 2,
				( size().height() - pixmap.height() ) / 2,
				 &pixmap, 0, 0,
				 pixmap.width(), pixmap.height() );
				imagePixmapLabel->setPixmap(pxm);
				break;
			}
		case 4:	// stretch
			if( !pixmap.isNull() )
			{
				float sx = (float)size().width() / pixmap.width();
				float sy = (float)size().height() /pixmap.height();
				QWMatrix matrix;
				matrix.scale( sx, sy );
				imagePixmapLabel->setPixmap(pixmap.xForm( matrix ));
				break;
			}
	}
*/
	imagePixmapLabel->setPixmap( pixmap );

}



void schemaDialog::clr0Clicked()
{
	QColor color=QColorDialog::getColor(clr0);
	if(color.isValid()==TRUE)
	{
		clr0 = color;
		clr0Button->setPaletteBackgroundColor(color);
		bModified = true;
	}
}
void schemaDialog::clr1Clicked()
{
	QColor color=QColorDialog::getColor(clr1);
	if(color.isValid()==TRUE)
	{
		clr1 = color;
		clr1Button->setPaletteBackgroundColor(color);
		bModified = true;
	}
}
void schemaDialog::clr2Clicked()
{	
	QColor color=QColorDialog::getColor(clr2);
	if(color.isValid()==TRUE)
	{
		clr2 = color;
		clr2Button->setPaletteBackgroundColor(color);
		bModified = true;
}

}
void schemaDialog::clr3Clicked()
{	
	QColor color=QColorDialog::getColor(clr3);
	if(color.isValid()==TRUE)
	{
		clr3 = color;
		clr3Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr4Clicked()
{	
	QColor color=QColorDialog::getColor(clr4);
	if(color.isValid()==TRUE)
	{
		clr4 = color;
		clr4Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr5Clicked()
{
	QColor color=QColorDialog::getColor(clr5);
	if(color.isValid()==TRUE)
	{
		clr5 = color;
		clr5Button->setPaletteBackgroundColor(color);
		bModified = true;
	}
}
void schemaDialog::clr6Clicked()
{	QColor color=QColorDialog::getColor(clr6);
	if(color.isValid()==TRUE)
	{
		clr6 = color;
		clr6Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr7Clicked()
{	QColor color=QColorDialog::getColor(clr7);
	if(color.isValid()==TRUE)
	{
		clr7 = color;
		clr7Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr8Clicked()
{	QColor color=QColorDialog::getColor(clr8);
	if(color.isValid()==TRUE)
	{
		clr8 = color;
		clr8Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr9Clicked()
{	QColor color=QColorDialog::getColor(clr9);
	if(color.isValid()==TRUE)
	{
		clr9 = color;
		clr9Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr10Clicked()
{	QColor color=QColorDialog::getColor(clr10);
	if(color.isValid()==TRUE)
	{
		clr10 = color;
		clr10Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr11Clicked()
{	QColor color=QColorDialog::getColor(clr11);
	if(color.isValid()==TRUE)
	{
		clr11 = color;
		clr11Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr12Clicked()
{	QColor color=QColorDialog::getColor(clr12);
	if(color.isValid()==TRUE)
	{
		clr12 = color;
		clr12Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}
void schemaDialog::clr13Clicked()
{	QColor color=QColorDialog::getColor(clr13);
	if(color.isValid()==TRUE)
	{
		clr13 = color;
		clr13Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}

void schemaDialog::clr14Clicked()
{	QColor color=QColorDialog::getColor(clr14);
	if(color.isValid()==TRUE)
	{
		clr14 = color;
		clr14Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}

void schemaDialog::clr15Clicked()
{	QColor color=QColorDialog::getColor(clr15);
	if(color.isValid()==TRUE)
	{
		clr15 = color;
		clr15Button->setPaletteBackgroundColor(color);
		bModified = true;
	}

}

void schemaDialog::nameChanged(QListBoxItem *item)
{
    if( bModified )
    {
        QMessageBox mb( "QTerm",
            "Setting changed, do you want to save?",
            QMessageBox::Warning,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No  | QMessageBox::Escape ,
            0,this,0,true);
        if ( mb.exec() == QMessageBox::Yes )
		{
			if(nLastItem!=-1)
			{
//				nameListBox->setCurrentItem(nLastItem);
				saveNumSchema(nLastItem);
//				nameListBox->setCurrentItem(item);
			}
		}
    }
	
	QStringList::Iterator it = fileList.begin();
	int n =nameListBox->index(item);
	nLastItem = n;
	while(n--)
		it++;
	loadSchema( *it );
}

void schemaDialog::bgType(int n)
{
	switch( n )
	{
		case 1: // image
			typeComboBox->setEnabled(true);
			imageLineEdit->setEnabled(true);
			chooseButton->setEnabled(true);

			typeComboBox->setCurrentItem(type-2);
			break;

		case 2: // none
			typeComboBox->setEnabled(false);
			imageLineEdit->setEnabled(false);
			chooseButton->setEnabled(false);
			type = 0;
			break;
		case 3: // transparent
			QMessageBox::information(this, "sorry", "We are trying to bring you this function soon :)");
//			typeComboBox->setEnabled(false);
//			imageLineEdit->setEnabled(false);
//			chooseButton->setEnabled(false);
//			type = 1;
			bgButtonGroup->setButton(2);
			break;
	}
	bModified = true;
	updateBgPreview();
}

void schemaDialog::imageType(int n)
{
	type = n +2;
	bModified = true;
	updateBgPreview();
}

void schemaDialog::chooseImage()
{
	QString img = QFileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !img.isEmpty() ) 
	{
		imageLineEdit->setText(img);
		pxmBg = img;
		type = 2 + typeComboBox->currentItem();
		bModified = true;
		updateBgPreview();
	}
}

void schemaDialog::fadeClicked()
{
	QColor color=QColorDialog::getColor(fade);
	if(color.isValid()==TRUE)
	{
		fade = color;
		fadeButton->setPaletteBackgroundColor(color);
		bModified = true;
		updateBgPreview();
	}
}

void schemaDialog::alphaChanged(int value)
{
	alpha = float(value)/100;
	bModified = true;
	updateBgPreview();
}

void schemaDialog::saveSchema()
{
	// get current schema file name
	int n =nameListBox->currentItem();
	saveNumSchema(n);
}

void schemaDialog::removeSchema()
{
	QFileInfo fi(strCurrentSchema);
	if(fi.isWritable())
	{
		QFile::remove(strCurrentSchema);
		QStringList::Iterator it = fileList.begin();
		int n =nameListBox->currentItem();
		nameListBox->removeItem(n);
		while(n--)
			it++;
		fileList.remove(it);
	}	else
	{
		QMessageBox::warning(this, "Error", "This is a system schema. Permission Denied");
	}
}

void schemaDialog::onOK()
{
	done(1);
}

void schemaDialog::onCancel()
{
	done(0);
}

void schemaDialog::textChanged(const QString&)
{
	bModified = true;
}

// from KImageEffect::fade
QImage& schemaDialog::fadeColor( QImage& img, float val, const QColor& color)
{
    if (img.width() == 0 || img.height() == 0)
      return img;

    // We don't handle bitmaps
    if (img.depth() == 1)
	return img;

    unsigned char tbl[256];
    for (int i=0; i<256; i++)
	tbl[i] = (int) (val * i + 0.5);

    int red = color.red();
    int green = color.green();
    int blue = color.blue();

    QRgb col;
    int r, g, b, cr, cg, cb;

    if (img.depth() <= 8) {
	// pseudo color
	for (int i=0; i<img.numColors(); i++) {
	    col = img.color(i);
	    cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
	    if (cr > red)
		r = cr - tbl[cr - red];
	    else
		r = cr + tbl[red - cr];
	    if (cg > green)
		g = cg - tbl[cg - green];
	    else
		g = cg + tbl[green - cg];
	    if (cb > blue)
		b = cb - tbl[cb - blue];
	    else
		b = cb + tbl[blue - cb];
	    img.setColor(i, qRgba(r, g, b, qAlpha(col)));
	}

    } else {
	// truecolor
        for (int y=0; y<img.height(); y++) {
            QRgb *data = (QRgb *) img.scanLine(y);
            for (int x=0; x<img.width(); x++) {
                col = *data;
                cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
                if (cr > red)
                    r = cr - tbl[cr - red];
                else
                    r = cr + tbl[red - cr];
                if (cg > green)
                    g = cg - tbl[cg - green];
                else
                    g = cg + tbl[green - cg];
                if (cb > blue)
                    b = cb - tbl[cb - blue];
                else
                    b = cb + tbl[blue - cb];
                *data++ = qRgba(r, g, b, qAlpha(col));
            }
        }
    }

    return img;
}

