
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QList<int> sizes = ui->MenuSplitter->sizes();
	sizes.replace(0, this->height() / 0.2);
	sizes.replace(1, this->height() / 0.8);
	ui->MenuSplitter->setSizes(sizes);

	ui->openGLWidget2D->mw = this;
	ui->openGLWidget3D->mw = this;

	mCurrentScalarSet = 0;
	this->tRender = NULL;
	this->sRender = NULL;
	addScalarsButton = NULL;

	bvh = NULL;

	createActions();
	createMenus();

	currentTab = -1;
	tabWidget = new QTabWidget(this);
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
	ui->controlPanel->addWidget(tabWidget);

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

QString MainWindow::cameraDialog()
{
	QMessageBox *askCamFileMessage = new QMessageBox();
	askCamFileMessage->setText("No file with camera settings detected. Would you like to select the file yourself?");
	askCamFileMessage->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	askCamFileMessage->setDefaultButton(QMessageBox::Cancel);
	int ret = askCamFileMessage->exec();
	
	QString camFile;
	if (ret == QMessageBox::Ok)
	{
		camFile = QFileDialog::getOpenFileName(this,
			tr("Select camera file"), ".", tr("Cammera settings files (*.cam)"));
	}
	
	return camFile;
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openSceneAct);
	fileMenu->addAction(openBVHAct);
}

void MainWindow::createActions()
{
	openSceneAct = new QAction(tr("&Open"), this);
	openSceneAct->setShortcuts(QKeySequence::Open);
	openSceneAct->setStatusTip(tr("Open file"));
	connect(openSceneAct, &QAction::triggered, this, &MainWindow::openScene);

	openBVHAct = new QAction(tr("&Add BVH"), this);
	openBVHAct->setStatusTip(tr("Add another BVH to actual scene"));
	connect(openBVHAct, &QAction::triggered, this, &MainWindow::addBVH);
}

void MainWindow::setSliders(int stepsNr, QSlider *a, QSlider *b, QLabel *aLabel, QLabel *bLabel, int scalarSetIndex)
{
	float scMin = tRender->bvhs[currentTab]->mScalarSets[mCurrentScalarSet]->localMin;
	float scMax = tRender->bvhs[currentTab]->mScalarSets[mCurrentScalarSet]->localMax;

	sliderStepAdd = scMin;
	sliderStepMult = (scMax - scMin) / stepsNr;
	//scalarMax = pow(1.05, stepsNr);

	aLabel->setText(QString::number(scMin, 'f', 2));
	bLabel->setText(QString::number(scMax, 'f', 2));

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
	setSliders(100, controlPanelTabs[currentTab]->scalars->first, 
		controlPanelTabs[currentTab]->scalars->second, 
		controlPanelTabs[currentTab]->scalars->localMin, 
		controlPanelTabs[currentTab]->scalars->localMax, 
		mCurrentScalarSet);

	connect(controlPanelTabs[currentTab]->scalars->first, 
		SIGNAL(valueChanged(int)), 
		this, 
		SLOT(changeRange()));

	connect(controlPanelTabs[currentTab]->scalars->second, 
		SIGNAL(valueChanged(int)), 
		this, 
		SLOT(changeRange()));

	// set combobox with scalar sets
	disconnect(controlPanelTabs[currentTab]->scalars->scalars);
	controlPanelTabs[currentTab]->scalars->scalars->clear();
	for (int i = 0; i < tRender->bvhs[currentTab]->mScalarSets.size(); i++)
	{
		controlPanelTabs[currentTab]->scalars->scalars->addItem(
			QString::fromStdString(tRender->bvhs[currentTab]->mScalarSets[i]->name));
	}
	// handle combobox selection
	connect(controlPanelTabs[currentTab]->scalars->scalars, 
		SIGNAL(currentIndexChanged(int)), 
		this, 
		SLOT(handleScalarButton(int)));
}

