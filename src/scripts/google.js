QTerm.loadScript("utils.js");
QTerm.onGoogle= function()
{
    text = QTerm.getSelectedText();
    if (text.size > 0) {
        url = "http://www.google.com/search?q="+QTerm.getSelectedText()+"&ie=UTF-8&oe=UTF-8";
        QTerm.openUrl(url);
    } else
        QTerm.osdMessage(qsTr("No text is selected to search for"), QTerm.OSDType.Warning, 5000);
}

if (QTerm.addPopupMenu( "googleSearch", qsTr("Search Selected Text in Google") ) ) {
        QTerm.googleSearch.triggered.connect(QTerm.onGoogle);
}


