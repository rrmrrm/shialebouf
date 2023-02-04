#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"
#include <QWidget>
#include <QTextEdit>
namespace Ui {
class TextBox;
}

class TextBox : public CanvasElementWidget
{
  Q_OBJECT
private:
  Ui::TextBox *ui;
  //MyTextEdit* textEdit;
	std::weak_ptr<TextComponent> tc;
  
public:
  explicit TextBox(std::shared_ptr<TextComponent> tc_, QWidget *parent);
  ~TextBox();
	
private slots:
  // update the model's text
	void textChanged();

public slots:
  // update text from model's
  void CE_geometryChanged() override;
  
	void keyPressEvent(QKeyEvent* e) override;
	virtual void derivedPaintEvent(QPaintEvent* e) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // TEXTBOX_H
