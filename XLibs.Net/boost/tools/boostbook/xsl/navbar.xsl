<?xml version = "1.0" encoding = "utf-8"?>

<xsl:stylesheet version = "1.0"
   xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
>
   <!--
      how to render the Home | Libraries | ... | More contents
         none       - do not display ("standalone" mode)
        *horizontal - display in old-Boost style format
         vertical   - like the new Getting Started layout
   -->
   <xsl:param name = "nav.layout" select = "'horizontal'"/>

   <!--
      header border layout
         Boost - place the old-Boost border around the header
        *none  - do not place a border around the header
   -->
   <xsl:param name = "nav.border" select = "'none'" />

   <!--
      nav.flow:
         none    - do not display navigation at the header
         DocBook - display the navigation after the header
        *Spirit  - display "mini" navigation on the right
   -->
   <xsl:param name = "nav.flow" select = "'Spirit'"/>

   <!-- location of the various Boost elements -->

   <xsl:param name = "boost.image.src" select = "'../../c++boost.gif'"/>
   <xsl:param name = "boost.image.alt" select = "'c++boost.gif (8819 bytes)'"/>
   <xsl:param name = "boost.image.w"   select = "277"/>
   <xsl:param name = "boost.image.h"   select = "86"/>
   <xsl:param name = "boost.root"      select = "'../..'"/>
   <xsl:param name = "boost.libraries" select = "'libraries.html'"/>

   <!-- header -->

   <xsl:template name = "header.navigation">
      <xsl:param name = "prev" select = "/foo"/>
      <xsl:param name = "next" select = "/foo"/>
      <xsl:param name = "nav.context"/>

      <xsl:variable name = "home" select = "/*[1]"/>
      <xsl:variable name = "up"   select = "parent::*"/>

      <table cellpadding = "2" width = "100%">
         <xsl:if test = "$nav.border = 'Boost'">
            <xsl:attribute name = "class">boost-head</xsl:attribute>
         </xsl:if>

         <td valign = "top">
            <xsl:if test = "$nav.border = 'Boost'">
               <xsl:attribute name = "style">background-color: white; width: 50%;</xsl:attribute>
            </xsl:if>
            <img src = "{$boost.image.src}" alt = "{$boost.image.alt}"
                 width = "{$boost.image.w}" height = "{$boost.image.h}"
            />
         </td><xsl:choose>
            <xsl:when test = "$nav.layout = 'horizontal'">
               <xsl:call-template name = "header.navdata-horiz"/>
            </xsl:when><xsl:when test = "$nav.layout = 'vertical'">
               <xsl:call-template name = "header.navdata-vert"/>
            </xsl:when>
         </xsl:choose>
      </table>
      <hr/>
      <xsl:choose>
         <xsl:when test = "$nav.flow = 'DocBook'">
            <table width = "100%" class = "navheader">
               <xsl:call-template name = "navbar.docbook-homeinfo">
                  <xsl:with-param name = "prev" select = "$prev"/>
                  <xsl:with-param name = "next" select = "$next"/>
                  <xsl:with-param name = "nav.context" select = "$nav.context"/>
               </xsl:call-template>
               <xsl:call-template name = "navbar.docbook-prevnext">
                  <xsl:with-param name = "prev" select = "$prev"/>
                  <xsl:with-param name = "next" select = "$next"/>
                  <xsl:with-param name = "nav.context" select = "$nav.context"/>
               </xsl:call-template>
            </table>
         </xsl:when><xsl:when test = "$nav.flow = 'Spirit'">
            <xsl:call-template name = "navbar.spirit">
               <xsl:with-param name = "prev" select = "$prev"/>
               <xsl:with-param name = "next" select = "$next"/>
               <xsl:with-param name = "nav.context" select = "$nav.context"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:template>

   <xsl:template name = "header.navdata-horiz">
      <xsl:choose>
         <xsl:when test = "$nav.border = 'Boost'">
            <td align = "center" class = "boost-headtd">
               <a href = "{$boost.root}/index.htm" class = "boost-headelem">Home</a>
            </td><td align = "center" class = "boost-headtd">
               <a href = "{$boost.libraries}" class = "boost-headelem">Libraries</a>
            </td><td align = "center" class = "boost-headtd">
               <a href = "{$boost.root}/people/people.htm" class = "boost-headelem">People</a>
            </td><td align = "center" class = "boost-headtd">
               <a href = "{$boost.root}/more/faq.htm" class = "boost-headelem">FAQ</a>
            </td><td align = "center" class = "boost-headtd">
               <a href = "{$boost.root}/more/index.htm" class = "boost-headelem">More</a>
            </td>
         </xsl:when><xsl:otherwise>
            <td align = "center">
               <a href = "{$boost.root}/index.htm">Home</a>
            </td><td align = "center">
               <a href = "{$boost.libraries}">Libraries</a>
            </td><td align = "center">
               <a href = "{$boost.root}/people/people.htm">People</a>
            </td><td align = "center">
               <a href = "{$boost.root}/more/faq.htm">FAQ</a>
            </td><td align = "center">
               <a href = "{$boost.root}/more/index.htm">More</a>
            </td>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:template>

   <xsl:template name = "header.navdata-vert">
      <td><div>
         <xsl:if test = "$nav.border != 'Boost'">
            <xsl:attribute name = "class">boost-toc</xsl:attribute>
         </xsl:if>
         <div><a href = "{$boost.root}/index.htm">Home</a></div>
         <div><a href = "{$boost.libraries}">Libraries</a></div>
         <div><a href = "{$boost.root}/people/people.htm">People</a></div>
         <div><a href = "{$boost.root}/more/faq.htm">FAQ</a></div>
         <div><a href = "{$boost.root}/more/index.htm">More</a></div>
      </div></td>
   </xsl:template>

   <!-- footer -->

   <xsl:template name = "footer.navigation">
      <xsl:param name = "prev" select = "/foo"/>
      <xsl:param name = "next" select = "/foo"/>
      <xsl:param name = "nav.context"/>

      <hr/>
      <xsl:choose>
         <xsl:when test = "$nav.flow = 'DocBook'">
            <table width = "100%" class = "navheader">
               <xsl:call-template name = "navbar.docbook-prevnext">
                  <xsl:with-param name = "prev" select = "$prev"/>
                  <xsl:with-param name = "next" select = "$next"/>
                  <xsl:with-param name = "nav.context" select = "$nav.context"/>
               </xsl:call-template>
               <xsl:call-template name = "navbar.docbook-homeinfo">
                  <xsl:with-param name = "prev" select = "$prev"/>
                  <xsl:with-param name = "next" select = "$next"/>
                  <xsl:with-param name = "nav.context" select = "$nav.context"/>
               </xsl:call-template>
            </table>
         </xsl:when><xsl:when test = "$nav.flow = 'Spirit'">
            <xsl:call-template name = "navbar.spirit">
               <xsl:with-param name = "prev" select = "$prev"/>
               <xsl:with-param name = "next" select = "$next"/>
               <xsl:with-param name = "nav.context" select = "$nav.context"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:template>

   <!-- navbar -->

   <xsl:template name = "navbar.docbook-homeinfo">
      <xsl:param name = "prev" select = "/foo"/>
      <xsl:param name = "next" select = "/foo"/>
      <xsl:param name = "nav.context"/>

      <xsl:variable name = "home" select = "/*[1]"/>
      <tr>
         <td align = "left" width = "40%">
            <xsl:if test = "$navig.showtitles != 0"> <!-- prev:name -->
               <xsl:apply-templates select = "$prev" mode = "object.title.markup"/>
            </xsl:if>
         </td><td align = "center" width = "20%">
            <!-- home -->
            <xsl:if test = "$home != . or $nav.context = 'toc'">
               <a accesskey = "h">
                  <xsl:attribute name = "href"><xsl:call-template name = "href.target">
                     <xsl:with-param name = "object" select = "$home"/>
                  </xsl:call-template></xsl:attribute>
                  <xsl:call-template name = "navig.content">
                     <xsl:with-param name = "direction" select = "'home'"/>
                  </xsl:call-template>
               </a>
               <xsl:if test = "$chunk.tocs.and.lots != 0 and $nav.context != 'toc'">
                  <xsl:text>|</xsl:text>
               </xsl:if>
            </xsl:if>
            <xsl:if test = "$chunk.tocs.and.lots != 0 and $nav.context != 'toc'"><a accesskey = "t">
               <xsl:attribute name = "href">
                  <xsl:apply-templates select = "/*[1]" mode = "recursive-chunk-filename"/>
                  <xsl:text>-toc</xsl:text>
                  <xsl:value-of select = "$html.ext"/>
               </xsl:attribute>
               <xsl:call-template name = "gentext">
                  <xsl:with-param name = "key" select = "'nav-toc'"/>
               </xsl:call-template>
            </a></xsl:if>
         </td><td align = "right" width = "40%">
            <xsl:if test = "$navig.showtitles != 0"> <!-- next:name -->
               <xsl:apply-templates select = "$next" mode = "object.title.markup"/>
            </xsl:if>
         </td>
      </tr>
   </xsl:template>

   <xsl:template name = "navbar.docbook-prevnext">
      <xsl:param name = "prev" select = "/foo"/>
      <xsl:param name = "next" select = "/foo"/>
      <xsl:param name = "nav.context"/>

      <xsl:variable name = "up" select = "parent::*"/>
      <tr>
         <td align = "left" width = "40%">
            <xsl:if test = "count($prev)>0"><a accesskey = "p"> <!-- prev -->
               <xsl:attribute name = "href"><xsl:call-template name = "href.target">
                   <xsl:with-param name = "object" select = "$prev"/>
               </xsl:call-template></xsl:attribute>
               <xsl:call-template name = "navig.content">
                  <xsl:with-param name = "direction" select = "'prev'"/>
               </xsl:call-template>
            </a></xsl:if>
         </td><td align = "center" width = "20%">
            <xsl:if test = "count($up)>0"><a accesskey = "u"> <!-- up -->
               <xsl:attribute name = "href"><xsl:call-template name = "href.target">
                  <xsl:with-param name = "object" select = "$up"/>
               </xsl:call-template></xsl:attribute>
               <xsl:call-template name = "navig.content">
                  <xsl:with-param name = "direction" select = "'up'"/>
               </xsl:call-template>
            </a></xsl:if>
         </td><td align = "right" width = "40%">
            <xsl:if test = "count($next)>0"><a accesskey = "n"> <!-- next -->
               <xsl:attribute name = "href"><xsl:call-template name = "href.target">
                  <xsl:with-param name = "object" select = "$next"/>
               </xsl:call-template></xsl:attribute>
               <xsl:call-template name = "navig.content">
                  <xsl:with-param name = "direction" select = "'next'"/>
               </xsl:call-template>
            </a></xsl:if>
         </td>
      </tr>
   </xsl:template>

   <xsl:template name = "navbar.spirit">
      <xsl:param name = "prev" select = "/foo"/>
      <xsl:param name = "next" select = "/foo"/>
      <xsl:param name = "nav.context"/>

      <xsl:variable name = "home" select = "/*[1]"/>
      <xsl:variable name = "up"   select = "parent::*"/>

      <div class = "spirit-nav">
         <!-- prev -->
         <xsl:if test = "count($prev)>0"><a accesskey = "p">
            <xsl:attribute name = "href"><xsl:call-template name = "href.target">
               <xsl:with-param name = "object" select = "$prev"/>
            </xsl:call-template></xsl:attribute>
            <xsl:call-template name = "navig.content">
               <xsl:with-param name = "direction" select = "'prev'"/>
            </xsl:call-template>
         </a></xsl:if>
         <!-- up -->
         <xsl:if test = "count($up)>0"><a accesskey = "u">
            <xsl:attribute name = "href"><xsl:call-template name = "href.target">
               <xsl:with-param name = "object" select = "$up"/>
            </xsl:call-template></xsl:attribute>
            <xsl:call-template name = "navig.content">
               <xsl:with-param name = "direction" select = "'up'"/>
            </xsl:call-template>
         </a></xsl:if>
         <!-- home -->
         <xsl:if test = "$home != . or $nav.context = 'toc'">
            <a accesskey = "h">
               <xsl:attribute name = "href"><xsl:call-template name = "href.target">
                  <xsl:with-param name = "object" select = "$home"/>
               </xsl:call-template></xsl:attribute>
               <xsl:call-template name = "navig.content">
                  <xsl:with-param name = "direction" select = "'home'"/>
               </xsl:call-template>
            </a>
            <xsl:if test = "$chunk.tocs.and.lots != 0 and $nav.context != 'toc'">
               <xsl:text>|</xsl:text>
            </xsl:if>
         </xsl:if>
         <xsl:if test = "$chunk.tocs.and.lots != 0 and $nav.context != 'toc'"><a accesskey = "t">
            <xsl:attribute name = "href">
               <xsl:apply-templates select = "/*[1]" mode = "recursive-chunk-filename"/>
               <xsl:text>-toc</xsl:text>
               <xsl:value-of select = "$html.ext"/>
            </xsl:attribute>
            <xsl:call-template name = "gentext">
               <xsl:with-param name = "key" select = "'nav-toc'"/>
            </xsl:call-template>
         </a></xsl:if>
         <!-- next -->
         <xsl:if test = "count($next)>0"><a accesskey = "n">
            <xsl:attribute name = "href"><xsl:call-template name = "href.target">
               <xsl:with-param name = "object" select = "$next"/>
            </xsl:call-template></xsl:attribute>
            <xsl:call-template name = "navig.content">
               <xsl:with-param name = "direction" select = "'next'"/>
            </xsl:call-template>
         </a></xsl:if>
      </div>
   </xsl:template>
</xsl:stylesheet>