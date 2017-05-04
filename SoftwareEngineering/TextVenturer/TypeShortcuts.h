#pragma once

typedef std::unordered_map<std::string, std::string> dictionary;
typedef std::vector<std::string> stringlist;
typedef std::unordered_set<std::string> taglist;

template<typename T>
using idlist = std::unordered_map<T, UINT>;

template<typename T>
using ref_idlist = idlist<std::reference_wrapper<T>, UINT>;

template<typename T>
using ref_vector = std::vector<std::reference_wrapper<T>>;

template<typename T>
using ref_unordered_map = std::unordered_map<std::reference_wrapper<T>>;
