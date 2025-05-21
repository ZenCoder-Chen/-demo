#include "GameScene.h"
USING_NS_CC;

GameScene* GameScene::create() {
    GameScene* ret = new (std::nothrow) GameScene();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

// bool GameScene::init() {
//     if (!Scene::init()) return false;
//     auto gameView = GameView::create();
//     this->addChild(gameView);
//     return true;
// }

bool GameScene::init() {
    if (!Scene::init()) return false;
    auto gameView = GameView::create();
    //CCLOG("Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!");
    if (!gameView) {
        CCLOG("GameView create failed!");
        return false;
    }else{
        CCLOG("GameView create chenggong!");
    }
    this->addChild(gameView);
    return true;
}