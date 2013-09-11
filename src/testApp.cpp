#include "testApp.h"

/*
 
 drag a shader file in - figure out what it is by type
 
 */


//--------------------------------------------------------------
void testApp::setup(){
	
	multilineTextInput.setup();

	multilineTextInput.text = "";
    multilineTextInput.multiline = true;
    multilineTextInput.bounds.x = 300;
    multilineTextInput.bounds.y = 100;
	multilineTextInput.bounds.width = 400;
	multilineTextInput.bounds.height = 500;
    
    shaderResultLogger = ofPtr<ofBaseLoggerChannel>(new shaderLog());
    ofSetLoggerChannel(shaderResultLogger);
    
    gui = new ofxUICanvas();
    
    gui->addLabelButton("vertex", false);
    gui->addLabelButton("fragment", false);
    gui->addLabelButton("compile", false);
    gui->addLabelButton("sphere", false);
    gui->addLabelButton("cube", false);
    gui->addLabelButton("plane", false);
    gui->addLabelButton("Add Uniform", false);
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    gui->setColorBack(ofColor(0,100));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(255,100));
    
    uniformGUI = new ofxUICanvas(240, 0, 250, 100);
    
    ofAddListener(uniformGUI->newGUIEvent,this,&testApp::uniformGuiEvent);
    
    uniformGUI->addTextInput("Uniform Name", "Uniform Name");
    vector<string> uniformTypes;
    uniformTypes.push_back("int");
    uniformTypes.push_back("float");
    uniformTypes.push_back("vec2");
    uniformTypes.push_back("vec3");
    uniformTypes.push_back("vec4");
    uniformTypes.push_back("mat4");
    uniformTypes.push_back("texture");
    uniformGUI->addDropDownList("Type", uniformTypes);
    uniformGUI->addLabelButton("Create This Uniform", false);
    
    uniformGUI->setColorBack(ofColor(0,100));
    uniformGUI->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(255,100));
    uniformGUI->setVisible(false);
    
    shaderCurrentlyEditing = 0;
    removeOnNextUpdate = addLabelOnNextUpdate = false;
    
    andaleMono.loadFont("font/Andale Mono.ttf", 12);
    multilineTextInput.setFont(andaleMono);
    
    vertexShader = "varying vec2 texcoord; \nvoid main() { \ntexcoord = gl_MultiTexCoord0.xy * 100.0; gl_Position = ftransform(); \n}";
    fragmentShader = "uniform sampler2DRect tex0; \nvarying vec2 texcoord;\nvoid main() { \ngl_FragColor = texture2DRect( tex0, texcoord.xy ); \n}";
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(20, 20, 20);
    
    cam.begin();
    ofEnableDepthTest();
    
    if(shader.isLoaded()) {
        shader.begin();
        
        
        vector<uniformObject>::iterator it = uniforms.begin();
        while(it != uniforms.end()) {
            
            switch( it->type )
            {
                case FLOAT:
                {
                    float* f = (float*) it->value;
                    shader.setUniform1f(it->name, *f);
                    break;
                }
                case INT:
                {
                    int* i = (int*) it->value;
                    shader.setUniform1i(it->name, *i);
                    break;
                }
                case VEC2:
                    shader.setUniform2fv(it->name, (float*) it->value);
                    break;
                case VEC3:
                    shader.setUniform3fv(it->name, (float*) it->value);
                    break;
                case VEC4:
                    shader.setUniform4fv(it->name, (float*) it->value);
                    break;
                case MAT4:
                {
                    ofMatrix4x4 m((float*) it->value);
                    shader.setUniformMatrix4f(it->name, m);
                }
                    break;
                case TEXTURE:
                {
                    
                    int *ind = static_cast<int*>(it->value);
                    if(*ind < loadedTextures.size()) {
                        ofTexture t = loadedTextures[*ind]->getTextureReference();
                        shader.setUniformTexture( it->name, t, *ind);
                    }
                }
                    break;
                
            }
            
            
            ++it;
        }
        
    }
    
    switch ( modelType )
    {
        case SPHERE:
            ofDrawSphere(0, 0, 100);
            break;
        case CUBE:
            ofDrawBox(0, 0, 100, 100, 100, 100);
            break;
        case PLANE:
            ofDrawPlane(0, 0, 400, 400);
            break;
    }
    
    if(shader.isLoaded())
    {
        shader.end();
    }
    
    ofEnableAlphaBlending();
    cam.end();
    
    vector<ofImage*>::iterator tit = loadedTextures.begin();
    int yPos = 0;
    while( tit != loadedTextures.end())
    {
        (*tit)->draw(900, yPos, 100, 100);
        yPos += 100;
        ++tit;
    }
    
    multilineTextInput.draw();
    ofDrawBitmapString(output, ofVec2f(10, 650));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    vector<string>::iterator it = dragInfo.files.begin();
    
    while(it != dragInfo.files.end())
    {
        string name = ofToLower(*it);
        if(name.find(".frag") != string::npos || name.find(".fs") != string::npos)
        {
            vector <string> ss = ofSplitString(name, ".");
            ofFile fShade(name);
            fragmentShader = ofToString(fShade.readToBuffer());
            
            if(shaderCurrentlyEditing == 1) {
                multilineTextInput.text = fragmentShader;
            }
            
        }
        
        if(name.find(".vert") != string::npos || name.find(".vs") != string::npos)
        {
            vector <string> ss = ofSplitString(name, ".");
            ofFile vShade(name);
            vertexShader = ofToString(vShade.readToBuffer());
            
            if(shaderCurrentlyEditing == 0) {
                //vertexShader = multilineTextInput.text;
                multilineTextInput.text = vertexShader;
            }
            
        }
            
        if(name.find("png")!= string::npos || name.find("jpeg")!= string::npos || name.find("jpg")!= string::npos)
        {
            
            // are we replacing a current image?
            
            ofRectangle imageRect(900, 0, 100, loadedTextures.size() * 100);
            if(imageRect.inside( dragInfo.position )) {
                
                int whichImage = (int) ( dragInfo.position.y / 100 );
                delete loadedTextures.at( whichImage );
                loadedTextures.at( whichImage ) = new ofImage();
                loadedTextures.at( whichImage )->loadImage(name);
                //loadedTextures.erase( loadedTextures.begin() + whichImage );
                
            } else {
            
                ofImage *img = new ofImage();
                img->loadImage(name);
                loadedTextures.push_back(img);
            }
        }
        
        ++it;
    }
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    
//    if(ofGetElapsedTimef() - debounceValue < 0.5) {
//        return;
//    }
//    
//    debounceValue = ofGetElapsedTimef();
    
	string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(kind == OFX_UI_WIDGET_TEXTINPUT)
    {
        
        string matInd = name.substr( name.find("uniform") + 7, name.size());
        name = name.substr( 0, name.find("uniform"));
        ofxUITextInput* uniformTI = (ofxUITextInput*) e.widget;
        
        vector<uniformObject>::iterator it = uniforms.begin();
        while( it != uniforms.end() ) {
            if( it->name == name ) {
                break;
            }
            ++it;
        }
        
        if(it->type == MAT4)
        {
            
            if(matInd == "00"){
                (*static_cast<ofMatrix4x4 *>(it->value))(0,0) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "10"){
                (*static_cast<ofMatrix4x4 *>(it->value))(1,0) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "20"){
                (*static_cast<ofMatrix4x4 *>(it->value))(2,0) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "30"){
                (*static_cast<ofMatrix4x4 *>(it->value))(3,0) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "10"){
                (*static_cast<ofMatrix4x4 *>(it->value))(0,1) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "11"){
                (*static_cast<ofMatrix4x4 *>(it->value))(1,1) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "12"){
                (*static_cast<ofMatrix4x4 *>(it->value))(2,1) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "13"){
                (*static_cast<ofMatrix4x4 *>(it->value))(3,1) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "20"){
                (*static_cast<ofMatrix4x4 *>(it->value))(0,2) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "21"){
                (*static_cast<ofMatrix4x4 *>(it->value))(1,2) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "22"){
                (*static_cast<ofMatrix4x4 *>(it->value))(2,2) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "23"){
                (*static_cast<ofMatrix4x4 *>(it->value))(3,2) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "30"){
                (*static_cast<ofMatrix4x4 *>(it->value))(0,3) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "31"){
                (*static_cast<ofMatrix4x4 *>(it->value))(1,3) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "32"){
                (*static_cast<ofMatrix4x4 *>(it->value))(2,3) = ::atof(uniformTI->getTextString().c_str());
            } else if(matInd == "33"){
                (*static_cast<ofMatrix4x4 *>(it->value))(3,3) = ::atof(uniformTI->getTextString().c_str());
            }            
        }
    }
    
    if(kind == OFX_UI_WIDGET_SLIDER_H)
    {
        ofxUISlider* uniformSlider = (ofxUISlider*) e.widget;
        string name = uniformSlider->getName();
        name = name.substr( 0, name.find("uniform") );
        
        vector<uniformObject>::iterator it = uniforms.begin();
        while( it != uniforms.end() ) {
            if( it->name == name ) {
                break;
            }
            ++it;
        }
        // did we find the slider
        if( it != uniforms.end() )
        {
            if(it->type == FLOAT) {
                static_cast<float *>(it->value)[0] = uniformSlider->getValue() * uniformSlider->getMax();
            }
            
            if(it->type == INT) {
                static_cast<int *>(it->value)[0] = uniformSlider->getValue()  * uniformSlider->getMax();
            }
            
            if(it->type == VEC2) {
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'X')
                {
                    static_cast<float *>(it->value)[0] = uniformSlider->getValue()  * uniformSlider->getMax();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Y')
                {
                    static_cast<float *>(it->value)[1] = uniformSlider->getValue() * uniformSlider->getMax();
                }
            }
            
            if(it->type == VEC3) {
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'X')
                {
                    static_cast<float *>(it->value)[0] = uniformSlider->getValue() * uniformSlider->getMax();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Y')
                {
                    static_cast<float *>(it->value)[1] = uniformSlider->getValue() * uniformSlider->getMax();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Z')
                {
                    static_cast<float *>(it->value)[2] = uniformSlider->getValue() * uniformSlider->getMax();
                }
            }
            
            if(it->type == VEC4) {
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'X')
                {
                    static_cast<float *>(it->value)[0] = uniformSlider->getValue() * uniformSlider->getMax();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Y')
                {
                    static_cast<float *>(it->value)[1] = uniformSlider->getValue() * uniformSlider->getMax();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Z')
                {
                    static_cast<float *>(it->value)[2] = uniformSlider->getValue() * uniformSlider->getMax();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'W')
                {
                    static_cast<float *>(it->value)[3] = uniformSlider->getValue() * uniformSlider->getMax();
                }
            }
            
            if(it->type == TEXTURE)
            {
                static_cast<int *>(it->value)[0] = static_cast<int>(uniformSlider->getValue() * uniformSlider->getMax());
                cout << static_cast<int *>(it->value)[0] << endl;
            }
        }
    }
    
    if(kind == OFX_UI_WIDGET_LABELBUTTON && e.type == MOUSE_RELEASED)
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;

        if( name == "vertex")
        {
            cout << multilineTextInput.text << endl;
            
            //fragmentShader = multilineTextInput.text;
            shaderCurrentlyEditing = 0;
            multilineTextInput.clear();
            multilineTextInput.text = vertexShader;
        }
        if( name == "fragment")
        {
            
            cout << multilineTextInput.text << endl;
            
            //vertexShader = multilineTextInput.text;
            shaderCurrentlyEditing = 1;
            multilineTextInput.clear();
            multilineTextInput.text = fragmentShader;
        }
        if( name == "compile")
        {
            
            cout << multilineTextInput.text << endl;
            dynamic_pointer_cast<shaderLog>(shaderResultLogger)->logs = "";
            
            if(shaderCurrentlyEditing == 0) {
                vertexShader = multilineTextInput.text;
            }
            
            if(shaderCurrentlyEditing == 1) {
                fragmentShader = multilineTextInput.text;
            }
            
            shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
            shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
            
            shader.linkProgram();
            
            output = "";
            output = dynamic_pointer_cast<shaderLog>(shaderResultLogger)->logs;
        }
        if( name == "cube")
        {
            modelType = CUBE;
        }
        if( name == "sphere")
        {
            modelType = SPHERE;
        }
        if( name == "plane")
        {
            modelType = PLANE;
        }
        if( name == "Add Uniform")
        {
            uniformGUI->setVisible(true);
            ofxUITextInput* uti = (ofxUITextInput*) uniformGUI->getWidget("Uniform Name");
            uti->setTextString("Uniform Name");
            
            ofxUIDropDownList* uddl = (ofxUIDropDownList*) uniformGUI->getWidget("Type");
        }
    }
}

