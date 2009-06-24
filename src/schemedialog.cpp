
#include "schemedialog.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
// #include <stdio.h>
#include <QFileDialog>
#include <QColorDialog>
#include <QComboBox>
#include <QMessageBox>
namespace QTerm
{

schemeDialog::schemeDialog(QWidget* parent, Qt::WFlags fl)
        : QDialog(parent, fl)
{
    ui.setupUi(this);

    nLastItem = -1;
    bModified = false;

    connectSlots();
    fileList = loadSchemeList();
    updateList();
}


schemeDialog::~schemeDialog()
{
}

void schemeDialog::setScheme(const QString& strSchemeFile)
{
    if (!QFile::exists(strSchemeFile))
        return;
    int  n = fileList.indexOf(strSchemeFile);
    ui.nameListWidget->setCurrentRow(n);
}

QString schemeDialog::getScheme()
{
    return strCurrentScheme;
}

void schemeDialog::connectSlots()
{
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(saveScheme()));
    connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(removeScheme()));
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(resetScheme()));

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

    connect(ui.titleLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
}

QStringList schemeDialog::loadSchemeList()
{
    QDir dir;
    QFileInfoList lstFile;
    QStringList schemeList;

    dir.setNameFilters(QStringList("*.scheme"));

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
    dir.setPath(Global::instance()->pathCfg() + "/scheme");
    lstFile = dir.entryInfoList();
    //if( lstFile.count()!=0 )
    {
        foreach(QFileInfo fi, lstFile) {
            schemeList.append(fi.absoluteFilePath());
        }
    }
#endif

    dir.setPath(Global::instance()->pathLib() + "scheme");
    lstFile = dir.entryInfoList();
    //if(lstFile != NULL)
    {
        foreach(QFileInfo fi, lstFile) {
            schemeList.append(fi.absoluteFilePath());
        }
    }
    return schemeList;
}

void schemeDialog::updateList()
{
    foreach (QString file, fileList) {
        Config *pConf = new Config(file);
        ui.nameListWidget->addItem(pConf->getItemValue("scheme", "title").toString());
        delete pConf;
    }
}

void schemeDialog::loadScheme(const QString& strSchemeFile)
{
    Config *pConf = new Config(strSchemeFile);

    strCurrentScheme = strSchemeFile;

    title = pConf->getItemValue("scheme", "title").toString();

    for (int i = 0; i < 16; i++) {
        QString colorName = QString("color%1").arg(i);
        schemeColor[i].setNamedColor(pConf->getItemValue("color", colorName).toString());
    }

    delete pConf;

    updateView();

}

void schemeDialog::saveNumScheme(int n)
{
    // FIXME: ?, and there is more below
    QStringList::Iterator it = fileList.begin();
    while (n--)
        it++;

    title = ui.titleLineEdit->text();

#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
    QDir dir(Global::instance()->pathLib());
    QString strSchemeFile = dir.absolutePath() + "/scheme/" + title + ".scheme";
#else
    // save it to $HOME/.qterm/scheme/ with filename=title
    QFileInfo fi(*it);
    QString strSchemeFile = QDir::homePath() + "/.qterm/scheme/" + title + ".scheme";
#endif

    // create a new scheme if title changed
    if (strSchemeFile != strCurrentScheme) {
        ui.nameListWidget->addItem(title);
        fileList.append(strSchemeFile);
    }

    strCurrentScheme = strSchemeFile;

    Config *pConf = new Config(strCurrentScheme);

    pConf->setItemValue("scheme", "title", title);

    for (int i = 0; i < 16; i++) {
        QString colorName = QString("color%1").arg(i);
        pConf->setItemValue("color", colorName, schemeColor[i].name());
    }

    pConf->save(strSchemeFile);

    delete pConf;

    bModified = false;

}

void schemeDialog::setBackgroundColor(QPushButton * widget, const QColor & color)
{
//    QPalette palette;
//    palette.setColor(widget->backgroundRole(), color);
//    widget->setPalette(palette);
    QPixmap pixmap(widget->size());
    pixmap.fill(color);
    QIcon icon(pixmap);
    widget->setIconSize(widget->size());
    widget->setIcon(icon);
}

void schemeDialog::updateView()
{
    // title
    ui.titleLineEdit->setText(title);

    for (int i = 0; i < 16; i++) {
        QString buttonName = QString("clr%1Button").arg(i);
        setBackgroundColor(findChild<QPushButton *>(buttonName), schemeColor[i]);
    }


    // load from file, nothing changed
    bModified = false;
}

void schemeDialog::buttonClicked()
{
    QPushButton * button = (QPushButton*)sender();
    QRegExp rx("\\d+");
    if (rx.indexIn(button->objectName()) == -1) {
        qDebug("object name error");
        return;
    }
    int index = rx.cap().toInt();
    QColor color = QColorDialog::getColor(schemeColor[index]);
    if (color.isValid() == true) {
        setBackgroundColor(button,color);
        schemeColor[index] = color;
        bModified = true;
    }
}

void schemeDialog::nameChanged(int item)
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
                saveNumScheme(nLastItem);
            }
        }
    }

    QStringList::Iterator it = fileList.begin();
    int n = item; //nameListBox->index(item);
    nLastItem = n;
    while (n--)
        it++;
    loadScheme(*it);
}

void schemeDialog::saveScheme()
{
    // get current scheme file name
    int n = ui.nameListWidget->currentRow();
    saveNumScheme(n);
}

void schemeDialog::resetScheme()
{
    loadScheme(strCurrentScheme);
}

void schemeDialog::removeScheme()
{
    QFileInfo fi(strCurrentScheme);
    if (fi.isWritable()) {
        QFile::remove(strCurrentScheme);
        QStringList::Iterator it = fileList.begin();
        int n = ui.nameListWidget->currentRow();
        ui.nameListWidget->takeItem(n);
        while (n--)
            it++;
        fileList.erase(it);
    } else {
        QMessageBox::warning(this, "Error", "This is a system scheme. Permission Denied");
    }
}

void schemeDialog::onOK()
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
            saveNumScheme(n);
        }
    }

    done(1);
}

void schemeDialog::onCancel()
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
            saveNumScheme(n);
        }
    }

    done(0);
}

void schemeDialog::textChanged(const QString&)
{
    bModified = true;
}

} // namespace QTerm

#include <schemedialog.moc>
