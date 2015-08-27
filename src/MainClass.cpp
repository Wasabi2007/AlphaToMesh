//
// Created by jerry on 24.08.15.
//

#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <fstream>
#include <chrono>
#include "MainClass.hpp"

bool MainClass::mvpreload = false;
int MainClass::width = 1024;
int MainClass::height = 768;

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/*void TW_CALL initRims(void *data) {
    MainClass *aClass = (MainClass *) data;

    aClass->canStep = true;
    Rim r{aClass->alpha_limit, aClass->img.get(), aClass->errorMarginDegree};
    aClass->status = "Finde Rims";
    aClass->init(r);
    aClass->status = "Found Rims";
    aClass->WindowSize();
}

void TW_CALL loadImage(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->img = std::unique_ptr<imageStruct>(imageStruct::load(aClass->filename.c_str()));
    aClass->renderImage1 = std::make_unique<renderImage>(*aClass->img);
    aClass->rimsToRender.clear();
    aClass->meshToRender.clear();
    aClass->status = "Image Loaded";
    aClass->WindowSize();
}

void TW_CALL doStep(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->stepp = true;
}


void TW_CALL doAutoStep(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->autoStep = !aClass->autoStep;
}


void TW_CALL doSave(void *data) {
    MainClass *aClass = (MainClass *) data;
    aClass->Save();
}*/

/*void WindowSizeCB(GLFWwindow *_window, int width, int height) {
    //glViewport(0, 0, width, height);
    //std::cout << "Callback?" << std::endl;

    MainClass::width = width;
    MainClass::height = height;
    MainClass::mvpreload = true;

    // Send the new window size to AntTweakBar
    //TwWindowSize(width, height);
}*/

