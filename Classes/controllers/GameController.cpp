#include "controllers/GameController.h"
#include "cocos2d.h"
#include "managers/UndoManager.h"
#include <algorithm>

USING_NS_CC;

GameController::GameController(GameView* view) : _gameView(view) {
    CCLOG("GameController initialized, setting callback for view=%p", _gameView);
    if (_gameView) {
        _gameView->setOnCardClickCallback([this](int cardId) {
            CCLOG("GameController: Card clicked, id=%d", cardId);
            onCardClicked(cardId);
        });
        _gameView->setOnUndoClickCallback([this]() {
            CCLOG("GameController: Undo clicked");
            onUndoClicked();
        });
    }
}

void GameController::startGame(const LevelConfig& config) {
    CCLOG("Starting game with %zu stack cards and %zu playfield cards",
          config.stackCards.size(), config.playfieldCards.size());
    
    _gameModel.clear();
    for (const auto& cardCfg : config.stackCards) {
        CardModel card;
        card.id = _gameModel.getNextCardId();
        card.face = cardCfg.face;
        card.suit = cardCfg.suit;
        card.isFaceUp = true;
        card.isRemoved = false;
        card.posX = cardCfg.position.x;
        card.posY = cardCfg.position.y;
        _gameModel.addCardToReserveStack(card);
    }
    for (const auto& cardCfg : config.playfieldCards) {
        CardModel card;
        card.id = _gameModel.getNextCardId();
        card.face = cardCfg.face;
        card.suit = cardCfg.suit;
        card.isFaceUp = true;
        card.isRemoved = false;
        card.posX = cardCfg.position.x;
        card.posY = cardCfg.position.y;
        _gameModel.addCardToPlayfield(card);
    }
    updateView();
}

