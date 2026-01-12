#include "agent.h"

Agent::Agent(Agent *parent)
{
    this->parent = parent;
}

QString AgentInfo::getName() const
{
    return name;
}

Agent *Agent::getChild(int row)
{
    if (row < 0 || row >= children.size())
    {
        return nullptr;
    }
    return children.at(row).get();
}

qint64 Agent::childCount() const
{
    return children.size();
}

qint64 Agent::columnCount() const
{
    int count = 0;

    // Name
    count++;

    // Description
    count++;

    return count;
}

QString AgentInfo::getDescription() const
{
    return description;
}

qint64 Agent::getRow() const
{
    if (!parent)
    {
        return 0;
    }
    else
    {
        const auto &container = parent->children;
        auto result = std::find_if(container.cbegin(), container.cend(),
            [&](const std::unique_ptr<Agent> &item)
            {
                if (item.get() == this)
                {
                    return true;
                }
                return false;
            }
        );
        if (result != container.cend())
        {
            return std::distance(container.cbegin(), result);
        }
        else
        {
            Q_ASSERT(false);
            return -1;
        }
    }
}

Agent *Agent::getParent()
{
    return parent;
}

Agent *Agent::find(const QString &name)
{
    for(const auto &child : children)
    {
        if (child->getName() == name)
        {
            return child.get();
        }
    }
    return nullptr;
}

void AgentInfo::setName(const QString &value)
{
    name = value;
}


void Agent::setParent(Agent *value)
{
    parent = value;
}

void AgentInfo::setDescription(const QString &value)
{
    description = value;
}

void Agent::addChild(std::unique_ptr<Agent> item)
{
    if (item)
    {
        item->setParent(this);
        children.push_back(std::move(item));
    }
}

void Agent::removeChild(int row)
{
    if (row < 0 || row >= children.size())
    {
        return;
    }
    children.erase(children.cbegin() + row);
}
