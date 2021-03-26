#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	// TODO
	std::string convert;
	//if tab, replace with 4 spaces
	if (ch == '\t') {
		convert = "    ";
	}
	//otherwise, push the char to the string
	else {
		convert.push_back(ch);
	}
	//create new UndoDetails object with relevant info
	UndoDetails* s = new UndoDetails(action, row, col, convert);
	//if first action, push to stack
	if (a.empty()) {
		a.push(s);
	}
	else {
		//check previous action for matching action
		UndoDetails* prev = a.top();
		if (prev->act == action) {
			if (action == Action::DELETE) {
				//if occurred in the same spot, add the previous letter deleted to the current string 
				//and pop the previous action (and delete it)
				if (prev->c != "" && s->r == prev->r) {
					if (s->co == prev->co - 1) {
						s->c = s->c + prev->c;
						a.pop();
						delete prev;
					}
					else if (s->co == prev->co) {
						s->c = prev->c + s->c;
						a.pop();
						delete prev;
					}
				}
				a.push(s);
			}
			else if (action == Action::INSERT) {
				//if occurred in the same spot, add current letter to previous letter
				//and pop the previous action (and delete it)
				if (s->r == prev->r && s->co == ((prev->co)+1)) {
					s->c = prev->c + s->c;
					a.pop();
					delete prev;
				}
				a.push(s);
			}
			else {
				a.push(s);
			}
		}
		else {
			a.push(s);
		}
	}
	
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
	//empty stack means ERROR
	if (a.empty()) {
		return Action::ERROR;  // TODO
	}
	//look at top of the stack for most recent action
	else {
		UndoDetails* s = a.top();
		a.pop();
		row = s->r;
		col = s->co;
		count = 1;
		text = s->c;
		//if we inserted a word, we need to know how much we inserted
		if (s->act == Action::INSERT) {
			count = text.size();
		}
		switch (s->act) {
		//if we insert, we need to undo by deleting
		case Action::INSERT:
			return Action::DELETE;
		//if we delete, we need to undo by inserting
		case Action::DELETE:
			return Action::INSERT;
		//if we split, we need to undo by joining
		case Action::SPLIT:
			return Action::JOIN;
		//if we join, we need to undo by splitting
		case Action::JOIN:
			return Action::SPLIT;
		//to satisfy g++
		case Action::ERROR:
			return Action::ERROR;
		}
	}
	return Action::ERROR;
}

void StudentUndo::clear() {
	// pop each UndoAction and delete it
	while (!a.empty()) {
		UndoDetails* s = a.top();
		a.pop();
		delete s;
	}
}

StudentUndo::~StudentUndo(){
	//just clear the stack of UndoAction objects
	clear();
}
