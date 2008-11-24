
#include "schemadialog.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
// #include <stdio.h>
#include <QFileDialog>
#include <QColorDialog>
#include <QComboBox>
#include <QMessageBox>
namespace QTerm
{

schemaDialog::schemaDialog( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl ),bgBackground(this)
{
	ui.setupUi(this);
	bgBackground.addButton(ui.noneRadioButton, 0);
	bgBackground.addButton(ui.imageRadioButton, 1);
	bgBackground.addButton(ui.transpRadioButton, 2);
	
	nLastItem = -1;
	bModified = false;

	ui.alphaSlider->setMinimum(0);
	ui.alphaSlider->setMaximum(100);
	ui.alphaSlider->setSingleStep(1);
	ui.alphaSlider->setPageStep(10);
	
	ui.imagePixmapLabel->setScaledContents( false );
// 	ui.bgButtonGroup->setRadioButtonExclusive(true);
	
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
	int  n = fileList.indexOf(strSchemaFile);
	ui.nameListWidget->setCurrentRow(n);
}

QString schemaDialog::getSchema()
{
	return strCurrentSchema;
}

void schemaDialog::connectSlots()
{
	connect( ui.saveButton, SIGNAL(clicked()), this, SLOT(saveSchema()));
	connect( ui.removeButton, SIGNAL(clicked()), this, SLOT(removeSchema()));
	connect( ui.okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect( ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));


	connect( ui.clr0Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr1Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr2Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr3Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr4Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr5Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr6Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr7Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr8Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr9Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr10Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr11Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr12Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr13Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr14Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
	connect( ui.clr15Button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );

	connect( ui.nameListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(nameChanged(int)) );

	connect( &bgBackground, SIGNAL(clicked(int)), this, SLOT(bgType(int)));
	connect( ui.typeComboBox, SIGNAL(activated(int)), this, SLOT(imageType(int)));
	connect( ui.chooseButton, SIGNAL(clicked()), this, SLOT(chooseImage()));
	connect( ui.fadeButton, SIGNAL(clicked()), this, SLOT(fadeClicked()));
	connect( ui.alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaChanged(int)));

	connect( ui.titleLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
	connect( ui.imageLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
}

void schemaDialog::loadList()
{
	QDir dir;
	QFileInfoList lstFile;
	//QFileInfo *fi;

	dir.setNameFilters(QStringList("*.schema"));

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
	dir.setPath(Global::instance()->pathCfg()+"/schema");
	lstFile = dir.entryInfoList();
	//if( lstFile.count()!=0 )
	{
		foreach (QFileInfo fi, lstFile) {
			Config *pConf = new Config(fi.absoluteFilePath());
			ui.nameListWidget->addItem(pConf->getItemValue("schema","title").toString());
			delete pConf;
			fileList.append(fi.absoluteFilePath());
		}
	}
#endif	

	dir.setPath( Global::instance()->pathLib()+"schema" );
	lstFile = dir.entryInfoList();
	//if(lstFile != NULL)
	{
		foreach (QFileInfo fi, lstFile) {
			Config *pConf = new Config(fi.absoluteFilePath());
			ui.nameListWidget->addItem(pConf->getItemValue("schema","title").toString());
			delete pConf;
			fileList.append(fi.absoluteFilePath());
		}
	}
}

void schemaDialog::loadSchema(const QString& strSchemaFile)
{
	Config *pConf = new Config(strSchemaFile);

	strCurrentSchema = strSchemaFile;

	title = pConf->getItemValue("schema","title").toString();
	pxmBg = pConf->getItemValue("image", "name").toString();
	QString strTmp = pConf->getItemValue("image","type").toString();
	type = strTmp.toInt();
	fade.setNamedColor(pConf->getItemValue("image","fade").toString());
	strTmp = pConf->getItemValue("image", "alpha").toString();
	alpha = strTmp.toFloat();
	
	clr0.setNamedColor(pConf->getItemValue("color","color0").toString());
	clr1.setNamedColor(pConf->getItemValue("color","color1").toString());
	clr2.setNamedColor(pConf->getItemValue("color","color2").toString());
	clr3.setNamedColor(pConf->getItemValue("color","color3").toString());
	clr4.setNamedColor(pConf->getItemValue("color","color4").toString());
	clr5.setNamedColor(pConf->getItemValue("color","color5").toString());
	clr6.setNamedColor(pConf->getItemValue("color","color6").toString());
	clr7.setNamedColor(pConf->getItemValue("color","color7").toString());
	clr8.setNamedColor(pConf->getItemValue("color","color8").toString());
	clr9.setNamedColor(pConf->getItemValue("color","color9").toString());
	clr10.setNamedColor(pConf->getItemValue("color","color10").toString());
	clr11.setNamedColor(pConf->getItemValue("color","color11").toString());
	clr12.setNamedColor(pConf->getItemValue("color","color12").toString());
	clr13.setNamedColor(pConf->getItemValue("color","color13").toString());
	clr14.setNamedColor(pConf->getItemValue("color","color14").toString());
	clr15.setNamedColor(pConf->getItemValue("color","color15").toString());

	delete pConf;

	updateView();
	
}

void schemaDialog::saveNumSchema(int n)
{
	// FIXME: ?, and there is more below
	QStringList::Iterator it = fileList.begin();
	while(n--)
		it++;
	
	title = ui.titleLineEdit->text();
	pxmBg = ui.imageLineEdit->text();

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
	QDir dir(Global::instance()->pathLib());
	QString strSchemaFile = dir.absolutePath()+"/schema/"+title+".schema";
#else
	// save it to $HOME/.qterm/schema/ with filename=title
	QFileInfo fi(*it);
	QString strSchemaFile = QDir::homePath()+"/.qterm/schema/"+title+".schema";
#endif

	// create a new schema if title changed
	if(strSchemaFile!=strCurrentSchema)
	{
		ui.nameListWidget->addItem(title);
		fileList.append(strSchemaFile);
	}

	Config *pConf = new Config(strCurrentSchema);
	
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
	ui.titleLineEdit->setText( title );

// #if (QT_VERSION>300)
	// color pane
// 	QPalette palette;
// 	palette.setColor(clr0Button->backgroundRole(), clr0);
// 	clr0Button->setPalette(palette);
// 	clr0Button->setPaletteBackgroundColor(clr0);
// 	clr1Button->setPaletteBackgroundColor(clr1);
// 	clr2Button->setPaletteBackgroundColor(clr2);
// 	clr3Button->setPaletteBackgroundColor(clr3);
// 	clr4Button->setPaletteBackgroundColor(clr4);
// 	clr5Button->setPaletteBackgroundColor(clr5);
// 	clr6Button->setPaletteBackgroundColor(clr6);
// 	clr7Button->setPaletteBackgroundColor(clr7);
// 	clr8Button->setPaletteBackgroundColor(clr8);
// 	clr9Button->setPaletteBackgroundColor(clr9);
// 	clr10Button->setPaletteBackgroundColor(clr10);
// 	clr11Button->setPaletteBackgroundColor(clr11);
// 	clr12Button->setPaletteBackgroundColor(clr12);
// 	clr13Button->setPaletteBackgroundColor(clr13);
// 	clr14Button->setPaletteBackgroundColor(clr14);
// 	clr15Button->setPaletteBackgroundColor(clr15);
// #else 
//     clr0Button->setPalette(clr0);
//     clr1Button->setPalette(clr1);
//     clr2Button->setPalette(clr2);
//     clr3Button->setPalette(clr3);
//     clr4Button->setPalette(clr4);
//     clr5Button->setPalette(clr5);
//     clr6Button->setPalette(clr6);
//     clr7Button->setPalette(clr7);
//     clr8Button->setPalette(clr8);
//     clr9Button->setPalette(clr9);
//     clr10Button->setPalette(clr10);
//     clr11Button->setPalette(clr11);
//     clr12Button->setPalette(clr12);
//     clr13Button->setPalette(clr13);
//     clr14Button->setPalette(clr14);
//     clr15Button->setPalette(clr15);
// #endif
	// bg type
	switch(type)
	{
		case 0: // none
			bgBackground.button(0)->setChecked(true);
			bgType(2);
			break;
		case 1: // transparent
			bgBackground.button(2)->setChecked(true);
			bgType(3);
			break;
		case 2: // tile
			bgBackground.button(1)->setChecked(true);
			bgType(1);
			break;
		case 3: // center
			bgBackground.button(1)->setChecked(true);
			bgType(1);
			break;
		case 4: // stretch
			bgBackground.button(1)->setChecked(true);
			bgType(1);
			break;
		default:
			bgBackground.button(0)->setChecked(true);
			break;
	}
	// image type
	
	// image file
	ui.imageLineEdit->setText(pxmBg);
	// fade color
#if (QT_VERSION>300)    
	QPalette palette;
	palette.setColor(ui.fadeButton->backgroundRole(), fade);
	ui.fadeButton->setPalette(palette);

// 	ui.fadeButton->setPaletteBackgroundColor(fade);
#else
	ui.fadeButton->setPalette(fade);
#endif
	// alpha
	ui.alphaSlider->setValue(alpha*100);
	
	// load from file, nothing changed
	bModified = false;
}

void schemaDialog::updateBgPreview()
{
#if (QT_VERSION>300)    
	QPalette palette;
	palette.setColor(ui.imagePixmapLabel->backgroundRole(), clr0);
	ui.imagePixmapLabel->setPalette(palette);

// 	ui.imagePixmapLabel->setPaletteBackgroundColor(clr0);
#else
	ui.imagePixmapLabel->setPalette(clr0);
#endif
	ui.imagePixmapLabel->clear();
	if(!QFile::exists(pxmBg) || type ==0)
		return;
	
	QPixmap pixmap;
	QImage img(pxmBg); 
 	img = fadeColor(img, alpha, fade);
	pixmap = QPixmap::fromImage( img.scaled(ui.imagePixmapLabel->width(),ui.imagePixmapLabel->height()) );
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
	ui.imagePixmapLabel->setPixmap( pixmap );

}

void schemaDialog::buttonClicked()
{
	QPushButton * button = (QPushButton* )sender();
	QColor color = QColorDialog::getColor(button->palette().color(button->backgroundRole()));
	if (color.isValid() == true)
	{
		QPalette palette;
		palette.setColor(button->backgroundRole(), color);
		button->setPalette(palette);
		bModified = true;
	}
}
/*
void schemaDialog::clr0Clicked()
{
	QColor color=QColorDialog::getColor(clr0);
	if(color.isValid()==TRUE)
	{
		clr0 = color;
#if (QT_VERSION>300)
		ui.clr0Button->setPaletteBackgroundColor(color);
#else
		ui.clr0Button->setPalette(color);
#endif
		bModified = true;
	}
}
void schemaDialog::clr1Clicked()
{
	QColor color=QColorDialog::getColor(clr1);
	if(color.isValid()==TRUE)
	{
		clr1 = color;
#if (QT_VERSION>300)
		ui.clr1Button->setPaletteBackgroundColor(color);
#else
		ui.clr1Button->setPalette(color);
#endif
		bModified = true;
	}
}
void schemaDialog::clr2Clicked()
{	
	QColor color=QColorDialog::getColor(clr2);
	if(color.isValid()==TRUE)
	{
		clr2 = color;
#if (QT_VERSION>300)
		ui.clr2Button->setPaletteBackgroundColor(color);
#else
		ui.clr2Button->setPalette(color);
#endif
		bModified = true;
}

}
void schemaDialog::clr3Clicked()
{	
	QColor color=QColorDialog::getColor(clr3);
	if(color.isValid()==TRUE)
	{
		clr3 = color;
#if (QT_VERSION>300)
		ui.clr3Button->setPaletteBackgroundColor(color);
#else
		ui.clr3Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr4Clicked()
{	
	QColor color=QColorDialog::getColor(clr4);
	if(color.isValid()==TRUE)
	{
		clr4 = color;
#if (QT_VERSION>300)
		ui.clr4Button->setPaletteBackgroundColor(color);
#else
		ui.clr4Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr5Clicked()
{
	QColor color=QColorDialog::getColor(clr5);
	if(color.isValid()==TRUE)
	{
		clr5 = color;
#if (QT_VERSION>300)
		ui.clr5Button->setPaletteBackgroundColor(color);
#else
		ui.clr5Button->setPalette(color);
#endif
		bModified = true;
	}
}
void schemaDialog::clr6Clicked()
{	QColor color=QColorDialog::getColor(clr6);
	if(color.isValid()==TRUE)
	{
		clr6 = color;
#if (QT_VERSION>300)
		ui.clr6Button->setPaletteBackgroundColor(color);
#else
		ui.clr6Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr7Clicked()
{	QColor color=QColorDialog::getColor(clr7);
	if(color.isValid()==TRUE)
	{
		clr7 = color;
#if (QT_VERSION>300)
		ui.clr7Button->setPaletteBackgroundColor(color);
#else
		ui.clr7Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr8Clicked()
{	QColor color=QColorDialog::getColor(clr8);
	if(color.isValid()==TRUE)
	{
		clr8 = color;
#if (QT_VERSION>300)
		ui.clr8Button->setPaletteBackgroundColor(color);
#else
		ui.clr8Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr9Clicked()
{	QColor color=QColorDialog::getColor(clr9);
	if(color.isValid()==TRUE)
	{
		clr9 = color;
#if (QT_VERSION>300)
		ui.clr9Button->setPaletteBackgroundColor(color);
#else
		ui.clr9Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr10Clicked()
{	QColor color=QColorDialog::getColor(clr10);
	if(color.isValid()==TRUE)
	{
		clr10 = color;
#if (QT_VERSION>300)
		ui.clr10Button->setPaletteBackgroundColor(color);
#else
		ui.clr10Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr11Clicked()
{	QColor color=QColorDialog::getColor(clr11);
	if(color.isValid()==TRUE)
	{
		clr11 = color;
#if (QT_VERSION>300)
		ui.clr11Button->setPaletteBackgroundColor(color);
#else
		ui.clr11Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr12Clicked()
{	QColor color=QColorDialog::getColor(clr12);
	if(color.isValid()==TRUE)
	{
		clr12 = color;
#if (QT_VERSION>300)
		ui.clr12Button->setPaletteBackgroundColor(color);
#else
		ui.clr12Button->setPalette(color);
#endif
		bModified = true;
	}

}
void schemaDialog::clr13Clicked()
{	QColor color=QColorDialog::getColor(clr13);
	if(color.isValid()==TRUE)
	{
		clr13 = color;
#if (QT_VERSION>300)
		ui.clr13Button->setPaletteBackgroundColor(color);
#else
		ui.clr13Button->setPalette(color);
#endif
		bModified = true;
	}

}

void schemaDialog::clr14Clicked()
{	QColor color=QColorDialog::getColor(clr14);
	if(color.isValid()==TRUE)
	{
		clr14 = color;
#if (QT_VERSION>300)
		ui.clr14Button->setPaletteBackgroundColor(color);
#else
		ui.clr14Button->setPalette(color);
#endif
		bModified = true;
	}

}

void schemaDialog::clr15Clicked()
{	QColor color=QColorDialog::getColor(clr15);
	if(color.isValid()==TRUE)
	{
		clr15 = color;
#if (QT_VERSION>300)
		ui.clr15Button->setPaletteBackgroundColor(color);
#else
		ui.clr15Button->setPalette(color);
#endif
		bModified = true;
	}

}
*/
void schemaDialog::nameChanged(int item)
{
    if( bModified )
    {
        QMessageBox mb( "QTerm",
            "Setting changed, do you want to save?",
            QMessageBox::Warning,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No  | QMessageBox::Escape ,
            0,this);
        if ( mb.exec() == QMessageBox::Yes )
		{
			if(nLastItem!=-1)
			{
				saveNumSchema(nLastItem);
			}
		}
    }
	
	QStringList::Iterator it = fileList.begin();
	int n = item; //nameListBox->index(item);
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
// 			typeComboBox->setEnabled(true);
// 			imageLineEdit->setEnabled(true);
// 			chooseButton->setEnabled(true);
			if(type==0)
				type=2;
			ui.typeComboBox->setCurrentIndex(type-2);
			break;

		case 2: // none
// 			typeComboBox->setEnabled(false);
// 			imageLineEdit->setEnabled(false);
// 			chooseButton->setEnabled(false);
			type = 0;
			break;
		case 3: // transparent
			QMessageBox::information(this, "sorry", "We are trying to bring you this function soon :)");
//			typeComboBox->setEnabled(false);
//			imageLineEdit->setEnabled(false);
//			chooseButton->setEnabled(false);
//			type = 1;
// 			bgButtonGroup->setButton(2);
			bgBackground.button(2)->setChecked(true);
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
	QString img = QFileDialog::getOpenFileName( this, "Choose an image", QDir::currentPath() );
    if ( !img.isNull() ) 
	{
		ui.imageLineEdit->setText(img);
		pxmBg = img;
		type = 2 + ui.typeComboBox->currentIndex();
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
#if (QT_VERSION>300)
		QPalette palette;
		palette.setColor(ui.fadeButton->backgroundRole(), color);
		ui.fadeButton->setPalette(palette);

// 		ui.fadeButton->setPaletteBackgroundColor(color);
#else
		ui.fadeButton->setPalette(color);
#endif

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
	int n =ui.nameListWidget->currentRow();
	saveNumSchema(n);
}

void schemaDialog::removeSchema()
{
	QFileInfo fi(strCurrentSchema);
	if(fi.isWritable())
	{
		QFile::remove(strCurrentSchema);
		QStringList::Iterator it = fileList.begin();
		int n =ui.nameListWidget->currentRow();
		ui.nameListWidget->takeItem(n);
		while(n--)
			it++;
		fileList.erase(it);
	}	else
	{
		QMessageBox::warning(this, "Error", "This is a system schema. Permission Denied");
	}
}

void schemaDialog::onOK()
{
	if( bModified )
    {
        QMessageBox mb( "QTerm",
            "Setting changed, do you want to save?",
            QMessageBox::Warning,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No  | QMessageBox::Escape ,
            0,this);
        if ( mb.exec() == QMessageBox::Yes )
		{
			int n = ui.nameListWidget->currentRow();
			saveNumSchema(n);
		}
    }
	
	done(1);
}

void schemaDialog::onCancel()
{
	if( bModified )
    {
        QMessageBox mb( "QTerm",
            "Setting changed, do you want to save?",
            QMessageBox::Warning,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No  | QMessageBox::Escape ,
            0,this);
        if ( mb.exec() == QMessageBox::Yes )
		{
			int n =ui.nameListWidget->currentRow();
			saveNumSchema(n);
		}
    }

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
                if (cr > Qt::red)
                    r = cr - tbl[cr - Qt::red];
                else
                    r = cr + tbl[Qt::red - cr];
                if (cg > Qt::green)
                    g = cg - tbl[cg - Qt::green];
                else
                    g = cg + tbl[Qt::green - cg];
                if (cb > Qt::blue)
                    b = cb - tbl[cb - Qt::blue];
                else
                    b = cb + tbl[Qt::blue - cb];
                *data++ = qRgba(r, g, b, qAlpha(col));
            }
        }
    }

    return img;
}

} // namespace QTerm

#include <schemadialog.moc>
