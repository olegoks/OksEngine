//#include <clang-c/Index.h>
//
//#include <iostream>
//
//void printCursorInfo(CXCursor cursor) {
//    CXString cursorSpelling = clang_getCursorSpelling(cursor);
//    std::cout << "Cursor: " << clang_getCString(cursorSpelling) << std::endl;
//    clang_disposeString(cursorSpelling);
//}
//
//CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData clientData) {
//    
//    
//    if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl) {
//        printCursorInfo(cursor);
//    }
//    return CXChildVisit_Recurse;
//}
//
//void parseFile(const char* filename) {
//    CXIndex index = clang_createIndex(0, 0);
//    CXTranslationUnit unit = clang_parseTranslationUnit(
//        index,
//        filename, nullptr, 0,
//        nullptr, 0,
//        CXTranslationUnit_None);
//        
//    if (unit == nullptr) {
//        std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
//        return;
//    }
//    
//    CXCursor rootCursor = clang_getTranslationUnitCursor(unit);
//    clang_visitChildren(rootCursor, visitor, nullptr);
//    
//    clang_disposeTranslationUnit(unit);
//    clang_disposeIndex(index);
//}
/// @file
/// @brief creates, renders simple graph with @ref gvLayoutJobs,
/// @ref gvRenderJobs

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

 /* Note that, with the call to gvParseArgs(), this application assumes that
  * a known layout algorithm is going to be specified. This can be done either
  * using argv[0] or requiring the user to run this code with a -K flag
  * specifying which layout to use. In the former case, after this program has
  * been built as 'demo', you will need to rename it as one of the installed
  * layout engines such as dot, neato, sfdp, etc.
  */
//#include <graphviz/gvc.h>

int main(int argc, char** argv) {

	//// set up a graphviz context
	//GVC_t* gvc = gvContext();

	//// parse command line args - minimally argv[0] sets layout engine
	//gvParseArgs(gvc, argc, argv);

	//// Create a simple digraph
	//Agraph_t* g = agopen(const_cast<char*>("g"), Agdirected, 0);
	//Agnode_t* n = agnode(g, const_cast<char*>("n"), 1);
	//Agnode_t* m = agnode(g, const_cast<char*>("m"), 1);
	//Agedge_t* e = agedge(g, n, m, 0, 1);

	//// Set an attribute - in this case one that affects the visible rendering
	//agsafeset(n, const_cast<char*>("color"), "red", "");

	//// Compute a layout using layout engine from command line args
	//gvLayoutJobs(gvc, g);

	//// Write the graph according to -T and -o options
	//gvRenderJobs(gvc, g);

	//// Free layout data
	//gvFreeLayout(gvc, g);

	//// Free graph structures
	//agclose(g);

	//// close output file, free context, and return number of errors
	//return gvFreeContext(gvc);
	return 0;
}
