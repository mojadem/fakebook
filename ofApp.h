#pragma once

#include "ofMain.h"

#include <future>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void exit();
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
			vector<string> keywords;
			int id;
			string url;
			ofPixels imgData;
		};
		static string OPENAI_API_KEY;
		static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
		static Image prepareImage();
		static void getKeywords(Image &img);
		static void generateImage(Image &img);
		static void loadImage(Image& img);

		deque<std::future<Image>> images;
		ofImage loadingImg;
		int loadingImgWidth;
		ofImage curImg;
		ofImage nextImg;
		string curKeywords;
		string nextKeywords;
		bool curLoaded;
		bool nextLoaded;
		
		ofColor white;
		ofColor bgColor;
		ofColor headerColor;
		ofColor fontColor;
		ofRectangle bgRect;
		ofRectangle headerRect;
		ofTrueTypeFont boldFont;
		ofTrueTypeFont regularFont;
};
