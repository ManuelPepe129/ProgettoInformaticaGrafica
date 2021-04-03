#include "octree.h"


void Octree::calculateBounds(BoundingRegion* out, Octree::Octant octant, BoundingRegion parentRegion) {
    glm::vec3 center = parentRegion.calculateCenter();
    if (octant == Octant::O1) {
        out = new BoundingRegion(center, parentRegion.max);
    }
    else if (octant == Octant::O2) {
        out = new BoundingRegion(glm::vec3(parentRegion.min.x, center.y, center.z), glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z));
    }
    else if (octant == Octant::O3) {
        out = new BoundingRegion(glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z), glm::vec3(center.x, center.y, parentRegion.max.z));
    }
    else if (octant == Octant::O4) {
        out = new BoundingRegion(glm::vec3(center.x, parentRegion.min.y, center.z), glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z));
    }
    else if (octant == Octant::O5) {
        out = new BoundingRegion(glm::vec3(center.x, center.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z));
    }
    else if (octant == Octant::O6) {
        out = new BoundingRegion(glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z), glm::vec3(center.x, parentRegion.max.y, center.z));
    }
    else if (octant == Octant::O7) {
        out = new BoundingRegion(parentRegion.min, center);
    }
    else if (octant == Octant::O8) {
        out = new BoundingRegion(glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, center.y, center.z));
    }
}

Octree::Node::Node() 
    : region(BoundTypes::AABB)
{ }

Octree::Node::Node(BoundingRegion bounds) 
    : region(bounds)
{ }

Octree::Node::Node(BoundingRegion bounds, std::vector<BoundingRegion> objectList) 
    : region(bounds)
{
    objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::Node::addToPending(RigidBody* instance, trie::Trie<Model*> models)
{
    // get all model bounding regions
    for (BoundingRegion br : models[instance->modelId]->boundingRegions)
    {
        br.instance = instance;
        br.transform();
        queue.push(br);
    }
}

Octree::Node::~Node()
{
    //delete[] children;
}

void Octree::Node::build()
{
    /*
    *   termination conditions
    *   - 1 or no objects (leaf node)
    *   - too small dimensions 
    */

    // <= 1 objects
    if (objects.size() <= 1)
    {
        return;
    }

    // too small dimensions
    glm::vec3 dimensions = region.calculateDimensions();
    for (int i = 0; i < 3; ++i)
    {
        if (dimensions[i] < MIN_BOUNDS)
        {
            return;
        }
    }

    // create regions
    BoundingRegion octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; ++i) {
        calculateBounds(&octants[i], (Octant)(1 << i), region);
    }

    // determine which octants to place the objects in 
    std::vector<BoundingRegion> octLists[NO_CHILDREN]; // array of lists of objects in each octant
    std::stack<int> delList; // list of object that have been placed

    for (int i = 0; i < objects.size(); ++i)
    {

        for (int j = 0; j < NO_CHILDREN; ++j)
        {
            if (octants[j].containsRegion(objects[i]))
            {
                octLists[j].push_back(objects[i]);
                delList.push(i);
                break;
            }
        }
    }

    // remove objects in delList
    while (delList.size() != 0)
    {
        objects.erase(objects.begin()+delList.top());
        delList.pop();
    }

    // populate octans
    for (int i = 0; i < NO_CHILDREN; ++i)
    {
        if (octLists[i].size() != 0)
        {
            children[i] = new Node(octants[i], octLists[i]);
            States::activateIndex(&activeOctants, i);
            children[i]->build();
            hasChildren = true;
        }
    }

    treeBuilt = true;
    treeReady = true;

}

