#include "AppDelegate.h"
#include "LoginScene.hpp"
#include "VoiceScene.hpp"
#include "VoiceAndTextScene.hpp"

#include "SimpleAudioEngine.h"

USING_NS_CC;
#include <string>
using namespace std;

#include"AudioEngine.h"
using namespace cocos2d::experimental;

//static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate* g_app = NULL;
extern string g_roomName;
extern string g_userID;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
    //YOUME
    //YOUME
    //要找合适的时机清理历史记录
    /*!
     *  清理本地私聊历史记录
     *  @param chatType     :私聊消息、房间消息
     *  @param time          Unix timestamp,精确到秒，表示删除这个时间点之前的所有历史记录
     *  @return 错误码
     */
    IMService::Inst()->GetMessageManager()->DeleteHistoryMessage(ChatType_PrivateChat,time(NULL) );
    IMService::Inst()->GetMessageManager()->DeleteHistoryMessage(ChatType_RoomChat,time(NULL) );
    
    //资源清理
    CocosDenshion::SimpleAudioEngine::end();
    AudioEngine::end();
    
    
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    
    g_app = this;
    
    //YOUME
    //调用YoumeIM 的初始化，并设置回调代理
    imService.start();
    imService.strTempDir = getTempDir();
    
    //YOUME
    //设置语音缓存目录
	IMService::Inst()->SetAudioCacheDir(UTF8TOXString2( imService.strTempDir.c_str()).c_str());
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {


#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("MyCppGame", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("MyCppGame");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
//    if (frameSize.height > mediumResolutionSize.height)
//    {        
//        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
//    }
//    // if the frame's height is larger than the height of small size.
//    else if (frameSize.height > smallResolutionSize.height)
//    {        
//        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
//    }
//    // if the frame's height is smaller than the height of medium size.
//    else
//    {        
//        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
//    }

    register_all_packages();
    
    //初始化一个房间ID和用户ID
    g_roomName = "123";
    //随机生成一个用户ID
    auto curTime = time(0);
    int userID = curTime % 1000;
    stringstream ss;
    ss<<"u"<<userID;
    g_userID = ss.str();


    // create a scene. it's an autorelease object
    auto scene = LoginScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

void AppDelegate::enterLoginScene(){
    auto director = Director::getInstance();

    auto scene = LoginScene::createScene();
    
    // run
    director->replaceScene(scene);
    
	IMService::Inst()->GetChatRoomManager()->LeaveChatRoom(UTF8TOXString2( g_roomName.c_str()).c_str());
    
    IMService::Inst()->Logout();
    
}

void AppDelegate::enterVoiceScene(){
    auto director = Director::getInstance();

    auto scene = VoiceScene::createScene();
    
    // run
    director->replaceScene(scene);
    
}

void AppDelegate::enterVoiceAndTextScene(){
    auto director = Director::getInstance();

    auto scene = VoiceAndTextScene::createScene();
    
    // run
    director->replaceScene(scene);
}


string AppDelegate::getTempDir(){
    FileUtils* f = FileUtils::getInstance();
    std::string path = f->getWritablePath();
    std::string strTemp = path + "temp/";
    
    std::string fullPath = f->fullPathForFilename( strTemp );
    return fullPath;
}



// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    //YOUME
    //进入后台
    IMService::Inst()->OnPause(false);

    // if you use SimpleAudioEngine, it must be paused
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    //YOUME
    //进入前台
    IMService::Inst()->OnResume();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
