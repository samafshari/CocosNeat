#include "MatryoshkaScene.h"
#include "MatryoshkaLayer.h"
#include "Sidebar.h"
#include "Widgets.h"

namespace Matryoshka
{
	///////////////// MATRYOSHKANODE ///////////////////
	bool MatryoshkaNode::init()
	{
		if (!Node::init()) return false;
		
		number = 0;
		sprite = MAKESPRITE(GetFrameName(number));
		addChild(sprite);

		//Size size = Size(DESIGN_X / Position::width, DESIGN_X / Position::width);
		//setContentSize(size);
		//sprite->setContentSize(size);
		//sprite->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));

		Size size = sprite->getContentSize();
		setContentSize(size);
		sprite->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));

		sprite->setOpacity(0);
		sprite->runAction(Sequence::createWithTwoActions(
			DelayTime::create(0.3f), FadeIn::create(0.3f)));

		return true;
	}

	void MatryoshkaNode::upgrade(int i)
	{
		number = i;
		sprite->setSpriteFrame(GetFrameName(number));
		auto size = getContentSize();
		sprite->setContentSize(size);
		sprite->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));
	}

	void MatryoshkaNode::poof()
	{

	}

	///////////////////// MATRYOSHKALAYER //////////////////////
	bool MatryoshkaLayer::init()
	{
		if (!Layer::init()) return false;

		auto HBar = HBar::create();
		addChild(HBar);

		LayerColor* lc = LayerColor::create(Color4B(0, 0, 0, 210), PX(1.0f), PY(0.8f));
		lc->setPosition(CP(0.5f, 0.5f));
		lc->setAnchorPoint(Point(0.5f, 0.5f));
		lc->ignoreAnchorPointForPosition(false);
		addChild(lc);

		lblScore = Label::createWithTTF(LS("ScoreColon") + " 0", NORMAL_TTF, 42.0f);
		lblScore->setColor(Color3B::WHITE);
		lblScore->setPosition(CP(0.5f, 0.05f));
		addChild(lblScore);

		//boardParent = Node::create();
		//addChild(boardParent);
		boardParent = this;

		score = 0;
		state = GameStates::Playing;

		Position::width = 4;
		Position::height = 4;

		spawn();
		spawn();
		spawn();
		print();

/*
		LayerColor* scorebg = LayerColor::create(Color4B::BLACK, PX(1.0f), PY(0.1f));
		scorebg->setPosition(Point::ZERO);
		addChild(scorebg);
*/
		scheduleUpdate();

		return true;
	}

	void MatryoshkaLayer::gameOver()
	{
		if (state == GameStates::Done) return;
		state = GameStates::Done;
		recursivePause(this);
		static_cast<MatryoshkaScene*>(getParent())->showGameOver();
	}

	void MatryoshkaLayer::move(Directions d)
	{
		log("Move: " + nToString((int) d));

		vector<Position> items;
		vector<int> xs;
		vector<int> ys;
		if (d == Directions::Left) for (int x = 0; x < Position::width; x++) xs.push_back(x);
		else for (int x = Position::width - 1; x >= 0; x--) xs.push_back(x);
		if (d == Directions::Down) for (int y = 0; y < Position::height; y++) ys.push_back(y);
		else for (int y = Position::height - 1; y >= 0; y--) ys.push_back(y);
		
		for (auto x : xs) for (auto y : ys) 
			if (board.find(Position{ x, y }) != board.end())
				items.push_back(Position{ x, y });

		for (auto item : items)
		{
			Position p = item;
			while (true)
			{
				Position t = move(p, d);
				if (t == p) break;
				p = t;
			}
		}

		spawn();
		print();
	}

	Position MatryoshkaLayer::move(Position p, Directions d)
	{
		if (board.find(p) == board.end()) return p;

		Position initialPos = p;
		if (d == Directions::Right) p.x++;
		else if (d == Directions::Left) p.x--;
		else if (d == Directions::Up) p.y++;
		else if (d == Directions::Down) p.y--;

		if (p.x >= Position::width || p.y >= Position::height || p.x < 0 || p.y < 0)
			return initialPos;

		if (board.find(p) == board.end()) //target is empty
		{
			move(p, initialPos);
			//erase(initialPos);
			return p;
		}

		//target is not empty
		if (board[p]->number == board[initialPos]->number)
		{
			erase(initialPos);
			activate(p);
			return p;
		}

		//target is not the same type
		return initialPos;
	}

	void MatryoshkaLayer::move(Position dest, Position source)
	{
		board[dest] = board[source];
		board.erase(source);
		board[dest]->stopAllActions();
		board[dest]->runAction(MoveTo::create(0.5f, calculatePosition(board[dest], dest)));
		//board[dest]->setPosition(calculatePosition(board[dest], dest));
	}

	void MatryoshkaLayer::spawn()
	{
		if (checkGameOver()) return;
		if (board.size() == Position::width * Position::height) return;

		vector<Position> items;
		for (auto p : board) items.push_back(p.first);

		Position target;
		do
		{
			target.x = rand() % Position::width;
			target.y = rand() % Position::height;
		} while (board.find(target) != board.end());

		auto node = MatryoshkaNode::create();
		boardParent->addChild(node);
		node->setAnchorPoint(Point::ZERO);
		reposition(node, target);
		board[target] = node;

		checkGameOver();
	}

	void MatryoshkaLayer::activate(Position p)
	{
		board[p]->upgrade(board[p]->number + 1);
		score += board[p]->number;
	}

	void MatryoshkaLayer::erase(Position p)
	{
		board[p]->poof();
		removeChild(board[p]);
		board.erase(p);
	}

	void MatryoshkaLayer::print()
	{
#ifdef WIN32
		string out = "";
		for (int y = Position::height - 1; y >= 0; y--)
		{
			for (int x = 0; x < Position::width; x++)
			{
				if (board.find(Position{ x, y }) == board.end()) out += ".";
				else out += nToString(board[Position{ x, y }]->number);
			}
			out += "\n";
		}
		out += "\n";
		log(out);
#endif
	}

	Vec2 MatryoshkaLayer::calculatePosition(MatryoshkaNode* node, Position p)
	{
		return Vec2(
			((float) p.x ) * node->getContentSize().width,
			PY(0.1f) + ((float) p.y ) * node->getContentSize().height);
	}

	void MatryoshkaLayer::reposition(MatryoshkaNode* node, Position p)
	{
		node->setPosition(calculatePosition(node, p));
	}

	bool MatryoshkaLayer::checkGameOver()
	{
		//first check if there is an empty block
		if (board.size() < Position::width * Position::height)
			return false;

		//check if there is an adjacent block with same number
		bool safe = false;
		for (int x = 0; x < Position::width; x++)
		{
			for (int y = 0; y < Position::height; y++)
			{
				for (int dx = -1; dx <= 1; dx++)
				{
					for (int dy = -1; dy <= 1; dy++)
					{
						if (x + dx < 0 || y + dy < 0 || x + dx >= Position::width || y + dy >= Position::height)
							continue;
						if (dx != 0 && dy != 0 && 
							board[Position{ x, y }]->number == board[Position{ x + dx, y + dy }]->number)
							return false;
					}
				}
			}
		}

		if (!safe)
		{
			gameOver();
			return true;
		}

		return false;
	}
}