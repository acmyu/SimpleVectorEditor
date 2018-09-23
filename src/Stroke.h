#pragma once

#include "ofMain.h"

#include "ofxPathFitter.h"

#define CLICKPADDING 10 // threshold to determine if object is clicked
#define SNAPPADDING 20 // threshold to determine if an endpoint can snap to a line or endpoint

class Stroke
{
	void drawEditableVertex(BezPoint vertex);

public:
	enum VertexTypes {
		INVALID,
		POINT,
		HANDLEIN,
		HANDLEOUT,
	};

	Stroke();
	~Stroke();
	Stroke(int i);
	
	bool isClosed;

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
	void modifyHandle(int id, VertexTypes selectedHandle, int x, int y);
	void modifyVertex(int id, int x, int y);
	
	// returns index of point or handle at x,y
	int getSelectedVertex(int iSelectedVertex, VertexTypes &handle, int x, int y);
	// returns index of stroke at x,y
	static int getSelectedStroke(vector<Stroke> *strokes, ofVec2f mousePos);

	// call on drag line
	void translateLine(ofVec2f lastMousePos, int x, int y);

	// snap to intersection, convert to closedform shape if endpoints are near each other
	void snapEndpoints();
	bool intersectLineToStroke(ofPoint lineStart, ofPoint lineDelta, ofPoint& intersection);
	bool snapToIntersection(BezPoint& pt, ofPoint& pt2);
	
	// the stroke that gets displayed
	ofPolyline displayLine;
	// contains handle coords
	vector<BezPoint> bezLine;
};


