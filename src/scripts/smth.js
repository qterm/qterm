function onInput()
{
//    QTerm.sendString("v");
//    QTerm.buzz();
    var text = QInputDialog.getText(this, "Text Input", "Text Input:",QLineEdit.Normal, "", Qt.WindowFlags(0));
    QTerm.sendString(text);
}

QTerm.pageState = -1;
QTerm.downloading = false;
QTerm.textList= [];
QTerm.articleText = "";

QTerm.init = function()
{
    QTerm.showMessage("system script loaded", 1, 0);
}

QTerm.getCursorType = function(pos)
{
    QTerm.accepted = false;
    return -1;
}

QTerm.getBG = function(color)
{
    var bg = (color & 0xf0) >> 4;
    return bg;
}

String.prototype.startsWith = function(list)
{
    for (var i = 0; i < list.length; i++) {
        var item = list[i];
        if (this.substr(0,item.length) == item)
            return true;
    }
    return false;
    //return(this.substr(0,str.length) == str);
}

String.prototype.contains = function(str) {
    return(this.indexOf(str) != -1);
}

String.prototype.trim = function() {
        return this.replace(/^\s+|\s+$/g,"");
}

String.prototype.ltrim = function() {
        return this.replace(/^\s+/,"");
}

String.prototype.rtrim = function() {
        return this.replace(/\s+$/,"");
}

QTerm.getText = function(line) {
    return QTerm.getLine(line).getText();
}

// This is for SMTH only
QTerm.setPageState = function()
{
    QTerm.accepted = true;
    var title = QTerm.getText(0);
    var bottom = QTerm.getText(QTerm.rows()-1);
    QTerm.pageState = -1;
    var menuList = ["主选单","聊天选单","[处理信笺选单]","工具箱选单","分类讨论区选单"];
    var listList = ["版主:","[好朋友列表]","[讨论区列表]","邮件选单","[个人定制区]"];
    var articleList = ["[阅读文章]","下面还有喔","(R)回信, (D)删除, (G)继续? [G]:"];
    if (title.startsWith(menuList))
        QTerm.pageState = 0;
    if (title.startsWith(listList))
        QTerm.pageState = 1;
    if (title.indexOf("水木社区 精华区公布栏")!=-1)
        QTerm.pageState = 1;
    if (title.indexOf("本日十大热门话题")!=-1)
        QTerm.pageState = 1;
    if (bottom.startsWith(articleList))
        QTerm.pageState = 2;
    return QTerm.pageState;
}

QTerm.isLineClickable = function(x, y)
{
    if (QTerm.pageState == 1) {
        if (y >= 3 && y < QTerm.rows() -1 && x > 12 && x < QTerm.columns() - 16 && QTerm.getText(y).search(/[^\s]/)!=-1) {
            QTerm.accepted = true;
            return true;
        }
    }
    QTerm.accepted = false;
    return false
}

QTerm.getClickableString = function(x, y)
{
    QTerm.accepted = true;
    if (QTerm.pageState != 0) {
        return "";
    }
    var line = QTerm.getLine(y);
    var text = line.getText();
    var pos = line.pos(x);
    var pattern = /[\(\[]?[a-zA-Z0-9][\).\]]\s?[^\s]+/g;
    var result;
    var clickableString = "";
    while ((result = pattern.exec(text)) != null) {
        clickableString = result[0];
        if (pos > result.index && pos < result.index + clickableString.length) {
            return clickableString;
        }
        if (pattern.lastIndex == 0)
            return "";
    }
    return ""
}

QTerm.onMouseEvent = function(type, button, buttons, modifiers, pt_x, pt_y)
{
    var x = QTerm.char_x(pt_x, pt_y);
    var y = QTerm.char_y(pt_x, pt_y);
    var accepted = false;
    if (type == 1 && button == 1 && modifiers == 0) {
        accepted = QTerm.sendKey(x, y);
    }
    QTerm.accepted = accepted;
}

QTerm.sendKey = function(x, y)
{
    if (QTerm.pageState == 0) {
        str = QTerm.getClickableString(x,y);
        if (str == "") {
            return false;
        }
        result = str.match(/[\(\[]?([a-zA-Z0-9])[\).\]]\s?[^\s]+/);
        QTerm.sendString(result[1]);
        QTerm.sendString("\n");
        return true;
    }
    return false;
}

QTerm.onKeyPressEvent = function(key, modifiers, text)
{
//    var msg = "The key pressed is: " + text;
//    QTerm.showMessage(msg,1,1000);
    QTerm.accepted = false;
}

QTerm.onWheelEvent = function(delta, buttons, modifiers, pt_x, pt_y, orientation)
{
    QTerm.accepted = false;
}

QTerm.onNewData = function()
{
    QTerm.accepted = false;
    return false;
}

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
        QTerm.downloadFinished();
        return;
    }

    // continue
    QTerm.sendString(" ");

    //strArticle = strList.join("\r\n");
    //strArticle = strList.join("\n");
}

QTerm.antiIdle = function()
{
    QTerm.accepted = false;
}

//The script shoul get the message by itself since the code in QTerm might not be reliable for every
//site.
QTerm.autoReply = function()
{
    QTerm.accepted = false;
}

QTerm.checkUrl = function(x, y)
{
    QTerm.accepted = false;
    return "";
}

QTerm.checkIP = function(x, y)
{
    QTerm.accepted = false;
    return "";
}

QTerm.onTelnetState = function(state)
{
    QTerm.accepted = false;
    return;
}

QTerm.onZmodemState = function(type, value, state)
{
    QTerm.accepted = false;
    return;
}
