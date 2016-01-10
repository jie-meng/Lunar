#ifndef JUMPMANAGER_H
#define JUMPMANAGER_H

#include <list>
#include "util/base.hpp"

class JumpManager
{
public:
    SINGLETON(JumpManager)
    const std::pair<std::string, int> kPositionNone = std::pair<std::string, int>("", 0);
    JumpManager();
    std::pair<std::string, int> getForwardPosition();
    std::pair<std::string, int> getBackPosition();
    bool moveForward();
    bool moveBack();
    void recordPosition(
            const std::string& file,
            int line);
    void clear();
private:
    std::list< std::pair<std::string, int> >::iterator getCursorLeft();
    std::list< std::pair<std::string, int> >::iterator getCursorRight();
private:
    std::list< std::pair<std::string, int> > jump_list_;
    std::list< std::pair<std::string, int> >::iterator cursor_;
private:
    DISALLOW_COPY_AND_ASSIGN(JumpManager)
};

#endif // JUMPMANAGER_H
