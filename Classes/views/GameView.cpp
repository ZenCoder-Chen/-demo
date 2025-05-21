#include "GameView.h"
#include "PlayfieldView.h"
#include "StackView.h"
#include "CardView.h"
#include "configs/loaders/LevelConfigLoader.h"
#include "2d/CCLayer.h"
#include "2d/CCActionInterval.h"
#include "2d/CCActionInstant.h"
#include "base/CCDirector.h"
#include "ui/CocosGUI.h"
//#include "controllerS/GameController.h"
#include "controllers/GameController.h" // 添加包含

USING_NS_CC;

namespace {
    int nextCardId = 0;
}

bool GameView::init() {
    if (!Scene::init()) return false;

    this->setScale(1.0f);
    this->setRotation(0.0f);
    CCLOG("GameView scale=%f, rotation=%f", this->getScale(), this->getRotation());

    // 设置背景色
    auto bg = LayerColor::create(Color4B(34, 139, 34, 255));
    this->addChild(bg, -1);

    // 主牌区
    _playfieldView = PlayfieldView::create();
    if (!_playfieldView) {
        CCLOG("PlayfieldView::create() failed!");
        return false;
    }
    _playfieldView->setPosition(Vec2(0, 580));
    this->addChild(_playfieldView, 1);

    // 底牌堆
    _baseStackView = StackView::create();
    if (!_baseStackView) {
        CCLOG("BaseStackView::create() failed!");
        return false;
    }
    _baseStackView->setPosition(Vec2(450, 200));
    this->addChild(_baseStackView, 5);

    // 备用牌堆
    _reserveStackView = StackView::create();
    if (!_reserveStackView) {
        CCLOG("ReserveStackView::create() failed!");
        return false;
    }
    _reserveStackView->setPosition(Vec2(150, 200));
    this->addChild(_reserveStackView, 10);

    // 调试位置
    CCLOG("reserveStackView world pos: (%f, %f)", 
          _reserveStackView->convertToWorldSpace(Vec2(0, 0)).x, 
          _reserveStackView->convertToWorldSpace(Vec2(0, 0)).y);
    CCLOG("baseStackView world pos: (%f, %f)", 
          _baseStackView->convertToWorldSpace(Vec2(0, 0)).x, 
          _baseStackView->convertToWorldSpace(Vec2(0, 0)).y);

    _controller = new GameController(this);
    if (!_controller) {
        CCLOG("GameView: Failed to create GameController");
        return false;
    }

    // 设置点击回调
    setOnCardClickCallback([this](int cardId) {
       CCLOG("￥￥￥￥￥￥GameView: Card click received, id=%d", cardId);
       if (_controller) {
           _controller->onCardClicked(cardId);
       }
    });

    // 初始化控制器
    _controller = new GameController(this);
    if (!_controller) {
       CCLOG("GameView: Failed to create GameController");
       return false;
    }

    // 撤销按钮
    _undoButton = ui::Button::create("button_undo_normal.png", "button_undo_pressed.png");
    if (_undoButton) {
        CCLOG("回退按钮创建成功，内容大小：%f x %f", 
          _undoButton->getContentSize().width, _undoButton->getContentSize().height);
        _undoButton->addClickEventListener([this](Ref*) {
            //CCLOG("撤销按钮被点击");
            if (_onUndoClickCallback) _onUndoClickCallback();
        });
        _undoButton->setPosition(Vec2(900, 200));
        this->addChild(_undoButton, 20);
        _undoButton->setVisible(true);
    }else {
        CCLOG("回退按钮创建失败，检查资源：button_undo_normal.png, button_undo_pressed.png");
    }

    // 加载关卡配置
    LevelConfig level = LevelConfigLoader::loadFromFile("level1.json");
    _controller->startGame(level);

    // 主牌区卡牌
    for (const auto& cardCfg : level.playfieldCards) {
        auto card = CardView::create(cardCfg.face, cardCfg.suit, true);
        if (card) {
            card->setCardId(nextCardId++);
            card->setPosition(cardCfg.position);
            card->setOnClickCallback([this](int cardId) {
                CCLOG("GameView: Forwarding playfield card click, id=%d", cardId);
                if (_onCardClickCallback) {
                    _onCardClickCallback(cardId);
                }
            });
            _playfieldView->addCard(card);
            CCLOG("Playfield card: face=%d, suit=%d, pos=(%.1f,%.1f)", 
                  cardCfg.face, cardCfg.suit, cardCfg.position.x, cardCfg.position.y);
        }
    }

    // 底牌堆卡牌
    for (const auto& cardCfg : level.stackCards) {
        auto card = CardView::create(cardCfg.face, cardCfg.suit, true);
        if (card) {
            card->setCardId(nextCardId++);
            card->setPosition(cardCfg.position);
            card->setOnClickCallback([this](int cardId) {
                CCLOG("底牌堆卡片被点击了，它的id=%d", cardId);
                if (_onCardClickCallback) {
                    _onCardClickCallback(cardId);
                }
            });
            //_baseStackView->addCard(card);
            _reserveStackView->addCard(card);
            CCLOG("Added card to base stack: id=%d", card->getCardId());
        }
    }

    // 备用牌堆卡牌
    std::vector<CardConfig> reserveCards = {
        {3, 2, Vec2(150, 0)}, // 红桃 3
        // {4, 1, Vec2(150, 0)}, // 方块 4
    };
    for (const auto& cardCfg : reserveCards) {
        auto card = CardView::create(cardCfg.face, cardCfg.suit, true);
        if (card) {
            card->setCardId(nextCardId++);
            card->setPosition(cardCfg.position);
            card->setOnClickCallback([this](int cardId) {
                CCLOG("GameView: Forwarding reserve stack card click, id=%d", cardId);
                if (_onCardClickCallback) {
                    _onCardClickCallback(cardId);
                }
            });
            //_reserveStackView->addCard(card);
            _baseStackView->addCard(card);
            CCLOG("Added card to reserve stack: id=%d", card->getCardId());
        }
    }

    CCLOG("GameView create chenggong!");
    showUndoButton(true); // 临时强制显示按钮
    return true;
}