MainClass::MainClass(const string &filename,float alpha_limit, float errorMarginDegree)
: nanogui::Screen(Eigen::Vector2i(1024, 768), "Alpha to Mesh"),
        alpha_limit(alpha_limit),

                                                                                errorMarginDegree(
                                                                                        errorMarginDegree),
                                                                                filename(filename),
                                                                                          stepp(false),
                                                                                          canStep(true),
                                                                                          autoStep(false),
                                                                                          timeCount(0.f),
                                                                                          stepTime(1.f)
{
    using namespace nanogui;

    /*Window *window = new Window(this, "Button demo");
    window->setPosition(Vector2i(15, 15));
    window->setLayout(new GroupLayout());

    /* No need to store a pointer, the data structure will be automatically
       freed when the parent window is deleted */
    /*new Label(window, "Push buttons", "sans-bold");

    Button *b = new Button(window, "Plain button");
    b->setCallback([] { cout << "pushed!" << endl; });
    b = new Button(window, "Styled", ENTYPO_ICON_ROCKET);
    b->setBackgroundColor(Color(0, 0, 255, 60));
    b->setCallback([] { cout << "pushed!" << endl; });

    new Label(window, "Toggle buttons", "sans-bold");
    b = new Button(window, "Toggle me");
    b->setButtonFlags(Button::ToggleButton);
    b->setChangeCallback([](bool state) { cout << "Toggle button state: " << state << endl; });

    new Label(window, "Radio buttons", "sans-bold");
    b = new Button(window, "Radio button 1");
    b->setButtonFlags(Button::RadioButton);
    b = new Button(window, "Radio button 2");
    b->setButtonFlags(Button::RadioButton);

    new Label(window, "A tool palette", "sans-bold");
    Widget *tools = new Widget(window);
    tools->setLayout(new BoxLayout(BoxLayout::Orientation::Horizontal,BoxLayout::Alignment::Middle,0,6));

    b = new ToolButton(tools, ENTYPO_ICON_CLOUD);
    b = new ToolButton(tools, ENTYPO_ICON_FF);
    b = new ToolButton(tools, ENTYPO_ICON_COMPASS);
    b = new ToolButton(tools, ENTYPO_ICON_INSTALL);

    new Label(window, "Popup buttons", "sans-bold");
    PopupButton *popupBtn = new PopupButton(window, "Popup", ENTYPO_ICON_EXPORT);
    Popup *popup = popupBtn->popup();
    popup->setLayout(new GroupLayout());
    new Label(popup, "Arbitrary widgets can be placed here");
    new CheckBox(popup, "A check box");
    popupBtn = new PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
    popup = popupBtn->popup();
    popup->setLayout(new GroupLayout());
    new CheckBox(popup, "Another check box");

    window = new Window(this, "Basic widgets");
    window->setPosition(Vector2i(200, 15));
    window->setLayout(new GroupLayout());

    new Label(window, "Message dialog", "sans-bold");
    tools = new Widget(window);
    tools->setLayout(new BoxLayout(BoxLayout::Orientation::Horizontal, BoxLayout::Alignment::Middle, 0, 6));
    b = new Button(tools, "Info");
    b->setCallback([&] {
        auto dlg = new MessageDialog(this, MessageDialog::Information, "Title", "This is an information message");
        dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
    });
    b = new Button(tools, "Warn");
    b->setCallback([&] {
        auto dlg = new MessageDialog(this, MessageDialog::Warning, "Title", "This is a warning message");
        dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
    });
    b = new Button(tools, "Ask");
    b->setCallback([&] {
        auto dlg = new MessageDialog(this, MessageDialog::Warning, "Title", "This is a question message", "Yes", "No", true);
        dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
    });

    std::vector<std::pair<int, std::string>>
            icons = loadImageDirectory(mNVGContext, "icons");

    new Label(window, "Image panel & scroll panel", "sans-bold");
    PopupButton *imagePanelBtn = new PopupButton(window, "Image Panel");
    imagePanelBtn->setIcon(ENTYPO_ICON_FOLDER);
    popup = imagePanelBtn->popup();
    VScrollPanel *vscroll = new VScrollPanel(popup);
    ImagePanel *imgPanel = new ImagePanel(vscroll);
    imgPanel->setImageData(icons);
    popup->setFixedSize(Vector2i(245, 150));
    new Label(window, "Selected image", "sans-bold");
    auto img = new ImageView(window);
    img->setFixedSize(Vector2i(40, 40));

    new Label(window, "File dialog", "sans-bold");
    tools = new Widget(window);
    tools->setLayout(new BoxLayout(BoxLayout::Orientation::Horizontal,BoxLayout::Alignment::Middle,0,6));
    b = new Button(tools, "Open");
    b->setCallback([&] {
        cout << "File dialog result: " << file_dialog(
                { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, false) << endl;
    });
    b = new Button(tools, "Save");
    b->setCallback([&] {
        cout << "File dialog result: " << file_dialog(
                { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, true) << endl;
    });

    img->setImage(icons[0].first);
    imgPanel->setCallback([&, img, imgPanel, imagePanelBtn](int i) {
        img->setImage(imgPanel->images()[i].first); cout << "Selected item " << i << endl;
    });

    new Label(window, "Combo box", "sans-bold");
    new ComboBox(window, { "Combo box item 1", "Combo box item 2", "Combo box item 3"});
    new Label(window, "Check box", "sans-bold");
    CheckBox *cb = new CheckBox(window, "Flag 1",
                                [](bool state) { cout << "Check box 1 state: " << state << endl; }
    );
    cb->setChecked(true);
    new CheckBox(window, "Flag 2",
                 [](bool state) { cout << "Check box 2 state: " << state << endl; }
    );
    new Label(window, "Progress bar", "sans-bold");
    mProgress = new ProgressBar(window);

    new Label(window, "Slider and text box", "sans-bold");

    Widget *panel = new Widget(window);
    panel->setLayout(
            new BoxLayout(BoxLayout::Orientation::Horizontal, BoxLayout::Alignment::Middle, 0, 20));

    Slider *slider = new Slider(panel);
    slider->setValue(0.5f);
    slider->setFixedWidth(80);

    TextBox *textBox = new TextBox(panel);
    textBox->setFixedSize(Vector2i(60,25));
    textBox->setValue("50");
    textBox->setUnits("%");
    textBox->setFontSize(20);
    slider->setCallback([textBox](float value) {
        textBox->setValue(std::to_string((int) (value * 100)));
    });
    slider->setFinalCallback([&](float value) {
        cout << "Final slider value: " << (int) (value * 100) << endl;
    });

    window = new Window(this,"Misc. widgets");
    window->setPosition(Vector2i(425,15));
    window->setLayout(new GroupLayout());
    new Label(window,"Color wheel","sans-bold");
    new ColorWheel(window);

    window = new Window(this,"Minimalistic Widgets");
    window->setPosition(Vector2i(600,15));
    window->setLayout(new BoxLayout(BoxLayout::Orientation::Vertical,BoxLayout::Alignment::Minimum,5,10));

    Widget* gridPanel = new Widget(window);
    GridLayout* layout = new GridLayout(GridLayout::Orientation::Horizontal,2,0,1);
    layout->setColAlignment({GridLayout::Alignment::Middle,GridLayout::Alignment::Middle});
    gridPanel->setLayout(layout);

    for(int i=0;i<10;i++)
    {
        new Label(gridPanel,"Attribute","sans-bold");

        textBox = new TextBox(gridPanel);
        textBox->setEditable(true);
        textBox->setFixedSize(Vector2i(100,20));
        textBox->setValue("50");
        textBox->setUnits("%");
        textBox->setDefaultValue("0.0");
        textBox->setFontSize(16);
        textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
    }

    for(int i=0;i<3;i++)
    {
        new Label(gridPanel,"Checkboxes","sans-bold");

        cb = new CheckBox(gridPanel,"",[](bool state) {});
        cb->setFixedSize(Vector2i(18,18));
        cb->setFontSize(18);
        cb->setChecked(true);
    }

    for(int i=0;i<5;i++)
    {
        new Label(gridPanel,"Attribute","sans-bold");

        textBox = new TextBox(gridPanel);
        textBox->setEditable(true);
        textBox->setFixedSize(Vector2i(100,20));
        textBox->setValue("50");
        textBox->setUnits("%");
        textBox->setDefaultValue("0.0");
        textBox->setFontSize(16);
        textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
    }

    new Label(gridPanel,"Combobox","sans-bold");
    ComboBox* cobo = new ComboBox(gridPanel,{"Item 1","Item 2","Item 3"});
    cobo->setFontSize(16);
    cobo->setFixedSize(Vector2i(100,20));

    new Label(gridPanel,"Color Button","sans-bold");
    popupBtn = new PopupButton(gridPanel,"",0);
    popupBtn->setBackgroundColor(Color(255,120,0,255));
    popupBtn->setFontSize(16);
    popupBtn->setFixedSize(Vector2i(100,20));
    popup = popupBtn->popup();
    popup->setLayout(new GroupLayout());

    ColorWheel* colorwheel = new ColorWheel(popup);
    colorwheel->setColor(popupBtn->backgroundColor().block<3,1>(0,0));

    Button* colorBtn = new Button(popup,"Pick");
    colorBtn->setFixedSize(Vector2i(100,25));
    Vector3f c = colorwheel->color();
    colorBtn->setBackgroundColor(Color(c));

    PopupButton& popupBtnRef = *popupBtn;
    Button& colorBtnRef = *colorBtn;
    colorwheel->setCallback([&](const Vector3f &value) {
        colorBtnRef.setBackgroundColor(Color(value));
    });

    colorBtn->setChangeCallback([&](bool pushed) {
        if(pushed) {
            popupBtnRef.setBackgroundColor(colorBtnRef.backgroundColor());
            popupBtnRef.setPushed(false);
        }
    });*/
    Window *window = new Window(this, "Algo Controlle");
    window->setPosition(Vector2i(800, 15));
    window->setLayout(new GroupLayout());
    Button* b = new Button(window, "Open");
    b->setCallback([&] {
        this->filename = file_dialog(
                { {"png", "Portable Network Graphics"} , {"", "All Data"} }, false);

        //cout << this->filename << endl;

        this->img = std::unique_ptr<imageStruct>(imageStruct::load(this->filename.c_str()));
        assert(this->img);
        
        this->renderImage1 = make_unique<renderImage>(this->img.get());
        this->rimsToRender.clear();
        this->meshToRender.clear();
        this->status = "Image Loaded";
        this->WindowSize();
    });

    Label* label = new Label(window,"Alpha Limit","sans-bold");
    Slider *slider = new Slider(window);
    slider->setValue(0.5f);
    //slider->setFixedWidth(80);

    TextBox *textBox = new TextBox(window);
    textBox->setFixedSize(Vector2i(60,25));
    textBox->setFontSize(20);
    textBox->setValue("0.50");
    textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
    textBox->setEditable(false);

    slider->setCallback([this,textBox](float v){
        this->alpha_limit = v;
        auto s = std::to_string(v);
        auto index = s.find('.',0);
        textBox->setValue(s.substr(0,index+3));
    });

    label = new Label(window,"Simplification","sans-bold");
    slider = new Slider(window);


    textBox = new TextBox(window);
    textBox->setEditable(false);
    textBox->setFixedSize(Vector2i(60,25));
    textBox->setFontSize(20);
    textBox->setValue("0.00");



    slider->setValue(0.0f);
    //slider->setFixedWidth(80);
    slider->setCallback([this,textBox](float v){
        this->errorMarginDegree = v*100.f;
        auto s = std::to_string(this->errorMarginDegree);
        auto index = s.find('.',0);
        textBox->setValue(s.substr(0,index+3));
        //textBox->setValue(std::to_string(int(this->errorMarginDegree)));

    });

    b = new Button(window, "Find Rims");
    b->setCallback([&] {
        this->canStep = true;
        Rim r{this->alpha_limit, this->img.get(), this->errorMarginDegree};
        this->status = "Finde Rims";
        this->init(r);
        this->status = "Found Rims";
        this->WindowSize();
    });


    b = new Button(window, "Save");
    b->setCallback([&] {
        auto file = file_dialog( { {"obj", "Wavefront Object File"} }, true);

        ofstream out(file, ios::out);
        for (auto &r : meshToRender) {
            out << r;
        }
        out.flush();
        out.close();
    });

    window = new Window(this, "Play Controlle");
    window->setPosition(Vector2i(15, 15));
    window->setLayout(new GroupLayout());

    label = new Label(window,"Play Speed in S","sans-bold");
    slider = new Slider(window);


    textBox = new TextBox(window);
    textBox->setEditable(false);
    textBox->setFixedSize(Vector2i(60,25));
    textBox->setFontSize(20);
    textBox->setValue("0.10");



    slider->setValue(0.01f);
    //slider->setFixedWidth(80);
    slider->setCallback([this,textBox](float v){
        this->stepTime = v*10.f;
        auto s = std::to_string(this->stepTime);
        auto index = s.find('.',0);
        textBox->setValue(s.substr(0,index+3));
        //textBox->setValue(std::to_string(int(this->errorMarginDegree)));

    });

    b = new Button(window, "Stepp");
    b->setCallback([&] {
        this->stepp = true;

    });

    b = new Button(window, "Play");
    b->setButtonFlags(Button::ToggleButton);
    b->setCallback([&] {
        this->autoStep = true;
    });

    performLayout(mNVGContext);

    /* All NanoGUI widgets are initialized at this point. Now
       create an OpenGL shader to draw the main window contents.

       NanoGUI comes with a simple Eigen-based wrapper around OpenGL 3,
       which eliminates most of the tedious and error-prone shader and
       buffer object management.
    */

    mShader.init(
            /* An identifying name */
            "a_simple_shader",

            /* Vertex shader */
            "#version 330\n"
                    "uniform mat4 modelViewProj;\n"
                    "in vec3 position;\n"
                    "void main() {\n"
                    "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
                    "}",

            /* Fragment shader */
            "#version 330\n"
                    "out vec4 color;\n"
                    "uniform float intensity;\n"
                    "void main() {\n"
                    "    color = vec4(vec3(0.5), 1.0);\n"
                    "}"
    );

    MatrixXu indices(3, 2); /* Draw 2 triangles */
    indices.col(0) << 0, 1, 2;
    indices.col(1) << 2, 3, 0;

    MatrixXf positions(3, 4);
    positions.col(0) << -1, -1, 0;
    positions.col(1) <<  1, -1, 0;
    positions.col(2) <<  1,  1, 0;
    positions.col(3) << -1,  1, 0;

    mShader.bind();
    mShader.uploadIndices(indices);
    mShader.uploadAttrib("position", positions);
    //mShader.setUniform("intensity", 0.5f);
}

