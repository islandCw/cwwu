#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"
#include "IMService.hpp"

#include <iostream>
using namespace std;

/**
@brief    The cocos2d Application.

Private inheritance here hides part of interface from Director.
*/

class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();
    
    void enterLoginScene();
    void enterVoiceScene();
    void enterVoiceAndTextScene();
    
    //获取语音存储的路径
    string getTempDir();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  Called when the application moves to the background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  Called when the application reenters the foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();
    
    
    
    IMService imService;
};

#endif // _APP_DELEGATE_H_