void Octree::Node::update()
{
    if (treeBuilt && treeReady)
    {
        if (objects.size() == 0)
        {
            if (!hasChildren)
            {
                if (currentLifespan == -1)
                {
                    // root check
                    currentLifespan = maxLifespan;
                }
                else if (currentLifespan > 0)
                {
                    currentLifespan--;
                }
            }
            else
            {
                if (currentLifespan != -1)
                {
                    if (maxLifespan <= 64)
                    {
                        // extend lifespan
                        maxLifespan <<= 2;
                    }
                    //currentLifespan = -1;
                }
            }
        }

        // remove objects that don't exist anymore
        for (int i = 0, listSize = objects.size(); i < listSize; ++i)
        {
            // remove if on list of dead objects
            if (States::isActive(&objects[i].instance->state, INSTANCE_DEAD))
            {
                objects.erase(objects.begin() + i);
                // update counter/size accordingly
                i--;
                listSize--;
            }
        }

        // get moved objects that were in this leaf in previous frame
        std::stack<std::pair<int,BoundingRegion>> movedObjects;
        for (int i = 0, listSize = objects.size(); i < listSize; ++i)
        {
            if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED))
            {
                objects[i].transform();
                movedObjects.push({ i, objects[i] });
            }
        }

        // remove dead branches
        unsigned char flags = activeOctants;
        for (int i = 0; flags > 0; flags >>= 1, ++i)
        {
            if (States::isIndexActive(&flags, 0) && children[i]->currentLifespan == 0)
            {
                // active and run out of time
                if (children[i]->objects.size() > 0)
                {
                    // branch is dead but we reset
                    children[i]->currentLifespan = -1;
                }
                else
                {
                    // branch is dead
                    children[i] = nullptr;
                    States::deactivateIndex(&activeOctants, i);
                }
            }
        }

        
        // updating child nodes
        if (children) {
            for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, ++i)
            {
                /*
                * fO = 01110100
                * f1 = 00111010
                * f2 = 00011101
                */
                if (States::isIndexActive(&flags, 0)) // takes always the LSB
                {
                    // active octant
                    if (children[i])
                    {
                        children[i]->update();
                    }
                }
            }
        }

        // move moved objects into new nodes
        BoundingRegion movedObj;
        while (movedObjects.size() != 0)
        {
            /*
            * for each moved object
            * - traverse up tree (starting with current node) until find a node that completely encloses the object
            * - call insert (push object as far down as possible
            */

            movedObj = movedObjects.top().second;
            Node* current = this;

            while (!current->region.containsRegion(movedObj))
            {
                if (current->parent != nullptr)
                {
                    current = current->parent;
                }
                else
                {
                    break; // if root node, leave
                }
            }

            /*
            * once finished
            * - remove from objects list
            * - remove from moveObjects stack
            * - inster into found region
            */
            objects.erase(objects.begin() + movedObjects.top().first);
            movedObjects.pop();
            current->insert(movedObj);
        }
    }
    else
    {
        // process pending results
        if(queue.size() > 0)
        {
            processPending();
        }
    }
}

void Octree::Node::processPending()
{
    if (!treeBuilt) 
    {
        // add objects to be sorted into branches when built
        while(queue.size() != 0)
        {
            objects.push_back(queue.front());
            queue.pop();
        }
        build();
    }
    else
    {
        // insert the objects immediately
        while (queue.size() != 0)
        {
            insert(queue.front());
            queue.pop();
        }
    }
}

bool Octree::Node::insert(BoundingRegion obj)
{
    /*
    * termination conditions
    * - no objects (leaf node)
    * - dimensions are less than MIN_BOUNDS
    */

    glm::vec3 dimensions = region.calculateDimensions();
    if (
        objects.size() == 0 ||
        dimensions.x < MIN_BOUNDS || 
        dimensions.y < MIN_BOUNDS ||
        dimensions.z < MIN_BOUNDS
        )
    {
        objects.push_back(obj);
        return true;
    }
    // safeguard if object doesn't fit
    if (!region.containsRegion(obj))
    {
        if (parent)
        {
            return parent->insert(obj);
        }
        else
        {
            return false;
        }
    }

    // create regions if not defined
    BoundingRegion octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; ++i)
    {
        if (children[i] != nullptr)
        {
            octants[i] = children[i]->region;
        }
        else
        {
            calculateBounds(&octants[i], (Octant)(1 << i), region);
        }
    }

    // find region that fits item entirely
    bool found = false;
    for (int i = 0; i < NO_CHILDREN; ++i)
    {
        if (octants[i].containsRegion(obj)) 
        {
            if (children[i])
            {
                return children[i]->insert(obj);
            }
            else
            {
                //create node for child
                children[i] = new Node(octants[i], { obj });
                States::activateIndex(&activeOctants, i);
                return true;
            }
        }
    }

    objects.push_back(obj);
    return true;
}

void Octree::Node::destroy()
{
    // clearing out children
    if (children)
    {
        for (int flags = activeOctants, i = 0; flags > 0; flags >> 1, i++)
        {
            if (States::isIndexActive(&flags, 0))
            {
                if (children[i] != nullptr)
                {
                    children[i]->destroy();
                    //delete children[i];
                    children[i] = nullptr;
                }
            }
        }
    }

    // clear this node
    objects.clear();
    while (queue.size() != 0)
    {
        queue.pop();
    }
}

