QTerm.import("utils.js");
QTerm.downloading = false;
QTerm.textList= [];
QTerm.articleText = "";

QTerm.getArticle = function()
{
    QTerm.textList = [];
    QTerm.downloading = true;
    QTerm.downloadArticle();
}

// check it there is duplicated string
// it starts from the end in the range of one screen height
// so this is a non-greedy match
QTerm.checkDuplicate = function()
{
    var strTemp = QTerm.getText(0).rtrim();
    var i=0;
    var start=0;
    if (QTerm.textList.length == 0) {
        return 0;
    }
    for(var index = QTerm.textList.length -1; index > 0, i < QTerm.rows()-1; // not exceeeding the last screen
            --index, i++)
    {
        var text = QTerm.textList[index];
        if(QTerm.textList[index] != strTemp)
            continue;
        var index2 = index;
        var dup=true;
        // match more to see if its duplicated
        for(var j=0; j<=i; j++, index2++)
        {
            str1 = QTerm.getText(j).rtrim();
            if(QTerm.textList[index2]!=str1)
            {
                dup = false;
                break;
            }
        }
        if(dup)
        {
            // set the start point
            start = i+1;
            break;
        }
    }
    return start;
}

QTerm.pageComplete = function()
{
    var bottom = QTerm.getText(QTerm.rows() - 1).rtrim();
    if (QTerm.caretY() == QTerm.rows() - 1 && QTerm.caretX() >= bottom.length - 1) {
        return true;
    }
}

QTerm.downloadArticle = function()
{
    if (!QTerm.pageComplete())
        return;
    var start = QTerm.checkDuplicate();
    // add new lines
    for(i=start;i<QTerm.rows()-1;i++)
        QTerm.textList[QTerm.textList.length]=QTerm.getText(i).rtrim();

    // the end of article
    if( QTerm.getText(QTerm.rows()-1).indexOf("%") == -1 ) {
        QTerm.showMessage("Download Complete",1,0);
        QTerm.downloading = false;
        QTerm.articleText = QTerm.textList.join("\n");
        //var file = new QFile("test.txt");
        //file.open(QIODevice.OpenMode(QIODevice.WriteOnly, QIODevice.Text));
        //var stream = new QTextStream(file);
        //stream.writeString(article);
        //file.close();
        QTerm.scriptEvent("downloadFinished");
        return;
    }

    // continue
    QTerm.sendString(" ");
}
