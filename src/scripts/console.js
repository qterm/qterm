function ScriptConsoleMainWindow()
{
    file = QTerm.findFile("ui/console.qml")
    component = Qt.createComponent(file)
    return component.createObject(null, {});
}

scriptConsoleMainWindow = ScriptConsoleMainWindow();
if (QTerm.addPopupMenu( "scriptConsole", qsTr("Script Console...") ) ) {
        QTerm.scriptConsole.triggered.connect(scriptConsoleMainWindow.show);
}

