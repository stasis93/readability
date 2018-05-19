#include "myhtml.h"
#include "utils.h"

#include <stdexcept>
#include <algorithm>
#include <stack>
#include <sstream>
//#include <cassert>

//#include <iostream>
//using std::cout;
//using std::cerr;
//using std::endl;

namespace Myhtml
{

class Handle
{
public:
    ~Handle();
    static myhtml_t* get();
    Handle(Handle&) = delete;
    Handle& operator=(Handle&) = delete;
private:
    Handle();
    myhtml_t* m_handle;
};

Handle::Handle()
{
    m_handle = myhtml_create();
    myhtml_init(m_handle, MyHTML_OPTIONS_DEFAULT, 1, 0);
//    cout << __FUNCTION__ << endl;
}

Handle::~Handle()
{
    myhtml_destroy(m_handle);
}

myhtml_t* Handle::get()
{
    static Handle instance;
    return instance.m_handle;
}

// ==============================================

Document::Document()
{
    m_tree = myhtml_tree_create();
    myhtml_tree_init(m_tree, Handle::get());
}

Document::~Document()
{
    m_tree = myhtml_tree_destroy(m_tree);
}

bool Document::isWindows1251(const std::string& html)
{
    std::string cs = "charset=";
    std::string w1251 = "1251";

    auto pos = html.find(cs);
    if (pos != std::string::npos) {
        std::string sub = html.substr(pos + cs.size(), 15);
        std::transform(sub.begin(), sub.end(), sub.begin(), tolower);
        if (sub.find(w1251) != std::string::npos)
            return true;
        else
            return false;
    }
    return false;
}

void Document::parse(const std::string& html)
{
    m_textElems.clear();
    myhtml_tree_clean(m_tree);
    myencoding_t enc = isWindows1251(html) ? MyENCODING_WINDOWS_1251 : MyENCODING_DEFAULT;

    if (MyCORE_STATUS_OK != myhtml_parse(m_tree, enc, html.c_str(), html.size()))
        throw std::runtime_error("myhtml_parse() failed");

    /* Write title */
    m_textElems.push_back({Tag::Text, title() + "\n\n", ""});

    /* Traverse HTML tree and Extract text elements into m_textElems */
    NodeRef body(myhtml_tree_get_node_body(m_tree));

    std::stack<NodeRef> s;
    s.push(body);

    while (!s.empty())
    {
        NodeRef node = s.top(); s.pop();

        Tag tag = node.tag();
        TextElement elem;

        if (tag == Tag::Script || tag == Tag::Style || tag == Tag::Comment)
            continue;

        /* collect only text, links, image links inside <p> and <h> tags */
        bool insideParagpraph = node.parent().tag() == Tag::P || node.parent().parent().tag() == Tag::P;

        if (tag == Tag::H)
        {
            elem.mainText = Utils::removeNewLines(node.textSubnodes());
        }
        else
        if (insideParagpraph)
        {
            if (tag == Tag::A)
            {
                elem.mainText = node.attributeByKey("href");
                elem.extraText = node.textSubnodes();
            }
            else if (tag == Tag::Img)
            {
                elem.mainText = node.attributeByKey("src");
                elem.extraText = node.attributeByKey("alt");
            }
            else if (tag == Tag::Text)
            {
                elem.mainText = Utils::removeNewLines(node.text());
            }
        }

        if (insideParagpraph || tag == Tag::H || tag == Tag::P)
        {
            elem.tag = tag;
            m_textElems.push_back(elem);
        }

        std::vector<NodeRef> ch = node.children();

        /* Skip Headers and links, they are already extracted with textSubnodes() */

        if (tag != Tag::H && tag != Tag::A) {
            for (auto it = ch.rbegin(); it != ch.rend(); ++it)
                s.push(*it);
        }
    }
}

std::vector<TextElement> Document::textElements() const
{
    return m_textElems;
}

std::string Document::title() const
{
    std::string result;

    myhtml_collection_t* collection = myhtml_get_nodes_by_tag_id(m_tree, nullptr, MyHTML_TAG_TITLE, nullptr);
    if (collection && collection->list && collection->length)
    {
        myhtml_tree_node_t* textNode = myhtml_node_child(collection->list[0]);

        if (textNode)
        {
            const char* text = myhtml_node_text(textNode, nullptr);
            if (text)
                result = text;
        }
        myhtml_collection_destroy(collection);
    }
    return result;
}

// ==============================================

NodeRef::NodeRef(myhtml_tree_node_t* node)
    : m_node(node)
{
    if (!m_node)
        throw std::invalid_argument("nullptr arg in NodeRef ctor");
}

std::string NodeRef::text() const
{
    std::string result;
    const char* p;
    if (tag() == Tag::Text && (p = myhtml_node_text(m_node, nullptr)))
        result = p;
    return result;
}

std::string NodeRef::textSubnodes() const
{
    std::ostringstream oss;
    std::stack<NodeRef> s;
    s.push(*this);

    while (!s.empty())
    {
        NodeRef node = s.top();
        s.pop();

        if (node.tag() == Tag::Text)
            oss << node.text();

        auto ch = node.children();
        for (auto it = ch.rbegin(); it != ch.rend(); ++it)
            s.push(*it);
    }
    return oss.str();
}

std::string NodeRef::attributeByKey(const std::string& key) const
{
    std::string result;
    myhtml_tree_attr_t* attr = myhtml_attribute_by_key(m_node, key.c_str(), key.size());
    if (attr) {
        const char* val = myhtml_attribute_value(attr, nullptr);
        if (val)
            result = val;
    }
    return result;
}

Tag NodeRef::tag() const
{
    Tag result = Tag::Other;

    switch (myhtml_node_tag_id(m_node))
    {
        case MyHTML_TAG_H1:
        case MyHTML_TAG_H2:
        case MyHTML_TAG_H3:
        case MyHTML_TAG_H4:
        case MyHTML_TAG_H5:
        case MyHTML_TAG_H6:     result = Tag::H; break;
        case MyHTML_TAG__TEXT:  result = Tag::Text; break;
        case MyHTML_TAG_BR:     result = Tag::Br; break;
        case MyHTML_TAG_P:      result = Tag::P; break;
        case MyHTML_TAG_A:      result = Tag::A; break;
        case MyHTML_TAG_IMG:    result = Tag::Img; break;
        case MyHTML_TAG_SCRIPT: result = Tag::Script; break;
        case MyHTML_TAG_STYLE:  result = Tag::Style; break;
        default: break;
    }
    return result;
}

std::vector<NodeRef> NodeRef::children() const
{
    std::vector<NodeRef> children;

    for (auto* ch = myhtml_node_child(m_node); ch; ch = myhtml_node_next(ch))
        children.emplace_back(ch);

    return children;
}

NodeRef NodeRef::parent() const
{
    myhtml_tree_node_t* parent = myhtml_node_parent(m_node);
    return NodeRef(parent);
}

}
