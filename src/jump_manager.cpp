#include "jump_manager.h"
#include "lunarcommon.h"

using namespace std;
using namespace util;

JumpManager::JumpManager()
{
    jump_list_.push_back(kPositionNone);
    cursor_ = jump_list_.begin();
}

std::pair<std::string, int> JumpManager::getForwardPosition()
{
    auto it = getCursorRight();
    return it != jump_list_.end() ? *it : kPositionNone;
}

std::pair<std::string, int> JumpManager::getBackPosition()
{
    return cursor_ != jump_list_.begin() ? *getCursorLeft() : kPositionNone;
}

void JumpManager::moveForward()
{
    auto it = getCursorRight();
    if (it != jump_list_.end())
    {
        auto it_next = it;
        ++it_next;
        if (it_next != jump_list_.end())
        {
            jump_list_.erase(cursor_);
            cursor_ = jump_list_.insert(it_next, kPositionNone);
        }
    }
}

void JumpManager::moveBack()
{
    if (cursor_ != jump_list_.begin())
    {
        auto it = getCursorLeft();
        if (it != jump_list_.begin())
        {
            jump_list_.erase(cursor_);
            cursor_ = jump_list_.insert(it, kPositionNone);
        }
    }
}

void JumpManager::recordPosition(
        const std::string& file,
        int line)
{
    while (getCursorRight() != jump_list_.end())
    {
        jump_list_.pop_back();
        //update it to prevent pop make it lose reference
    }
    jump_list_.insert(cursor_, std::pair<string, int>(file, line));
    if (jump_list_.size() > 30)
        jump_list_.erase(jump_list_.begin());
}

void JumpManager::clear()
{
    jump_list_.clear();
    jump_list_.push_back(kPositionNone);
    cursor_ = jump_list_.begin();
}

std::list< std::pair<std::string, int> >::iterator JumpManager::getCursorLeft()
{
    auto it = cursor_;
    return --it;
}

std::list< std::pair<std::string, int> >::iterator JumpManager::getCursorRight()
{
    auto it = cursor_;
    return ++it;
}
