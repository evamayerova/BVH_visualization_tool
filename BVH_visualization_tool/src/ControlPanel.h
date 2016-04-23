#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>

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

struct TreeDepth {
	TreeDepth(QWidget *parent)
	{
		container = new QWidget(parent);
		QHBoxLayout *l = new QHBoxLayout(container);
		label = new QLabel("Maximal tree depth", container);
		depthHolder = new QSpinBox(container);
		l->addWidget(label);
		l->addWidget(depthHolder);

	}
	QWidget *container;
	QLabel *label;
	QSpinBox *depthHolder;
};

class ControlPanel {
public:
	ControlPanel();
	QWidget *container;
	SceneStats *sceneStats;
	CurrentTreeStats *treeStats;
	CurrentNodeStats *currNodeStats;
	ScalarValuesGUI *scalars;
	TreeDepth *treeDepth;
};
