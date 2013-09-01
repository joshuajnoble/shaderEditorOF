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
    
    shaderCurrentlyEditing = 0;
    removeOnNextUpdate = addLabelOnNextUpdate = false;
    
    andaleMono.loadFont("font/Andale Mono.ttf", 12);
    multilineTextInput.setFont(andaleMono);
    
    vertexShader = "varying vec2 texcoord; \nvoid main() { \ntexcoord = gl_MultiTexCoord0.xy * 100.0; gl_Position = ftransform(); \n}";
    fragmentShader = "uniform sampler2DRect tex0; \nvarying vec2 texcoord;\nvoid main() { \ngl_FragColor = texture2DRect( tex0, texcoord.xy ); \n}";
}

//--------------------------------------------------------------
void testApp::update(){
    if(addLabelOnNextUpdate) {
        
        // make a text input at the bottom
        // add a dropdown next to it.
        // once the dropdown is set, remove dropdown, add slider
        
        gui->addTextInput("Uniform Name", "Uniform Name");
        vector<string> uniformTypes;
        uniformTypes.push_back("int");
        uniformTypes.push_back("float");
        uniformTypes.push_back("vec2");
        uniformTypes.push_back("vec3");
        uniformTypes.push_back("vec4");
        uniformTypes.push_back("mat4");
        uniformTypes.push_back("texture");
        gui->addDropDownList("Name Type", uniformTypes);
        gui->addLabelButton("Create This Uniform", false);
        gui->autoSizeToFitWidgets();
        addLabelOnNextUpdate = false;
        
    }
    
    if(removeOnNextUpdate)
    {
        
        ofxUIDropDownList* uddl = (ofxUIDropDownList*) gui->getWidget("Name Type");
        ofxUITextInput* uti = (ofxUITextInput*) gui->getWidget("Uniform Name");
        ofxUILabelButton* ubtn = (ofxUILabelButton*) gui->getWidget("Create This Uniform");
        
        gui->removeWidget(uti);
        gui->removeWidget(uddl);
        gui->removeWidget(ubtn);
        
        gui->autoSizeToFitWidgets();
        removeOnNextUpdate = false;
        
    }
    
        
}

//--------------------------------------------------------------
void testApp::draw(){
    
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
                    float *f = (float*) it->value;
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
        string name = *it;
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
    
    if(ofGetElapsedTimef() - debounceValue < 0.5) {
        return;
    }
    
    debounceValue = ofGetElapsedTimef();
    
	string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    cout << ofGetElapsedTimef() << endl;
    
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
                static_cast<float *>(it->value)[0] = uniformSlider->getValue();
            }
            
            if(it->type == INT) {
                static_cast<int *>(it->value)[0] = uniformSlider->getValue();
            }
            
            if(it->type == VEC2) {
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'X')
                {
                    static_cast<float *>(it->value)[0] = uniformSlider->getValue();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Y')
                {
                    static_cast<float *>(it->value)[1] = uniformSlider->getValue();
                }
            }
            
            if(it->type == VEC3) {
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'X')
                {
                    static_cast<float *>(it->value)[0] = uniformSlider->getValue();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Y')
                {
                    static_cast<float *>(it->value)[1] = uniformSlider->getValue();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Z')
                {
                    static_cast<float *>(it->value)[2] = uniformSlider->getValue();
                }
            }
            
            if(it->type == VEC4) {
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'X')
                {
                    static_cast<float *>(it->value)[0] = uniformSlider->getValue();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Y')
                {
                    static_cast<float *>(it->value)[1] = uniformSlider->getValue();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'Z')
                {
                    static_cast<float *>(it->value)[2] = uniformSlider->getValue();
                }
                if(uniformSlider->getName().at(uniformSlider->getName().size()-1) == 'W')
                {
                    static_cast<float *>(it->value)[3] = uniformSlider->getValue();
                }
            }
            
            if(it->type == TEXTURE)
            {
                static_cast<int *>(it->value)[0] = static_cast<int>(uniformSlider->getValue() * uniformSlider->getMax());
                cout << static_cast<int *>(it->value)[0] << endl;
            }
        }
    }
    
    if(kind == OFX_UI_WIDGET_LABELBUTTON)
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        if(name == "Set Texture Name")
        {
            
        }
        if(name == "Create This Uniform")
        {
            ofxUIDropDownList* uddl = (ofxUIDropDownList*) gui->getWidget("Name Type");
            ofxUITextInput* uti = (ofxUITextInput*) gui->getWidget("Uniform Name");
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
                u.value = new float[16];
            } else if( selectedName == "texture" ) {
                u.type = TEXTURE;
                u.name = uti->getLabel()->getLabel();
                u.value = new int();
                gui->addSlider(u.name + "uniform", 0.0, 16.0, 0.0);
            }
            uniforms.push_back(u);
            removeOnNextUpdate = true;
            gui->autoSizeToFitWidgets();
        }
        
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
            addLabelOnNextUpdate = true;
        }
    }
    
}
