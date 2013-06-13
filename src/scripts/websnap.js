QTerm.loadExtension("qt.core");
QTerm.loadExtension("qt.network");
QTerm.loadExtension("qt.gui");
QTerm.loadExtension("qt.webkit");

var WebSnap = WebSnap ? WebSnap : new Object;
WebSnap.webPage = new QWebPage;
WebSnap.label = new QLabel;
//WebSnap.label = new QLabel(QTerm.window());
WebSnap.loading = false;

WebSnap.showSnap = function(ok)
{
    if (!ok) {
        QTerm.osdMessage("cannot download the webpage");
        return;
    }
    this.loading = false;
    var target = new QSize(400, 300);
    var size = this.webPage.mainFrame().contentsSize;
    size.setHeight(size.width() * target.height() / target.width());

    // create the target surface
    var image = new QPixmap(size);
    image.fill(Qt.transparent);

    // render and rescale
    var p = new QPainter;
    p.begin(image);
    this.webPage.viewportSize = this.webPage.mainFrame().contentsSize;
    this.webPage.mainFrame().render(p);
    p.end();
    image = image.scaled(target, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation);
//    QTerm.osdMessage("generating finished", 1, 0);
    this.label.size = target;
    this.label.pixmap = image;
}

WebSnap.getWebsnap = function( urlStr )
{
//    QTerm.osdMessage("generating websnap", 1, 0);
    var url = new QUrl(urlStr, QUrl.TolerantMode);
//    this.webPage = new QWebPage;
//    this.label = new QLabel;
    this.webPage.loadFinished.connect(this,this.showSnap);
    this.webPage.mainFrame().load(url);
    this.loading = true;
    var image = new QPixmap;
    image.load(QTerm.localPath()+"pic/loading.png");
    this.label.pixmap = image;
//    this.label.setWindowFlags(Qt.FramelessWindowHint)
    this.label.move(QTerm.posX(),QTerm.posY());
    this.label.show();
}

QTerm.WebSnap = WebSnap;

QTerm.onWebsnap = function()
{
    QTerm.WebSnap.getWebsnap(QTerm.getUrl());
}

if (QTerm.addUrlMenu( "snapshot", "Web Snapshot" ) ) {
        QTerm.snapshot.triggered.connect(QTerm.onWebsnap);
}

