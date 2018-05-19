#ifndef MYHTML_H_INCLUDED
#define MYHTML_H_INCLUDED

#include <vector>
#include <string>

#include <myhtml/api.h>


namespace Myhtml
{

enum class Tag {Text, Br, H, P, A, Img, Script, Style, Comment, Other};

struct TextElement
{
    Tag tag;
    std::string mainText, extraText;
};


class Document
{
public:
    Document();
    ~Document();
    void parse(const std::string& html);
    std::vector<TextElement> textElements() const;

private:
    std::string title() const;
    static bool isWindows1251(const std::string& html);

private:
    std::vector<TextElement> m_textElems;
    myhtml_tree_t* m_tree;
};


class NodeRef
{
public:
    NodeRef(myhtml_tree_node_t* node);
    std::string text() const;
    std::string textSubnodes() const;
    std::string attributeByKey(const std::string& key) const;
    Tag tag() const;
    std::vector<NodeRef> children() const;
    NodeRef parent() const;

private:
    myhtml_tree_node_t* m_node;
};


}


#endif // MYHTML_H_INCLUDED
