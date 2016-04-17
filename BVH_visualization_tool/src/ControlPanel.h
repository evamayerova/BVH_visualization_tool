#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>

struct CurrentNodeStats {
	CurrentNodeStats()
	{
		container = NULL;
		layout = NULL;
		index = NULL;
		bounds = NULL;
		triangleNr = NULL;
	}
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *index, *bounds, *triangleNr;
};

struct CurrentTreeStats {
	CurrentTreeStats()
	{
		container = NULL;
		layout = NULL;
		importedBVHnodeCount = NULL;
		realBVHnodeCount = NULL;
	}
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *importedBVHnodeCount, *realBVHnodeCount;
};

struct SceneStats {
	SceneStats();
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *triangleCountLabel;
};

struct ScalarValuesGUI {
	ScalarValuesGUI();
	QWidget *container;
	QVBoxLayout *layout;
	QLabel *label, *absMin, *absMax, *firstLabel, *secondLabel;
	QComboBox *scalars;
	QSlider *first, *second;
};
