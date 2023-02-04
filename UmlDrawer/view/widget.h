#ifndef WIDGET_H
#define WIDGET_H

#include "view/canvas.h"
#include "model/model.h"
#include "userinputdialog.h"

#include <QWidget>
#include <QTimer>
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
	#include <QRandomGenerator>
#else
	#include <qrandomgenerator.h>
#endif
#include <QFileDialog>
#include <QValidator>
#include <QInputDialog>
#include <QKeyEvent>
#include <QPixmap>

#include "savelastdrawingwidget.h"
#include "ui_widget.h"

#include <QLabel>


#include <memory>
#include <functional>
namespace Ui {
class Widget;
}

class Controller;
class QToolBar;

class Widget : 
		public QWidget
		//public virtual IHasAggregate
{
	Q_OBJECT
	
	Model* m;
	//size_t wid;//size of Canvas widget
	
	Canvas* canvas;
	int dWidth = 110;
  // drawingsPixmap erre a labelre fog kirajzolodni
	QLabel* imageLabel;
	QPixmap* drawingsPixmap;
	std::vector<DotMatrix> drawingDotMatrices;
	DotMatrix* lastDrawing;
	QToolBar* toolBar;
	
	SaveLastDrawingWidget* saveLDW;
	QString lastLoadFile {};
public:
	explicit Widget(QWidget *parent = nullptr);
	~Widget();
private:
	void addActions();
public slots:
	void toggleDevTools();
	void askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)> callback);

	///
	/// \brief addDotMatrixMiniature ha pos_canBeOverIndexed túlindexelné a 'drawings' vectort,
	///  akkor 'drawing' hozzáadasa előtt kibővíti a vector-t:
	/// \param drawing
	/// \param pos_canBeOverIndexed
	///
	void addDotMatrixMiniature(DotMatrix drawing, int pos_canBeOverIndexed);
	void removeDotMatrixMiniature(int pos_canBeOverIndexed);
	void updateDrawingDotMatrixMiniatures();
	void keyPressEvent(QKeyEvent* e) override;
	//void prepareForClosing();
  ///megjeleníti a képernyő tetején a 'msg'-ben megadott szöveget, 
	/// majd pár másodperc múlva eltünteti a megjelenített szöveget
	void showMessage(QString msg);
  void removeDrawingDotMatrixMiniatures();
private slots:
  //eltünteti az ui->messageLabel-ben levö üzenetet:
	void resetLabelState();
	void copySelected();
	void pasteSelected();
  void deleteSelected();
	///
	///\brief produceDatasFromDir
	///
	///egy 2 mappaválasztó dialog ugrik fel, amiben egy forrás és egy célkönyvtárat kell kiválasztani.
	///a forráskönyvtárban levő összes filet Drawingként értelmezve
	///ezek Gesture-adatai alapján több Drawing file-t készít a célkönyvtárban úgy, hogy a Gesture-ök pontjait kicsit eltolja véletlenszerűen
	///FONTOS:a forráskönyvtárban csak Drawing fileoknak szabad lennie
	void produceDatasFromDir();
	void trainNN();
	void saveCEs();
	//exportálás képként
	void saveAsImage();
	void loadCEs();
	//void saveDrawings();
	//void exportDrawingImages();
private:
	Ui::Widget *ui;
};

#endif // WIDGET_H
