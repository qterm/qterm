
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

schemaDialog::schemaDialog(QWidget* parent, Qt::WFlags fl)
        : QDialog(parent, fl), bgBackground(this)
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

    ui.imagePixmapLabel->setScaledContents(false);
//  ui.bgButtonGroup->setRadioButtonExclusive(true);

    connectSlots();
    loadList();
}


schemaDialog::~schemaDialog()
{
}

void schemaDialog::setSchema(const QString& strSchemaFile)
{
    if (!QFile::exists(strSchemaFile))
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
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(saveSchema()));
    connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(removeSchema()));
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));


    connect(ui.clr0Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr1Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr2Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr3Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr4Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr5Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr6Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr7Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr8Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr9Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr10Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr11Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr12Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr13Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr14Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(ui.clr15Button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    connect(ui.nameListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(nameChanged(int)));

    connect(&bgBackground, SIGNAL(clicked(int)), this, SLOT(bgType(int)));
    connect(ui.typeComboBox, SIGNAL(activated(int)), this, SLOT(imageType(int)));
    connect(ui.chooseButton, SIGNAL(clicked()), this, SLOT(chooseImage()));
    connect(ui.fadeButton, SIGNAL(clicked()), this, SLOT(fadeClicked()));
    connect(ui.alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaChanged(int)));

    connect(ui.titleLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(ui.imageLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
}

void schemaDialog::loadList()
{
    QDir dir;
    QFileInfoList lstFile;
    //QFileInfo *fi;

    dir.setNameFilters(QStringList("*.schema"));

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
    dir.setPath(Global::instance()->pathCfg() + "/schema");
    lstFile = dir.entryInfoList();
    //if( lstFile.count()!=0 )
    {
        foreach(QFileInfo fi, lstFile) {
            Config *pConf = new Config(fi.absoluteFilePath());
            ui.nameListWidget->addItem(pConf->getItemValue("schema", "title").toString());
            delete pConf;
            fileList.append(fi.absoluteFilePath());
        }
    }
#endif

    dir.setPath(Global::instance()->pathLib() + "schema");
    lstFile = dir.entryInfoList();
    //if(lstFile != NULL)
    {
        foreach(QFileInfo fi, lstFile) {
            Config *pConf = new Config(fi.absoluteFilePath());
            ui.nameListWidget->addItem(pConf->getItemValue("schema", "title").toString());
            delete pConf;
            fileList.append(fi.absoluteFilePath());
        }
    }
}

void schemaDialog::loadSchema(const QString& strSchemaFile)
{
    Config *pConf = new Config(strSchemaFile);

    strCurrentSchema = strSchemaFile;

    title = pConf->getItemValue("schema", "title").toString();
    pxmBg = pConf->getItemValue("image", "name").toString();
    QString strTmp = pConf->getItemValue("image", "type").toString();
    type = strTmp.toInt();
    fade.setNamedColor(pConf->getItemValue("image", "fade").toString());
    strTmp = pConf->getItemValue("image", "alpha").toString();
    alpha = strTmp.toFloat();

    for (int i = 0; i < 16; i++) {
        QString colorName = QString("color%1").arg(i);
        schemaColor[i].setNamedColor(pConf->getItemValue("color", colorName).toString());
    }

    delete pConf;

    updateView();

}

void schemaDialog::saveNumSchema(int n)
{
    // FIXME: ?, and there is more below
    QStringList::Iterator it = fileList.begin();
    while (n--)
        it++;

    title = ui.titleLineEdit->text();
    pxmBg = ui.imageLineEdit->text();

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
    QDir dir(Global::instance()->pathLib());
    QString strSchemaFile = dir.absolutePath() + "/schema/" + title + ".schema";
#else
    // save it to $HOME/.qterm/schema/ with filename=title
    QFileInfo fi(*it);
    QString strSchemaFile = QDir::homePath() + "/.qterm/schema/" + title + ".schema";
#endif

    // create a new schema if title changed
    if (strSchemaFile != strCurrentSchema) {
        ui.nameListWidget->addItem(title);
        fileList.append(strSchemaFile);
    }

    strCurrentSchema = strSchemaFile;

    Config *pConf = new Config(strCurrentSchema);

    pConf->setItemValue("schema", "title", title);

    pConf->setItemValue("image", "name", pxmBg);

    QString strTmp;
    strTmp.setNum(type);
    pConf->setItemValue("image", "type", strTmp);

    pConf->setItemValue("image", "fade", fade.name());

    strTmp.setNum(alpha);
    pConf->setItemValue("image", "alpha", strTmp);

    for (int i = 0; i < 16; i++) {
        QString colorName = QString("color%1").arg(i);
        pConf->setItemValue("color", colorName, schemaColor[i].name());
    }

    pConf->save(strSchemaFile);

    delete pConf;

    bModified = false;

}

void schemaDialog::setBackgroundColor(QWidget * widget, const QColor & color)
{
    QPalette palette;
    palette.setColor(widget->backgroundRole(), color);
    widget->setPalette(palette);
}

void schemaDialog::updateView()
{
    // title
    ui.titleLineEdit->setText(title);

//  QPalette palette;
//  palette.setColor(clr0Button->backgroundRole(), clr0);
    for (int i = 0; i < 16; i++) {
        QString buttonName = QString("clr%1Button").arg(i);
        setBackgroundColor(findChild<QPushButton *>(buttonName), schemaColor[i]);
    }

    // bg type
    switch (type) {
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

//  ui.fadeButton->setPaletteBackgroundColor(fade);
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
/*
#if (QT_VERSION>300)
    QPalette palette;
    palette.setColor(ui.imagePixmapLabel->backgroundRole(), clr0);
    ui.imagePixmapLabel->setPalette(palette);

//  ui.imagePixmapLabel->setPaletteBackgroundColor(clr0);
#else
    ui.imagePixmapLabel->setPalette(clr0);
#endif
*/
    ui.imagePixmapLabel->clear();
    if (!QFile::exists(pxmBg) || type == 0)
        return;

    QPixmap pixmap;
    QImage img(pxmBg);
    img = fadeColor(img, alpha, fade);
    pixmap = QPixmap::fromImage(img.scaled(ui.imagePixmapLabel->width(), ui.imagePixmapLabel->height()));
    /*
     switch(type)
     {
      case 2: // tile
       if( !pixmap.isNull() )
       {
        imagePixmapLabel->setPixmap( pixmap );
        break;
       }
      case 3: // center
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
      case 4: // stretch
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
    ui.imagePixmapLabel->setPixmap(pixmap);

}

void schemaDialog::buttonClicked()
{
    QPushButton * button = (QPushButton*)sender();
    QRegExp rx("\\d+");
    if (rx.indexIn(button->objectName()) == -1) {
        qDebug("object name error");
        return;
    }
    int index = rx.cap().toInt();
    QColor color = QColorDialog::getColor(button->palette().color(button->backgroundRole()));
    if (color.isValid() == true) {
        setBackgroundColor(button,color);
        schemaColor[index] = color;
        bModified = true;
    }
}

void schemaDialog::nameChanged(int item)
{
    if (bModified) {
        QMessageBox mb("QTerm",
                       "Setting changed, do you want to save?",
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No  | QMessageBox::Escape ,
                       0, this);
        if (mb.exec() == QMessageBox::Yes) {
            if (nLastItem != -1) {
                saveNumSchema(nLastItem);
            }
        }
    }

    QStringList::Iterator it = fileList.begin();
    int n = item; //nameListBox->index(item);
    nLastItem = n;
    while (n--)
        it++;
    loadSchema(*it);
}

void schemaDialog::bgType(int n)
{
    switch (n) {
    case 1: // image
//    typeComboBox->setEnabled(true);
//    imageLineEdit->setEnabled(true);
//    chooseButton->setEnabled(true);
        if (type == 0)
            type = 2;
        ui.typeComboBox->setCurrentIndex(type - 2);
        break;

    case 2: // none
//    typeComboBox->setEnabled(false);
//    imageLineEdit->setEnabled(false);
//    chooseButton->setEnabled(false);
        type = 0;
        break;
    case 3: // transparent
        QMessageBox::information(this, "sorry", "We are trying to bring you this function soon :)");
//   typeComboBox->setEnabled(false);
//   imageLineEdit->setEnabled(false);
//   chooseButton->setEnabled(false);
//   type = 1;
//    bgButtonGroup->setButton(2);
        bgBackground.button(2)->setChecked(true);
        break;
    }
    bModified = true;
    updateBgPreview();
}

void schemaDialog::imageType(int n)
{
    type = n + 2;
    bModified = true;
    updateBgPreview();
}

void schemaDialog::chooseImage()
{
    QString img = QFileDialog::getOpenFileName(this, "Choose an image", QDir::currentPath());
    if (!img.isNull()) {
        ui.imageLineEdit->setText(img);
        pxmBg = img;
        type = 2 + ui.typeComboBox->currentIndex();
        bModified = true;
        updateBgPreview();
    }
}

void schemaDialog::fadeClicked()
{
    QColor color = QColorDialog::getColor(fade);
    if (color.isValid() == TRUE) {
        fade = color;
#if (QT_VERSION>300)
        QPalette palette;
        palette.setColor(ui.fadeButton->backgroundRole(), color);
        ui.fadeButton->setPalette(palette);

//   ui.fadeButton->setPaletteBackgroundColor(color);
#else
        ui.fadeButton->setPalette(color);
#endif

        bModified = true;
        updateBgPreview();
    }
}

void schemaDialog::alphaChanged(int value)
{
    alpha = float(value) / 100;
    bModified = true;
    updateBgPreview();
}

void schemaDialog::saveSchema()
{
    // get current schema file name
    int n = ui.nameListWidget->currentRow();
    saveNumSchema(n);
}

void schemaDialog::removeSchema()
{
    QFileInfo fi(strCurrentSchema);
    if (fi.isWritable()) {
        QFile::remove(strCurrentSchema);
        QStringList::Iterator it = fileList.begin();
        int n = ui.nameListWidget->currentRow();
        ui.nameListWidget->takeItem(n);
        while (n--)
            it++;
        fileList.erase(it);
    } else {
        QMessageBox::warning(this, "Error", "This is a system schema. Permission Denied");
    }
}

void schemaDialog::onOK()
{
    if (bModified) {
        QMessageBox mb("QTerm",
                       "Setting changed, do you want to save?",
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No  | QMessageBox::Escape ,
                       0, this);
        if (mb.exec() == QMessageBox::Yes) {
            int n = ui.nameListWidget->currentRow();
            saveNumSchema(n);
        }
    }

    done(1);
}

void schemaDialog::onCancel()
{
    if (bModified) {
        QMessageBox mb("QTerm",
                       "Setting changed, do you want to save?",
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No  | QMessageBox::Escape ,
                       0, this);
        if (mb.exec() == QMessageBox::Yes) {
            int n = ui.nameListWidget->currentRow();
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
QImage& schemaDialog::fadeColor(QImage& img, float val, const QColor& color)
{
    if (img.width() == 0 || img.height() == 0)
        return img;

    // We don't handle bitmaps
    if (img.depth() == 1)
        return img;

    unsigned char tbl[256];
    for (int i = 0; i < 256; i++)
        tbl[i] = (int)(val * i + 0.5);

    int red = color.red();
    int green = color.green();
    int blue = color.blue();

    QRgb col;
    int r, g, b, cr, cg, cb;

    if (img.depth() <= 8) {
        // pseudo color
        for (int i = 0; i < img.numColors(); i++) {
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
        for (int y = 0; y < img.height(); y++) {
            QRgb *data = (QRgb *) img.scanLine(y);
            for (int x = 0; x < img.width(); x++) {
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
