
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QList<int> list = ui->MenuSplitter->sizes();
	list.replace(0, this->height() / 0.2);
	list.replace(1, this->height() / 0.8);
	ui->MenuSplitter->setSizes(list);

	ui->openGLWidget2D->mw = this;
	ui->openGLWidget3D->mw = this;
	//ui->openGLWidget3D->sceneName = ui->openGLWidget2D->sceneName = "data/FairyForest/export.bvh";

	mCurrentScalarSet = 0;
	this->tRender = NULL;
	this->sRender = NULL;
	addScalarsButton = NULL;

	bvh = NULL;

	createActions();
	createMenus();

	//bvh = new QWidget(this);
	resViewT = new QPushButton("Reset view", ui->openGLWidget2D);
	connect(resViewT, SIGNAL(released()), this, SLOT(resetViewTree()));
	resViewT->hide();

	widget3D = new QWidget(ui->openGLWidget3D);
	QVBoxLayout *layout3D = new QVBoxLayout(widget3D);
	changeCam = new QPushButton("Next view", widget3D);
	connect(changeCam, SIGNAL(released()), this, SLOT(switchCam()));

	QHBoxLayout *nLay = new QHBoxLayout();
	QLabel *n = new QLabel("Near");
	nearPlane = new QDoubleSpinBox(widget3D);
	nearPlane->setRange(0.01f, 10.f);
	nearPlane->setValue(0.1f);
	nearPlane->setSingleStep(0.01f);
	nLay->addWidget(n);
	nLay->addWidget(nearPlane);
	connect(nearPlane, SIGNAL(valueChanged(double)), this, SLOT(updateNearPlane(double)));

	QHBoxLayout *fLay = new QHBoxLayout();
	QLabel *f = new QLabel("Far");
	farPlane = new QDoubleSpinBox(widget3D);
	farPlane->setRange(100.f, 100000.f);
	farPlane->setSingleStep(100.f);
	fLay->addWidget(f);
	fLay->addWidget(farPlane);
	connect(farPlane, SIGNAL(valueChanged(double)), this, SLOT(updateFarPlane(double)));

	layout3D->addWidget(changeCam);
	layout3D->addLayout(nLay);
	layout3D->addLayout(fLay);
	widget3D->setLayout(layout3D);

	widget3D->setStyleSheet("background-color:white");
	widget3D->setWindowOpacity(0.5);

	widget3D->hide();

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::AddRender(SceneRender *r)
{
	this->sRender = r;
	if (this->tRender)
		showControlPanel();
}

void MainWindow::AddRender(TreeRender *r)
{
	this->tRender = r;
	if (this->sRender)
		showControlPanel();
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
}

