#pragma once

#include "ofMain.h"

#include "ofxPathFitter.h"

#define SELECTPADDING	10

#define POINT		0
#define HANDLEIN	1
#define HANDLEOUT	2

class Stroke
{
	void drawEditableVertex(BezPoint vertex);

public:
	Stroke();
	~Stroke();
	Stroke(int i);
	
	void clear();

	void draw();
	void drawActive(int smoothness);
	void drawEditable(int iSelectedVertex);

	// call on brush down
	void startStroke();
	// call on brush dragged
	void addVertex(ofVec2f v);
	// call on brush up
	void finishStroke(int smoothness);

	// call when displayLine modified (finishStroke)
	void updateBezLine();
	// call when bezLine modified
	void updateDisplayLine(bool simplify = true);
	// call when point/handle modified
	void modifyHandle(int id, int selectedHandle, int x, int y);
	void modifyVertex(int id, ofVec2f lastMousePos, int x, int y);
	
	// returns index of vertex at x,y
	int getSelectedVertex(int iSelectedVertex, int &handle, int x, int y);
	// returns index of stroke at x,y
	static int getSelectedStroke(vector<Stroke> *strokes, ofVec2f mousePos);

	// call on drag line
	void translateLine(ofVec2f lastMousePos, int x, int y);
	
	// the stroke that gets displayed
	ofPolyline displayLine;
	// contains handle coords
	vector<BezPoint> bezLine;
};


