#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H
#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"
#include <QWidget>
#include <QTextEdit>
class MyTextEdit: public QTextEdit{
	Q_OBJECT
	int charSize;
  int letterSpacing;
public:
	MyTextEdit(QWidget* parent = nullptr);
	//const int minHeight = 20;
public:
  void setCharSize(int);
  void setLetterSpacing(int);
private slots:
	void onTextChanged();
public slots:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};
#endif // MYTEXTEDIT_H
