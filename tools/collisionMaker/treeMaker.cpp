#include "treeMaker.h"
#include "tree.h"
#include "main.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

// when press button, spawn tree
// when click on tree, move it (move the most in front one, and use surface)

// then save into a tree file, just the locations in a vector<vector>, { {,}, ... };

void treeMaker::spawnTree(bool isTree) {
	vector mouseWorldPos = math::screenToWorld(Main::mousePos);
	Atree* tree = new Atree(mouseWorldPos, isTree);
	sortWhenAdd(tree);
}

void treeMaker::sortWhenAdd(Atree* tree) {
	// lowest y in front, height y at end
	// sort list when adding
	std::vector<Atree*>* list;
	list = &trees;

	if (list->size() == 0)
		list->push_back(tree);
	else {
		bool added = false;
		for (int i = 0; i < list->size(); i++) {
			if (tree->loc.y <= (*list)[i]->loc.y) {
				// insert at i
				list->insert(list->begin() + i, tree);
				added = true;
				break;
			}
		}
		if (!added)
			list->push_back(tree);
	}
}

void treeMaker::sortList() {
	std::vector<Atree*> sortedList;
	int treeSize = trees.size();
	for (int j = 0; j < treeSize; j++) {
		Atree* curr = trees[0];
		trees.erase(trees.begin());
		if (sortedList.size() == 0)
			sortedList.push_back(curr);
		else {
			bool added = false;
			for (int i = 0; i < sortedList.size(); i++) {
				if (curr->loc.y <= sortedList[i]->loc.y) {
					// insert at i
					sortedList.insert(sortedList.begin() + i, curr);
					added = true;
					break;
				}
			}
			if (!added)
				sortedList.push_back(curr);
		}
	}
	trees = sortedList;
}

void treeMaker::moveTree() {
	if (selectedTree) {
		selectedTree = nullptr;
	} else {
		for (int i = trees.size() - 1; i >= 0; i--) {
			if (trees[i]->isMouseOver()) {
				selectedTree = trees[i];

				prevLoc = selectedTree->loc;
				vector mouseWorldLoc = math::screenToWorld(Main::mousePos);
				selectOffset =  mouseWorldLoc - selectedTree->loc;
				break;
			}
		}
	}
}

void treeMaker::resetPosition() {
	if (selectedTree) {
		selectedTree->setLoc(prevLoc);
		sortList();
		selectedTree = nullptr;
	}
}

void treeMaker::draw(SDL_Renderer* renderer) {
	if (selectedTree) {
		selectedTree->setLoc(math::screenToWorld(Main::mousePos) - selectOffset);
		sortList();
	}

	for (int i = 0; i < trees.size(); i++) {
		trees[i]->draw(renderer);
	}
}

void treeMaker::save() {
	std::ofstream colFile("./data/trees.trees");
	colFile << "{";
	for (int i = 0; i < trees.size(); i++) {
		if (trees[i]->isTree) {
			if (i != 0)
				colFile << ",";
			colFile << "{" << trees[i]->loc << "}";
		}
	}
	colFile << "}" << std::endl << "{";

	for (int i = 0; i < trees.size(); i++) {
		if (!trees[i]->isTree) {
			if (i != 0)
				colFile << ",";
			colFile << "{" << trees[i]->loc << "}";
		}
	}
	colFile << "}";
}

void treeMaker::load() {

	std::ifstream colFile("./data/trees.trees");

	int lineNum = 0;
	if (colFile.is_open()) {
		while (colFile.good()) {
			std::string input;
			std::getline(colFile, input);

			// Remove the outer curly braces
			input = input.substr(1, input.length() - 2);

			// Create a stringstream to process the individual pairs
			std::stringstream ss(input);
			std::string pair;

			// Process each pair of numbers
			while (std::getline(ss, pair, '}')) {
				if (pair.empty()) continue;
				// Remove any potential leading or trailing whitespace or commas
				pair.erase(0, pair.find_first_not_of("{"));
				pair.erase(pair.find_last_not_of("}") + 1);

				if (pair[0] == ',')
					pair.erase(pair.begin());
				if (pair[0] == '{')
					pair.erase(pair.begin());

				std::stringstream pairStream(pair);
				std::string num;
				std::vector<float> innerVector;

				// Split the pair into two numbers and store them in the inner vector
				while (std::getline(pairStream, num, ',')) {
					innerVector.push_back(std::stof(num));
				}

				// Add the inner vector to the result vector
				vector temp = { innerVector[0], innerVector[1] };
				Atree* tree = new Atree(temp, lineNum == 0);
				trees.push_back(tree);
			}
			lineNum++;
		}
	}

	sortList();
}