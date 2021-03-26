#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	row = 0;
	column = 0;
	text.push_back("");
	loc = text.begin();
}

StudentTextEditor::~StudentTextEditor()
{
	text.clear();
	loc = text.begin();
}

bool StudentTextEditor::load(std::string file) {
	std::ifstream infile(file);
	if (!infile) {
		return false;
	}
	//if valid file, reset current editor
	reset();
	std::string s = "";
	while (getline(infile, s)) {
		//delete carriage returns
		if (s.size() > 1 && s.at(s.size() - 2) == '\r') {
			s.erase(s.size() - 2);
		}
		text.push_back(s);
	}
	//start at top of file
	row = 0; column = 0;
	loc = text.begin();
	return true;
}

bool StudentTextEditor::save(std::string file) {
	std::ofstream outfile(file);
	if (!outfile) {
		return false;
	}
	for (std::list<std::string>::iterator it = text.begin(); it != text.end(); it++) {
		outfile << *it << '\n';
	}
	return true;
}

void StudentTextEditor::reset() {
	row = 0; column = 0;
	text.clear();
	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	if (dir == Dir::UP){
		//if not at top, go up one row
		if (row != 0) {
			row--;
			loc--;
			//in case you go from long line to short line
			if (column > (*loc).size()) {
				column = (*loc).size();
			}
			//in case you go from non empty line to empty line
			if ((*loc).size() == 0) {
				column = 0;
			}
		}
	}

	else if (dir == Dir::DOWN) {
		//if not at end of file, go down one row
		if (row != text.size() - 1) {
			row++;
			loc++;
			//in case you go from from long line to short line
			if (column > (*loc).size()) {
				column = (*loc).size();
			}
			//in case you go from non empty line to empty line
			if ((*loc).size() == 0) {
				column = 0;
			}
		}
	}

	else if (dir == Dir::LEFT) {
		//not top row
		if (row != 0) {
			//at beginning of column
			if (column == 0) {
				row--;
				loc--;
				column = (*loc).size();
			}
			//somewhere else other than beginning
			else {
				column--;
			}
		}
		//top row
		else {
			//not at beginning of file
			if (column != 0) {
				column--;
			}
		}
	}

	else if (dir == Dir::RIGHT) {
		//not bottom row
		if (row != text.size()-1) {
			//at end of column
			if (column == (*loc).size()) {
				row++;
				loc++;
				column = 0;
			}
			//somewhere else other than beginning
			else {
				column++;
			}
		}
		//bottom row
		else {
			//not at end of last line
			if (column !=(*loc).size()) {
				column++;
			}
		}
	}

	else if (dir == Dir::HOME) {
		column = 0;
	}

	else if (dir == Dir::END) {
		column = (*loc).size();
	}
}

void StudentTextEditor::del() {
	//empty line
	if (text.size() == 1 && (*loc) == "") {
		return;
	}
	//at end of line but not last line
	if (column == (*loc).size() && row != text.size()-1) {
		std::list<std::string>::iterator it = loc;
		it++;
		//combine this line with next line and erase the next line
		(*loc) += (*it);
		text.erase(it);
		getUndo()->submit(Undo::Action::JOIN, row, column);
	}
	else if (column != (*loc).size()){
		//otherwise, delete the character at row and column
		char c = (*loc).at(column);
		(*loc).erase(column,1);
		getUndo()->submit(Undo::Action::DELETE, row, column, c);
	}
	
}

void StudentTextEditor::backspace() {
	//any line but first line and first column
	if (column == 0 && row != 0) {
		//is empty line
		if ((*loc).size() == 0) {
			loc = text.erase(loc);
			row--;
			column = (*(--loc)).size();
			getUndo()->submit(Undo::Action::JOIN, row, column);
		}
		//line has content and needs to be merged with previous line
		else {
			std::list<std::string>::iterator it = loc;
			loc--;
			row--;
			column = (*loc).size();
			(*loc) += *it;
			loc = text.erase(it);
			loc--;
			getUndo()->submit(Undo::Action::JOIN, row, column);
		}
	}
	//anywhere else
	else if (column > 0){
		//remove the character one before column
		char c = (*loc).at(column - 1);
		(*loc).erase(column - 1, 1);
		column--;
		getUndo()->submit(Undo::Action::DELETE, row, column, c);
	}
}

