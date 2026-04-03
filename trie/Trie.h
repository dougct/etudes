#pragma once

#include <map>
#include <memory>
#include <string>

struct TrieNode {
    std::map<char, std::shared_ptr<TrieNode>> children;
    bool is_end{false};
};

class Trie {
private:
    std::shared_ptr<TrieNode> root;

    std::shared_ptr<TrieNode> prefixMatch(const std::string& prefix) {
        auto node = root;
        for (const auto& ch : prefix) {
            auto it = node->children.find(ch);
            if (it == node->children.end()) {
                return nullptr;
            }
            node = it->second;
        }
        return node;
    }

public:
    Trie() : root(std::make_shared<TrieNode>()) {}

    void insert(const std::string& word) {
        auto node = root;
        for (const auto& ch : word) {
            auto [it, inserted] = node->children.try_emplace(ch);
            if (inserted) {
                it->second = std::make_shared<TrieNode>();
            }
            node = it->second;
        }
        node->is_end = true;
    }

    bool hasPrefix(const std::string& prefix) {
        auto node = prefixMatch(prefix);
        return node != nullptr;
    }

    bool search(const std::string& word) {
        auto node = prefixMatch(word);
        if (node == nullptr) {
            return false;
        }
        return node->is_end;
    }
};