void MainClass::initTW(){
    //auto window = glfwGetCurrentContext();
    int width = 0;
    int height = 0;

    //glfwGetFramebufferSize(window,&width,&height);


    /*TwDefine(" GLOBAL fontscaling=3 ");
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(width, height);

    animationBar = TwNewBar("PlayControlle");
    TwDefine(" PlayControlle position='0 0' ");
    TwDefine(" PlayControlle size='500 300' ");
    TwAddButton(animationBar,"Step",doStep,this," label='Step' ");
    TwAddButton(animationBar,"AutoStep",doAutoStep,this," label='Auto Step' ");
    TwAddVarRW(animationBar, "speed", TW_TYPE_FLOAT, &stepTime,
               " label='Step speed' min=0 max=10 step=0.1");

    TwAddVarRO(animationBar,"Process",TW_TYPE_STDSTRING,&this->status,"");



    allgoBar = TwNewBar("AlgoControlle");
    TwDefine(" AlgoControlle position='900 0' ");
    TwDefine(" AlgoControlle size='500 400' ");
    TwCopyStdStringToClientFunc([](std::string& destinationClientString, const std::string& sourceLibraryString)
    {
        // Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
        destinationClientString = sourceLibraryString;
    });

    TwAddVarRW(allgoBar,"Filename",TW_TYPE_STDSTRING,&this->filename,"");
    TwAddButton(allgoBar,"Loadfile",loadImage,this," label='Loadfile' ");

    TwAddVarRW(allgoBar,"Alphaschwellwert",TW_TYPE_FLOAT, &alpha_limit," label='Alpha Limit' min=0 max=1 step=0.01");
    TwAddVarRW(allgoBar,"Simplification",TW_TYPE_FLOAT, &errorMarginDegree," label='Simplification' min=0 step=0.1");

    TwAddButton(allgoBar,"Find Rims",initRims,this," label='Find Rims' ");
    TwAddButton(allgoBar,"Save Data",doSave,this," label='Save Mesh' ");
*/
    //glfwSetFramebufferSizeCallback(window,WindowSizeCB);
}

