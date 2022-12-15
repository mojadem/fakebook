#include "ofApp.h"

#include <curl/curl.h>

#include <algorithm>
#include <random>

#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // for boost::split

constexpr auto NUM_T = 5;
auto rng = std::default_random_engine{};

string ofApp::OPENAI_API_KEY;
vector<string> ofApp::likedKeywords;


void ofApp::setup() {
    curl_global_init(CURL_GLOBAL_ALL);
    
    ofBuffer buf = ofBufferFromFile("key.txt");
    ofApp::OPENAI_API_KEY = buf.getText();
    
    loadingImg.load("loadingImg.png");
    loadingImgWidth = ofGetWidth() / 4;
    loadingImg.resize(loadingImgWidth, loadingImgWidth);
    
    for (int i = 0; i < NUM_T; i++) {
        images.emplace_back(std::async(std::launch::async, prepareImage));
    }
    
    curLoaded = false;
    nextLoaded = false;
    
    white.set(255);
    bgColor.setHex(0x070d23);
    headerColor.setHex(0x18233d);
    fontColor.setHex(0xfcd2ae);
    
    bgRect.set(0, 0, ofGetWidth(), ofGetHeight());
    headerSize = ofGetHeight() / 12;
    headerRect.set(0, 0, ofGetWidth(), headerSize);
    
    boldFontSize = ofGetWidth() / 15;
    regularFontSize = ofGetWidth() / 30;
    
    boldFont.load("SourceCodePro-Bold.ttf", boldFontSize);
    regularFont.load("SourceCodePro-Regular.ttf", regularFontSize);
    
    loadingSound.load("loadingSound.mp3");
    scrollSound.load("scrollSound.mp3");
    
    fps = 60;
    ofSetFrameRate(fps);
    moving = false;
    animFrame = 0.0f;
    animDuration = 1.0f;
    animStep = 1 / (fps * animDuration);
    
    startupScreen = true;
    lookedAtDuration = -1;
    loadedAt = 0.0f;
    ofEnableAlphaBlending();
}

void ofApp::exit() {
    curl_global_cleanup();
}

void ofApp::update() {
    try {
        if (images.front().valid() && images.front().wait_for(std::chrono::seconds(0)) == std::future_status::ready && !curLoaded) {
            if (!startupScreen) loadingSound.play();
            Image i = images.front().get();
            curImg.clear();
            curImg.getPixels() = i.imgData;
            curImg.update();
            curKeywordsStr.clear();
            curKeywordsVec = i.keywords;
            for (auto& s : i.keywords) {
                curKeywordsStr.append(s);
                curKeywordsStr.append("\n");
            }
            curLoaded = true;
            loadedAt = ofGetElapsedTimef();
        }
    }
    catch (const std::exception& e) {
        std::cout << "ERROR IN LOAD CURRENT: " << e.what() << std::endl;
        images.emplace_back(std::async(std::launch::async, prepareImage));
        images.pop_front();
    }
    
    try {
        if (images[1].valid() && images[1].wait_for(std::chrono::seconds(0)) == std::future_status::ready && !nextLoaded) {
            Image i = images[1].get();
            nextImg.clear();
            nextImg.getPixels() = i.imgData;
            nextImg.update();
            nextKeywordsVec = i.keywords;
            nextKeywordsStr.clear();
            for (auto& s : i.keywords) {
                nextKeywordsStr.append(s);
                nextKeywordsStr.append("\n");
            }
            nextLoaded = true;
        }
    }
    catch (const std::exception& e) {
        std::cout << "ERROR IN LOAD NEXT: " << e.what() << std::endl;
        images.emplace_back(std::async(std::launch::async, prepareImage));
        images.erase(images.begin() + 1);
    }
    
}

