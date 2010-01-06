QTerm.loadScript("utils.js");

var SendDelay = SendDelay ? SendDelay : new Object;
SendDelay.stringQueue = new Array;

SendDelay.t = new QTimer;

SendDelay.q = new QEventLoop;

SendDelay.send = function(text, times, delay)
{
    for (var i = 0; i < times; i++) {
        this.addSendString(text);
    }
    this.t.start(delay);
    this.t.timeout.connect(this,this.sendOneChar);
    QTerm.scriptEvent.connect(this,this.q.quit);
    this.q.exec();
    QTerm.showMessage("Send String Finished", QTerm.OSDType.Info, 2000);
    this.t.timeout.disconnect(this,this.sendOneChar);
    QTerm.scriptEvent.disconnect(this,this.q.quit);
}

SendDelay.addSendString = function(str)
{
    this.stringQueue[this.stringQueue.length] = str;
}

SendDelay.sendOneChar = function()
{
    if (this.stringQueue.length == 0) {
        this.t.stop();
        QTerm.scriptEvent("Finished");
        return;
    }
    var text = this.stringQueue.shift();
    QTerm.sendParsedString(text);
}

QTerm.SendDelay = SendDelay;

QTerm.onSendDelay = function()
{
    var text = QInputDialog.getText(this, "Send String With Delay", "String:", QLineEdit.Normal, "", Qt.WindowFlags(0));
    var times = QInputDialog.getInteger(this, "Send String With Delay", "Times:", 1, 1, 10000, 1, Qt.WindowFlags(0));
    var delay= QInputDialog.getInteger(this, "Send String With Delay", "Delay(s):", 1, 1, 10000, 1, Qt.WindowFlags(0)) * 1000;
    QTerm.showMessage("send \""+text+"\" "+times+" times with "+delay+" ms delay", 1, 2000);
    QTerm.SendDelay.send(text,times,delay);
}

if (QTerm.addPopupMenu( "sendDelay", "Send String With Delay..." ) ) {
        QTerm.sendDelay.triggered.connect(QTerm.onSendDelay);
}


