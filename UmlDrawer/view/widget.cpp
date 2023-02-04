#include "view/widget.h"
#include "network/network.h"
#include "savelastdrawingwidget.h"
#include "userinputdialog.h"
#include "common.hpp"
#include "model/CanvasElements/canvaselement.h"
#include "model/CanvasElements/canvaselementbase.h"
#include "ui_widget.h"


#include <QObject>
#include <QString>
#include <QSpinBox>
#include <QDirIterator>
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
	#include <QRandomGenerator>
#else
	#include <qrandomgenerator.h>
#endif
#include <QMouseEvent>
#include <QFileDialog>
#include <QScrollArea>
#include <QPixmap>
#include <QMessageBox>
#include <QInputDialog>
#include <QTime>
#include <QAction>
#include <QAction>
#include <QToolBar>
#include <QScreen>

#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>
#include <memory>

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	m(new Model),
   // wid(800),
	
	canvas(
		new Canvas(
			m,
			this
		)
	),
	imageLabel(new QLabel),
	drawingsPixmap(new QPixmap(QSize(dWidth,20000))),
	ui(new Ui::Widget)
{
	ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose); // tells qt to call destructors on close
	ui->painterLayout->addWidget(canvas);
  //get available space on screen
  setGeometry(this->screen()->availableGeometry());

	
	drawingsPixmap->fill(Qt::black);

	imageLabel->setPixmap(*drawingsPixmap);
	ui->imagesArea->setBackgroundRole(QPalette::Dark);
	ui->imagesArea->setWidget(imageLabel);
	
	
	saveLDW = new SaveLastDrawingWidget(m->getSaveLastDrawingModel(), this);
	ui->saveLastDrawingLayout->addWidget(saveLDW);		
	// set whether devToolsContainer is at the start
	ui->devToolsContainer->setVisible(false);
	connect(ui->devTools, SIGNAL(clicked()), this, SLOT(toggleDevTools()));
	connect(saveLDW, SIGNAL(showMessage_W(QString)), this, SLOT(showMessage(QString)));
	connect(ui->populateDataFromDir, SIGNAL(clicked()), this, SLOT(produceDatasFromDir()));
	connect(ui->trainButton, SIGNAL(clicked()), m, SLOT(trainNN()));
	
	connect(m, SIGNAL(updateCanvas(QRect)), canvas, SLOT(Update(QRect)));
	connect(m, SIGNAL(updateCanvas()), canvas, SLOT(Update()));
	connect(m, SIGNAL(destroyView_W(IdType)), canvas, SLOT(M_destroyCEW(IdType)));
	connect(
		m,
		SIGNAL(askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)>)),
		this,
		SLOT(askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)>))
	);
	connect(m, SIGNAL(showMessage(QString)),this, SLOT(showMessage(QString)));
	connect(
		m, 
		SIGNAL(addDotMatrixMiniature(DotMatrix, int)), 
		this, 
		SLOT(addDotMatrixMiniature(DotMatrix, int))
	);
	connect(
		m, 
		SIGNAL(removeDotMatrixMiniature(int)), 
		this, 
		SLOT(removeDotMatrixMiniature(int))
	);
	connect(
		m, 
		SIGNAL(removeDrawingDotMatrixMiniatures()), 
		this, 
		SLOT(removeDrawingDotMatrixMiniatures())
	);
	connect(
		m, 
		SIGNAL(canvasElementAdded(
				   std::weak_ptr<CanvasElement>
			   )
		), 
		canvas, 
		SLOT(Model_CanvasElementAdded(
				std::weak_ptr<CanvasElement>
			)
		)
	);
	connect(
		m,
		SIGNAL(removeCanvasElements()),
		canvas,
		SLOT(removeCanvasElementWidgets())
	);
	setEnabled(true);
	
	toolBar = new QToolBar(tr("&tools"), this);
	addActions();
	ui->toolBarLayout->addWidget(toolBar);
	std::weak_ptr<CanvasElement> cl1w;
	std::weak_ptr<CanvasElement> cl2w;
	{
	auto cl1 = CanvasElement::createCanvasElement(
		m, 
		CanvasElement_Type::UMLCLASST,
		QRect(0,500,100,500)
	);
	auto cl2 = CanvasElement::createCanvasElement(
		m, 
		CanvasElement_Type::UMLCLASST,
		QRect(500,500,500,800)
	);
	cl1w = cl1;
	cl2w = cl2;
	m->addCanvasElement(std::move(cl1));
	
	
	
	m->addCanvasElement(std::move(cl2));
	/// cl1 és cl2 itt felszabadul, de már move-oltuk azokat:
	}
	
	m->addCanvasElement(
		CanvasElement::createCanvasElement(
			m, 
			CanvasElement_Type::TEXTBOXT,
			QRect(100,0,100,100)
	));
}
Widget::~Widget()
{
	//delete canvas;
  delete m; m = nullptr;
	//delete ui;
}