void ofApp::draw() {
    ofSetColor(bgColor);
    ofDrawRectangle(bgRect);
    
    /* STARTUP SCREEN */
    if (startupScreen) {
        ofSetColor(fontColor);
        boldFont.drawString("hello!", regularFontSize, regularFontSize*6);
        regularFont.drawString("i'm fakebook, and i provide you\nwith endless content to consume.\n\nthe following images are not real;\nthey are artificially generated\nfrom the set of keywords below it.\neach image is unique.\n\nas you scroll, i will learn\nwhat keywords keep you engaged\nthe longest.\n\nhopefully i can learn enough\nabout you to keep you\nendlessly scrolling!\n\n\ncreated by matt dembiczak\nand trevor barlock", regularFontSize, regularFontSize*10);
        regularFont.drawString("tap anywhere to continue", regularFontSize, ofGetHeight() - regularFontSize*2);
        
        return;
    }
    
    /* TRANSITION IMAGE LOGIC */
    if (animFrame > 1.0) {
        moving = false;
        animFrame = 0.0;
        
        for (int i = 0; i < min(lookedAtDuration - 3, 20); i++) {
            ofApp::likedKeywords.insert(ofApp::likedKeywords.end(), curKeywordsVec.begin(), curKeywordsVec.end());
        }
        std::shuffle(std::begin(ofApp::likedKeywords), std::end(ofApp::likedKeywords), rng);
        
        images.pop_front();
        curImg = nextImg;
        curLoaded = nextLoaded;
        curKeywordsVec = nextKeywordsVec;
        curKeywordsStr = nextKeywordsStr;
        nextLoaded = false;
        ofResetElapsedTimeCounter();
        loadedAt = ofGetElapsedTimef();
    }
    
    /* TRANSLATE LOGIC */
    ofPushMatrix();
    if (moving) {
        animFrame += animStep;
        ofTranslate(0, -easeOutQuint(animFrame) * ofGetHeight());
    } else if (ofGetMousePressed()) {
        int offset = ofGetMouseY() - mousePressedAtY;
        offset = max(offset, -ofGetHeight() / 5);
        offset = min(offset, ofGetHeight() / 5);
        ofTranslate(0, offset);
    }
    
    /* DRAW CURRENT IMAGE */
    ofSetColor(white);
    if (curLoaded) {
        curImg.draw(0, ofGetHeight() / 2 - ofGetWidth() / 2, ofGetWidth(), ofGetWidth());
        ofSetColor(fontColor);
        regularFont.drawString(curKeywordsStr, regularFontSize, ofGetHeight() / 2 + ofGetWidth() / 2 + boldFontSize);
    }
    else {
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ofRotateDeg(ofGetElapsedTimeMillis() % 360);
        loadingImg.draw(-loadingImgWidth / 2, -loadingImgWidth / 2);
        ofPopMatrix();
    }
    
    /* DRAW NEXT IMAGE */
    ofSetColor(white);
    if (nextLoaded) {
        nextImg.draw(0, ofGetHeight() + ofGetHeight() / 2 - ofGetWidth() / 2, ofGetWidth(), ofGetWidth());
        ofSetColor(fontColor);
        regularFont.drawString(nextKeywordsStr, regularFontSize, ofGetHeight() + ofGetHeight() / 2 + ofGetWidth() / 2 + boldFontSize);
    }
    else {
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() + ofGetHeight() / 2);
        ofRotateDeg(ofGetElapsedTimeMillis() % 360);
        loadingImg.draw(-loadingImgWidth / 2, -loadingImgWidth / 2);
        ofPopMatrix();
    }
    
    ofPopMatrix();
    
    /* DRAW HEADER */
    ofSetColor(headerColor);
    ofDrawRectangle(headerRect);
    
    /* DRAW TITLE */
    ofSetColor(fontColor);
    boldFont.drawString("fakebook", regularFontSize, headerSize - regularFontSize);
    
    /* DRAW LOOKED AT DURATION */
    if (lookedAtDuration >= 0) {
        int alpha = 255 - ofGetElapsedTimef() * 255;
        if (!moving) ofSetColor(fontColor, alpha);
        
        std::stringstream lookedAtStr;
        lookedAtStr << lookedAtDuration;
        if (lookedAtDuration != 1) {
            lookedAtStr << " seconds";
        } else {
            lookedAtStr << " second";
        }
        regularFont.drawString(lookedAtStr.str(), 2 * ofGetWidth() / 3, headerSize - regularFontSize);
    }
}

float ofApp::easeOutQuint(float x) {
    return 1 - pow(1 - x, 5);
}

void ofApp::keyPressed(int key) {
    
}

void ofApp::keyReleased(int key) {
    
}

void ofApp::mouseMoved(int x, int y) {
    
}