void MainWindow::setTreeDepthRange(QWidget *parent)
{
	controlPanelTabs[currentTab]->treeDepth->depthHolder->setRange(
		1, tRender->bvhs[currentTab]->depth);
	controlPanelTabs[currentTab]->treeDepth->depthHolder->setValue(
		tRender->bvhs[currentTab]->depth);
}

/*
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
*/

void MainWindow::setSceneStats()
{
	unsigned triangleNr = sRender->sc->mTriangles.size();
	controlPanelTabs[currentTab]->sceneStats->triangleCountLabel->setText("Triangle count: " + QString::number(triangleNr));
}

void MainWindow::connectControlPanelSignals(int index)
{
	connect(controlPanelTabs[index]->scalars->addScalarsButton, 
		SIGNAL(released()), 
		this, 
		SLOT(addScalars())
		);

	connect(controlPanelTabs[index]->treeDepth->depthHolder,
		SIGNAL(valueChanged(int)),
		this,
		SLOT(changeTreeDepth(int))
		);
}

void MainWindow::showControlPanel()
{
	QVBoxLayout *controlPanel = ui->controlPanel;

	ControlPanel *c = new ControlPanel();
	currentTab = controlPanelTabs.size();
	controlPanelTabs.push_back(c);

	tabWidget->addTab(c->container, "tab" + QString::number(currentTab + 1));
	tabWidget->setCurrentIndex(currentTab);

	// scalar value buttons
	c->treeStats->importedBVHnodeCount->setText(QString("Imported node count: ") + QString::number(tRender->bvhs[currentTab]->mNodes.size()));
	c->treeStats->realBVHnodeCount->setText(QString("Real node count: ") + QString::number(tRender->bvhs[currentTab]->mMeshCenterCoordinatesNr));

	setScalars(this);
	setTreeDepthRange(this);
	setSceneStats();

	resViewT->show();
	widget3D->show();
	if (sRender->cams.size() > 1)
		changeCam->show();
	else
		changeCam->hide();

	connectControlPanelSignals(currentTab);
}

void MainWindow::fillStats(QVBoxLayout * w)
{
	QLabel *triangleCountLabel = new QLabel(this);
	unsigned triangleNr = sRender->sc->mTriangles.size();
	triangleCountLabel->setText("Triangle count: " + QString::number(triangleNr));

	QLabel *ibvhncl = new QLabel(this);
	assert(tRender != NULL);
	ibvhncl->setText("Imported BVH nodes count: " + QString::number(tRender->bvhs[currentTab]->mNodes.size()));

	QLabel *rbvhncl = new QLabel(this);
	rbvhncl->setText("Real BVH nodes count: " + QString::number(tRender->bvhs[currentTab]->mMeshCenterCoordinatesNr));
	//QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

	w->addWidget(ibvhncl);
	w->addWidget(rbvhncl);
	w->addWidget(triangleCountLabel);
}

void MainWindow::PrintNodeInfo(int nodeIndex)
{
	if (nodeIndex == -1)
	{
		controlPanelTabs[currentTab]->currNodeStats->container->hide();
		sRender->removeBBox();
		tRender->drawers[currentTab]->clearPath();
	}
	else
	{
		controlPanelTabs[currentTab]->currNodeStats->index->setText("index: " + QString::number(nodeIndex));
		BVHNode *n = &tRender->bvhs[currentTab]->mNodes[nodeIndex];
		controlPanelTabs[currentTab]->currNodeStats->bounds->setText(
			"bounds:\n min: [" + QString::number(n->bounds[0][0]) + "; " +
			QString::number(n->bounds[0][1]) + "; " +
			QString::number(n->bounds[0][2]) + "]\n " +
			"max: [" + QString::number(n->bounds[1][0]) + "; " +
			QString::number(n->bounds[1][1]) + "; " +
			QString::number(n->bounds[1][2]) + "]");
		controlPanelTabs[currentTab]->currNodeStats->triangleNr->setText(
			"triangles: " + QString::number(tRender->bvhs[currentTab]->getTriangleCount(nodeIndex)));
		controlPanelTabs[currentTab]->currNodeStats->container->show();

		tRender->drawers[currentTab]->changeScalarSet(mCurrentScalarSet);
		tRender->drawers[currentTab]->highlightNode(nodeIndex);
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
		current = tRender->bvhs[currentTab]->mNodeParents[current];
	}

	tRender->drawers[currentTab]->changeScalarSet(mCurrentScalarSet);
	tRender->displayPath(toDisplay);
}

