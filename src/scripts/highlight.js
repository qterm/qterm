QTerm.import("utils.js");
QTerm.highlightKeywords = function(pattern)
{
    for (var i = 0; i < QTerm.rows(); i++) {
        var line = QTerm.getLine(i);
        var text = line.getText();
        pattern.lastIndex = 0;
        while ((result = pattern.exec(text)) != null) {
            var index = line.beginIndex(result.index);
            line.replaceText(result[0],1281,index, -1);
        }
    }
}

