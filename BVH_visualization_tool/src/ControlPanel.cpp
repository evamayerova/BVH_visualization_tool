#include "ControlPanel.h"

ScalarValuesGUI::ScalarValuesGUI()
{
	container = new QWidget();
	label = new QLabel("Selected property", container);
	scalars = new QComboBox(container);

	first = new QSlider(Qt::Horizontal, container);
	second = new QSlider(Qt::Horizontal, container);

	layout = new QVBoxLayout(container);
	layout->addWidget(label);
	layout->addWidget(scalars);
	layout->addWidget(first);
	layout->addWidget(second);

	container->setLayout(layout);
}

SceneStats::SceneStats()
{
	container = new QWidget();
	QLabel *statsHeading = new QLabel("Scene stats", container);
	layout = new QVBoxLayout(container);
	triangleCountLabel = new QLabel(container);
	layout->addWidget(statsHeading);
	layout->addWidget(triangleCountLabel);
}
