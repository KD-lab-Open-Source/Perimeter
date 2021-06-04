#ifndef BOOST_GRAPH_YYSTYPE_H
#define BOOST_GRAPH_YYSTYPE_H

union yystype {
  int         i;
  void*       ptr;
};
#define YYSTYPE yystype

#endif // BOOST_GRAPH_YYSTYPE_H
