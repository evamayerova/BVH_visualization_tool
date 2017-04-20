#include "ControlPanel.h"

ScalarValuesGUI::ScalarValuesGUI(QWidget *parent)
{
	container = new QGroupBox("Selected property", parent);
	scalars = new QComboBox(container);

	localMin = new QLabel(container);
	localMax = new QLabel(container);
	first = new QSlider(Qt::Horizontal, container);
	second = new QSlider(Qt::Horizontal, container);
	polynomDegreeLabel = new QLabel();
	polynomDegreeLabel->setText("Transfer function exponent");
	polynomDegree = new QDoubleSpinBox(container);
	polynomDegree->setValue(1.0);
	polynomDegree->setMinimum(0.0);
	polynomDegree->setMaximum(19.0);
	polynomDegree->setSingleStep(0.5);

	QWidget *slidersHolder = new QWidget(container);

	QHBoxLayout *slidersWithLabels = new QHBoxLayout(slidersHolder);
	QVBoxLayout *sliders = new QVBoxLayout(slidersHolder);
	sliders->addWidget(first);
	sliders->addWidget(second);

	slidersWithLabels->addWidget(localMin);
	slidersWithLabels->addLayout(sliders);
	slidersWithLabels->addWidget(localMax);

	addScalarsButton = new QPushButton("Add values", container);

	slidersHolder->setLayout(slidersWithLabels);

	layout = new QVBoxLayout(container);
	layout->addWidget(scalars);
	layout->addWidget(slidersHolder);

	QWidget *polydegreeholder = new QWidget(container);
	QHBoxLayout *polynomdegreewithlabel = new QHBoxLayout(polydegreeholder);
	polynomdegreewithlabel->addWidget(polynomDegreeLabel);
	polynomdegreewithlabel->addWidget(polynomDegree);

	layout->addWidget(polydegreeholder);
	layout->addWidget(addScalarsButton);

	container->setLayout(layout);
}

ControlPanel::ControlPanel()
{
	scrollArea = new QScrollArea();
	container = new QWidget();
	treeStats = new CurrentTreeStats(container);
	currNodeStats = new CurrentNodeStats(container);
	scalars = new ScalarValuesGUI(container);
	treeDepth = new TreeDepth(container);
	treeDepth->container->hide();
	blendingType = new BlendingType(container);
	displayMode = new DisplayMode(container);
	screenShots = new ScreenShots(container);
	
	QVBoxLayout *l = new QVBoxLayout();
	l->addWidget(currNodeStats->container);
	l->addWidget(treeStats->container);
	l->addWidget(scalars->container);
	l->addWidget(treeDepth->container);
	l->addWidget(blendingType->groupBox);
	l->addWidget(displayMode->container);

	l->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
	l->addWidget(screenShots->container);
	
	container->setLayout(l);

	currNodeStats->container->hide();
	screenShots->container->hide();
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidget(container);
	scrollArea->setWidgetResizable(true);
}

ScreenShots::ScreenShots(QWidget *parent)
{
	container = new QWidget(parent);
	QVBoxLayout *l = new QVBoxLayout(container);
	topScreen = new QPushButton("Make top screenshot", container);
	bottomScreen = new QPushButton("Make bottom screenshot", container);
	l->addWidget(topScreen);
	l->addWidget(bottomScreen);
}
