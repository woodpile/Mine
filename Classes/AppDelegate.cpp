#include "AppDelegate.h"
#include "GameMenu.h"

#include "config/Config.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("Mine");
        director->setOpenGLView(glview);
        glview->setFrameSize(320, 480);
    }

    //设置屏幕设计尺寸
    glview->setDesignResolutionSize(640, 1136, ResolutionPolicy::NO_BORDER);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    log("visibleSize: %f, %f.", visibleSize.width, visibleSize.height);
    log("origin: %f, %f.", origin.x, origin.y);
    auto ws = Director::getInstance()->getWinSize();
    log("win: %f, %f.", ws.width, ws.height);
    auto gs = Director::getInstance()->getOpenGLView()->getFrameSize();
    log("gs: %f, %f.", gs.width, gs.height);


    //load config
    Config::init();

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = GameMenu::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
