/**
 *	"Procedural 3D Texture Library"
 *	
 *	Utility methods for parsing XML and serializing "tx3d" library objects to XML
 *
 *	@version $Id: XMLUtils.h,v 1.1 2002/11/05 17:36:19 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_XMLUTILS_H
#define _TX3D_XMLUTILS_H

#include "Vector3D.hpp"
#include "Serializable.hpp"

namespace tx3d {

	class XMLUtils {
		public :
//------"serializing"-methods-------------
			static string floatToString(float d);
			static string intToString(int i);
			static string vectorToString(const Vector3D& v);

			static string createTag( const string& tagName,
									 const string& propertyName = "",
									 const string& value = "",
									 const string& content = "" );

			static string vectorToTag(const string& tagName, const Vector3D& v);
			static string floatToTag(const string& tagName, float d);
			static string intToTag(const string& tagName, int i);

			static string serializableToTag(const string& tagName, const Serializable& s);

//------"parsing"-methods-------------
			static Vector3D vectorFromString(const string& v);

			static string extractPropertyFromTag(const string& propertyName, const string& tag);
			static string extractContentFromTag(const string& tag);

			static string typeFromTag(const string& tag);
			static Vector3D vectorFromTag(const string& tag);
			static float floatFromTag(const string& tag);
			static int intFromTag(const string& tag);

			static bool isTagHasName(const string& tag, const string& tagName);

			static string extractTagFromXML( const string& tagName,
											 int orderNumber,
											 const string& xml );

			static void indentXML(string& xml);

//------constants-----------------
			static const string VALUE;
			static const string TYPE;

		private:
			XMLUtils();
	};

}

#endif