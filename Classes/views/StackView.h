#pragma once
#include "2d/CCNode.h"
#include <vector>
#include "CardView.h"
#include <functional>
/**
 * StackView
 * 底牌堆/备用牌堆UI，负责展示和管理堆区的所有卡牌。
 */


static const int STACK_WIDTH = 1080;
static const int STACK_HEIGHT = 580;


class StackView : public cocos2d::Node {
public:
    void moveCardToTop(CardView* cardView);
    static StackView* create();
    virtual bool init();
    void addCard(CardView* cardView);
    void removeCard(CardView* cardView);
    void setOnCardClickCallback(const std::function<void(int)>& callback);
    void layoutCards();
    // 获取顶部牌
    CardView* getTopCard() const {
        if (_cards.empty()) return nullptr;
        return _cards.back();
    }
    // 获取所有牌（如有需要）
    const std::vector<CardView*>& getCards() const { return _cards; }

    // 设置点击回调
    void setOnCardClicked(const std::function<void(CardView*)>& cb) { onCardClicked = cb; }
    // ... 其他 public 方法 ...
private:
    std::vector<CardView*> _cards;
    std::function<void(int)> _onCardClickCallback;
    std::function<void(CardView*)> onCardClicked;
}; 
