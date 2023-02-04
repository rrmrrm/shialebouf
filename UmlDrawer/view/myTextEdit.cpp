#include "myTextEdit.h"
#include "model/plug.h"
#include "connwidget.h"

#include <QKeyEvent>
#include <QPainterPath>
#include <QVBoxLayout>
MyTextEdit::MyTextEdit(QWidget* parent): 
	QTextEdit(parent)
{
	setWordWrapMode(QTextOption::NoWrap);
	//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumHeight(fontPointSize());
	QFont font = QFont(
		fontFamily(),
		10, 
		-1, 
		false
	);
  setTextInteractionFlags ( Qt::TextSelectableByKeyboard |
                             Qt::LinksAccessibleByMouse   | 
                             Qt::LinksAccessibleByKeyboard |
                             Qt::TextEditable);
  QVBoxLayout* layout  = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  setLayout(layout);
  
	setCurrentFont(font);
	setFont(font);
	setAttribute(Qt::WA_TranslucentBackground);
	connect( this, SIGNAL( textChanged() ), this, SLOT( onTextChanged() ) );
  setMouseTracking(true);
}
void MyTextEdit::setCharSize(int charSize_){
  charSize = charSize_;
}
void MyTextEdit::setLetterSpacing(int letterSpacing_){
  letterSpacing = letterSpacing_;
  QFont newFont = font();
  newFont.setLetterSpacing(QFont::PercentageSpacing, letterSpacing);
  setCurrentFont(newFont);
	setFont(newFont);
}
void MyTextEdit::onTextChanged(){
  //todo set exapnading policy, set vertical streatch to document pixelHeight
  //    and test with linecomponent  and umlClass
	QSize size = document()-> size().toSize();
	qDebug() <<fontPointSize();
	int textMargin = 10;
	if(size.height() < charSize + textMargin)
		setMaximumHeight(charSize + textMargin);
	else
		setMaximumHeight( size.height() + 3 );
}
void MyTextEdit::mousePressEvent(QMouseEvent *event){
	QTextEdit::mousePressEvent(event);
	if(parent()){
		if(auto pcew = dynamic_cast<CanvasElementWidget*>(parent())){
			pcew->mousePressEvent(event);
		}
	}
}
void MyTextEdit::mouseMoveEvent(QMouseEvent *event){
	QTextEdit::mouseMoveEvent(event);
	if(parent()){
		if(auto pcew = dynamic_cast<CanvasElementWidget*>(parent())){
			pcew->mouseMoveEvent(event);
		}
	}
}
void MyTextEdit::mouseReleaseEvent(QMouseEvent *event){
	QTextEdit::mouseReleaseEvent(event);
	if(parent()){
		if(auto pcew = dynamic_cast<CanvasElementWidget*>(parent())){
			pcew->mouseReleaseEvent(event);
		}
	}
}
