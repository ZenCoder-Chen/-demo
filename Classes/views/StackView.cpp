#include "StackView.h"
#include "CardView.h"
#include <algorithm>
#include "controllers/GameController.h"
USING_NS_CC;

StackView* StackView::create() {
    StackView* ret = new (std::nothrow) StackView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool StackView::init() {
    if (!Node::init()) return false;
    this->setContentSize(Size(STACK_WIDTH, STACK_HEIGHT));
    // 移除硬编码位置
    //this->setPosition(Vec2(0, 0));
    return true;
}

// void StackView::addCard(CardView* cardView) {
//     if (!cardView) return;
//     this->addChild(cardView);
//     _cards.push_back(cardView);
//     cardView->setOnClickCallback([this](int cardId) {
//         if (_cards.back()->getCardId() == cardId) { // 仅顶部卡牌可点击
//             CCLOG("中文StackView: Top card clicked, id=%d", cardId);
//             if (_onCardClickCallback) {
//                 _onCardClickCallback(cardId);
//             }
//         }
//     });
//     layoutCards();
//     CCLOG("中文StackView: Added card, id=%d, pos=(%f, %f)", cardView->getCardId(), cardView->getPosition().x, cardView->getPosition().y);
// }

void StackView::addCard(CardView* card) {
    if (!card) return;
    card->retain(); // 保留引用
    _cards.push_back(card);
    this->addChild(card);
    layoutCards();
    CCLOG("StackView: 添加卡牌, id=%d, pos=(%f, %f), refCount=%d", 
          card->getCardId(), card->getPosition().x, card->getPosition().y, card->getReferenceCount());card->release(); // 添加完成后释放
}




// void StackView::removeCard(CardView* cardView) {
//     this->removeChild(cardView);
//     _cards.erase(std::remove(_cards.begin(), _cards.end(), cardView), _cards.end());
//     layoutCards();
// }

void StackView::removeCard(CardView* card) {
    if (!card) return;
    auto it = std::find(_cards.begin(), _cards.end(), card);
    if (it != _cards.end()) {
        card->retain(); // 保留引用
        _cards.erase(it);
        card->removeFromParentAndCleanup(false); // 避免销毁
        CCLOG("StackView: Removed card id=%d", card->getCardId());
        card->release(); // 在确认不再使用后释放
    }
}


void StackView::setOnCardClickCallback(const std::function<void(int)>& callback) {
    _onCardClickCallback = callback;
    for (auto card : _cards) {
        card->setOnClickCallback([this](int cardId) {
            if (_cards.back()->getCardId() == cardId) { // 仅顶部卡牌可点击
                CCLOG("StackView: Top card clicked, id=%d", cardId);
                if (_onCardClickCallback) {
                    _onCardClickCallback(cardId);
                }
            }
        });
    }
}

void StackView::layoutCards() {
    float x = 150, y = 0;
    for (size_t i = 0; i < _cards.size(); ++i) {
        _cards[i]->setPosition(Vec2(x, y));
        _cards[i]->setLocalZOrder(static_cast<int>(i));
        //_cards[i]->setVisible(i == _cards.size() - 1); // 仅顶部卡牌可见
        _cards[i]->setVisible(i);//测试是否全部可见
        CCLOG("StackView: Laid out card id=%d, pos=(%f, %f), visible=%d", 
              _cards[i]->getCardId(), x, y, _cards[i]->isVisible());
    }
}

void StackView::moveCardToTop(CardView* cardView) {
    auto it = std::find(_cards.begin(), _cards.end(), cardView);
    if (it != _cards.end()) {
        std::iter_swap(it, _cards.end() - 1);
        layoutCards();
    }
}