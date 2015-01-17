#include "Mesh2Graph.h"
#include "LineNode.h"

namespace Mesh2
{
	Mesh2Graph Mesh2Graph::Create(Position size, float fillPercent, bool d, bool vh)
	{
		Mesh2Graph graph;
		graph.size = size;
		graph.dual = false;
		graph.v = false;
		graph.h = false;
		if (d)
		{
			graph.diags = GenerateDiags(graph.size);
			graph.dual = !vh;
		}
		if (vh)
		{
			graph.v = true;
			graph.h = true;
			graph.horis = GenerateHoris(graph.size);
			graph.verts = GenerateVerts(graph.size);
		}
		graph.vertices = GenerateVertices(graph);

		log(graph.ToString());

		if (fillPercent < 1.0f)
		{
			int delCount = (int)((1.0f - fillPercent) * static_cast<float>(graph.vertices.size()));
			
			for (int i = 0; i < delCount; i++)
			{
				auto itr = graph.vertices.begin();
				std::advance(itr, rand() % graph.vertices.size());
				graph.vertices.erase(itr);
			}
		}
		return graph;
	}

	//Return a MxN matrix filled with / or \ or x or space. 
	map<Position, char> Mesh2Graph::GenerateDiags(Position size, bool dual)
	{
		const string valid(dual ? "/\\" : "x/\\ ");
		map<Position, char> o;
		for (int x = 0; x < size.x; x++)
			for (int y = 0; y < size.y; y++)
				o[{x, y}] = CHOOSE(valid);

		return o;
	}

	//Return a (M-1)xN boolean matrix
	map<Position, bool> Mesh2Graph::GenerateHoris(Position size)
	{
		map<Position, bool> o;
		for (int x = 0; x < size.x; x++)
			for (int y = 0; y <= size.y; y++)
				o[{x, y}] = rand() % 2 == 0;

		return o;
	}

	//Return a Mx(N-1) boolean matrix
	map<Position, bool> Mesh2Graph::GenerateVerts(Position size)
	{
		map<Position, bool> o;
		for (int x = 0; x <= size.x; x++)
			for (int y = 0; y < size.y; y++)
				o[{x, y}] = rand() % 2 == 0;

		return o;
	}

	//Generate vertices diags based on a value matrix.
	map<Position, int> Mesh2Graph::GenerateVertices(Mesh2Graph dhv)
	{
		Position size = dhv.size;

		map<Position, int> o;
		for (int x = 0; x <= size.x; x++)
			for (int y = 0; y <= size.y; y++)
			{
				int sum = 0;

				//Step 1: Count diagonals
				if (x - 1 >= 0 && y - 1 >= 0 &&
					(dhv.diags[{x - 1, y - 1}] == 'x' || dhv.diags[{x - 1, y - 1}] == '\\'))
					sum++;
				if (x < size.x && y - 1 >= 0 &&
					(dhv.diags[{x, y - 1}] == 'x' || dhv.diags[{x, y - 1}] == '/'))
					sum++;
				if (x - 1 >= 0 && y < size.y &&
					(dhv.diags[{x - 1, y}] == 'x' || dhv.diags[{x - 1, y}] == '/'))
					sum++;
				if (x < size.x && y < size.y &&
					(dhv.diags[{x, y}] == 'x' || dhv.diags[{x, y}] == '\\'))
					sum++;

				//Step 2: Count horizontals
				if (dhv.h)
				{
					if (x < size.x && dhv.horis[{x, y}]) sum++;
					if (x - 1 >= 0 && dhv.horis[{x - 1, y}]) sum++;
				}

				//Step 3: Count verticals
				if (dhv.v)
				{
					if (y < size.y && dhv.verts[{x, y}]) sum++;
					if (y - 1 >= 0 && dhv.verts[{x, y - 1}]) sum++;
				}

				o[{x, y}] = sum;
			}

		return o;
	}

	string Mesh2Graph::ToString()
	{
		string o = string_format("\nGraph Size:%d,%d\n", size.x, size.y);
		for (int y = 0; y < size.y; y++)
		{
			//Draw Vertices
			for (int x = 0; x < size.x; x++)
			{
				o += nToString(vertices[{x, y}]);
				o += horis[{x, y}] ? "-" : " ";
			}
			o += nToString(vertices[{size.x, y}]) + "\n";

			//Draw Values
			for (int x = 0; x < size.x; x++)
			{
				o += verts[{x, y}] ? "|" : " ";
				o += nToString(diags[{x, y}]);
			}
			o += verts[{size.x, y}] ? "|" : " ";
			o += " \n";
		}

		//Draw last line of vertices
		for (int x = 0; x < size.x; x++)
		{
			o += nToString(vertices[{x, size.y}]);
				o += horis[{x, size.y}] ? "-" : " ";
		}
		o += nToString(vertices[{size.x, size.y}]) + "\n";

		return o;
	}

