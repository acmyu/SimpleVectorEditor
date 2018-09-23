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
	pt = pt.handlesAbsolute();
	if (pt.hasHandleIn()) {
		ofDrawLine(pt.point, pt.handleIn);
		ofDrawCircle(pt.handleIn, 5);
	}
	if (pt.hasHandleOut()) {
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
		if (i != iSelectedVertex) {
			ofDrawCircle(pt.point, 5);
		}
	}
	if (iSelectedVertex >= 0 && iSelectedVertex < bezLine.size())
		drawEditableVertex(bezLine[iSelectedVertex]);
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
	snapEndpoints();
}

void Stroke::snapEndpoints() {
	isClosed = false;
	if (bezLine.size() <= 2) return;
	ofPoint endpoint1 = displayLine[0];
	ofPoint endpoint2 = *(displayLine.end() - 1);
	// snap endpoints
	if (endpoint1.squareDistance(endpoint2) <= SNAPPADDING * SNAPPADDING) {
		isClosed = true;
		BezPoint lastPt = bezLine.back(); bezLine.pop_back();
		bezLine[0].point = lastPt.point;
		bezLine[0].handleIn = lastPt.handleIn;
		updateDisplayLine();
		return;
	}

	// snap endpoint to line
	/*if (snapToIntersection(bezLine[0], bezLine[0].handleOut) // snap forward
		|| snapToIntersection(bezLine[0], bezLine[0].point - bezLine[1].point)) // snap back
		updateDisplayLine();
	if(snapToIntersection(bezLine.back(), bezLine.back().handleIn) // snap forward
		|| snapToIntersection(bezLine.back(), bezLine.back().point - bezLine.rbegin()[1].point)) // snap back
		updateDisplayLine();*/
}

bool Stroke::snapToIntersection(BezPoint& pt, ofPoint& pt2) {
	ofPoint intersection = ofPoint(-1, -1);
	if (intersectLineToStroke(pt.point, pt2.normalize() * SNAPPADDING, intersection)) {
		pt.point = intersection;
		return true;
	}
	return false;
}

bool Stroke::intersectLineToStroke(ofPoint lineStart, ofPoint lineDelta, ofPoint& intersection) {
	ofPoint lineEnd = lineStart - lineDelta;
	auto iterator = displayLine.getVertices().begin();
	while (iterator != displayLine.getVertices().end() - 1) {
		ofPoint v1 = *iterator;
		ofPoint v2 = *(iterator + 1);
		if (lineStart != v1 && lineStart != v2
			&& lineEnd != v1 && lineEnd != v2
			&& ofLineSegmentIntersection(lineStart, lineEnd, v1, v2, intersection))
			return true;
		iterator++;
	}
	return false;
}

void Stroke::updateBezLine() {
	bezLine = ofxPathFitter::simplify(displayLine, 1);
}

void Stroke::updateDisplayLine(bool simplify) {
	ofPolyline newDisplayLine;
	newDisplayLine.addVertex(bezLine.front().point);
	for (int i = 1; i < bezLine.size(); i++) {
		BezPoint p1 = bezLine[i - 1].handlesAbsolute();
		BezPoint p2 = bezLine[i].handlesAbsolute();
		newDisplayLine.bezierTo(p1.handleOut, p2.handleIn, p2.point);
	}

	if (isClosed) {
		newDisplayLine.close();
		BezPoint p1 = bezLine.back().handlesAbsolute();
		BezPoint p2 = bezLine[0].handlesAbsolute();
		newDisplayLine.bezierTo(p1.handleOut, p2.handleIn, p2.point);
	}

	displayLine = newDisplayLine;
	if (simplify) {
		//displayLine.simplify(0.5f);
	}	
}

void Stroke::modifyHandle(int id, VertexTypes selectedHandle, int x, int y) {
	ofPoint pt = bezLine[id].point;
	if (selectedHandle == VertexTypes::HANDLEIN) {
		bezLine[id].handleIn.x = x - pt.x;
		bezLine[id].handleIn.y = y - pt.y;
	}
	else {
		bezLine[id].handleOut.x = x - pt.x;
		bezLine[id].handleOut.y = y - pt.y;
	}
	updateDisplayLine();
}

void Stroke::modifyVertex(int id, int x, int y) {
	BezPoint* pt = &bezLine[id];
	pt->point.x = x;
	pt->point.y = y;
	updateDisplayLine();
}

int Stroke::getSelectedVertex(int iSelectedVertex, VertexTypes &handle, int x, int y) {
	handle = VertexTypes::INVALID;
	int nPoints = bezLine.size();
	for (int i = 0; i < nPoints; i++) {
		BezPoint s = bezLine[i].handlesAbsolute();
		if (iSelectedVertex == i) {
			if ((isClosed || i != 0) && ofDist(s.handleIn.x, s.handleIn.y, x, y) < CLICKPADDING) {
				handle = VertexTypes::HANDLEIN;
			}
			else if ((isClosed || i != nPoints - 1) && ofDist(s.handleOut.x, s.handleOut.y, x, y) < CLICKPADDING) {
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
	}
	for (int i = 0; i < displayLine.size(); i++) {
		displayLine[i].x += xShift;
		displayLine[i].y += yShift;
	}
}