void StudentTextEditor::insert(char ch) {
	// TODO
	//if tab, insert 4 spaces and push cursor forward by 4 spots
	if (ch == '\t') {
		(*loc) = (*loc).substr(0, column) + "    " + (*loc).substr(column);
		column += 4;
	}
	//otherwise, insert the character and push cursor forward by 1 spot
	else {
		(*loc) = (*loc).substr(0, column) + ch + (*loc).substr(column);
		column++;
	}
	getUndo()->submit(Undo::Action::INSERT, row, column, ch);
}

void StudentTextEditor::enter() {
	// TODO
	getUndo()->submit(Undo::Action::SPLIT, row, column);
	//if at right edge of line
	if (column == (*loc).size()) {
		//if not the last line, then just insert a new line
		if (row != text.size() - 1) {
			loc++;
			loc = text.insert(loc, "");
		}
		//if the last line, then just push back a new line
		else {
			text.push_back("");
			loc++;
		}
		//set the cursor position to next row and start at the front of the column
		row++;
		column = 0;
	}
	else {
		std::string next = (*loc).substr(column, (*loc).size() - column);
		(*loc) = (*loc).substr(0, (*loc).size() - next.size());
		//(*loc).push_back('\n');
		if (row != text.size()-1) {
			loc++;
			loc = text.insert(loc, next);
		}
		else {
			text.push_back(next);
			loc++;
		}
		row++;
		column = 0;
	}
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = this->row; col = column; 
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows < 0 || startRow > text.size()) {
		return -1;
	}
	lines.clear();
	if (startRow == text.size()) {
		return 0;
	}
	int end = startRow + numRows;
	if (end >= text.size()) {
		end = text.size();
	}
	std::list<std::string>::const_iterator it = loc;
	//startRow is ahead of row
	if (startRow - row > 0) {
		for (int i = startRow - row; i > 0; i--) {
			//push iterator to startRow's location
			it++;
		}
	}
	//startRow is before/equal to row
	else if (startRow - row < 0) {
		for (int i = row - startRow; i > 0; i--) {
			//pull iterator to startRow's location
			it--;
		}
	}
	for (int i = startRow; i < end; i++) {
		lines.push_back(*it);
		it++;
	}
	return lines.size();
}

void StudentTextEditor::undo() {
	//Get the most recent 
	Undo* rewind = getUndo();
	int r = 0, c = 0;
	Undo::Action a;
	int count = 0;
	std::string change = "";
	a = rewind->get(r, c, count, change);
	std::list<std::string>::iterator ite;
	std::string next;
	switch (a) {
	case Undo::Action::ERROR:
		return;
	case Undo::Action::SPLIT:
		//go to row for splitting
		setIteratorToRow(r);
		row = r;
		column = c;
		//get text that was joined together
		next = (*loc).substr(column);
		//set the line to empty line if it is an empty line
		if ((*loc).substr(0, column) == "")
			(*loc) = "";
		//otherwise just take the rest of the line and add a line break
		else
			(*loc) = (*loc).substr(0,column) + '\n';
		ite = loc;
		ite++;
		//insert the text into the next line where it used to be
		text.insert(ite, next);
		break;
	case Undo::Action::DELETE:
		setIteratorToRow(r);
		row = r;
		column = c;
		//remove count amount of characters that were inserted
		(*loc).erase(column-count, count);
		column = c - count;
		break;
	case Undo::Action::INSERT:
		setIteratorToRow(r);
		row = r;
		column = c;
		//insert count amount of characters that were deleted
		(*loc).insert(column, change);
		break;
	case Undo::Action::JOIN:
		setIteratorToRow(r);
		row = r;
		column = c;
		std::list<std::string>::iterator it = loc;
		it++;
		//join together the rows that were separated
		(*loc) += (*it);
		text.erase(it);
		break;
	}
}

void StudentTextEditor::setIteratorToRow(int currentRow){
	//invalid parameter
	if (currentRow > text.size() - 1 || currentRow < 0) {
		return;
	}
	//need to go back to a certain row
	else if (row > currentRow) {
		for (int i = row - currentRow; i > 0; i--) {
			loc--;
		}
	}
	//need to go forward to a certain row
	else {
		for (int i = currentRow - row; i > 0; i--) {
			loc++;
		}
	}
}