void GameView::setOnCardClickCallback(const std::function<void(int)>& callback) {
    _onCardClickCallback = callback;
    if (_playfieldView) {
        _playfieldView->setOnCardClickCallback([this](int cardId) {
            CCLOG("GameView: Forwarding playfield card click, id=%d", cardId);
            if (_onCardClickCallback) {
                _onCardClickCallback(cardId);
            }
        });
    }
    if (_baseStackView) {
        _baseStackView->setOnCardClickCallback([this](int cardId) {
            CCLOG("GameView: Forwarding base stack card click, id=%d", cardId);
            if (_onCardClickCallback) {
                _onCardClickCallback(cardId);
            }
        });
    }
    if (_reserveStackView) {
        _reserveStackView->setOnCardClickCallback([this](int cardId) {
            CCLOG("GameView: Forwarding reserve stack card click, id=%d", cardId);
            if (_onCardClickCallback) {
                _onCardClickCallback(cardId);
            }
        });
    }
}

void GameView::setOnUndoClickCallback(const std::function<void()>& callback) {
    _onUndoClickCallback = callback;
    CCLOG("撤回按钮被调用了");
}

void GameView::showUndoButton(bool show) {
    if (_undoButton) {
        _undoButton->setVisible(show);
        CCLOG("撤回按钮显示");
    }
}

void GameView::addCardToPlayfield(CardView* cardView) {
    if (!cardView || !_playfieldView) return;
    _playfieldView->addCard(cardView);
    CCLOG("GameView: Added playfield card, id=%d, pos=(%f, %f)", cardView->getCardId(), cardView->getPosition().x, cardView->getPosition().y);
}

void GameView::addCardToStack(CardView* cardView) {
    if (!cardView || !_baseStackView) return;
    Vec2 targetPos = _baseStackView->getTopCard() ? _baseStackView->getTopCard()->getPosition() : Vec2(150, 0);
    cardView->retain();
    auto moveAction = MoveTo::create(0.3f, targetPos);
    auto callback = CallFunc::create([this, cardView]() {
        if (_baseStackView) {
            _baseStackView->addCard(cardView);
            _baseStackView->layoutCards();
        }
        cardView->release();
    });
    cardView->runAction(Sequence::create(moveAction, callback, nullptr));
    CCLOG("GameView: Added base stack card, id=%d, pos=(%f, %f)", cardView->getCardId(), targetPos.x, targetPos.y);
}

void GameView::onHandCardClicked(CardView* cardView) {
    if (!cardView || !_baseStackView) return;
    auto topCard = _baseStackView->getTopCard();
    if (!topCard || cardView == topCard) return;
    const Vec2 targetPos = topCard->getPosition();
    cardView->retain();
    auto moveAction = MoveTo::create(0.3f, targetPos);
    auto callback = CallFunc::create([this, cardView]() {
        if (_baseStackView) {
            _baseStackView->moveCardToTop(cardView);
            _baseStackView->layoutCards();
        }
        cardView->release();
    });
    cardView->runAction(Sequence::create(moveAction, callback, nullptr));
}

void GameView::onReserveCardClicked(CardView* cardView) {
    if (!cardView || !_baseStackView || !_reserveStackView) return;
    Vec2 targetPos;
    auto topCard = _baseStackView->getTopCard();
    if (topCard) {
        targetPos = topCard->getPosition();
    } else {
        targetPos = Vec2(150, 0);
    }
    cardView->retain();
    auto moveAction = MoveTo::create(0.3f, targetPos);
    auto callback = CallFunc::create([this, cardView]() {
        if (_reserveStackView && _baseStackView) {
            _reserveStackView->removeCard(cardView);
            _baseStackView->addCard(cardView);
            _reserveStackView->layoutCards();
            _baseStackView->layoutCards();
        }
        cardView->release();
    });
    cardView->runAction(Sequence::create(moveAction, callback, nullptr));
}