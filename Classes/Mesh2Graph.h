#ifndef __MESH2GRAPH_H__
#define __MESH2GRAPH_H__

#include "helpers.h"

class LineNode;

namespace Mesh2
{
	class Mesh2Graph
	{
	public:
		Position size;
		bool dual; //True: {\,/} False: {\,x,/, }
		bool v; //True: Allow Vertical Lines
		bool h; //True: Allow Horizontal Lines
		map<Position, char> diags; //Diagonals, can be "\,/, ,x"
		map<Position, bool> verts; //Am I connected to the node below?
		map<Position, bool> horis; //Am I connected to the right?
		map<Position, int> vertices;

		static Mesh2Graph Create(Position size, float fillPercent=1.0f,
			bool d = true, bool vh = true);

		//dual=true: Fills with / or \
		//dual=false: Fills with { ,x,/,\}
		static map<Position, char> GenerateDiags(Position size, bool dual=false);
		static map<Position, bool> GenerateHoris(Position size);
		static map<Position, bool> GenerateVerts(Position size);

		static map<Position, int> GenerateVertices(Mesh2Graph dhv);

		string ToString();
	};

	class Mesh2GraphVertex : public Node
	{
	public:
		int trueValue, currentValue;
		Sprite* circ;
		Label* label;
		Sprite* ring;

		static Mesh2GraphVertex* create(int _trueValue = 0);
		bool init();
		void shine();
		void setValue(int _newValue);
	};

	class Mesh2GraphNode : public Node
	{
	public:
		Mesh2Graph data;
		map<Position, Node*> diags, horis, verts;
		map<Position, Node*> vertices;
		function<void()> end;
		Size mysize, csize, vsize, hsize;
		bool done;

		Sprite* ring;

		LineNode* tempLine;
		Position beginNode, endNode;
		bool dragActive;

		static Mesh2GraphNode* create(Position size, float fillPercent=0.5f);
		bool init(Position size, float fillPercent);
		Node* createSlash(char);
		void createSlash(char, Node*);
		void createH(Node*);
		void createV(Node*);

		bool onTouchBegan(Touch*, Event*);
		void onTouchMoved(Touch*, Event*);
		void onTouchEnded(Touch*, Event*);

		void flip(Position);
		void recalculate();
	};
}

#endif