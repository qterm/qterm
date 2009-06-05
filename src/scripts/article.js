QTerm.import("utils.js");
var Article = Article ? Article : new Object;
Article.downloading = false;
Article.textList= [];
Article.articleText = "";

Article.getArticle = function()
{
    this.textList = [];
    this.downloading = true;
    this.downloadArticle();
}

// check it there is duplicated string
// it starts from the end in the range of one screen height
// so this is a non-greedy match
Article.checkDuplicate = function()
{
    var strTemp = QTerm.getText(0).rtrim();
    var i=0;
    var start=0;
    if (this.textList.length == 0) {
        return 0;
    }
    for(var index = this.textList.length -1; index > 0, i < QTerm.rows()-1; // not exceeeding the last screen
            --index, i++)
    {
        var text = this.textList[index];
        if(this.textList[index] != strTemp)
            continue;
        var index2 = index;
        var dup=true;
        // match more to see if its duplicated
        for(var j=0; j<=i; j++, index2++)
        {
            str1 = QTerm.getText(j).rtrim();
            if(this.textList[index2]!=str1)
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

Article.pageComplete = function()
{
    var bottom = QTerm.getText(QTerm.rows() - 1).rtrim();
    if (QTerm.caretY() == QTerm.rows() - 1 && QTerm.caretX() >= bottom.length - 1) {
        return true;
    }
}

Article.downloadArticle = function()
{
    if (!this.pageComplete())
        return;
    var start = this.checkDuplicate();
    // add new lines
    for(i=start;i<QTerm.rows()-1;i++)
        this.textList[this.textList.length]=QTerm.getText(i).rtrim();

    // the end of article
    if( QTerm.getText(QTerm.rows()-1).indexOf("%") == -1 ) {
        QTerm.showMessage("Download Complete",1,0);
        this.downloading = false;
        this.articleText = this.textList.join("\n");
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
