QTerm.loadScript("utils.js");
var Article = Article ? Article : new Object;
Article.downloading = false;
Article.textList= [];
Article.articleText = "";

Article.q = new QEventLoop;

Article.getArticle = function()
{
    this.textList = [];
    this.downloading = true;
    QTerm.scriptEvent.connect(this, this.downloadArticle);
    this.downloadArticle("Article: start");
    QTerm.eventFinished.connect(this,this.q.quit);
    if (this.downloading)
        this.q.exec();
    QTerm.eventFinished.disconnect(this,this.q.quit);
    return Article.articleText;
}

// check it there is duplicated string
// it starts from the end in the range of one screen height
// so this is a non-greedy match
Article.checkDuplicate = function()
{
    if (this.textList.length == 0) {
        return 0;
    }
    var i=0;
    var emptyLines = 0
    var start=0;
    var strTemp = QTerm.getText(0).rtrim();
    var size = strTemp.length
    while ( emptyLines < QTerm.rows() -1 && strTemp.length == 0) {
        emptyLines++;
        strTemp = QTerm.getText(emptyLines).rtrim();
    }

    if (strTemp.length == 0) {
        return 0;
    }

    for(var index = this.textList.length -1; index > 0, i < QTerm.rows()-1; // not exceeeding the last screen
            --index, i++)
    {
        if(this.textList[index] != strTemp)
            continue;
        var index2 = index;
        var dup=true;
        // match more to see if its duplicated
        for(var j=emptyLines; j<=i; j++, index2++)
        {
            var str1 = QTerm.getText(j).rtrim();
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

Article.downloadArticle = function(message)
{
    if ((message == "QTerm: new data")||(message == "Article: start"))
    {
        if (!this.pageComplete())
            return;
        var start = this.checkDuplicate();
        // add new lines
        for(i=start;i<QTerm.rows()-1;i++)
            this.textList[this.textList.length]=QTerm.getText(i).rtrim();

        // the end of article
        if( QTerm.endOfArticle() ) {
            this.downloading = false;
            this.articleText = this.textList.join("\n");
            QTerm.scriptEvent.disconnect(this, this.downloadArticle);
            QTerm.eventFinished();
            return;
        }

        // continue
        QTerm.sendString(" ");
    }
}

QTerm.Article = Article;

