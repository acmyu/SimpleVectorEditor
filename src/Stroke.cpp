#include "Stroke.h"

Stroke::Stroke() {
}

Stroke::~Stroke()
{
}

Stroke::Stroke(int i) {
}

void Stroke::clear() {
	displayLine.clear();
	bezLine.clear();
}

void Stroke::draw() {
	displayLine.draw();
}

void Stroke::drawActive(int smoothness) {
	Stroke displayStroke = *this;
	if (smoothness > 0) {
		displayStroke.displayLine = displayLine.getSmoothed(smoothness);
	}
	displayStroke.draw();
}

void Stroke::drawEditableVertex(BezPoint pt) {
	ofSetColor(255);
	ofDrawCircle(pt.point, 5);
	ofNoFill();
	if (pt.handleIn.x != ofxPathFitter::NONE && pt.handleIn.y != ofxPathFitter::NONE) {
		ofDrawLine(pt.point, pt.handleIn);
		ofDrawCircle(pt.handleIn, 5);
	}
	if (pt.handleOut.x != ofxPathFitter::NONE && pt.handleOut.y != ofxPathFitter::NONE) {
		ofDrawLine(pt.point, pt.handleOut);
		ofDrawCircle(pt.handleOut, 5);
	}
	ofFill();
	ofSetColor(100);
}

void Stroke::drawEditable(int iSelectedVertex) {
	ofSetColor(100);
	for (int i = 0; i<bezLine.size(); i++) {
		BezPoint pt = bezLine[i];
		if (i == iSelectedVertex) {
			drawEditableVertex(pt);
		}
		else {
			ofDrawCircle(pt.point, 5);
		}
	}
}

void Stroke::startStroke() {
	
}

void Stroke::addVertex(ofVec2f v) {
	displayLine.addVertex(v);
}

void Stroke::finishStroke(int smoothness) {
	displayLine = displayLine.getSmoothed(smoothness);
	displayLine.simplify(0.5f);
	updateBezLine();
}

void Stroke::updateBezLine() {
	bezLine = ofxPathFitter::simplify(displayLine, 10);
}

void Stroke::updateDisplayLine(bool simplify) {
	ofPolyline newDisplayLine;
	newDisplayLine.addVertex(bezLine.front().point);
	for (int i = 1; i < bezLine.size(); i++) {
		newDisplayLine.bezierTo(bezLine[i - 1].handleOut, bezLine[i].handleIn, bezLine[i].point);
	}
	displayLine = newDisplayLine;
	if (simplify) {
		displayLine.simplify(0.5f);
	}	
}

void Stroke::modifyHandle(int id, VertexTypes selectedHandle, int x, int y) {
	ofPoint pt = bezLine[id].point;
	if (selectedHandle == VertexTypes::HANDLEIN) {
		bezLine[id].handleIn.x = x;
		bezLine[id].handleIn.y = y;
	}
	else {
		bezLine[id].handleOut.x = x;
		bezLine[id].handleOut.y = y;
	}
	updateDisplayLine();
}

void Stroke::modifyVertex(int id, ofVec2f lastMousePos, int x, int y) {
	BezPoint* pt = &bezLine[id];
	pt->point.x = x;
	pt->point.y = y;
	pt->handleIn.x += x - lastMousePos.x;
	pt->handleIn.y += y - lastMousePos.y;
	pt->handleOut.x += x - lastMousePos.x;
	pt->handleOut.y += y - lastMousePos.y;
	updateDisplayLine();
}

int Stroke::getSelectedVertex(int iSelectedVertex, VertexTypes &handle, int x, int y) {
	handle = VertexTypes::INVALID;
	for (int i = 0; i < bezLine.size(); i++) {
		BezPoint s = bezLine[i];
		if (iSelectedVertex == i) {
			if (ofDist(s.handleIn.x, s.handleIn.y, x, y) < CLICKPADDING) {
				handle = VertexTypes::HANDLEIN;
			}
			else if (ofDist(s.handleOut.x, s.handleOut.y, x, y) < CLICKPADDING) {
				handle = VertexTypes::HANDLEOUT;
			}
		}
		
		// if no handle is selected, determine if point is selected
		if (handle == VertexTypes::INVALID && ofDist(s.point.x, s.point.y, x, y) < CLICKPADDING) {
			handle = VertexTypes::POINT;
		}
		
		if (handle != VertexTypes::INVALID) {
			return i;
		}
	}
	return -1;
}

int distLine(ofPoint pt, ofPolyline *line) {
	ofPoint cl = (*line).getClosestPoint(pt);
	return cl.distance(pt);
}

int Stroke::getSelectedStroke(vector<Stroke> *strokes, ofVec2f mousePos) {
	for (int i = 0; i < (*strokes).size(); i++) {
		ofPolyline *displayLine = &((*strokes)[i].displayLine);
		if (distLine(mousePos, displayLine) < CLICKPADDING) {
			return i;
		}
	}
	return -1;
}

void Stroke::translateLine(ofVec2f lastMousePos, int x, int y) {
	int xShift = x - lastMousePos.x;
	int yShift = y - lastMousePos.y;
	for (int i = 0; i < bezLine.size(); i++) {
		BezPoint* pt = &bezLine[i];
		pt->point.x += xShift;
		pt->point.y += yShift;
		pt->handleIn.x += xShift;
		pt->handleIn.y += yShift;
		pt->handleOut.x += xShift;
		pt->handleOut.y += yShift;
	}
	for (int i = 0; i < displayLine.size(); i++) {
		displayLine[i].x += xShift;
		displayLine[i].y += yShift;
	}
}


