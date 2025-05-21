#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <stack>
#include "cocos2d.h"

enum class MoveType {
    RESERVE_TO_BASE,
    REORDER_BASE,
    PLAYFIELD_TO_BASE
};

struct UndoRecord {
    int cardId;
    MoveType moveType;
    cocos2d::Vec2 originalPos;
    int originalParent; // 0: Playfield, 1: Reserve, 2: Base
};

class UndoManager {
public:
    void recordMove(const UndoRecord& record);
    bool canUndo() const;
    UndoRecord undo();
    void push(const UndoRecord& record);

private:
    std::stack<UndoRecord> undoStack;
};

#endif // UNDO_MANAGER_H