void GameController::onCardClicked(int cardId) {
    auto playfieldView = _gameView->getPlayfieldView();
    auto baseStackView = _gameView->getBaseStackView();
    auto reserveStackView = _gameView->getReserveStackView();
    CCLOG("GameController: onCardClicked called for cardId=%d", cardId);
    // 查找卡牌来源
    CardView* cardView = nullptr;
    bool fromPlayfield = false, fromReserve = false, fromBase = false;

    // 检查主牌区
    cardView = findCardViewById(cardId, playfieldView);
    if (cardView) {
        fromPlayfield = true;
    } else {
        // // 检查备用牌堆
        // for (CardView* card : reserveStackView->getCards()) {
        //     if (card->getCardId() == cardId) {
        //         cardView = card;
        //         fromReserve = true;
        //         break;
        //     }
        // }
        // // 检查底牌堆
        // if (!cardView) {
        //     for (CardView* card : baseStackView->getCards()) {
        //         if (card->getCardId() == cardId) {
        //             cardView = card;
        //             fromBase = true;
        //             break;
        //         }
        //     }
        // }
        cardView = findCardViewById(cardId, reserveStackView);
        if (cardView) {
            fromReserve = true;
        } else {
            cardView = findCardViewById(cardId, baseStackView);
            if (cardView) {
                fromBase = true;
            }
        }
    }

    if (!cardView) {
        CCLOG("GameController: Card id=%d not found", cardId);
        return;
    }
    CCLOG("@@@@@@@@@@GameController: Card clicked, fromReserve=%d, fromBase=%d, fromPlayfield=%d", 
          fromReserve, fromBase, fromPlayfield);

    // 处理卡牌移动
    // if (fromPlayfield) {
    //     playfieldView->removeCard(cardView);
    //     baseStackView->addCard(cardView);
    //     _gameView->showUndoButton(true);
    //     _moveHistory.push({cardId, true, true});
    //     _undoManager.push({cardId, MoveType::PLAYFIELD_TO_BASE});
    //     CCLOG("GameController: Moved card id=%d from playfield to base stack", cardId);
    // } else if (fromReserve && cardView == reserveStackView->getTopCard()) {
    //     _gameView->onReserveCardClicked(cardView);
    //     _moveHistory.push({cardId, false, true});
    //     _undoManager.push({cardId, MoveType::RESERVE_TO_BASE});
    //     CCLOG("GameController: Moved card id=%d from reserve to base stack", cardId);
    // } else if (fromBase && cardView == baseStackView->getTopCard()) {
    //     CCLOG("GameController: Base stack top card id=%d clicked, no action", cardId);
    // } else {
    //     CCLOG("GameController: Card id=%d not top card, ignored", cardId);
    // }




    //稳定状态
//     if (fromPlayfield) {
//         auto topCard = baseStackView->getTopCard();
//         if (topCard && canMatch(cardId, topCard->getCardId())) {
//             playfieldView->removeCard(cardView);
//             baseStackView->addCard(cardView);
//             _gameView->showUndoButton(true);
//             _moveHistory.push({cardId, true, true});
//             _undoManager.push({cardId, MoveType::PLAYFIELD_TO_BASE});
//             CCLOG("GameController: Moved card id=%d from playfield to base stack", cardId);
//             _gameView->showUndoButton(true); // 应该显示撤销按钮
//         } else {
//             CCLOG("GameController: Card id=%d cannot match top card", cardId);
//         }
//     } else if (fromReserve && cardView == reserveStackView->getTopCard()) {
//         bool canMatchAny = false;
//         auto topCard = baseStackView->getTopCard();
//         if (topCard) {
//             for (auto card : playfieldView->getCards()) {
//                 if (canMatch(card->getCardId(), topCard->getCardId())) {
//                     canMatchAny = true;
//                     break;
//                 }
//             }
//         }
//         if (!canMatchAny) {
//             _gameView->onReserveCardClicked(cardView);
//             _moveHistory.push({cardId, false, true});
//             _undoManager.push({cardId, MoveType::RESERVE_TO_BASE});
//             CCLOG("GameController: Moved card id=%d from reserve to base stack", cardId);
//         } else {
//             CCLOG("GameController: Cannot draw from reserve, valid matches exist");
//         }
//     } else if (fromBase && cardView == baseStackView->getTopCard()) {
//         // 检查主牌区是否有可匹配的卡牌
//         bool matched = false;
//         for (auto playfieldCard : playfieldView->getCards()) {
//             if (canMatch(playfieldCard->getCardId(), cardId)) {
//                 // 移动主牌区卡牌到底牌堆
//                 playfieldView->removeCard(playfieldCard);
//                 baseStackView->addCard(playfieldCard);
//                 _gameView->showUndoButton(true);
//                 _moveHistory.push({playfieldCard->getCardId(), true, true});
//                 _undoManager.push({playfieldCard->getCardId(), MoveType::PLAYFIELD_TO_BASE});
//                 CCLOG("GameController: Moved card id=%d from playfield to base stack due to base top card id=%d", 
//                       playfieldCard->getCardId(), cardId);
//                 matched = true;
//                 break; // 一次匹配一个卡牌
//             }
//         }
//         if (!matched) {
//             CCLOG("GameController: Base stack top card id=%d clicked, no matching playfield card", cardId);
//         }
//     } else if (fromBase && cardView != baseStackView->getTopCard()) {
//         _gameView->onHandCardClicked(cardView);
//         _moveHistory.push({cardId, false, true});
//         _undoManager.push({cardId, MoveType::REORDER_BASE});
//         _gameView->showUndoButton(true);
//         CCLOG("GameController: Moved card id=%d to base stack top", cardId);
//     } else {
//         CCLOG("GameController: Card id=%d not top card, ignored", cardId);
//     }

    if (fromReserve && cardView == reserveStackView->getTopCard()) {
        bool canMatchAny = false;
        auto topCard = baseStackView->getTopCard();
        if (topCard) {
            for (auto card : playfieldView->getCards()) {
                if (canMatch(card->getCardId(), topCard->getCardId())) {
                    canMatchAny = true;
                    break;
                }
            }
        }
        if (!canMatchAny) {
            UndoRecord record;
            record.cardId = cardId;
            record.moveType = MoveType::RESERVE_TO_BASE;
            record.originalPos = cardView->getPosition();
            record.originalParent = 1; // 备用牌堆
            _gameView->onReserveCardClicked(cardView);
            _moveHistory.push({cardId, false, true});
            _undoManager.push(record);
            CCLOG("GameController: Moved card id=%d from reserve to base stack", cardId);
            _gameView->showUndoButton(true);
        }
    }
}