void ofApp::mouseDragged(int x, int y, int button) {
    int dy = ofGetPreviousMouseY() - y;
    if (curLoaded && !moving && dy > ofGetHeight() / regularFontSize) {
        moving = true;
        images.emplace_back(std::async(std::launch::async, prepareImage));
        scrollSound.play();
        lookedAtDuration = ofGetElapsedTimef() - loadedAt;
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    mousePressedAtY = y;
}

void ofApp::mouseReleased(int x, int y, int button) {
    if (startupScreen) {
        startupScreen = false;
        ofResetElapsedTimeCounter();
        loadedAt = ofGetElapsedTimef();
        loadingSound.play();
    }
}

void ofApp::mouseEntered(int x, int y) {
    
}

void ofApp::mouseExited(int x, int y) {
    
}

void ofApp::windowResized(int w, int h) {
    
}

void ofApp::gotMessage(ofMessage msg) {
    
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}

//--------------------------------------------------------------
size_t ofApp::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

ofApp::Image ofApp::prepareImage() {
    std::cout << "Image prepare started on thread " << std::this_thread::get_id() << std::endl;
    Image img;
    getKeywords(img);
    generateImage(img);
    loadImage(img);
    std::cout << "Image prepare finished on thread " << std::this_thread::get_id() << std::endl;
    return img;
}

void ofApp::getKeywords(Image &img) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://random-word-api.herokuapp.com/word?number=5");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout << "KEYWORD REQUEST ERROR " << (int)res << " on thread " << std::this_thread::get_id() << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '['), readBuffer.cend());
    readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), ']'), readBuffer.cend());
    readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '"'), readBuffer.cend());
    
    boost::split(img.keywords, readBuffer, boost::is_any_of(", "), boost::token_compress_on);
    
    if (ofApp::likedKeywords.size() < 5) {
        return;
    }
    
    for (int i = 0; i < std::rand() % 5; i++) {
        img.keywords[i] = ofApp::likedKeywords[i];
    }
    
    /*std::cout << "KEYWORDS REQUEST COMPLETE: ";
     for (auto& s : img.keywords) std::cout << s << " ";
     std::cout << "on thread " << std::this_thread::get_id() << std::endl;*/
}

void ofApp::generateImage(Image &img) {
    CURL* curl;
    CURLcode res;
    struct curl_slist* slist1;
    string readBuffer;
    
    curl = curl_easy_init();
    if (curl) {
        slist1 = NULL;
        slist1 = curl_slist_append(slist1, "Content-Type: application/json");
        std::stringstream key_ss;
        key_ss << "Authorization: Bearer " << ofApp::OPENAI_API_KEY;
        const string key_tmp = key_ss.str();
        const char* key = key_tmp.c_str();
        slist1 = curl_slist_append(slist1, key);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/images/generations");
        std::stringstream post_ss;
        post_ss.clear();
        post_ss << "{ \"prompt\": \"";
        for (auto& s : img.keywords) post_ss << s << " ";
        post_ss << "\", \"n\": 1, \"size\": \"512x512\"}";
        const string& tmp = post_ss.str();
        const char* postfields = tmp.c_str();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout << "GENERATE IMAGE REQUEST ERROR " << (int)res << " on thread " << std::this_thread::get_id() << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), '\n'), readBuffer.cend());
    readBuffer.erase(std::remove(readBuffer.begin(), readBuffer.end(), ' '), readBuffer.cend());
    
    string idSearch = string("\"created\":");
    size_t idStart = readBuffer.find(idSearch) + idSearch.length();
    size_t idEnd = readBuffer.find(",", idStart);
    img.id = std::stoi(readBuffer.substr(idStart, idEnd - idStart));
    
    string urlSearch = string("\"url\":\"");
    size_t urlStart = readBuffer.find(urlSearch) + urlSearch.length();
    size_t urlEnd = readBuffer.find("}", urlStart);
    img.url = readBuffer.substr(urlStart, urlEnd - urlStart - 1); // -1 for ending quotation
    
    //std::cout << "GENERATE IMAGE REQUEST COMPLETE: " << img.id << " on thread " << std::this_thread::get_id() << std::endl;
}

void ofApp::loadImage(Image& img) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, img.url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout << "LOAD IMAGE REQUEST ERROR " << (int)res << " on thread " << std::this_thread::get_id() << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    ofBuffer buf(readBuffer.c_str(), readBuffer.size());
    ofLoadImage(img.imgData, buf);
    
    //std::cout << "LOAD IMAGE REQUEST COMPLETE: " << img.id << " on thread " << std::this_thread::get_id() << std::endl;
}