void MainClass::init(Rim r) {
    rimsToRender.clear();
    meshToRender.clear();


    //std::cout << std::fixed << std::setprecision(1);
    /*for (unsigned y = 0; y < img->height; ++y) {
        for (unsigned x = 0; x < img->width; ++x) {
            std::cout << (img->getPixel(x, y).a>0.5?"#":(img->getPixel(x, y).a>0.25?"+":(img->getPixel(x, y).a>0.01?"-":" ")));
        }
        std::cout << std::endl;
    }*/

    auto rims = r.getSimpelRims();

    for (const auto &rim : rims) {
        rimsToRender.emplace_back(rim, long(img->width), long(img->height));
    }
    for (const auto &rim: rims) {
        /*cout << endl << "simple rim" << endl;
        for(auto& v : rim){
            cout<< "("<< v.x << "/"<< v.y << ")->";
        }*/
        //rimsToRender.emplace_back(rim,long(img->width),long(img->height));
        meshToRender.emplace_back(rim, long(img->width), long(img->height));
    }

    for (auto &rim : rimsToRender) {
        rim.setRenderVerts(true);
    }
    //glEnable(GL_ALPHA_TEST);
}

void MainClass::mainLoop(float dt) {
    glClearColor(0.4f, 0.6f, 0.9f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto window = glfwGetCurrentContext();
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window,&width,&height);

    if(MainClass::mvpreload || MainClass::width != width || MainClass::height != height){
        MainClass::width = width;
        MainClass::height = height;
        MainClass::mvpreload = false;
        WindowSize();
    }

    //glLineWidth(5.0f);
    //glPointSize(50.f);
    if(renderImage1)
        renderImage1->Render();

    for (auto &r : rimsToRender) {
        r.Render();
    }

    for (auto &r : meshToRender) {
        r.Render();
    }

    if (stepp) {
        stepp = false;
        canStep = false;
        for (auto &r : meshToRender) {
            canStep = canStep || r.Step();
        }

        if(canStep) {
            status = "Triangolation In progress";
        } else{
            status = "Finished";
        }
    }

    if(autoStep && timeCount >= stepTime){
        timeCount = 0.f;
        canStep = false;
        for (auto &r : meshToRender) {
            canStep = canStep || r.Step();
        }

        if(canStep) {
            status = "Triangolation In progress";
        } else{
            status = "Finished";
        }
    }


    timeCount += dt;
    //cout << "timeCount: " << timeCount << endl;
}

