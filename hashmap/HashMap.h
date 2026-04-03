#pragma once

#include <functional>
#include <optional>
#include <vector>

template<typename K, typename V>
class HashMap {
private:
    constexpr static unsigned int InitialCapacity = 16;

    unsigned int size_{0};
    struct Entry {
        K key;
        V value;
        bool used{false};
    };
    std::vector<Entry> table;

    void insert(const K& k, const V& v) {
        const auto h = std::hash<K>{}(k);
        auto idx = h % table.size();
        while (table[idx].used) {
            if (table[idx].key == k) {
                table[idx].value = v;
                return;
            }
            idx = (idx + 1) % table.size();
        }
        table[idx] = Entry{k, v, true};
        ++size_;
    }

    void rehash() {
        auto curr = std::move(table);
        table.resize(curr.size() * 2);
        for (const auto& entry : curr) {
            insert(entry.key, entry.value);
        }
    }

    int find(const K& k) {
        const auto h = std::hash<K>{}(k);
        auto idx = h % table.size();
        while (table[idx].used) {
            if (table[idx].key == k) {
                return idx;
            }
            idx = (idx + 1) % table.size();
        }
        return -1;
    }

public:
    HashMap() : table(InitialCapacity) {}

    void put(const K& k, const V& v) {
        if (size_ >= (3 * table.size()) / 4) {
            rehash();
        }
        insert(k, v);
    }

    std::optional<V> get(const K& k) {
        int idx = find(k);
        if (idx == -1) {
            return std::nullopt;
        }
        return std::make_optional<V>(table[idx].value);
    }

    bool remove(const K& k) {
        int idx = find(k);
        if (idx == -1) { // Not found
            return false;
        }
        table[idx].used = false;
        return true;
    }
};
