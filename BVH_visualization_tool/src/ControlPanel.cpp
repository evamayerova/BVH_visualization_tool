#include "ControlPanel.h"

ScalarValuesGUI::ScalarValuesGUI(QWidget *parent)
{
	container = new QWidget(parent);
	label = new QLabel("Selected property", container);
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
	layout->addWidget(label);
	layout->addWidget(scalars);
	layout->addWidget(slidersHolder);
	layout->addWidget(addScalarsButton);

	container->setLayout(layout);
}

ControlPanel::ControlPanel()
{
	container = new QWidget();
	sceneStats = new SceneStats(container);
	treeStats = new CurrentTreeStats(container);
	currNodeStats = new CurrentNodeStats(container);
	scalars = new ScalarValuesGUI(container);
	treeDepth = new TreeDepth(container);

	QVBoxLayout *l = new QVBoxLayout();
	l->addWidget(currNodeStats->container);
	l->addWidget(treeStats->container);
	//l->addWidget(currNodeStats->container);
	l->addWidget(scalars->container);
	l->addWidget(treeDepth->container);
	l->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
	l->addWidget(sceneStats->container);
	container->setLayout(l);

	currNodeStats->container->hide();
}
