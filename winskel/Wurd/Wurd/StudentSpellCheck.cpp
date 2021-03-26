#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	// Only thing to delete is the dictionary we loaded in 
	del(dictionary);
	dictLoaded = false;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	std::ifstream infile(dictionaryFile);
	if (!infile) {
		return false;
	}
	//if valid dictionary file is found, delete current one
	if (dictLoaded) {
		del(dictionary);
	}
	std::string s = "";
	while (getline(infile, s)) {
		for (int i = 0; i < s.size(); i++) {
			//strip nonalpha letters, apostrophes go in array[26]
			if (s[i] != '\'' && (s[i] - 'a' < 0 || s[i] - 'a' > 25)) {
				s = s.substr(0, i) + s.substr(i + 1, s.size() - i);
			}
		}
		//if the word actually exists, insert it
		if (s != "") {
			insert(dictionary, s);
		}
	}
	dictLoaded = true;
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	//change all letters to lowercase for dictionary comparison
	//if word is in dictionary, return true
	for (int i = 0; i < word.size(); i++) {
		if (isupper(word[i])) {
			word[i] = tolower(word[i]);
		}
	}
	if (search(dictionary, word))
		return true;
	else {
		suggestions.clear();
	}
	std::string presug = "";
	int count = 0;
	Trie* root = dictionary;
	//go through each letter of the word
	for (int i = 0; i < word.size(); i++) {
		std::string postsug = "";
		if (root == nullptr || count >= max_suggestions) {
			break;
		}
		Trie* savePoint = root;
		//hold one letter to be malleable out of the word
		for (int j = 0; j < 27; j++) {
			root = savePoint;
			if (count >= max_suggestions) {
				break;
			}
			if (root == nullptr) {
				postsug = "";
				continue;
			}
			//run through each letter of the alphabet, replacing the "malleable" letter
			root = root->children[j];
			//if the letter combination doesn't exist, skip it
			if (root == nullptr) {
				postsug = "";
				continue;
			}
			//if the letter isn't an apostrophe
			if (j != 26) {
				postsug += (j + 'a');
			}
			else {
				postsug += '\'';
			}
			//if this is the last letter
			if (i == word.size() - 1) {
				//check if it's a word or a prefix
				if (root->endOfWord) {
					suggestions.push_back(presug+postsug);
				}
				//if we add, we need to reset the rest of the word
				postsug = "";
			}
			else {
				//for the rest of the word
				for (int k = i+1; k < word.size(); k++) {
					//if not apostrophe, we go to the corresponding letter of word
					if (word[k] != '\'') {
						root = root->children[word[k] - 'a'];
					}
					//otherwise, we go to the apostrophe
					else {
						root = root->children[26];
					}
					//such a combo of letters doesn't exist
					if (root == nullptr) {
						postsug = "";
						break;
					}
					else {
						//if it does exist, we add the corresponding letter of word
						postsug += word[k];
						if (k == word.size() - 1){
							//if we still have space for suggestions, it's the end of a word, and the word is the same length
							if (count < max_suggestions && root->endOfWord && (presug+postsug).size() == word.size()) {
								suggestions.push_back(presug+postsug);
								count++;
							}
							//reset the rest of the word
							postsug = "";
						}
					}
				}
			}
		}
		//go to apostrophe section
		if (word[i] != '\'') {
			root = savePoint->children[word[i] - 'a'];
		}
		//otherwise, go to corresponding letter
		else {
			root = savePoint->children[26];
		}
		//need to add the letter we attempted to replace since we won't be replacing it in the future
		presug += word[i];
	}
	return false; 
}


void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	bool startWord = false;
	int s = 0;
	int e = 0;
	int count = 0;
	std::string word;
	for (int i = 0; i < line.size(); i++) {
		//if we haven't encountered a word yet and the current char is a letter, this is the start
		if (!startWord && isalpha(line[i])) {
			startWord = true;
			s = i;
		}
		//if we have encountered a word and we encounter a separator
		else if (startWord && !isalpha(line[i]) && line[i] != '\'') {
			e = i;
			Position p;
			p.start = s;
			p.end = e-1;
			word = line.substr(s, e - s);
			//if word doesn't exist, push it to problems 
			if (!search(dictionary, line.substr(s, e - s))) {
				problems.push_back(p);
				count++;
			}
			//now we have moved on from the current word and we don't know if we're at the start of a word yet
			startWord = false;
		}
		//otherwise, if we are at the end of a line and have encountered a word, it's the end of the word
		else if (startWord && isalpha(line[i]) && i == line.size() - 1) {
			e = i;
			Position p;
			p.start = s;
			p.end = e;
			word = line.substr(s);
			//if word doesn't exist, push it to problems 
			if (!search(dictionary, line.substr(s))) {
				problems.push_back(p);
				count++;
			}
			//now we have moved on from the current word and we don't know if we're at the start of a word yet
			startWord = false;
		}
	}
	//if we found no problematic words, clear problems
	if (count == 0) {
		problems.clear();
	}
}
