QTerm.webPage= new QWebPage;
//QTerm.label = new QLabel(QTerm.window());
QTerm.label = new QLabel;

QTerm.onWebsnap = function()
{
//    QTerm.showMessage("generating websnap", 1, 0);
    var url = new QUrl(QTerm.getUrl(), QUrl.TolerantMode);
    QTerm.webPage.loadFinished.connect(QTerm.showSnap);
    QTerm.webPage.mainFrame().load(url);
    QTerm.webLoading = true;
    var image = new QPixmap;
    image.load(QTerm.localPath()+"pic/loading.png");
    QTerm.label.pixmap = image;
//    QTerm.label.setWindowFlags(Qt.FramelessWindowHint)
    QTerm.label.move(QTerm.posX(),QTerm.posY());
    QTerm.label.show();
}

QTerm.showSnap = function(ok)
{
    if (!ok) {
        QTerm.showMessage("cannot download the webpage");
        return;
    }
    QTerm.webLoading = false;
    var target = new QSize(400, 300);
    var size = QTerm.webPage.mainFrame().contentsSize;
    size.setHeight(size.width() * target.height() / target.width());

    // create the target surface
    var image = new QPixmap(size);
    image.fill(Qt.transparent);

    // render and rescale
    var p = new QPainter;
    p.begin(image);
    QTerm.webPage.viewportSize = QTerm.webPage.mainFrame().contentsSize;
    QTerm.webPage.mainFrame().render(p);
    p.end();
    image = image.scaled(target, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation);
//    QTerm.showMessage("generating finished", 1, 0);
    QTerm.label.size = target;
    QTerm.label.pixmap = image;
}

if (QTerm.addUrlMenu( "websnap", "Websnap" ) ) {
        QTerm.websnap.triggered.connect(QTerm.onWebsnap);
}

