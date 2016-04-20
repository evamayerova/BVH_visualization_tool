#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

struct CurrentNodeStats {
	CurrentNodeStats(QWidget *parent)
	{
		container = new QWidget(parent);
		layout = new QVBoxLayout(container);
		index = new QLabel(container);
		bounds = new QLabel(container);
		triangleNr = new QLabel(container);
		
		layout->addWidget(index);
		layout->addWidget(bounds);
		layout->addWidget(triangleNr);

		container->setLayout(layout);
	}
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *index, *bounds, *triangleNr;
};

struct CurrentTreeStats {
	CurrentTreeStats(QWidget *parent)
	{
		container = new QWidget(parent);
		layout = new QVBoxLayout(container);
		importedBVHnodeCount = new QLabel(container);
		realBVHnodeCount = new QLabel(container);

		layout->addWidget(importedBVHnodeCount);
		layout->addWidget(realBVHnodeCount);

		container->setLayout(layout);
	}
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *importedBVHnodeCount, *realBVHnodeCount;
};

struct SceneStats {
	SceneStats(QWidget *parent)
	{
		container = new QWidget(parent);
		QLabel *statsHeading = new QLabel("Scene stats", container);
		layout = new QVBoxLayout(container);
		triangleCountLabel = new QLabel(container);

		layout->addWidget(statsHeading);
		layout->addWidget(triangleCountLabel);

		container->setLayout(layout);
	}
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *triangleCountLabel;
};

struct ScalarValuesGUI {
	ScalarValuesGUI(QWidget *parent);
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *label, *localMin, *localMax;
	QComboBox *scalars;
	QSlider *first, *second;
	QPushButton *addScalarsButton;
};

class ControlPanel {
public:
	ControlPanel();
	QWidget *container;
	SceneStats *sceneStats;
	CurrentTreeStats *treeStats;
	CurrentNodeStats *currNodeStats;
	ScalarValuesGUI *scalars;
};