void MainClass::WindowSize() {
    //glViewport(0, 0, MainClass::width, MainClass::height);

    float wsize = float(img->width)/float(MainClass::width);
    float hsize = float(img->height)/float(MainClass::height);
    float size = fmax(wsize,hsize);

    mat4 mvp{1};
    mvp *= glm::ortho(0.f,float(MainClass::width)*size,0.f,float(MainClass::height)*size);
    //std::cout << "width: " << width << "height: " << height << std::endl;

    //std::cout << glm::to_string(mvp) << std::endl;

    renderRim::myshader.mvp = mvp;
    renderRim::myshader.reloadMVP();

    triangleMesh::myshader.mvp = mvp;
    triangleMesh::myshader.reloadMVP();

    triangleMesh::lineShader.mvp = mvp;
    triangleMesh::lineShader.reloadMVP();

    renderImage1->reloadMVP(mvp);

    // Send the new window size to AntTweakBar
    //TwWindowSize(width, height);
}

void MainClass::draw(NVGcontext *ctx) {
    /* Animate the scrollbar */
    //mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));
    static auto time = std::chrono::high_resolution_clock::now();
    static float dt = 0.f;

    auto t2 = std::chrono::high_resolution_clock::now();
    dt = float(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - time).count()) / 1000;
    mainLoop(dt);
    //        running = glWindow.renderFrame();
    //TwDraw();
    time = t2;
    //ctx->
    //mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

    /* Draw the user interface */
    Screen::draw(ctx);
}

void MainClass::drawContents() {
    using namespace nanogui;

    /* Draw the window contents using OpenGL */
    mShader.bind();

    Matrix4f mvp;
    mvp.setIdentity();
    mvp.topLeftCorner<3,3>() = Matrix3f(Eigen::AngleAxisf((float) glfwGetTime(),  Vector3f::UnitZ())) * 0.25f;

    mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

    mShader.setUniform("modelViewProj", mvp);

    /* Draw 2 triangles starting at index 0 */
    mShader.drawIndexed(GL_TRIANGLES, 0, 2);
}

void MainClass::Save() {
    ofstream out("mesh.obj", ios::out);
    for (auto &r : meshToRender) {
        out << r;
    }
    out.flush();
    out.close();
}

MainClass::~MainClass() {
    mShader.free();
}

