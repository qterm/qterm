/****************************************************************************
** Form implementation generated from reading ui file 'addrdialog.ui'
**
** Created: Mon Sep 13 21:42:40 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "addrdialogui.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qframe.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a addrDialogUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addrDialogUI::addrDialogUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "addrDialogUI" );
    setMinimumSize( QSize( 650, 360 ) );
    setMaximumSize( QSize( 650, 360 ) );
    QFont f( font() );
    setFont( f ); 
    setSizeGripEnabled( FALSE );

    applyPushButton = new QPushButton( this, "applyPushButton" );
    applyPushButton->setGeometry( QRect( 20, 330, 70, 21 ) );

    connectPushButton = new QPushButton( this, "connectPushButton" );
    connectPushButton->setGeometry( QRect( 170, 330, 70, 21 ) );
    connectPushButton->setAutoDefault( TRUE );
    connectPushButton->setDefault( TRUE );

    closePushButton = new QPushButton( this, "closePushButton" );
    closePushButton->setGeometry( QRect( 300, 330, 64, 21 ) );
    closePushButton->setAutoDefault( TRUE );

    nameListBox = new QListBox( this, "nameListBox" );
    nameListBox->setGeometry( QRect( 470, 10, 166, 310 ) );
    nameListBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, nameListBox->sizePolicy().hasHeightForWidth() ) );

    deletePushButton = new QPushButton( this, "deletePushButton" );
    deletePushButton->setGeometry( QRect( 580, 330, 51, 21 ) );
    deletePushButton->setAutoDefault( TRUE );

    addPushButton = new QPushButton( this, "addPushButton" );
    addPushButton->setGeometry( QRect( 490, 330, 50, 21 ) );

    Line2 = new QFrame( this, "Line2" );
    Line2->setGeometry( QRect( 450, 0, 20, 380 ) );
    Line2->setFrameShape( QFrame::VLine );
    Line2->setFrameShadow( QFrame::Sunken );
    Line2->setFrameShape( QFrame::VLine );

    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setGeometry( QRect( 10, 10, 420, 313 ) );
    QFont tabWidget_font(  tabWidget->font() );
    tabWidget->setFont( tabWidget_font ); 

    Widget2 = new QWidget( tabWidget, "Widget2" );

    userLineEdit = new QLineEdit( Widget2, "userLineEdit" );
    userLineEdit->setGeometry( QRect( 120, 170, 140, 23 ) );

    postloginLineEdit = new QLineEdit( Widget2, "postloginLineEdit" );
    postloginLineEdit->setGeometry( QRect( 120, 230, 140, 23 ) );

    preloginLineEdit = new QLineEdit( Widget2, "preloginLineEdit" );
    preloginLineEdit->setGeometry( QRect( 120, 140, 140, 23 ) );

    passwdLineEdit = new QLineEdit( Widget2, "passwdLineEdit" );
    passwdLineEdit->setGeometry( QRect( 120, 200, 140, 23 ) );
    passwdLineEdit->setEchoMode( QLineEdit::Password );

    hostTypeComboBox = new QComboBox( FALSE, Widget2, "hostTypeComboBox" );
    hostTypeComboBox->setGeometry( QRect( 120, 80, 140, 22 ) );

    addrLineEdit = new QLineEdit( Widget2, "addrLineEdit" );
    addrLineEdit->setGeometry( QRect( 120, 40, 160, 23 ) );

    portLineEdit = new QLineEdit( Widget2, "portLineEdit" );
    portLineEdit->setGeometry( QRect( 350, 40, 40, 23 ) );

    nameLineEdit = new QLineEdit( Widget2, "nameLineEdit" );
    nameLineEdit->setGeometry( QRect( 120, 10, 160, 23 ) );

    TextLabel1 = new QLabel( Widget2, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 10, 70, 20 ) );
    TextLabel1->setAlignment( int( QLabel::AlignVCenter ) );

    TextLabel2 = new QLabel( Widget2, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 8, 43, 80, 20 ) );
    TextLabel2->setAlignment( int( QLabel::AlignVCenter ) );

    TextLabel3 = new QLabel( Widget2, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 288, 43, 50, 20 ) );

    TextLabel3_6 = new QLabel( Widget2, "TextLabel3_6" );
    TextLabel3_6->setGeometry( QRect( 10, 80, 65, 20 ) );

    autoLoginCheckBox = new QCheckBox( Widget2, "autoLoginCheckBox" );
    autoLoginCheckBox->setGeometry( QRect( 10, 110, 130, 22 ) );

    TextLabel1_2 = new QLabel( Widget2, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 20, 140, 80, 20 ) );

    TextLabel4 = new QLabel( Widget2, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 20, 170, 80, 20 ) );
    TextLabel4->setAlignment( int( QLabel::AlignVCenter ) );

    TextLabel5 = new QLabel( Widget2, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 20, 200, 80, 20 ) );
    TextLabel5->setAlignment( int( QLabel::AlignVCenter ) );

    TextLabel11 = new QLabel( Widget2, "TextLabel11" );
    TextLabel11->setGeometry( QRect( 20, 230, 90, 20 ) );
    tabWidget->insertTab( Widget2, QString("") );

    tab = new QWidget( tabWidget, "tab" );

    ansicolorCheckBox = new QCheckBox( tab, "ansicolorCheckBox" );
    ansicolorCheckBox->setGeometry( QRect( 200, 80, 130, 20 ) );

    highlightCheckBox = new QCheckBox( tab, "highlightCheckBox" );
    highlightCheckBox->setGeometry( QRect( 10, 80, 150, 22 ) );

    bgcolorPushButton = new QPushButton( tab, "bgcolorPushButton" );
    bgcolorPushButton->setGeometry( QRect( 260, 190, 90, 21 ) );

    fgcolorPushButton = new QPushButton( tab, "fgcolorPushButton" );
    fgcolorPushButton->setGeometry( QRect( 260, 160, 90, 21 ) );

    schemaPushButton = new QPushButton( tab, "schemaPushButton" );
    schemaPushButton->setGeometry( QRect( 260, 220, 90, 21 ) );

    displayTextLabel = new QLabel( tab, "displayTextLabel" );
    displayTextLabel->setGeometry( QRect( 40, 120, 170, 130 ) );

    autofontCheckBox = new QCheckBox( tab, "autofontCheckBox" );
    autofontCheckBox->setGeometry( QRect( 10, 50, 340, 22 ) );

    TextLabel6 = new QLabel( tab, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 10, 20, 100, 20 ) );

    bbscodeComboBox = new QComboBox( FALSE, tab, "bbscodeComboBox" );
    bbscodeComboBox->setGeometry( QRect( 120, 20, 60, 23 ) );

    TextLabel7 = new QLabel( tab, "TextLabel7" );
    TextLabel7->setGeometry( QRect( 190, 20, 100, 20 ) );

    displaycodeComboBox = new QComboBox( FALSE, tab, "displaycodeComboBox" );
    displaycodeComboBox->setGeometry( QRect( 300, 20, 60, 23 ) );

    fontPushButton = new QPushButton( tab, "fontPushButton" );
    fontPushButton->setGeometry( QRect( 260, 130, 90, 21 ) );
    tabWidget->insertTab( tab, QString("") );

    tab_2 = new QWidget( tabWidget, "tab_2" );

    termtypeLineEdit = new QLineEdit( tab_2, "termtypeLineEdit" );
    termtypeLineEdit->setGeometry( QRect( 100, 20, 90, 23 ) );

    escapeLineEdit = new QLineEdit( tab_2, "escapeLineEdit" );
    escapeLineEdit->setGeometry( QRect( 210, 150, 140, 23 ) );

    columnLineEdit = new QLineEdit( tab_2, "columnLineEdit" );
    columnLineEdit->setGeometry( QRect( 100, 60, 90, 23 ) );

    TextLabel3_4 = new QLabel( tab_2, "TextLabel3_4" );
    TextLabel3_4->setGeometry( QRect( 10, 60, 80, 20 ) );

    TextLabel1_5 = new QLabel( tab_2, "TextLabel1_5" );
    TextLabel1_5->setGeometry( QRect( 10, 20, 80, 20 ) );
    QFont TextLabel1_5_font(  TextLabel1_5->font() );
    TextLabel1_5->setFont( TextLabel1_5_font ); 

    TextLabel6_3 = new QLabel( tab_2, "TextLabel6_3" );
    TextLabel6_3->setGeometry( QRect( 210, 100, 90, 20 ) );

    TextLabel4_3 = new QLabel( tab_2, "TextLabel4_3" );
    TextLabel4_3->setGeometry( QRect( 10, 100, 66, 20 ) );

    rowLineEdit = new QLineEdit( tab_2, "rowLineEdit" );
    rowLineEdit->setGeometry( QRect( 100, 100, 90, 23 ) );

    cursorTypeComboBox = new QComboBox( FALSE, tab_2, "cursorTypeComboBox" );
    cursorTypeComboBox->setGeometry( QRect( 310, 100, 100, 23 ) );

    TextLabel5_3 = new QLabel( tab_2, "TextLabel5_3" );
    TextLabel5_3->setGeometry( QRect( 210, 60, 100, 20 ) );

    TextLabel1_4 = new QLabel( tab_2, "TextLabel1_4" );
    TextLabel1_4->setGeometry( QRect( 10, 150, 190, 20 ) );

    TextLabel2_5 = new QLabel( tab_2, "TextLabel2_5" );
    TextLabel2_5->setGeometry( QRect( 210, 20, 80, 20 ) );

    keytypeComboBox = new QComboBox( FALSE, tab_2, "keytypeComboBox" );
    keytypeComboBox->setGeometry( QRect( 310, 20, 100, 23 ) );

    scrollLineEdit = new QLineEdit( tab_2, "scrollLineEdit" );
    scrollLineEdit->setGeometry( QRect( 330, 60, 70, 23 ) );
    tabWidget->insertTab( tab_2, QString("") );

    tab_3 = new QWidget( tabWidget, "tab_3" );

    GroupBox2 = new QGroupBox( tab_3, "GroupBox2" );
    GroupBox2->setGeometry( QRect( 10, 160, 400, 120 ) );

    protocolComboBox = new QComboBox( FALSE, GroupBox2, "protocolComboBox" );
    protocolComboBox->setGeometry( QRect( 110, 20, 85, 23 ) );

    sshuserLineEdit = new QLineEdit( GroupBox2, "sshuserLineEdit" );
    sshuserLineEdit->setGeometry( QRect( 110, 50, 108, 23 ) );

    sshpasswdLineEdit = new QLineEdit( GroupBox2, "sshpasswdLineEdit" );
    sshpasswdLineEdit->setGeometry( QRect( 110, 80, 108, 23 ) );
    sshpasswdLineEdit->setEchoMode( QLineEdit::Password );

    TextLabel3_3 = new QLabel( GroupBox2, "TextLabel3_3" );
    TextLabel3_3->setGeometry( QRect( 10, 80, 90, 20 ) );

    TextLabel2_3 = new QLabel( GroupBox2, "TextLabel2_3" );
    TextLabel2_3->setGeometry( QRect( 10, 50, 90, 20 ) );

    TextLabel1_3 = new QLabel( GroupBox2, "TextLabel1_3" );
    TextLabel1_3->setGeometry( QRect( 10, 20, 80, 20 ) );

    GroupBox1 = new QGroupBox( tab_3, "GroupBox1" );
    GroupBox1->setGeometry( QRect( 10, 0, 400, 160 ) );

    proxyaddrLineEdit = new QLineEdit( GroupBox1, "proxyaddrLineEdit" );
    proxyaddrLineEdit->setGeometry( QRect( 110, 50, 108, 23 ) );

    proxyuserLineEdit = new QLineEdit( GroupBox1, "proxyuserLineEdit" );
    proxyuserLineEdit->setGeometry( QRect( 150, 100, 108, 23 ) );

    proxypasswdLineEdit = new QLineEdit( GroupBox1, "proxypasswdLineEdit" );
    proxypasswdLineEdit->setGeometry( QRect( 150, 130, 108, 23 ) );
    proxypasswdLineEdit->setEchoMode( QLineEdit::Password );

    TextLabel5_2 = new QLabel( GroupBox1, "TextLabel5_2" );
    TextLabel5_2->setGeometry( QRect( 58, 103, 80, 20 ) );

    TextLabel6_2 = new QLabel( GroupBox1, "TextLabel6_2" );
    TextLabel6_2->setGeometry( QRect( 58, 133, 80, 20 ) );

    proxyportLineEdit = new QLineEdit( GroupBox1, "proxyportLineEdit" );
    proxyportLineEdit->setGeometry( QRect( 300, 50, 60, 23 ) );

    TextLabel4_2 = new QLabel( GroupBox1, "TextLabel4_2" );
    TextLabel4_2->setGeometry( QRect( 240, 50, 50, 20 ) );

    TextLabel3_2 = new QLabel( GroupBox1, "TextLabel3_2" );
    TextLabel3_2->setGeometry( QRect( 10, 50, 66, 20 ) );

    TextLabel2_2 = new QLabel( GroupBox1, "TextLabel2_2" );
    TextLabel2_2->setGeometry( QRect( 10, 20, 66, 20 ) );

    proxytypeComboBox = new QComboBox( FALSE, GroupBox1, "proxytypeComboBox" );
    proxytypeComboBox->setGeometry( QRect( 110, 20, 85, 23 ) );

    authCheckBox = new QCheckBox( GroupBox1, "authCheckBox" );
    authCheckBox->setGeometry( QRect( 10, 80, 130, 22 ) );
    tabWidget->insertTab( tab_3, QString("") );

    Widget3 = new QWidget( tabWidget, "Widget3" );

    TextLabel9 = new QLabel( Widget3, "TextLabel9" );
    TextLabel9->setGeometry( QRect( 270, 50, 30, 20 ) );

    idletimeLineEdit = new QLineEdit( Widget3, "idletimeLineEdit" );
    idletimeLineEdit->setGeometry( QRect( 170, 50, 80, 23 ) );

    retryLineEdit = new QLineEdit( Widget3, "retryLineEdit" );
    retryLineEdit->setGeometry( QRect( 140, 180, 60, 20 ) );

    scriptPushButton = new QPushButton( Widget3, "scriptPushButton" );
    scriptPushButton->setGeometry( QRect( 290, 240, 50, 30 ) );

    antiLineEdit = new QLineEdit( Widget3, "antiLineEdit" );
    antiLineEdit->setGeometry( QRect( 170, 20, 80, 23 ) );

    scriptCheckBox = new QCheckBox( Widget3, "scriptCheckBox" );
    scriptCheckBox->setGeometry( QRect( 10, 210, 170, 22 ) );

    replyCheckBox = new QCheckBox( Widget3, "replyCheckBox" );
    replyCheckBox->setGeometry( QRect( 10, 110, 160, 22 ) );

    textLabel1_2 = new QLabel( Widget3, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 80, 170, 20 ) );

    TextLabel8 = new QLabel( Widget3, "TextLabel8" );
    TextLabel8->setGeometry( QRect( 10, 50, 130, 20 ) );

    TextLabel7_2 = new QLabel( Widget3, "TextLabel7_2" );
    TextLabel7_2->setGeometry( QRect( 10, 20, 150, 20 ) );

    scriptLineEdit = new QLineEdit( Widget3, "scriptLineEdit" );
    scriptLineEdit->setGeometry( QRect( 40, 240, 230, 22 ) );

    replyLineEdit = new QLineEdit( Widget3, "replyLineEdit" );
    replyLineEdit->setGeometry( QRect( 190, 110, 220, 23 ) );

    replykeyLineEdit = new QLineEdit( Widget3, "replykeyLineEdit" );
    replykeyLineEdit->setGeometry( QRect( 190, 80, 90, 23 ) );

    textLabel4 = new QLabel( Widget3, "textLabel4" );
    textLabel4->setGeometry( QRect( 30, 180, 100, 20 ) );

    textLabel5 = new QLabel( Widget3, "textLabel5" );
    textLabel5->setGeometry( QRect( 210, 180, 200, 20 ) );

    textLabel2 = new QLabel( Widget3, "textLabel2" );
    textLabel2->setGeometry( QRect( 370, 150, 20, 20 ) );

    reconnectCheckBox = new QCheckBox( Widget3, "reconnectCheckBox" );
    reconnectCheckBox->setGeometry( QRect( 10, 150, 310, 20 ) );

    reconnectLineEdit = new QLineEdit( Widget3, "reconnectLineEdit" );
    reconnectLineEdit->setGeometry( QRect( 320, 150, 30, 22 ) );
    tabWidget->insertTab( Widget3, QString("") );

    tab_4 = new QWidget( tabWidget, "tab_4" );

    menuGroup = new QButtonGroup( tab_4, "menuGroup" );
    menuGroup->setGeometry( QRect( 10, 10, 390, 110 ) );

    radioButton1 = new QRadioButton( menuGroup, "radioButton1" );
    radioButton1->setGeometry( QRect( 10, 30, 90, 20 ) );

    radioButton2 = new QRadioButton( menuGroup, "radioButton2" );
    radioButton2->setGeometry( QRect( 110, 30, 80, 20 ) );

    radioButton3 = new QRadioButton( menuGroup, "radioButton3" );
    radioButton3->setGeometry( QRect( 200, 30, 107, 20 ) );

    menucolorButton = new QPushButton( menuGroup, "menucolorButton" );
    menucolorButton->setGeometry( QRect( 180, 70, 93, 25 ) );

    menuLabel = new QLabel( menuGroup, "menuLabel" );
    menuLabel->setGeometry( QRect( 50, 70, 100, 20 ) );
    tabWidget->insertTab( tab_4, QString("") );
    languageChange();
    resize( QSize(650, 360).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( nameListBox, tabWidget );
    setTabOrder( tabWidget, nameLineEdit );
    setTabOrder( nameLineEdit, addrLineEdit );
    setTabOrder( addrLineEdit, portLineEdit );
    setTabOrder( portLineEdit, hostTypeComboBox );
    setTabOrder( hostTypeComboBox, autoLoginCheckBox );
    setTabOrder( autoLoginCheckBox, preloginLineEdit );
    setTabOrder( preloginLineEdit, userLineEdit );
    setTabOrder( userLineEdit, passwdLineEdit );
    setTabOrder( passwdLineEdit, postloginLineEdit );
    setTabOrder( postloginLineEdit, bbscodeComboBox );
    setTabOrder( bbscodeComboBox, displaycodeComboBox );
    setTabOrder( displaycodeComboBox, autofontCheckBox );
    setTabOrder( autofontCheckBox, highlightCheckBox );
    setTabOrder( highlightCheckBox, ansicolorCheckBox );
    setTabOrder( ansicolorCheckBox, fontPushButton );
    setTabOrder( fontPushButton, fgcolorPushButton );
    setTabOrder( fgcolorPushButton, bgcolorPushButton );
    setTabOrder( bgcolorPushButton, schemaPushButton );
    setTabOrder( schemaPushButton, termtypeLineEdit );
    setTabOrder( termtypeLineEdit, keytypeComboBox );
    setTabOrder( keytypeComboBox, columnLineEdit );
    setTabOrder( columnLineEdit, scrollLineEdit );
    setTabOrder( scrollLineEdit, rowLineEdit );
    setTabOrder( rowLineEdit, cursorTypeComboBox );
    setTabOrder( cursorTypeComboBox, escapeLineEdit );
    setTabOrder( escapeLineEdit, proxytypeComboBox );
    setTabOrder( proxytypeComboBox, proxyaddrLineEdit );
    setTabOrder( proxyaddrLineEdit, proxyportLineEdit );
    setTabOrder( proxyportLineEdit, authCheckBox );
    setTabOrder( authCheckBox, proxyuserLineEdit );
    setTabOrder( proxyuserLineEdit, proxypasswdLineEdit );
    setTabOrder( proxypasswdLineEdit, protocolComboBox );
    setTabOrder( protocolComboBox, sshuserLineEdit );
    setTabOrder( sshuserLineEdit, sshpasswdLineEdit );
    setTabOrder( sshpasswdLineEdit, antiLineEdit );
    setTabOrder( antiLineEdit, idletimeLineEdit );
    setTabOrder( idletimeLineEdit, replykeyLineEdit );
    setTabOrder( replykeyLineEdit, replyCheckBox );
    setTabOrder( replyCheckBox, replyLineEdit );
    setTabOrder( replyLineEdit, reconnectCheckBox );
    setTabOrder( reconnectCheckBox, reconnectLineEdit );
    setTabOrder( reconnectLineEdit, retryLineEdit );
    setTabOrder( retryLineEdit, scriptCheckBox );
    setTabOrder( scriptCheckBox, scriptLineEdit );
    setTabOrder( scriptLineEdit, scriptPushButton );
    setTabOrder( scriptPushButton, radioButton1 );
    setTabOrder( radioButton1, radioButton2 );
    setTabOrder( radioButton2, radioButton3 );
    setTabOrder( radioButton3, menucolorButton );
    setTabOrder( menucolorButton, applyPushButton );
    setTabOrder( applyPushButton, connectPushButton );
    setTabOrder( connectPushButton, closePushButton );
    setTabOrder( closePushButton, addPushButton );
    setTabOrder( addPushButton, deletePushButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
addrDialogUI::~addrDialogUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addrDialogUI::languageChange()
{
    setCaption( tr( "AddressBook" ) );
    applyPushButton->setText( tr( "Apply" ) );
    connectPushButton->setText( tr( "Connect" ) );
    connectPushButton->setAccel( QKeySequence( QString::null ) );
    closePushButton->setText( tr( "Close" ) );
    closePushButton->setAccel( QKeySequence( QString::null ) );
    deletePushButton->setText( tr( "Delete" ) );
    deletePushButton->setAccel( QKeySequence( QString::null ) );
    addPushButton->setText( tr( "Add" ) );
    hostTypeComboBox->clear();
    hostTypeComboBox->insertItem( tr( "BBS" ) );
    hostTypeComboBox->insertItem( tr( "*NIX" ) );
    TextLabel1->setText( tr( "Name" ) );
    TextLabel2->setText( tr( "Address" ) );
    TextLabel3->setText( tr( "Port" ) );
    TextLabel3_6->setText( tr( "Type" ) );
    autoLoginCheckBox->setText( tr( "Auto Login" ) );
    TextLabel1_2->setText( tr( "Prelogin" ) );
    TextLabel4->setText( tr( "User" ) );
    TextLabel5->setText( tr( "Password" ) );
    TextLabel11->setText( tr( "Postlogin" ) );
    tabWidget->changeTab( Widget2, tr( "General" ) );
    ansicolorCheckBox->setText( tr( "ANSI Color" ) );
    highlightCheckBox->setText( tr( "Always Hightlight" ) );
    bgcolorPushButton->setText( tr( "Background" ) );
    fgcolorPushButton->setText( tr( "Text Color" ) );
    schemaPushButton->setText( tr( "Schema" ) );
    displayTextLabel->setText( tr( "TextLabel3" ) );
    autofontCheckBox->setText( tr( "Auto Select Font Size When Resize Window" ) );
    TextLabel6->setText( tr( "BBS Encoding" ) );
    bbscodeComboBox->clear();
    bbscodeComboBox->insertItem( tr( "GBK" ) );
    bbscodeComboBox->insertItem( tr( "BIG5" ) );
    TextLabel7->setText( tr( "Display Code" ) );
    displaycodeComboBox->clear();
    displaycodeComboBox->insertItem( tr( "GBK" ) );
    displaycodeComboBox->insertItem( tr( "BIG5" ) );
    fontPushButton->setText( tr( "Font" ) );
    tabWidget->changeTab( tab, tr( "Display" ) );
    TextLabel3_4->setText( tr( "Columns" ) );
    TextLabel1_5->setText( tr( "Term Type" ) );
    TextLabel6_3->setText( tr( "Cursor Type" ) );
    TextLabel4_3->setText( tr( "Rows" ) );
    cursorTypeComboBox->clear();
    cursorTypeComboBox->insertItem( tr( "Block" ) );
    cursorTypeComboBox->insertItem( tr( "Underline" ) );
    cursorTypeComboBox->insertItem( tr( "I Type" ) );
    TextLabel5_3->setText( tr( "Scroll Lines" ) );
    TextLabel1_4->setText( tr( "Escape string (i.e. *[ )" ) );
    TextLabel2_5->setText( tr( "Key Type" ) );
    keytypeComboBox->clear();
    keytypeComboBox->insertItem( tr( "VT100" ) );
    keytypeComboBox->insertItem( tr( "XTERM" ) );
    keytypeComboBox->insertItem( tr( "BBS" ) );
    tabWidget->changeTab( tab_2, tr( "Terminal" ) );
    GroupBox2->setTitle( tr( "Protocol" ) );
    protocolComboBox->clear();
    protocolComboBox->insertItem( tr( "Telnet" ) );
    protocolComboBox->insertItem( tr( "SSH1" ) );
    protocolComboBox->insertItem( tr( "SSH2" ) );
    TextLabel3_3->setText( tr( "Password" ) );
    TextLabel2_3->setText( tr( "User" ) );
    TextLabel1_3->setText( tr( "Type" ) );
    GroupBox1->setTitle( tr( "Proxy" ) );
    TextLabel5_2->setText( tr( "User" ) );
    TextLabel6_2->setText( tr( "Password" ) );
    TextLabel4_2->setText( tr( "Port" ) );
    TextLabel3_2->setText( tr( "Address" ) );
    TextLabel2_2->setText( tr( "Type" ) );
    proxytypeComboBox->clear();
    proxytypeComboBox->insertItem( tr( "None" ) );
    proxytypeComboBox->insertItem( tr( "Wingate" ) );
    proxytypeComboBox->insertItem( tr( "SOCKS4" ) );
    proxytypeComboBox->insertItem( tr( "SOCKS5" ) );
    proxytypeComboBox->insertItem( tr( "HTTP" ) );
    authCheckBox->setText( tr( "Authentation" ) );
    tabWidget->changeTab( tab_3, tr( "Connection" ) );
    TextLabel9->setText( tr( "s" ) );
    scriptPushButton->setText( tr( "..." ) );
    scriptCheckBox->setText( tr( "Load Control Script" ) );
    replyCheckBox->setText( tr( "Autoreply Message" ) );
    textLabel1_2->setText( tr( "Key to reply message" ) );
    TextLabel8->setText( tr( "Max Idle Time" ) );
    TextLabel7_2->setText( tr( "Anti-idle String" ) );
    textLabel4->setText( tr( "For Maximum" ) );
    textLabel5->setText( tr( "Times (-1 means infinite)" ) );
    textLabel2->setText( tr( "s" ) );
    reconnectCheckBox->setText( tr( "When disconnected, reconnect every" ) );
    tabWidget->changeTab( Widget3, tr( "Misc" ) );
    menuGroup->setTitle( tr( "Menu Type" ) );
    radioButton1->setText( tr( "Underline" ) );
    radioButton2->setText( tr( "Reverse" ) );
    radioButton3->setText( tr( "Custom Color" ) );
    menucolorButton->setText( tr( "Color..." ) );
    menuLabel->setText( tr( "Menu Label" ) );
    tabWidget->changeTab( tab_4, tr( "Mouse" ) );
}

