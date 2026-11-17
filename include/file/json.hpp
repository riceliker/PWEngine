// SPDX-License-Identifier: MIT
// Copyright (C) 2026 riceliker
/*
MIT License 

Copyright (c) 2026 riceliker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*
It's a lite json util to help you load C++ string
as Json File, read and edit json tree and store as C++ string.
This library is basic by C++ 20 and standard library.
Notice: It is output not-standard json. It mean it allow end of comma and commit.
*/

#pragma once

#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace PWEngine::Json
{
    struct Token
    {
        enum class Type {Alpha, Number, Symbol, String} type;
        std::string value;
    };

    class JsonAST;
    struct JsonNull {};
    struct JsonValue
    {
        std::variant<double, bool, std::string, JsonNull> data;
        JsonValue() : data(JsonNull{}) {}
        JsonValue(int data) : data((double)data) {}
        JsonValue(double data) : data(data) {}
        JsonValue(bool data) : data(data) {}
        JsonValue(std::string data) : data(data) {}
        JsonValue(const char* data) : data(std::string(data)) {}
        template<typename T> std::optional<T> as();
        template<std::invocable<double> NUM, std::invocable<bool> BOOL, std::invocable<std::string> STRING>
        void match(NUM&& num_callback, BOOL&& bool_callback, STRING&& string_callback);
    };

    class JsonNode;
    using JsonObject = std::unordered_map<std::string, JsonNode*>;
    using JsonArray = std::vector<JsonNode*>;

    enum class JsonReadError : uint8_t
    {
        Okay = 0, 
        HeadIsNotObject = 1,
        KeyMustUseQuotation = 2,
        AfterKeyMustUseColon = 3,
        ErrorJsonNode_BUG = 4,
        TheNumberIsNotStandNumber = 5,
        TheStringIsEmpty = 6,
        CanNotUseSymbolOrAntherUnknownWordAfterColon = 7,
        MustCloseAreaBySameSymbol = 8,
        JsonContextNotIntegrity = 9,
    };

    enum class JsonArea : uint8_t
    {
        Object,
        Array,
        Value
    };

    class JsonAST
    {
    private:
        JsonNode* root;
        std::unordered_map<JsonNode*, std::unique_ptr<JsonNode>> node_owner;
        std::unordered_map<JsonObject*, std::unique_ptr<JsonObject>> object_owner;
        std::unordered_map<JsonArray*, std::unique_ptr<JsonArray>> array_owner;
        std::unordered_map<JsonValue*, std::unique_ptr<JsonValue>> value_owner;
    public:
        JsonAST();
        std::tuple<JsonReadError, size_t> load(std::string& text);
        JsonNode* getRoot() { return this->root; }
        std::optional<std::string> toString(size_t max_depth);
        void toString_r(bool& is_okay, std::string& result, JsonNode* next_node, size_t depth);
        friend class JsonNode;
    };

    class JsonNode 
    {
    public:
        JsonAST* p_ast;
        JsonObject* next_node_object;
        JsonArray* next_node_array;
        JsonValue* value;
        void clear_r(bool& is_okay, bool is_from, size_t depth);
    public:
        bool clear(size_t max_depth);
        template<typename T> requires std::is_same_v<T, JsonObject> || std::is_same_v<T, JsonArray> || std::is_same_v<T, JsonValue>
        bool switchNodeType(size_t max_depth);
        std::optional<JsonNode*> getByKey(const char* key);
        std::optional<JsonNode*> getByIndex(size_t index);
        std::optional<JsonValue*> getValue();
        bool removeByIndex(size_t index, size_t max_depth);
        bool removeByKey(const char* key, size_t max_depth);
        template<class ...Args> void setValue(Args&& ...args);
        template<typename T> requires std::is_same_v<T, JsonObject> || std::is_same_v<T, JsonArray> || std::is_same_v<T, JsonValue>
        std::optional<JsonNode*> addNodeInObject(std::string key);
        template<typename T> requires std::is_same_v<T, JsonObject> || std::is_same_v<T, JsonArray> || std::is_same_v<T, JsonValue>
        std::optional<JsonNode*> addNodeInArray(size_t index);
        friend class JsonAST;
    };

/*
   ███  ████     ███░ ░███████▒  ███
   ███  █████   ████░ ░██    ███ ███
   ███  ██▓██  █████░ ░██    ███ ███
   ███  ██▓ ██ ██ ██░ ░███████   ███
   ███  ██▓ ████  ██░ ░██        ███
   ███  ██▓  ███  ██░ ░██        ████████
 */

    /* ast */
    inline JsonAST::JsonAST()
    {
        auto node_root = std::make_unique<JsonObject>();
        auto node_root_ptr = node_root.get();
        this->object_owner.insert({node_root_ptr, std::move(node_root)});

        auto node = std::make_unique<JsonNode>();
        node->p_ast = this;
        node->value = nullptr;
        node->next_node_object = node_root_ptr;
        this->root = node.get();
        this->node_owner.insert({node.get(), std::move(node)});
    }

    /* node */
    inline void JsonNode::clear_r(bool& is_okay, bool is_from, size_t depth)
    {
        if (depth == 0) {
            is_okay = false;
        }
        if (this->next_node_object != nullptr) {
            for (auto pair : *this->next_node_object)
            {
                pair.second->clear_r(is_okay, false, depth-1);
            }
            if (is_from){
                this->p_ast->object_owner.erase(this->next_node_object);
                auto object = std::make_unique<JsonObject>();
                auto object_ptr = object.get();
                this->p_ast->object_owner.insert({object_ptr, std::move(object)});
                this->next_node_object = object_ptr;
            } else { 
                this->p_ast->object_owner.erase(this->next_node_object);
            }
        }
        if (this->next_node_array != nullptr) {
            for (auto value : *this->next_node_array) {
                value->clear_r(is_okay, false, depth-1);
            }
            if (is_from) {
                this->p_ast->array_owner.erase(this->next_node_array);
                auto array = std::make_unique<JsonArray>();
                auto array_ptr = array.get();
                this->p_ast->array_owner.insert({array_ptr, std::move(array)});
                this->next_node_array = array_ptr;
            } else {
                this->p_ast->array_owner.erase(this->next_node_array);
            }
        }
        if (this->value != nullptr) {
            if (is_from) {
                this->p_ast->value_owner.erase(this->value);
                auto value = std::make_unique<JsonValue>();
                auto value_ptr = value.get();
                this->p_ast->value_owner.insert({value_ptr, std::move(value)});
                this->value = value_ptr;
            } else {
                this->p_ast->value_owner.erase(this->value);
            }
        }
        if (!is_from) this->p_ast->node_owner.erase(this);
    }

    inline bool JsonNode::clear(size_t max_depth)
    {
        bool is_okay = true;
        clear_r(is_okay, true, max_depth);
        return is_okay;
    }

    template<typename T> requires std::is_same_v<T, JsonObject> || std::is_same_v<T, JsonArray> || std::is_same_v<T, JsonValue>
    bool JsonNode::switchNodeType(size_t max_depth)
    {
        bool result = clear(max_depth);
        if (!result) return false;
        if constexpr (std::is_same_v<T, JsonObject>) {
            auto object = std::make_unique<JsonObject>();
            auto object_ptr = object.get();
            this->p_ast->object_owner.insert({object_ptr, std::move(object)});

            this->next_node_object = object_ptr;
            this->next_node_array = nullptr;
            this->value = nullptr;
        } else if constexpr (std::is_same_v<T, JsonArray>) {
            auto array = std::make_unique<JsonArray>();
            auto array_ptr = array.get();
            this->p_ast->array_owner.insert({array_ptr, std::move(array)});

            this->next_node_object = nullptr;
            this->next_node_array = array_ptr;
            this->value = nullptr;
        } else {
            auto value = std::make_unique<JsonValue>();
            auto value_ptr = value.get();
            this->p_ast->value_owner.insert({value_ptr, std::move(value)});

            this->next_node_object = nullptr;
            this->next_node_array = nullptr;
            this->value = value_ptr;
        }
    }

    inline std::optional<JsonNode*> JsonNode::getByKey(const char* key)
    {
        if (this->next_node_object == nullptr) return std::nullopt;
        auto result = this->next_node_object->find(key);
        if (result == this->next_node_object->end()) return std::nullopt;
        return result->second;
    }

    inline bool JsonNode::removeByKey(const char* key, size_t max_depth)
    {
        auto destroy_node_maybe = this->getByKey(key);
        if (!destroy_node_maybe.has_value()) {
            return false;
        }
        auto destroy_node = destroy_node_maybe.value();
        destroy_node->clear(max_depth);

        if (this->next_node_object == nullptr)
            return false;
        auto iter = this->next_node_object->find(key);
        if (iter == this->next_node_object->end()) {
            this->p_ast->node_owner.erase(destroy_node);
            this->next_node_object->erase(key);
            return true;
        }
        return false;
    }

    inline std::optional<JsonNode*> JsonNode::getByIndex(size_t index)
    {
        if (this->next_node_array == nullptr) return std::nullopt;
        if (this->next_node_array->size() < index) return std::nullopt;
        return this->next_node_array->at(index);
    }

    inline bool JsonNode::removeByIndex(size_t index, size_t max_depth)
    {
        auto destroy_node_maybe = this->getByIndex(index);
        if (!destroy_node_maybe.has_value()) {
            return false;
        }
        auto destroy_node = destroy_node_maybe.value();
        destroy_node->clear(max_depth);

        if (this->next_node_object == nullptr)
            return false;
        if (index < this->next_node_array->size()) {
            this->p_ast->node_owner.erase(destroy_node);
            this->next_node_array->erase(this->next_node_array->begin() + index);
            return true;
        }
        return false;
    }

    inline std::optional<JsonValue*> JsonNode::getValue()
    {
        if (this->value == nullptr) return std::nullopt;
        return this->value;
    }

    template<class ...Args> 
    void JsonNode::setValue(Args&& ...args) 
    {
        auto data = std::make_unique<JsonValue>(std::forward<Args>(args)...);
        auto data_ptr = data.get();
        this->p_ast->value_owner.erase(this->value);
        this->p_ast->value_owner.insert({data_ptr, std::move(data)});
        this->value = data_ptr;
    }

    template<typename T> requires std::is_same_v<T, JsonObject> || std::is_same_v<T, JsonArray> || std::is_same_v<T, JsonValue>
    std::optional<JsonNode*> JsonNode::addNodeInObject(std::string key)
    {
        if (this->next_node_object == nullptr) return std::nullopt;

        auto node = std::make_unique<JsonNode>();
        auto node_ptr = node.get();
        node->p_ast = this->p_ast;
        
        if constexpr (std::is_same_v<T, JsonObject>) {
            auto object = std::make_unique<JsonObject>();
            auto object_ptr = object.get();
            this->p_ast->object_owner.insert({object_ptr, std::move(object)});

            node->next_node_object = object_ptr;
            node->next_node_array = nullptr;
            node->value = nullptr;
        } else if constexpr (std::is_same_v<T, JsonArray>) {
            auto array = std::make_unique<JsonArray>();
            auto array_ptr = array.get();
            this->p_ast->array_owner.insert({array_ptr, std::move(array)});

            node->next_node_object = nullptr;
            node->next_node_array = array_ptr;
            node->value = nullptr;
        } else {
            auto value = std::make_unique<JsonValue>();
            auto value_ptr = value.get();
            this->p_ast->value_owner.insert({value_ptr, std::move(value)});

            node->next_node_object = nullptr;
            node->next_node_array = nullptr;
            node->value = value_ptr;
        }
        this->next_node_object->insert({key, node_ptr});
        this->p_ast->node_owner.insert({node_ptr, std::move(node)});
        return node_ptr;
    }

    template<typename T> requires std::is_same_v<T, JsonObject> || std::is_same_v<T, JsonArray> || std::is_same_v<T, JsonValue>
    std::optional<JsonNode*> JsonNode::addNodeInArray(size_t index)
    {
        if (this->next_node_array == nullptr) return std::nullopt;

        auto node = std::make_unique<JsonNode>();
        auto node_ptr = node.get();
        node->p_ast = this->p_ast;
        node->next_node_object = nullptr;
        node->next_node_array = nullptr;
        node->value = nullptr;

        if constexpr (std::is_same_v<T, JsonObject>) {
            auto object = std::make_unique<JsonObject>();
            auto object_ptr = object.get();
            this->p_ast->object_owner.insert({object_ptr, std::move(object)});

            node->next_node_object = object_ptr;
        } else if constexpr (std::is_same_v<T, JsonArray>) {
            auto array = std::make_unique<JsonArray>();
            auto array_ptr = array.get();
            this->p_ast->array_owner.insert({array_ptr, std::move(array)});

            node->next_node_array = array_ptr;
        } else {
            auto value = std::make_unique<JsonValue>();
            auto value_ptr = value.get();
            this->p_ast->value_owner.insert({value_ptr, std::move(value)});

            node->value = value_ptr;
        }
        if (index == this->next_node_array->size()) {
            this->next_node_array->push_back(node_ptr);
        } else {
            auto begin = this->next_node_array->begin() + index + 1;
            this->next_node_array->insert(begin, node_ptr);
        }
        this->p_ast->node_owner.insert({node_ptr, std::move(node)});
        
        return node_ptr;
    }

    /* value */
    template<typename T> std::optional<T> JsonValue::as()
    {
        std::optional<T> result = std::nullopt;
        std::visit([&result](auto& data){
            if constexpr (std::is_same_v<std::decay_t<decltype(data)>, T>) {
                result = data;
            } else {
                result = std::nullopt;
            }
        }, this->data);
        return result;
    }

    template<std::invocable<double> NUM, std::invocable<bool> BOOL, std::invocable<std::string> STRING>
    void JsonValue::match(NUM&& num_callback, BOOL&& bool_callback, STRING&& string_callback)
    {
        std::visit([num_callback, bool_callback, string_callback](auto& arg){
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
                num_callback(arg);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, bool>) {
                bool_callback(arg);
            } else if constexpr(std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                string_callback(arg);
            } else {
                static_assert("unhandled variant type");
            }
        }, this->data);
    }


    /* lexer */
    static inline bool is_alpha(char key)
    {
        return (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z');
    }

    static inline bool is_number(char key)
    {
        return key >= '0' && key <= '9';
    }

    static inline std::unique_ptr<std::vector<Token>> lexer(std::string& code)
    {
        auto token_list =  std::make_unique<std::vector<Token>>();
        size_t word_index = 0;
        bool is_in_commit = false;
        bool is_in_string = false;
        while (word_index < code.size())
        {
            if (code[word_index] == ' ' || code[word_index] == '\n') 
            {
                ++word_index;
            }
            /* when meet string */
            else if (code[word_index] == '"')
            {
                ++word_index;
                size_t string_begin = word_index;
                while (code[word_index] != '"' && word_index < code.size()) 
                {
                    ++word_index;
                }
                std::string string_token(code.data() + string_begin - 1, word_index-string_begin + 2);
                token_list->push_back({Token::Type::String, string_token});
                ++word_index;
            }
            /* when meet commit */
            else if (code[word_index] == '/' && code[word_index+1] == '*')
            {
                word_index += 2;
                while (!(code[word_index-1] == '*' && code[word_index] == '/') && word_index < code.size()) 
                {
                    ++word_index;
                }
                word_index += 1;
            }
            else
            {
                if (is_alpha(code[word_index]) || code[word_index] == '_')
                {
                    size_t string_begin = word_index;
                    while ((is_alpha(code[word_index]) || is_number(code[word_index]) || code[word_index] == '_') && word_index < code.size()) 
                    {
                        ++word_index;
                    }
                    std::string string_token(code.data() + string_begin, word_index-string_begin);
                    token_list->push_back({Token::Type::Alpha, string_token});
                }
                else if (is_number(code[word_index]) || code[word_index] == '-')
                {
                    size_t string_begin = word_index;
                    while ((is_number(code[word_index]) || code[word_index] == '.' || code[word_index] == '-' || code[word_index] == 'e' || code[word_index] == 'E') && word_index < code.size()) 
                    {
                        ++word_index;
                    }
                    std::string string_token(code.data() + string_begin, word_index-string_begin);
                    token_list->push_back({Token::Type::Number, string_token});
                } else {
                    std::string string_token;
                    string_token = std::string_view(code.data() + word_index, 1);
                    token_list->push_back({Token::Type::Symbol, string_token});
                    ++word_index;
                }
            } 
        }
        return token_list;
    }

    /* parser */
    inline std::tuple<JsonReadError, size_t> JsonAST::load(std::string& text)
    {
        JsonReadError err_info = JsonReadError::Okay;
        size_t err_line = 0;
        auto token_list = lexer(text);

        size_t i = 0;
        /* root */
        if (i >= token_list->size()) {
            err_info = JsonReadError::JsonContextNotIntegrity;
            err_line = i;
            return std::make_tuple(err_info, err_line);
        }
        if (token_list->at(i++).value != "{") {
            err_info = JsonReadError::HeadIsNotObject;
            err_line = i;
            return std::make_tuple(err_info, err_line);
        }
        
        std::stack<JsonNode*> parent_node;
        std::stack<JsonArea> area;
        std::stack<size_t> array_index;
        area.push(JsonArea::Object);

        parent_node.push(this->getRoot());

        while (i < token_list->size()-1) {
            if (area.top() == JsonArea::Object)
            {
                std::string key_name;
                if (i >= token_list->size()) {
                    err_info = JsonReadError::JsonContextNotIntegrity;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                Token token = token_list->at(i++);
                /* If end object is '}' end of this level object */
                if (token.value == "}") {
                    parent_node.pop();
                    JsonArea this_area = area.top();
                    area.pop();
                    if (this_area != JsonArea::Object) {
                        err_info = JsonReadError::MustCloseAreaBySameSymbol;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    if (i >= token_list->size()) {
                        err_info = JsonReadError::JsonContextNotIntegrity;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    Token token_peek = token_list->at(i);
                    if (token_peek.value == ",") {
                        if (i >= token_list->size()) {
                            err_info = JsonReadError::JsonContextNotIntegrity;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        token = token_list->at(i++);
                    }
                    continue;
                }
                /* object area can not have ']' */
                if (token.value == "]") {
                    err_info = JsonReadError::MustCloseAreaBySameSymbol;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                /* We will only get key string. Because value string will be skip by it. */
                /* "a" : ? */
                /* ^       */
                if (token.type != Token::Type::String) {
                    err_info = JsonReadError::KeyMustUseQuotation;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                if (token.value.size() <= 2) {
                    err_info = JsonReadError::TheStringIsEmpty,
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                key_name = token.value.substr(1, token.value.size() - 2);
                if (i >= token_list->size()) {
                    err_info = JsonReadError::JsonContextNotIntegrity;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                token = token_list->at(i++);
                /* "a" : ? */
                /*     ^   */
                if (token.value != ":") {
                    err_info = JsonReadError::AfterKeyMustUseColon;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                if (i >= token_list->size()) {
                    err_info = JsonReadError::JsonContextNotIntegrity;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                token = token_list->at(i++);
                /* three case: object, array, value */
                /* object : next is { */
                /* "a" : { */
                /*       ^ */
                if (token.value == "{") {
                    auto parent_object = parent_node.top();
                    auto object_maybe = parent_object->addNodeInObject<JsonObject>(key_name);
                    area.push(JsonArea::Object);
                    if (!object_maybe.has_value()) {
                        err_info = JsonReadError::ErrorJsonNode_BUG;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    auto object = object_maybe.value();
                    parent_node.push(object);
                    
                    continue;
                } 
                /* array : next is [ */
                /* "a" : [ */
                /*       ^ */
                else if (token.value == "[") {
                    auto parent_object = parent_node.top();
                    auto object_maybe = parent_object->addNodeInObject<JsonArray>(key_name);
                    area.push(JsonArea::Array);
                    if (!object_maybe.has_value()) {
                        err_info = JsonReadError::ErrorJsonNode_BUG;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    auto object = object_maybe.value();
                    parent_node.push(object);
                    array_index.push(0);
                    continue;
                }
                else {
                    /* value : next is number */
                    /* "a" : 12 */
                    /*       ^ */
                    if (token.type == Token::Type::Number) {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInObject<JsonValue>(key_name);
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        std::string_view str = token.value;
                        double res;
                        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), res);
                        if (ec == std::errc{} && ptr == str.data() + str.size())
                        {
                            value->setValue(res);
                        }
                        else
                        {
                            err_info = JsonReadError::TheNumberIsNotStandNumber;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                    } else if (token.type == Token::Type::String) {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInObject<JsonValue>(key_name);
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        if (token.value.size() <= 2) {
                            err_info = JsonReadError::TheStringIsEmpty,
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        std::string value_name = token.value.substr(1, token.value.size() - 2);
                        value->setValue(value_name);
                    } else if (token.value == "true") {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInObject<JsonValue>(key_name);
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        value->setValue(true);
                    } else if (token.value == "false") {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInObject<JsonValue>(key_name);
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        value->setValue(false);
                    } else {
                        err_info = JsonReadError::CanNotUseSymbolOrAntherUnknownWordAfterColon;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    /* the end. if meet ',' to skip */
                    if (i >= token_list->size()) {
                        err_info = JsonReadError::JsonContextNotIntegrity;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    token = token_list->at(i);
                    if (token.value == ",") {
                        token = token_list->at(i++);
                        continue;
                    } 
                }
            } else if (area.top() == JsonArea::Array) {
                std::string key_name;
                if (i >= token_list->size()) {
                    err_info = JsonReadError::JsonContextNotIntegrity;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                Token token = token_list->at(i++);
                /* array area can not have '}' */
                if (token.value == "}") {
                    err_info = JsonReadError::MustCloseAreaBySameSymbol;
                    err_line = i;
                    return std::make_tuple(err_info, err_line);
                }
                /* If end array is ']' end */
                if (token.value == "]") {
                    parent_node.pop();
                    JsonArea this_area = area.top();
                    area.pop();
                    array_index.pop();
                    if (this_area != JsonArea::Array) {
                        err_info = JsonReadError::MustCloseAreaBySameSymbol;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    if (i >= token_list->size()) {
                        err_info = JsonReadError::JsonContextNotIntegrity;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    Token token_peek = token_list->at(i);
                    if (token_peek.value == ",") {
                        token = token_list->at(i++);
                    }
                    continue;
                }
                /* We will only get key string. Because value string will be skip by it. */
                /* "a" : ? */
                /* ^       */
                if (token.value == "{") {
                    auto parent_object = parent_node.top();
                    auto object_maybe = parent_object->addNodeInArray<JsonObject>(array_index.top());
                    area.push(JsonArea::Object);
                    if (!object_maybe.has_value()) {
                        err_info = JsonReadError::ErrorJsonNode_BUG;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    auto object = object_maybe.value();
                    parent_node.push(object);
                    
                    continue;
                } 
                /* array : next is [ */
                /* "a" : [ */
                /*       ^ */
                else if (token.value == "[") {
                    auto parent_object = parent_node.top();
                    auto object_maybe = parent_object->addNodeInArray<JsonArray>(array_index.top());
                    area.push(JsonArea::Array);
                    if (!object_maybe.has_value()) {
                        err_info = JsonReadError::ErrorJsonNode_BUG;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    auto object = object_maybe.value();
                    parent_node.push(object);
                    array_index.push(0);
                    continue;
                }
                else {
                    /* value : next is number */
                    /* "a" : 12 */
                    /*       ^ */
                    if (token.type == Token::Type::Number) {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInArray<JsonValue>(array_index.top());
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        std::string_view str = token.value;
                        double res;
                        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), res);
                        if (ec == std::errc{} && ptr == str.data() + str.size())
                        {
                            value->setValue(res);
                        }
                        else
                        {
                            err_info = JsonReadError::TheNumberIsNotStandNumber;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                    } else if (token.type == Token::Type::String) {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInArray<JsonValue>(array_index.top());
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        if (token.value.size() <= 2) {
                            err_info = JsonReadError::TheStringIsEmpty,
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        std::string value_name = token.value.substr(1, token.value.size() - 2);
                        value->setValue(value_name);
                    } else if (token.value == "true") {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInArray<JsonValue>(array_index.top());
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        value->setValue(true);
                    } else if (token.value == "false") {
                        auto parent_object = parent_node.top();
                        auto value_maybe = parent_object->addNodeInArray<JsonValue>(array_index.top());
                        if (!value_maybe.has_value()) {
                            err_info = JsonReadError::ErrorJsonNode_BUG;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        auto value = value_maybe.value();
                        value->setValue(false);
                    } else {
                        err_info = JsonReadError::CanNotUseSymbolOrAntherUnknownWordAfterColon;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    /* the end. if meet ',' to skip */
                    if (i >= token_list->size()) {
                        err_info = JsonReadError::JsonContextNotIntegrity;
                        err_line = i;
                        return std::make_tuple(err_info, err_line);
                    }
                    token = token_list->at(i);
                    if (token.value == ",") {
                        if (i >= token_list->size()) {
                            err_info = JsonReadError::JsonContextNotIntegrity;
                            err_line = i;
                            return std::make_tuple(err_info, err_line);
                        }
                        token = token_list->at(i++);
                    } 
                }
                ++array_index.top();
            }
        }
        return std::make_tuple(JsonReadError::Okay, 0);
    }

    inline void JsonAST::toString_r(bool& is_okay, std::string& result, JsonNode* next_node, size_t depth)
    {
        if (next_node->next_node_object != nullptr) {
            result += "{";
            for (const auto& [key, value] : *next_node->next_node_object) {
                result += "\"" + key + "\":";
                toString_r(is_okay, result, value, depth-1);
            }
            result += "},";
        } else if (next_node->next_node_array != nullptr) {
            result += "[";
            for (const auto& value : *next_node->next_node_array) {
                toString_r(is_okay, result, value, depth-1);
            }
            result += "],";
        } else if (next_node->value != nullptr) {
            next_node->value->match(
                [&](double value_d){
                    result += std::to_string(value_d) + ","; 
                },
                [&](bool value_b){
                    result += std::string(value_b?"true":"false") + ",";
                },
                [&](std::string value_s){
                    result += "\"" + value_s + "\"" + ","; 
                }
            );
        } else {
            is_okay = false;
            return;
        }
        
    }

    inline std::optional<std::string> JsonAST::toString(size_t max_depth)
    {
        std::string result = "";
        bool is_okay = true;
        toString_r(is_okay, result, root, max_depth);
        if (is_okay)
            return result;
        else 
            return std::nullopt;
    } 

    inline std::string toStandard(std::string& text)
    {
        std::string result = "";
        for (size_t i = 0; i < text.size() - 1; ++i)
        {
            char currect_word = text[i];
            char next_word = text[i+1];
            result += currect_word;
            if (currect_word == ',' && (next_word == '}' || next_word == ']')) {
                ++i;
            }
        }
        return result;
    }
}