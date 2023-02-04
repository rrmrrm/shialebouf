#include "textbox.h"
#include "ui_textbox.h"

#include "model/plug.h"
#include "connwidget.h"

#include <QKeyEvent>
#include <QPainterPath>
#include <QVBoxLayout>


TextBox::TextBox(std::shared_ptr<TextComponent> tc_, QWidget* parent) :
		CanvasElementWidget(tc_, parent),
    ui(new Ui::TextBox),
		tc(tc_)
{
  ui->setupUi(this);
	assert(ce.lock());
	tc = std::static_pointer_cast<TextComponent>(ce.lock());
	auto tcObs = tc.lock();
	assert(tcObs);
	//QVBoxLayout* layout  = new QVBoxLayout(this);
  //layout->setContentsMargins(3,3,3,3);
	//ui->textEdit->setCharSize(tcObs->charSize);
  //ui->textEdit->setLetterSpacing(tcObs->letterSpacing);
  //
	//ui->textEdit->setTabChangesFocus(true);
	//
	setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
  
  setAttribute(Qt::WA_TranslucentBackground);
	setMouseTracking(true);
	
	setGeometryInC(tcObs->geometry);
	
	connect(ui->textEdit, &QTextEdit::textChanged, 
			this, &TextBox::textChanged);
	/// text beállítása a CE textje alapján:
	//TextBox::textChanged();
}
TextBox::~TextBox(){
  delete ui;
}
void TextBox::CE_geometryChanged(){
  CanvasElementWidget::CE_geometryChanged();
  qDebug("TextBox::CE_geometryChanged()");
  if(auto tcObs = tc.lock()){
    qDebug() << "text: " << tcObs->text;
    ui->textEdit->setPlainText(tcObs->text);
  }
}
void TextBox::keyPressEvent(QKeyEvent* e){
	if(e->key() == Qt::Key::Key_Escape){
		ui->textEdit->clearFocus();
	}
}
QSize grownByy(QSize s, QMargins m){
	return {s.width() + m.left() + m.right(), s.height() + m.top() + m.bottom()}; 
}

void TextBox::textChanged(){
	
	if(auto obs = tc.lock()){
		QString newt = ui->textEdit->toPlainText();
		qDebug() << "TextBox::textChanged(): old text:" << obs.get()->text << "\n";
		
		obs.get()->CEW_textChanged(newt);
	}
	/// ha egy connectorWidget-hez csatlakozik, akkor a szülő CEW-nek jelezzük, hogy méretezze át, hogy az új stzöveg beleférjen:
	QMargins extraMargin(10,10,10,10);
	if(auto pConn = dynamic_cast<ConnWidget*>(parent())){
		if(auto ppCEW = dynamic_cast<CanvasElementWidget*>(pConn->parent())){
			/// setWordWrapMode(QTextOption::NoWrap);
			ppCEW->ConnW_wantsToResize( 
						pConn,
						grownByy(ui->textEdit->document()->size().toSize(), extraMargin) 
					);
		}
	}
	qDebug() << "DEBUG: TextBox::textChanged() called\n";
}
void TextBox::derivedPaintEvent(QPaintEvent* e){
	QWidget::paintEvent(e);
	if(auto tcObs  = tc.lock()){
		//if(auto connectedC = tcObs->pl->getConnectedConn().lock()){
		//	
		//}
		//else{
		QPainter p(this);
    QRect whole = QRect(QPoint(0,0), geometry().size());
    QColor translucentGrey(100, 100, 100, 20);
    p.fillRect(whole,QBrush(translucentGrey));
		// QPainterPath path;
		// path.addEllipse(QRect(0, 0, width(), height()));
		// p.fillPath(path, QBrush(QColor(100,0,0,120), Qt::SolidPattern));
		//}
		
	}
}
void TextBox::mousePressEvent(QMouseEvent *event){
	qDebug() << "DEBUG: TextBox::mousePressEvent(..) called\n";
	raise();//a képernyőn a többi widget elé helyezzük 
	CanvasElementWidget::mousePressEvent(event);
}
void TextBox::mouseMoveEvent(QMouseEvent *event){
	CanvasElementWidget::mouseMoveEvent(event);
}
void TextBox::mouseReleaseEvent(QMouseEvent *event){
	qDebug() << "DEBUG: TextBox::mouseReleaseEvent(..) called\n";
	CanvasElementWidget::mouseReleaseEvent(event);
}