void testApp::uniformGuiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
	int kind = e.widget->getKind();

    
    if(name == "Set Texture Name")
    {
        
    }
    if(name == "Create This Uniform" && e.type == MOUSE_RELEASED)
    {
        ofxUIDropDownList* uddl = (ofxUIDropDownList*) uniformGUI->getWidget("Type");
        ofxUITextInput* uti = (ofxUITextInput*) uniformGUI->getWidget("Uniform Name");
        string selectedName = uddl->getSelected()[0]->getName();
        
        uniformObject u;

        if( selectedName == "int") {
            u.type = INT;
            u.name = uti->getLabel()->getLabel();
            u.value = new int();
            gui->addSlider(u.name + "uniform", -500.0, 500.0, 0.0);
        }else if( selectedName == "float") {
            u.type = FLOAT;
            u.name = uti->getLabel()->getLabel();
            u.value = new float();
            gui->addSlider(u.name + "uniform", -500.0, 500.0, 0.0);
        }else if( selectedName == "vec2") {
            u.type = VEC2;
            u.name = uti->getLabel()->getLabel();
            u.value = new float[2];
            gui->addSlider(u.name + "uniformX", -500, 500, 0.0);
            gui->addSlider(u.name + "uniformY", -500, 500, 0.0);
        } else if( selectedName == "vec3") {
            u.type = VEC3;
            u.name = uti->getLabel()->getLabel();
            u.value = new float[3];
            gui->addSlider(u.name + "uniformX", -500.0, 500.0, 0.0);
            gui->addSlider(u.name + "uniformY", -500.0, 500.0, 0.0);
            gui->addSlider(u.name + "uniformZ", -500.0, 500.0, 0.0);
        } else if ( selectedName == "vec4") {
            u.type = VEC4;
            u.name = uti->getLabel()->getLabel();
            u.value = new float[4];
            gui->addSlider(u.name + "uniformX", -500.0, 500.0, 0.0);
            gui->addSlider(u.name + "uniformY", -500.0, 500.0, 0.0);
            gui->addSlider(u.name + "uniformZ", -500.0, 500.0, 0.0);
            gui->addSlider(u.name + "uniformW", -500.0, 500.0, 0.0);
        } else if( selectedName == "mat4" ) {
            
            u.type = MAT4;
            u.name = uti->getLabel()->getLabel();
            
            int twidth = 30, theight = 10;
            
            gui->addTextInput(u.name + "uniform00", "0.0", twidth, theight, 0, 0);
            
            gui->addWidgetEastOf( gui->addTextInput(u.name + "uniform10", "0.0", twidth, theight, 40, 0), u.name+"uniform00", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform20", "0.0", twidth, theight, 80, 0), u.name+"uniform10", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform30", "0.0", twidth, theight, 120, 0), u.name+"uniform20", true);
            
            gui->addTextInput(u.name + "uniform01", "0.0", twidth, theight, 0, 20);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform11", "0.0", twidth, theight, 40, 20), u.name+"uniform01", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform21", "0.0", twidth, theight, 80, 20), u.name+"uniform11", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform31", "0.0", twidth, theight, 120, 20), u.name+"uniform21", true);
            
            gui->addTextInput(u.name + "uniform02", "0.0", twidth, theight, 0, 40);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform12", "0.0", twidth, theight, 40, 40), u.name+"uniform02", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform22", "0.0", twidth, theight, 80, 40), u.name+"uniform12", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform32", "0.0", twidth, theight, 120, 40), u.name+"uniform22", true);
            
            gui->addTextInput(u.name + "uniform03", "0.0", twidth, theight, 0, 60);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform13", "0.0", twidth, theight, 40, 60), u.name+"uniform03", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform23", "0.0", twidth, theight, 80, 60), u.name+"uniform13", true);
            gui->addWidgetEastOf(gui->addTextInput(u.name + "uniform33", "0.0", twidth, theight, 120, 60), u.name+"uniform23", true);
            
            u.name = uti->getLabel()->getLabel();
            u.value = new float[16];
        } else if( selectedName == "texture" ) {
            u.type = TEXTURE;
            u.name = uti->getLabel()->getLabel();
            u.value = new int();
            //float w, float h, float x = 0, float y = 0
            gui->addSlider(u.name + "uniform", 0.0, 16.0, 0.0, 200, 18, 0, 0);
        }
        uniforms.push_back(u);
        removeOnNextUpdate = true;
        gui->autoSizeToFitWidgets();
        uniformGUI->setVisible(false);
        
        e.widget->setState(OFX_UI_STATE_NORMAL);
    }
    
    if(kind == OFX_UI_WIDGET_LABELBUTTON && e.type == MOUSE_RELEASED)
    {
    
        if( name == "Add Uniform")
        {
            uniformGUI->setVisible(false);
        }
    }

}
