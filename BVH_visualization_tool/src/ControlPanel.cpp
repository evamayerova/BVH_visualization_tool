#include "ControlPanel.h"

ScalarValuesGUI::ScalarValuesGUI(QWidget *parent)
{
	container = new QGroupBox("Selected property", parent);
	scalars = new QComboBox(container);

	localMin = new QLabel(container);
	localMax = new QLabel(container);
	first = new QSlider(Qt::Horizontal, container);
	second = new QSlider(Qt::Horizontal, container);

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
	blendingType = new BlendingType(container);
	displayMode = new DisplayMode(container);
	
	QVBoxLayout *l = new QVBoxLayout();
	l->addWidget(currNodeStats->container);
	l->addWidget(treeStats->container);
	l->addWidget(scalars->container);
	l->addWidget(treeDepth->container);
	l->addWidget(blendingType->groupBox);
	l->addWidget(displayMode->container);

	l->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
	container->setLayout(l);

	currNodeStats->container->hide();

	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidget(container);
	scrollArea->setWidgetResizable(true);
}
