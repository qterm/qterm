QTerm.loadScript("utils.js");
QTerm.highlightKeywords = function(pattern)
{
    for (var i = 0; i < QTerm.rows(); i++) {
        var line = QTerm.getLine(i);
        var text = line.getText();
        pattern.lastIndex = 0;
        while ((result = pattern.exec(text)) != null) {
            var index = line.beginIndex(result.index);
            // Definition of color and attr can be found in qterm.h, The actuall color might change once we have schema support back
            var color = QTerm.setAttr(QTerm.Attr.Default|QTerm.Attr.Bold)|QTerm.setColor(QTerm.setFG(QTerm.Color.Red)|QTerm.setBG(QTerm.Color.Black));
            line.replaceText(result[0],color,index, -1);
        }
    }
}