void MainWindow::contextMenuEvent(QContextMenuEvent * event)
{
	QMenu menu(this);
	menu.exec(event->globalPos());
}

float MainWindow::recalculateValue(const float &val)
{
	return sliderStepMult * val + sliderStepAdd;
}

void MainWindow::changeRange()
{
	//float a = pow(1.05, scalarsGUI.first->value()) / scalarMax * tRender->bvh->mScalarSets[mCurrentScalarSet]->localMax;
	//float b = pow(1.05, scalarsGUI.second->value()) / scalarMax * tRender->bvh->mScalarSets[mCurrentScalarSet]->localMax;
	//qDebug() << scalarMax << pow(1.05, scalarsGUI.second->value()) / scalarMax << pow(1.05, scalarsGUI.second->value()) / scalarMax * tRender->bvh->mScalarSets[mCurrentScalarSet]->localMax;
	//qDebug() << 1 * pow(1.05, scalarsGUI.second->value()) / scalarMax << pow(1.05, scalarsGUI.second->value()) / scalarMax * tRender->bvh->mScalarSets[mCurrentScalarSet]->localMax;
	float a = recalculateValue((float)controlPanelTabs[currentTab]->scalars->first->value());
	float b = recalculateValue((float)controlPanelTabs[currentTab]->scalars->second->value());

	controlPanelTabs[currentTab]->scalars->localMin->setText(QString::number(min(a, b), 'f', 2));
	controlPanelTabs[currentTab]->scalars->localMax->setText(QString::number(max(a, b), 'f', 2));

	tRender->bvhs[currentTab]->normalizeScalarSet(mCurrentScalarSet, min(a, b), max(a, b));
	tRender->drawers[currentTab]->changeScalarSet(mCurrentScalarSet);
	tRender->drawers[currentTab]->showDisplayedNodes();
}

void MainWindow::changeTab(int current)
{
	currentTab = current;
	if (current == -1)
		return;

	if (tRender && sRender)
	{
		tRender->currentBVHIndex = current;
		sRender->currentBVHIndex = current;
	}
}

void MainWindow::changeTreeDepth(int newDepth)
{
	ui->openGLWidget2D->makeCurrent();
	tRender->changeTreeDepth(newDepth, mCurrentScalarSet);
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
	if (!ifstream(camFile.toStdString()))
		camFile = cameraDialog();

	resetControlPanel();
	currentTab = -1;

	ui->openGLWidget2D->initializeRender(sceneFile.toStdString());
	ui->openGLWidget3D->initializeRender(sceneFile.toStdString(), camFile.toStdString());
}

void MainWindow::addBVH()
{
	sceneFile = QFileDialog::getOpenFileName(this,
		tr("Open file"), "/home/", tr("BVH files (*.bvh)"));

	if (sceneFile.isEmpty())
		return;

	BVH *bvh = ui->openGLWidget2D->addBVH(sceneFile.toStdString());
	ui->openGLWidget3D->addBVH(bvh);
	showControlPanel();
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
	if (currentTab >= 0) {
		clearWidget(controlPanelTabs[currentTab]->currNodeStats->layout);
		clearWidget(ui->controlPanel);
	}
	currentTab = -1;
	controlPanelTabs.clear();
	tabWidget->clear();
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
	this->tRender->drawers[currentTab]->changeScalarSet(index);

	setSliders(100, 
		controlPanelTabs[currentTab]->scalars->first, 
		controlPanelTabs[currentTab]->scalars->second, 
		controlPanelTabs[currentTab]->scalars->localMin,
		controlPanelTabs[currentTab]->scalars->localMax,
		index);
}
