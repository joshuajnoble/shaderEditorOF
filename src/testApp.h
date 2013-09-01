#pragma once

#include <iomanip>
#include <string>
#include <sstream>

#define OFX_UI_NO_XML

#include "ofMain.h"
#include "ofxTextInputField.h"
#include "ofxUI.h"


enum uniformType {
    FLOAT, INT, VEC2, VEC3, VEC4, MAT4, TEXTURE
};

enum MODEL_TYPE {
    SPHERE, CUBE, PLANE
};

class shaderLog : public ofBaseLoggerChannel
{
    public:
	
    string logs;
    
    ~shaderLog(){};
	
    void log(ofLogLevel level, const string & module, const string & message)
    {
        logs += message;
    }
    
	void log(ofLogLevel level, const string & module, const char* format, ...)
    {
        //logs += message;
    }
    
	void log(ofLogLevel level, const string & module, const char* format, va_list args)
    {
        //logs += message;
    }
};

class uniformObject {
    
public:
    
    string name;
    uniformType type;
    
    void *value; // we just don't know what this will be
    
};


class testApp : public ofBaseApp{
	public:

    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void guiEvent(ofxUIEventArgs &e);
    
    ofTrueTypeFont andaleMono;
    
    ofxTextInputField monoLineTextInput, multilineTextInput;
    ofShader shader;

    //map<string, string> fragmentShaders, vertexShaders;
    string fragmentShader,vertexShader;
    
    int shaderCurrentlyEditing;
    string output;

    ofPtr<ofBaseLoggerChannel> shaderResultLogger;
    ofxUICanvas *gui;
    
    ofEasyCam cam;
    
    vector<uniformObject> uniforms;
    
    MODEL_TYPE modelType;
    
    float debounceValue;
    bool addLabelOnNextUpdate, removeOnNextUpdate;
    
    vector<ofImage *> loadedTextures;
    
};
