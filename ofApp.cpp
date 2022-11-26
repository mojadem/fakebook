#include "ofApp.h"
#include <curl/curl.h>

//--------------------------------------------------------------
void ofApp::setup() {
	Image i = prepareImage("a white siamese cat");
	images.push_back(i);
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	images[0].img.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
size_t ofApp::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

ofApp::Image ofApp::prepareImage(string prompt) {
	string data = performRequest(prompt);
	Image i = parseData(data);
	return i;
}

string ofApp::performRequest(string prompt) {
	CURL* curl;
	CURLcode res;
	struct curl_slist* slist1;
	string readBuffer;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if (curl) {
		slist1 = NULL;
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");
		slist1 = curl_slist_append(slist1, "Authorization: Bearer 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");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/images/generations");

		std::ostringstream ss;
		ss << "{ \"prompt\": \"" << prompt << "\", \"n\": 1, \"size\": \"256x256\"}";
		const string& tmp = ss.str();
		const char* postfields = tmp.c_str();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ofApp::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cout << "REQUEST ERROR" << (int)res << std::endl;
		}

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	// strip newline and whitespace
	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '\n'), readBuffer.cend());
	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), ' '), readBuffer.cend());
	cout << readBuffer << endl;
	return readBuffer;
}

ofApp::Image ofApp::parseData(string data) {
	string idSearch = string("\"created\":");
	size_t idStart = data.find(idSearch) + idSearch.length();
	size_t idEnd = data.find(",", idStart);
	int id = std::stoi(data.substr(idStart, idEnd - idStart));

	string urlSearch = string("\"url\":\"");
	size_t urlStart = data.find(urlSearch) + urlSearch.length();
	size_t urlEnd = data.find("}", urlStart);
	string url = data.substr(urlStart, urlEnd - urlStart - 1); // -1 for ending quotation

	ofImage img;
	ofHttpResponse r = ofLoadURL(url);
	img.load(r.data);

	return Image{ id, url, img };
}