void MainWindow::createActions()
{
	openAct = new QAction(tr("&Open"), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open file"));
	connect(openAct, &QAction::triggered, this, &MainWindow::openScene);
}

void MainWindow::setSliders(int stepsNr, QSlider *a, QSlider *b, int scalarSetIndex)
{
	float scMin = tRender->bvh->mScalarSets[mCurrentScalarSet]->localMin;
	float scMax = tRender->bvh->mScalarSets[mCurrentScalarSet]->localMax;

	sliderStepAdd = scMin;
	sliderStepMult = (scMax - scMin) / stepsNr;

	a->setRange(0, stepsNr);
	b->setRange(0, stepsNr);
	a->setValue(0);
	b->setValue(stepsNr);
}

void MainWindow::setScalars(QWidget *parent)
{
	if (mCurrentScalarSet == -1)
		return;

	
	//scalarsGUI.container->setParent(parent); 
	//scalarsGUI.container->setStyleSheet("margin:5px; border:1px solid rgb(255, 255, 255); ");//");// #58ACFA");
	setSliders(100, scalarsGUI.first, scalarsGUI.second, mCurrentScalarSet);

	connect(scalarsGUI.first, SIGNAL(valueChanged(int)), this, SLOT(changeRange()));
	connect(scalarsGUI.second, SIGNAL(valueChanged(int)), this, SLOT(changeRange()));

	// set combobox with scalar sets
	disconnect(scalarsGUI.scalars);
	scalarsGUI.scalars->clear();
	for (int i = 0; i < tRender->bvh->mScalarSets.size(); i++)
	{
		scalarsGUI.scalars->addItem(QString::fromStdString(tRender->bvh->mScalarSets[i]->name));
	}
	// handle combobox selection
	connect(scalarsGUI.scalars, SIGNAL(currentIndexChanged(int)), this, SLOT(handleScalarButton(int)));
}

void MainWindow::setCurrNodeStats(QWidget *parent)
{
	currNodeStats.container = new QWidget(parent);
	currNodeStats.layout = new QVBoxLayout(currNodeStats.container);
	QLabel *currentNodeLabel = new QLabel("Node info", currNodeStats.container);
	currNodeStats.index = new QLabel("index: ", currNodeStats.container);
	currNodeStats.bounds = new QLabel("bounds: ", currNodeStats.container);
	currNodeStats.triangleNr = new QLabel("triangles: ", currNodeStats.container);
	currNodeStats.layout->addWidget(currentNodeLabel);
	currNodeStats.layout->addWidget(currNodeStats.index);
	currNodeStats.layout->addWidget(currNodeStats.bounds);
	currNodeStats.layout->addWidget(currNodeStats.triangleNr);
	currNodeStats.container->setStyleSheet("background-color:white");
	currNodeStats.container->hide();
}

void MainWindow::setSceneStats()
{
	unsigned triangleNr = sRender->sc->mTriangles.size();
	sceneStats.triangleCountLabel->setText("Triangle count: " + QString::number(triangleNr));
}

void MainWindow::showControlPanel()
{
	QVBoxLayout *controlPanel = ui->controlPanel;

	//bvh->setStyleSheet("background-color:white");
	setCurrNodeStats(bvh);
	controlPanel->addWidget(currNodeStats.container);

	// scalar value buttons
	
	if (!treeStats.container)
	{
		treeStats.container = new QWidget(this);
		treeStats.layout = new QVBoxLayout(treeStats.container);
		QLabel *heading = new QLabel("BVH stats:", treeStats.container);
		treeStats.importedBVHnodeCount = new QLabel(QString("Imported node count: ") + QString::number(tRender->bvh->mNodes.size()), treeStats.container);
		treeStats.realBVHnodeCount = new QLabel(QString("Real node count: ") + QString::number(tRender->bvh->mMeshCenterCoordinatesNr), treeStats.container);
		treeStats.layout->addWidget(heading);
		treeStats.layout->addWidget(treeStats.importedBVHnodeCount);
		treeStats.layout->addWidget(treeStats.realBVHnodeCount);
		treeStats.container->setLayout(treeStats.layout);
	}
	else
	{
		treeStats.importedBVHnodeCount->setText(QString("Imported node count: ") + QString::number(tRender->bvh->mNodes.size()));
		treeStats.realBVHnodeCount->setText(QString("Real node count: ") + QString::number(tRender->bvh->mMeshCenterCoordinatesNr));
	}

	controlPanel->addWidget(treeStats.container);

	setScalars(this);
	if (!addScalarsButton) {
		addScalarsButton = new QPushButton("Add values", scalarsGUI.container);
		connect(addScalarsButton, SIGNAL(released()), this, SLOT(addScalars()));
	}
	scalarsGUI.layout->addWidget(addScalarsButton);
	controlPanel->addWidget(scalarsGUI.container);

	controlPanel->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

	// Stats widget
	setSceneStats();
	controlPanel->addWidget(sceneStats.container);

	resViewT->show();
	widget3D->show();
	if (sRender->cams.size() > 1)
		changeCam->show();
	else
		changeCam->hide();

	//scalarsGUI.container->show();
}

void MainWindow::fillStats(QVBoxLayout * w)
{
	QLabel *triangleCountLabel = new QLabel(this);
	unsigned triangleNr = sRender->sc->mTriangles.size();
	triangleCountLabel->setText("Triangle count: " + QString::number(triangleNr));

	QLabel *ibvhncl = new QLabel(this);
	assert(tRender != NULL);
	ibvhncl->setText("Imported BVH nodes count: " + QString::number(tRender->bvh->mNodes.size()));

	QLabel *rbvhncl = new QLabel(this);
	rbvhncl->setText("Real BVH nodes count: " + QString::number(tRender->bvh->mMeshCenterCoordinatesNr));
	//QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

	w->addWidget(ibvhncl);
	w->addWidget(rbvhncl);
	w->addWidget(triangleCountLabel);
}

void MainWindow::PrintNodeInfo(int nodeIndex)
{
	if (nodeIndex == -1)
	{
		currNodeStats.container->hide();
		sRender->removeBBox();
		tRender->drawer->clearPath();
	}
	else
	{
		currNodeStats.index->setText("index: " + QString::number(nodeIndex));
		BVHNode *n = &tRender->bvh->mNodes[nodeIndex];
		currNodeStats.bounds->setText(
			"bounds:\n min: [" + QString::number(n->bounds[0][0]) + "; " +
			QString::number(n->bounds[0][1]) + "; " +
			QString::number(n->bounds[0][2]) + "]\n " +
			"max: [" + QString::number(n->bounds[1][0]) + "; " +
			QString::number(n->bounds[1][1]) + "; " +
			QString::number(n->bounds[1][2]) + "]");
		currNodeStats.triangleNr->setText(
			"triangles: " + QString::number(tRender->bvh->getTriangleCount(nodeIndex)));
		currNodeStats.container->show();

		tRender->drawer->changeScalarSet(mCurrentScalarSet);
		tRender->drawer->highlightNode(nodeIndex);
		sRender->addBBox(n);
	}
}

void MainWindow::DisplayBVHPath(int nodeIndex)
{
	PrintNodeInfo(nodeIndex);

	if (nodeIndex == -1)
		return;

	std::vector<unsigned> toDisplay;
	unsigned current = nodeIndex;

	while (current != -1) {
		toDisplay.push_back(current);
		current = tRender->bvh->mNodeParents[current];
	}

	tRender->drawer->changeScalarSet(mCurrentScalarSet);
	tRender->displayPath(toDisplay);
}

void MainWindow::contextMenuEvent(QContextMenuEvent * event)
{
	QMenu menu(this);
	menu.exec(event->globalPos());
}

void MainWindow::changeRange()
{
	float a = sliderStepMult * (float)scalarsGUI.first->value() + sliderStepAdd;
	float b = sliderStepMult * (float)scalarsGUI.second->value() + sliderStepAdd;

	tRender->bvh->normalizeScalarSet(mCurrentScalarSet, a <= b ? a : b, a <= b ? b : a);
	tRender->drawer->changeScalarSet(mCurrentScalarSet);
	tRender->drawer->showDisplayedNodes();
}

void MainWindow::resetViewTree()
{
	tRender->resetView();
}

void MainWindow::switchCam()
{
	sRender->switchCamera((sRender->currentCamera + 1) % sRender->cams.size());
}

QString extractCamFile(const QString &s)
{
	QStringList l = s.split('/');
	QString folderName = l[l.size() - 2];
	l.removeLast();
	QString path = l.join('/');

	QString rawName = folderName.toLower().split('.')[0];

	return path + "/" + rawName + ".cam";
}

void MainWindow::openScene()
{
	sceneFile = QFileDialog::getOpenFileName(this,
		tr("Open file"), "/home/", tr("BVH files (*.bvh)"));

	if (sceneFile.isEmpty())
		return;

	QString camFile = extractCamFile(sceneFile);
	resetControlPanel();

	ui->openGLWidget2D->initializeRender(sceneFile.toStdString());
	ui->openGLWidget3D->initializeRender(sceneFile.toStdString(), camFile.toStdString());
}

void MainWindow::addScalars()
{
	QString scalarFile = QFileDialog::getOpenFileName(this,
		tr("Open file"), "/home/", tr("Scalar value files (*.scal)"));

	if (scalarFile.isEmpty())
		return;

	tRender->sceneImporter->loadScalars(scalarFile.toStdString());
	setScalars(this);
}

void clearWidget(QBoxLayout *w)
{
	if (!w)
		return;

	while (QLayoutItem *child = w->takeAt(0))
	{
		delete child;
	}
}

void MainWindow::resetControlPanel()
{
	mCurrentScalarSet = 0;
	tRender = NULL;
	sRender = NULL;
	clearWidget(currNodeStats.layout);
	clearWidget(ui->controlPanel);
}

void MainWindow::updateNearPlane(const double & n)
{
	ui->openGLWidget3D->setNearPlane((float)n);
}

void MainWindow::updateFarPlane(const double & f)
{
	ui->openGLWidget3D->setFarPlane((float)f);
}

void MainWindow::keyPressEvent(QKeyEvent * e)
{
	pressedKeys.insert(e->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
	pressedKeys.remove(e->key());
}

void MainWindow::handleScalarButton(int index)
{
	if (index < 0)
		return;

	this->mCurrentScalarSet = index;
	this->tRender->drawer->changeScalarSet(index);

	setSliders(10000, scalarsGUI.first, scalarsGUI.second, index);
}
