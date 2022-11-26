#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

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
		
		struct Image {
			int id;
			string url;
			ofImage img;
		};
		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
		Image prepareImage(string prompt);
		string performRequest(string prompt);
		Image parseData(string data);
		
		std::vector<Image> images;
};
