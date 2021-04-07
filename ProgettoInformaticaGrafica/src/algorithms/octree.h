#pragma once

/*
* Based on https://www.gamedev.net/articles/programming/general-and-gameplay-programming/introduction-to-octrees-r3529/ by Eric Nevala
*/

#include <vector>
#include <queue>
#include <stack>

#include <glm/glm.hpp>

#include "list.hpp"
#include "states.hpp"
#include "bounds.h"

#include "../graphics/model.h"
#include "trie.hpp"

#define NO_CHILDREN 8
#define MIN_BOUNDS 0.5

class BoundingRegion;
class Box;

enum class NodeType
{
	EMPTY,
	FULL,
	PARTITION
};

namespace Octree {

	enum class Octant : unsigned char {
		O1 = 0x01, // = 0b00000001
		O2 = 0x02, // = 0b00000010
		O3 = 0x04, // = 0b00000100
		O4 = 0x08, // = 0b00001000
		O5 = 0x10, // = 0b00010000
		O6 = 0x20, // = 0b00100000
		O7 = 0x40, // = 0b01000000
		O8 = 0x80, // = 0b10000000
	};

	/*
	* Utility
	*/

	void calculateBounds(BoundingRegion& out, Octant octant, BoundingRegion parentRegion);

	class Node
	{
	public:
		Node();
		Node(BoundingRegion bounds);
		Node(BoundingRegion bounds, std::vector<BoundingRegion> objectList);

		void addToPending(RigidBody* instance, trie::Trie<Model*> model);

		~Node();

		void build();

		void update(Box& box);

		void processPending();

		bool insert(BoundingRegion obj);

		// check collision with all objects in node
		void checkCollisionsSelf(BoundingRegion obj);

		// check collisions with all objects in child nodes
		void checkCollisionsChildren(BoundingRegion obj);

		void destroy();

		NodeType type;

		Node* parent;
		Node* children[NO_CHILDREN];

		unsigned char activeOctants;

		bool hasChildren = false;

		bool treeReady = false;
		bool treeBuilt = false;

		short maxLifespan = 8;
		short currentLifespan = -1;

		std::vector<BoundingRegion> objects;
		std::queue<BoundingRegion> queue;

		BoundingRegion region;

	private:
		void setStateVariables();
	};

}