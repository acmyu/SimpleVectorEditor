#pragma once

#include "ofMain.h"

#include "Stroke.h"

class ofApp : public ofBaseApp{
	enum Mode {
		IDLE,
		DRAWING,
		EDITING
	};

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// ui
		ofRectangle canvas;
		

		// line
		vector <Stroke> strokes;
		Stroke currentStroke;
		int iSelectedStroke;
		int iSelectedVertex;
		Stroke::VertexTypes selectedHandle;

		// state
		ofVec2f lastMousePos;
		bool isLeftMouseButtonPressed;
		bool isSavingSVG;
		bool isSavingRaster;
		string save_name;
		bool flushCanvas;
		Mode usrtask;

		// settings
		int set_smoothness;
};
