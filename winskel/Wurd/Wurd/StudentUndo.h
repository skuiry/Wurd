#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();
	StudentUndo() {};
	~StudentUndo();

private:
	struct UndoDetails {
		UndoDetails(Action action, int row, int col, std::string ch) :act(action), r(row), co(col), c(ch) {};
		Action act = Action::ERROR;
		int r = 0;
		int co = 0;
		std::string c = "";
	};
	std::stack<UndoDetails*> a;
};

#endif // STUDENTUNDO_H_
