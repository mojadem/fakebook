#include "ofApp.h"\

#include <curl/curl.h>
#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // for boost::split

//--------------------------------------------------------------
void ofApp::setup() {
	curl_global_init(CURL_GLOBAL_ALL);
	
	ofBuffer buf = ofBufferFromFile("key.txt");
	OPENAI_API_KEY = buf.getText();

	Image i = prepareImage();
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

ofApp::Image ofApp::prepareImage() {
	vector<string> keywords = getKeywords();
	return getImage(keywords);
}

vector<string> ofApp::getKeywords() {
	// Load keywords
	CURL* curl;
	CURLcode res;
	string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://random-word-api.herokuapp.com/word?number=5");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ofApp::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cout << "KEYWORD REQUEST ERROR" << (int)res << std::endl;
		}

		curl_easy_cleanup(curl);
	}

	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '['), readBuffer.cend());
	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), ']'), readBuffer.cend());
	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '"'), readBuffer.cend());
	
	vector<string> keywords;
	boost::split(keywords, readBuffer, boost::is_any_of(", "), boost::token_compress_on);
	
	std::cout << "KEYWORDS REQUEST COMPLETE: ";
	for (auto& s : keywords) std::cout << s << " ";
	std::cout << std::endl;
	
	return keywords;
}

ofApp::Image ofApp::getImage(vector<string>& keywords) {
	// Load image
	CURL* curl;
	CURLcode res;
	struct curl_slist* slist1;
	string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		slist1 = NULL;
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");
		std::stringstream key_ss;
		key_ss << "Authorization: Bearer " << OPENAI_API_KEY;
		const string key_tmp = key_ss.str();
		const char* key = key_tmp.c_str();
		slist1 = curl_slist_append(slist1, key);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/images/generations");

		std::stringstream post_ss;
		post_ss.clear();
		post_ss << "{ \"prompt\": \"";
		for (auto& s : keywords) post_ss << s << " ";
		post_ss << "\", \"n\": 1, \"size\": \"256x256\"}";
		const string& tmp = post_ss.str();
		const char* postfields = tmp.c_str();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ofApp::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cout << "IMAGE REQUEST ERROR" << (int)res << std::endl;
		}

		curl_easy_cleanup(curl);
	}

	// Parse data
	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '\n'), readBuffer.cend());
	readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), ' '), readBuffer.cend());

	string idSearch = string("\"created\":");
	size_t idStart = readBuffer.find(idSearch) + idSearch.length();
	size_t idEnd = readBuffer.find(",", idStart);
	int id = std::stoi(readBuffer.substr(idStart, idEnd - idStart));

	string urlSearch = string("\"url\":\"");
	size_t urlStart = readBuffer.find(urlSearch) + urlSearch.length();
	size_t urlEnd = readBuffer.find("}", urlStart);
	string url = readBuffer.substr(urlStart, urlEnd - urlStart - 1); // -1 for ending quotation

	ofImage img;
	ofHttpResponse r = ofLoadURL(url);
	img.load(r.data);

	std::cout << "IMAGE REQUEST COMPLETE: " << readBuffer << std::endl;

	return Image{ id, url, img };
}