#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
#include <iostream>
class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Trie {
	public:
		Trie() {};
		Trie* children[27] = { nullptr };
		bool endOfWord = false;
	};
	void insert(Trie* root, std::string s) {
		//if word isn't present in the trie
		for (int i = 0; i < s.size(); i++) {
			//get corresponding index based on letter
			int index = tolower(s[i]) - 'a';
			if (s[i] == '\'') {
				index = 26;
			}
			if (root->children[index] == nullptr) {
				//if letter doesn't exist, create new trie for it
				Trie* t = new Trie();
				root->children[index] = t;
				if (i == s.size() - 1) {
					//if at last letter, indicate that it is the end of a word
					t->endOfWord = true;
				}
			}
			//go to next letter
			root = root->children[index];
		}
		//in case, prefix becomes a word
		root->endOfWord = true;
	};

	bool search(Trie* root, std::string s) {
		if (root != nullptr) {
			for (int i = 0; i < s.size(); i++) {
				//get corresponding index based on letter
				int index = tolower(s[i]) - 'a';
				if (s[i] == '\'') {
					index = 26;
				}
				if (root->children[index] == nullptr) {
					//if letter doesn't exist, means word isn't part of the trie
					return false;
				}
				//move to next letter
				root = root->children[index];
			}
			//we should've ended on a present letter and it has to be the end of a word
			return (root != nullptr && root->endOfWord);
		}
		return false;
	};
	void del(Trie* root) {
		if (root == nullptr) {
			return;
		}
		for (int i = 0; i < 27; i++) {
			del(root->children[i]);
		}
		delete root;
	};
	Trie* dictionary = new Trie();
	bool dictLoaded = false;
};


#endif  // STUDENTSPELLCHECK_H_
