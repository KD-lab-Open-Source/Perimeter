#ifndef _NODE_H
#define _NODE_H

#include <Texture3D.hpp>
#include <XMLUtils.hpp>

class Node {

	public :
		Node(const string& paramString);
		virtual ~Node() {
		}
	
		string getTitle() const {
			return title;
		}

		static string loadXML(const string& paramString, const string& tagName);


	protected :
		Node() {
		}

		static const string TITLE;

		string title;
};

#endif // _NODE_H