void GameController::onUndoClicked() {
    if (!_undoManager.canUndo()) {
        CCLOG("无动作可撤销");
        return;
    }

    UndoRecord record = _undoManager.undo();
    CCLOG("撤销动作: cardId=%d, type=%d", record.cardId, static_cast<int>(record.moveType));



    CardView* cardView = nullptr;
    if (record.originalParent == 0) {
        cardView = findCardViewById(record.cardId, _gameView->getPlayfieldView());
    } else if (record.originalParent == 1) {
        cardView = findCardViewById(record.cardId, _gameView->getBaseStackView());
    } else if (record.originalParent == 2) {
        cardView = findCardViewById(record.cardId, _gameView->getBaseStackView());
    }

    if (!cardView) {
        CCLOG("GameController: 未找到卡牌 id=%d 用于撤销", record.cardId);
        return;
    }
    if (!cardView) {
        CCLOG("GameController: 未找到卡牌 id=%d 用于撤销", record.cardId);
        return;
    }
    
    
cardView->retain(); // 防止被释放
    switch (record.moveType) {
        case MoveType::RESERVE_TO_BASE: {
            _gameModel.moveCardFromBaseToReserve(record.cardId);
            auto moveAction = MoveTo::create(0.3f, record.originalPos);
            auto callback = CallFunc::create([this, cardView]() {
                if (_gameView) {
                    _gameView->getBaseStackView()->removeCard(cardView);
                    _gameView->getReserveStackView()->addCard(cardView);
                    _gameView->getBaseStackView()->layoutCards();
                    _gameView->getReserveStackView()->layoutCards();
                }
                cardView->release(); // 在回调完成后释放
            });
            cardView->runAction(Sequence::create(moveAction, callback, nullptr));
            break;
        }
        case MoveType::REORDER_BASE: {
            _gameModel.restoreBaseStackOrder(record.cardId);
            auto moveAction = MoveTo::create(0.3f, record.originalPos);
            auto callback = CallFunc::create([this, cardView]() {
                if (_gameView) {
                    _gameView->getBaseStackView()->layoutCards();
                }
                cardView->release();
            });
            cardView->runAction(Sequence::create(moveAction, callback, nullptr));
            break;
        }
        case MoveType::PLAYFIELD_TO_BASE: {
            CardModel playfieldCard = _gameModel.getLastRemovedPlayfieldCard();
            if (playfieldCard.id != -1) {
                _gameModel.addCardToPlayfield(playfieldCard);
                cardView->setPosition(Vec2(playfieldCard.posX, playfieldCard.posY));
                auto moveAction = MoveTo::create(0.3f, Vec2(playfieldCard.posX, playfieldCard.posY));
                auto callback = CallFunc::create([this, cardView]() {
                    if (_gameView) {
                        _gameView->getBaseStackView()->removeCard(cardView);
                        _gameView->getPlayfieldView()->addCard(cardView);
                        _gameView->getPlayfieldView()->layoutCards();
                        _gameView->getBaseStackView()->layoutCards();
                    }
                    cardView->release();
                });
                cardView->runAction(Sequence::create(moveAction, callback, nullptr));
            }
            break;
        }
    }
    _gameView->showUndoButton(_undoManager.canUndo());
    updateView();
}

void GameController::updateView() {
    if (_gameView) {
        _gameView->showUndoButton(_undoManager.canUndo());
    }
}

bool GameController::canMatch(int playfieldCardId, int stackTopCardId) {
    CardModel playfieldCard = _gameModel.getCardById(playfieldCardId);
    CardModel stackCard = _gameModel.getCardById(stackTopCardId);
    if (playfieldCard.id == -1 || stackCard.id == -1) {
        return false;
    }
    return std::abs(playfieldCard.face - stackCard.face) == 1 &&
           playfieldCard.suit == stackCard.suit;
}

CardView* GameController::findCardViewById(int cardId, PlayfieldView* view) {
    for (CardView* card : view->getCards()) {
        if (card->getCardId() == cardId) {
            return card;
        }
    }
    return nullptr;
}

CardView* GameController::findCardViewById(int cardId, StackView* view) {
    for (CardView* card : view->getCards()) {
        if (card->getCardId() == cardId) {
            return card;
        }
    }
    return nullptr;
}