void Widget::addActions(){
	QAction *copyAct = new QAction(tr("&copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy selected element"));
	connect(copyAct, &QAction::triggered, this, &Widget::copySelected);
	toolBar->addAction(copyAct);
	
	QAction *pasteAct = new QAction(tr("&paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("&Paste copied element"));
	connect(pasteAct, &QAction::triggered, this, &Widget::pasteSelected);
	toolBar->addAction(pasteAct);
	
	
	QAction *delAct = new QAction(tr("&delete"), this);
	delAct->setShortcuts(QKeySequence::Delete);
	delAct->setStatusTip(tr("&delete selected element"));
	connect(delAct, &QAction::triggered, this, &Widget::deleteSelected);
	toolBar->addAction(delAct);
	
	
	QAction *saveAct = new QAction(tr("&save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("&save diagram"));
	connect(saveAct, &QAction::triggered, this, &Widget::saveCEs);
	toolBar->addAction(saveAct);
	
	QAction *saveAsImageAct = new QAction(tr("&save as image"), this);
	//saveAct->setShortcuts(QKeySequence::Save);
	saveAsImageAct->setStatusTip(tr("&save diagram as image"));
	connect(saveAsImageAct, &QAction::triggered, this, &Widget::saveAsImage);
	toolBar->addAction(saveAsImageAct);
	
	QAction *loadAct = new QAction(tr("&load"), this);
	//loadAct->setShortcuts(QKeySequence());
	loadAct->setStatusTip(tr("&load diagram"));
	connect(loadAct, &QAction::triggered, this, &Widget::loadCEs);
	toolBar->addAction(loadAct);
}

void Widget::copySelected(){
	m->Wid_copySelected();
}
void Widget::pasteSelected(){
	m->Wid_pasteSelected();
}
void Widget::deleteSelected(){
	m->Wid_deleteSelected();
}

void Widget::toggleDevTools(){
	if(ui->devToolsContainer->isVisible()){
		// hide it
		ui->devToolsContainer->hide();
	}
	else{
		// show it
		ui->devToolsContainer->show();
	}
}
void Widget::askForUserInput(std::shared_ptr<UserInputModel> uim, std::function<void (std::shared_ptr<UserInputModel>)> callback){
	
	QWidget* parent = this;
	
	UserInputDialog dialog(uim, callback, parent);
	dialog.exec();
}
void Widget::addDotMatrixMiniature(DotMatrix drawing, int pos_canBeOverIndexed){
	assert(pos_canBeOverIndexed >= 0);
	if(pos_canBeOverIndexed >= (int)drawingDotMatrices.size()){
		drawingDotMatrices.resize(pos_canBeOverIndexed + 1);
	}
	drawingDotMatrices[pos_canBeOverIndexed] = drawing;
	updateDrawingDotMatrixMiniatures();
}
void Widget::removeDotMatrixMiniature(int pos_canBeOverIndexed){
	assert(pos_canBeOverIndexed >= 0);
	if(pos_canBeOverIndexed >= (int)drawingDotMatrices.size()){
		return;
	}
	drawingDotMatrices.erase(drawingDotMatrices.begin()+pos_canBeOverIndexed);
	updateDrawingDotMatrixMiniatures();
}
void Widget::updateDrawingDotMatrixMiniatures(){
	///képek megjelenítése kicsinyítve :
	drawingsPixmap->fill(Qt::black);
	///draw on pixmap
	QPainter drawingMiniaturesPainter(drawingsPixmap);
	drawingMiniaturesPainter.setPen(QColor(255,20,10));
	double translation = 0;
	for(uint cnt = 0 ; cnt < drawingDotMatrices.size() ; ++cnt){
		double scale = drawingDotMatrices[cnt].size() > drawingDotMatrices[cnt][0].size() ?
					dWidth / (double)drawingDotMatrices[cnt].size() :
					dWidth / (double)drawingDotMatrices[cnt].at(0).size();
		if(translation > drawingsPixmap->height()){
			std::cerr << "WARNING: Widget::Widget(..): miniature drawings exceed the maximal size! some won't appear" << std::endl;
			break;
		}
		drawingMiniaturesPainter.drawText(QPointF(0,translation),QString(std::to_string(cnt+1).c_str()));
		for (uint i = 0; i < drawingDotMatrices[cnt].size(); ++i) {//horizontal direction
			for (uint j = 0; j < drawingDotMatrices[cnt][0].size(); ++j) {//vertical direction
				int x0 = (int)(i)*scale;
				int y0 = translation + (int)(j)*scale;
				int w = scale;
				int h = scale;
				double val = drawingDotMatrices[cnt][i][j];
				QColor col(val*255, val*255, val*255);
				QBrush brush(col);
				drawingMiniaturesPainter.fillRect(x0,y0, w,h, brush);
			}
		}
		translation += drawingDotMatrices[cnt][0].size()*scale;
	}
	imageLabel->setPixmap(*drawingsPixmap);
	ui->imagesArea->setBackgroundRole(QPalette::Dark);
	ui->imagesArea->setWidget(imageLabel);
}
void Widget::resetLabelState(){
	auto l = ui->messageLabel;
	l->setStyleSheet("color: red; background-color: transparent");
	l->setEnabled(false);
	l->setText("");
}
void Widget::keyPressEvent(QKeyEvent* e){
	if(e->key() == Qt::Key::Key_Return){
		m->widget_enterPressed();
	}
	else{
		return QWidget::keyPressEvent(e);
	}
}
void Widget::showMessage(QString msg){
	auto l = ui->messageLabel;
	l->setEnabled(true);
	l->setStyleSheet("color: red; background-color: black");
	l->setText(msg);
	//egy idő után eltüntetjük az üzenetet a resetLabelState() eljárással: 
	QTimer t;
	int showMessageInterval = 10000;
	t.singleShot(
		showMessageInterval, 
		[this]{
			resetLabelState();
		}
	);
}

void Widget::produceDatasFromDir(){
	QString sourceDir = QFileDialog::getExistingDirectory(this, tr("Choose Source directory"), "");
	QString destDir = QFileDialog::getExistingDirectory(this, tr("Choose Destination directory"), "");
	
	m->produceDatasFromDir(sourceDir, destDir);
}
void Widget::trainNN(){
	m->trainNN();
}
void Widget::saveCEs(){
	QString	file = QFileDialog::getSaveFileName(this, "diagram mentése");
	
	if(file != "" || !file.isEmpty()){
		m->Widget_saveCEs(file);
	}
	else{
		showMessage("Sikertelen a mentés, mert üres a filenév!");
	}
}
void Widget::saveAsImage(){
	QString file = QFileDialog::getSaveFileName(this, "diagram mentése képként");
	//}
	
	if(file != "" && !file.isEmpty() && canvas->saveAsImage(file)){
		showMessage("sikeres mentés");
	}
	else{
		showMessage("Sikertelen a mentés, mert üres a filenév!");
	}
}
void Widget::loadCEs(){
	QString initialDir;
	// If we already loaded once since the program started, we will use the directory we last loaded from.
	if(!lastLoadFile.isEmpty()){
		QFileInfo fi(lastLoadFile);
		initialDir = fi.absolutePath();
	}
	QString file = QFileDialog::getOpenFileName(this, "diagram betöltése", initialDir);
	if(file != "" || !file.isEmpty()){
		m->Widget_loadCEs(file);
	}
	else{
		showMessage("Sikertelen a betöltés, mert üres a filenév!");
	}
}
void Widget::removeDrawingDotMatrixMiniatures(){
	drawingDotMatrices.resize(0);
}