	Mesh2GraphVertex* Mesh2GraphVertex::create(int _newValue)
	{
		auto e = new Mesh2GraphVertex();
		e->trueValue = _newValue;
		if (e && e->init())
		{
			e->autorelease();
		}
		else
		{
			delete e;
			e = NULL;
		}
		return e;
	}

	bool Mesh2GraphVertex::init()
	{
		if (!Node::init()) return false;

		ring = MAKESPRITE("ring");
		ring->setAnchorPoint(Point(0.5f, 0.5f));
		ring->setPosition(Point::ZERO);
		ring->setName("ring");
		ring->setScale(0.1f);
		addChild(ring);
		
		circ = MAKESPRITE("circle");
		addChild(circ);
		circ->setColor(Color3B::WHITE);
		circ->setPosition(Point::ZERO);
		circ->setScale(0.5f);

		label = Label::createWithTTF(nToString(trueValue), NORMAL_TTF, 36.0f);
		addChild(label);
		label->setColor(Color3B::WHITE);
		label->setPosition(Point::ZERO);

		return true;
	}

	void Mesh2GraphVertex::shine()
	{
		ring->setColor(circ->getDisplayedColor());
		ring->setScale(0.5f);
		ring->setOpacity(255);
		ring->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f), FadeOut::create(0.3f), NULL));
		PlayEffect("blimp_low1.wav");
		//circ->setScale(0.5f);
		//circ->runAction(Sequence::create(ScaleTo::create(0.25f, 0.6f), ScaleTo::create(0.25f, 0.5f), NULL));
	}

	void Mesh2GraphVertex::setValue(int _newValue)
	{
		currentValue = _newValue;
		Color3B color = Color3B(0, 70, 140);// Color3B::WHITE;
		if (trueValue == currentValue) color = Color3B(105, 140, 0);
		else if (trueValue < currentValue) color = Color3B(217, 109, 0);// Color3B(255, 127, 127);
		circ->stopAllActions();
		circ->runAction(TintTo::create(0.3f, color.r, color.g, color.b));
/*
		if (trueValue > currentValue) label->setColor(Color3B::BLACK);
		else label->setColor(Color3B::WHITE);*/
	}

	Mesh2GraphNode* Mesh2GraphNode::create(Position size, float fillPercent)
	{
		auto e = new Mesh2GraphNode();
		if (e && e->init(size, fillPercent))
		{
			e->autorelease();
		}
		else
		{
			delete e;
			e = NULL;
		}
		return e;
	}

	bool Mesh2GraphNode::init(Position size, float fillPercent)
	{
		if (!Node::init()) return false;

		data = Mesh2Graph::Create(size, fillPercent);
		//data.diags = data.GenerateDiags(size); //shuffle
		//if (data.h) data.horis = data.GenerateHoris(size);
		//if (data.v) data.verts = data.GenerateVerts(size);

		for (auto item : data.diags) data.diags[item.first] = ' ';
		for (auto item : data.horis) data.horis[item.first] = false;
		for (auto item : data.verts) data.verts[item.first] = false;

		mysize = Size(VPX(0.8f), VPY(0.8f));
		csize = mysize;
		csize.width /= (float) size.x;
		csize.height = csize.width;
		mysize.height = csize.height * size.y;
		
		hsize = csize;
		hsize.height *= 0.5f;

		vsize = csize;
		vsize.width *= 0.5f;

		done = false;

		tempLine = nullptr;
		beginNode = Position{ -1, -1 };
		endNode = Position{ -1, -1 };
		dragActive = false;

		setContentSize(mysize);
		//addChild(LayerColor::create(Color4B::BLUE, mysize.width, mysize.height));
		
		for (auto value : data.diags)
		{
			Point pos(value.first.x * csize.width, (-1-value.first.y) * csize.height);
			pos += Point(0, mysize.height); //flip y

			Node* node = Node::create();
			node->setContentSize(csize);
			node->setAnchorPoint(Point::ZERO);
			addChild(node);
			node->setPosition(pos);

			createSlash(value.second, node);
			diags[value.first] = node;
		}

		for (auto value : data.horis)
		{
			Point pos(value.first.x * csize.width,
				-hsize.height * 0.5f + 
				(-value.first.y) * csize.height);

			pos += Point(0, mysize.height); //flip y

			Node* node = Node::create();
			node->setContentSize(hsize);
			node->setAnchorPoint(Point::ZERO);
			addChild(node);
			node->setPosition(pos);

			if (value.second) 
				createH(node);
			horis[value.first] = node;
		}

		for (auto value : data.verts)
		{
			Point pos(value.first.x * csize.width
				- vsize.width * 0.5f
				,
				(-1 - value.first.y) * csize.height);

			pos += Point(0, mysize.height); //flip y

			Node* node = Node::create();
			node->setContentSize(vsize);
			node->setAnchorPoint(Point::ZERO);
			addChild(node);
			node->setPosition(pos);

			if (value.second) 
				createV(node);
			verts[value.first] = node;
		}
		
		for (auto vertex : data.vertices)
		{
			Point pos(vertex.first.x * csize.width, -vertex.first.y * csize.height);
			pos += Point(0, mysize.height); //flip y

			auto node = Mesh2GraphVertex::create(vertex.second);
			addChild(node);
			node->setPosition(pos);

			vertices[vertex.first] = node;
		}

		recalculate();

		ring = MAKESPRITE("ring");
		ring->setAnchorPoint(Point(0.5f, 0.5f));
		ring->setPosition(Point::ZERO);
		ring->setName("ring");
		ring->setScale(0.1f);
		ring->setOpacity(0);
		addChild(ring);

		auto touch = EventListenerTouchOneByOne::create();
		touch->onTouchBegan = CC_CALLBACK_2(Mesh2GraphNode::onTouchBegan, this);
		touch->onTouchMoved = CC_CALLBACK_2(Mesh2GraphNode::onTouchMoved, this);
		touch->onTouchEnded = CC_CALLBACK_2(Mesh2GraphNode::onTouchEnded, this);
		touch->onTouchCancelled = CC_CALLBACK_2(Mesh2GraphNode::onTouchEnded, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

		return true;
	}

	Node* Mesh2GraphNode::createSlash(char c)
	{
		// /:
		Point start = Point::ZERO;
		Point end = Point(csize.width, csize.height);

		// \:
		if (c == '\\')
		{
			start = Point(0, csize.height);
			end = Point(csize.width, 0);
		}

		LineNode* slash = LineNode::create(start, end, Color4B::BLACK, 4.0f);
		return slash;
	}

	void Mesh2GraphNode::createH(Node* n)
	{
		Point start = Point::ZERO;
		Point end = Point(csize.width, 0);
		start.y += hsize.height * 0.5f;
		end.y += hsize.height * 0.5f;
		n->addChild(LineNode::create(start, end, Color4B::BLACK, 4.0f));
	}

	void Mesh2GraphNode::createV(Node* n)
	{
		Point start = Point::ZERO;
		Point end = Point(0, csize.height);
		start.x += vsize.width * 0.5f;
		end.x += vsize.width * 0.5f;
		n->addChild(LineNode::create(start, end, Color4B::BLACK, 4.0f));
	}
	
	bool Mesh2GraphNode::onTouchBegan(Touch* t, Event*)
	{
		dragActive = false;
		beginNode = Position{ -1, -1 };
		endNode = Position{ -1, -1 };

		if (tempLine)
		{
			tempLine->removeFromParentAndCleanup(true);
			tempLine = nullptr;
		}

		auto p = convertToNodeSpace(t->getLocation());
		//if (getBoundingBox().containsPoint(t->getLocation()))
		{
			for (auto node : vertices)
			{
				auto r = static_cast<Mesh2GraphVertex*>(node.second)->circ->getBoundingBox().size *
					node.second->getScale() * 0.75f;
				if (node.second->getPosition().distance(p) < r.width)
				{
					dragActive = true;
					tempLine = LineNode::create(node.second->getPosition(), 
						node.second->getPosition() + Point(1,1), Color4B::BLUE, 4);
					addChild(tempLine,-1);

					beginNode = node.first;

					static_cast<Mesh2GraphVertex*>(node.second)->shine();
					break;
				}
			}

			return true;
		}

		return false;
	}

	void Mesh2GraphNode::onTouchMoved(Touch* t, Event*)
	{
		if (dragActive && tempLine != nullptr)
		{
			auto p = convertToNodeSpace(t->getLocation());
			for (auto node : vertices)
			{
				if (node.first == beginNode) continue;
				if (abs(node.first.x - beginNode.x) > 1) continue;
				if (abs(node.first.y - beginNode.y) > 1) continue;
				auto r = static_cast<Mesh2GraphVertex*>(node.second)->circ->getBoundingBox().size *
					node.second->getScale();
				if (node.second->getPosition().distance(p) < r.width)
				{
					if (!(endNode == node.first))
					{
						tempLine->setEnd(node.second->getPosition());
						endNode = node.first;
						static_cast<Mesh2GraphVertex*>(node.second)->shine();
					}
					return;
				}
			}

			endNode = Position{ -1, -1 };
			tempLine->setEnd(p);
		}
	}

	void Mesh2GraphNode::onTouchEnded(Touch* t, Event*)
	{
		if (dragActive && beginNode.x != -1 && endNode.x != -1) //draw line
		{
			if (beginNode.x == endNode.x) //v
			{
				int u = min(beginNode.y, endNode.y);
				if (!data.verts[{beginNode.x, u}])
				{
					data.verts[{beginNode.x, u}] = true;
					createV(verts[{ beginNode.x, u }]);
					recalculate();
				}
			}
			else if (beginNode.y == endNode.y) //h
			{
				int u = min(beginNode.x, endNode.x);
				if (!data.horis[{u, beginNode.y}])
				{
					data.horis[{u, beginNode.y}] = true;
					createH(horis[{u, beginNode.y}]);
					recalculate();
				}
			}
			else
			{
				int u = min(beginNode.x, endNode.x);
				int v = min(beginNode.y, endNode.y);
				Position cell{ u, v };
				if ((beginNode.x == endNode.x - 1 && beginNode.y == endNode.y - 1) ||
					(endNode.x == beginNode.x - 1 && endNode.y == beginNode.y - 1))
				{
					if (data.diags[cell] == ' ')
					{
						data.diags[cell] = '\\';
					}
					else if (data.diags[cell] == '/')
					{
						data.diags[cell] = 'x';
					}

					createSlash(data.diags[cell], diags[cell]);
					recalculate();
				}
				else if ((beginNode.x == endNode.x - 1 && beginNode.y -1 == endNode.y) ||
					(endNode.x == beginNode.x - 1 && endNode.y - 1 == beginNode.y))
				{
					if (data.diags[cell] == ' ')
					{
						data.diags[cell] = '/';
					}
					else if (data.diags[cell] == '\\')
					{
						data.diags[cell] = 'x';
					}

					createSlash(data.diags[cell], diags[cell]);
					recalculate();
				}
			}
		}
		else
		{
			auto p = convertToNodeSpace(t->getLocation());
			for (auto item : diags)
			{
				if (item.second->getBoundingBox().containsPoint(p))
				{
					data.diags[item.first] = ' ';
					createSlash(data.diags[item.first], diags[item.first]);
				}
			}
			for (auto item : verts)
			{
				if (item.second->getBoundingBox().containsPoint(p))
				{
					data.verts[item.first] = false;
					verts[item.first]->removeAllChildren();
				}
			}
			for (auto item : horis)
			{
				if (item.second->getBoundingBox().containsPoint(p))
				{
					data.horis[item.first] = false;
					horis[item.first]->removeAllChildren();
				}
			}
			recalculate();

			ring->setColor(Color3B::ORANGE);
			ring->setScale(0.5f);
			ring->setOpacity(255);
			ring->setPosition(p);
			ring->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f), FadeOut::create(0.3f), NULL));

		}
		if (tempLine != nullptr)
		{
			tempLine->removeFromParent();
			tempLine = nullptr;
			dragActive = false;
		}
	}

	void Mesh2GraphNode::createSlash(char c, Node* n)
	{
		n->removeAllChildren();
		
		if (c == '/' || c == 'x')
		{
			n->addChild(createSlash('/'));
		}
		if (c == '\\' || c == 'x')
		{
			n->addChild(createSlash('\\'));
		}
	}

	void Mesh2GraphNode::flip(Position p)
	{
		if (data.diags[p] == '\\')
		{
			data.diags[p] = '/';
		}
		else if (data.diags[p] == '/' && !data.dual)
		{
			data.diags[p] = 'x';
		}
		else if (data.diags[p] == 'x' && !data.dual)
		{
			data.diags[p] = ' ';
		}
		else
		{
			data.diags[p] = '\\';
		}

		createSlash(data.diags[p], diags[p]);
		recalculate();
	}

	void Mesh2GraphNode::recalculate()
	{
		if (done) return;
		auto nv = Mesh2Graph::GenerateVertices(data);
		
		bool fine = true;
		for (auto v : vertices)
		{
			if (data.vertices[v.first] != nv[v.first]) fine = false;
			static_cast<Mesh2GraphVertex*>(v.second)->setValue(nv[v.first]);
		}

		if (fine && end) 
		{
			end();
			done = true;
		}
	}
}