<?xml version="1.0"?>

<!--
  Copyright (c) 2012 Cycling '74

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
  and associated documentation files (the "Software"), to deal in the Software without restriction, 
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies 
  or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:strip-space elements="*"/>

  <xsl:variable name="doctype" select="'vignette'"/>

  <xsl:include href='_c74_platform.xsl'/>

  <xsl:variable name="jargon_enable">
    <xsl:choose>
      <xsl:when test="$compliant=1">
        <xsl:value-of select="document('_c74_common.xml')/root/setting[@name='jargon_enable']/@value"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="document('_c74_common.xml')/root/setting[@name='jargon_enable']/@value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="jargon_mouseover">
    <xsl:choose>
      <xsl:when test="$compliant=1">
        <xsl:value-of select="document('_c74_common.xml')/root/setting[@name='jargon_mouseover']/@value"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="document('_c74_common.xml')/root/setting[@name='jargon_mouseover']/@value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="refs_root_loc">
    <xsl:choose> 
      <xsl:when test="$compliant=1"> 
        <xsl:value-of select="'./refpages'"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="'refpages'"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="refs_root_link" select="'refpages'"/> <!-- relative to calling XML page -->
  <xsl:variable name="patchname" /> <!-- for openfilefront -->
  <xsl:variable name="patchfile" /> <!-- for openfilefront -->

  <xsl:template match="/">
    <html>
      <head>
        <link rel="stylesheet" type="text/css" href="_c74_common.css"/>
        <title><xsl:value-of select="/vignette/@name"/></title>
      </head>
      <body>
<!-- 
      Go and apply all the templates for the root
      -->
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

<!--
      Body styles
      -->

  <xsl:template match="openpatchlink">
    <xsl:if test="$patchfile != ''">
      <a>
        <xsl:attribute name="href">
          <xsl:value-of select="concat('max:openfilefront/', $patchname, '/', $patchfile)"/>
        </xsl:attribute>
        <xsl:apply-templates />
      </a>
    </xsl:if>
  </xsl:template>

  <!--
        Body styles
        -->

    <xsl:template match="vignette">
      <div class="basestyle">
        <xsl:apply-templates/>
      </div>
    </xsl:template>

  <!-- 
        markup elements
        -->
    <xsl:template match="o">
      <xsl:call-template name="object_link">
        <xsl:with-param name="name" select="."/>
        <xsl:with-param name="objname" select="@objname"/>
      </xsl:call-template>
    </xsl:template>

    <xsl:template match="intro">
      <h2>Introduction</h2>
      <p>
      <xsl:apply-templates/>
      </p>
    </xsl:template>
    
    <xsl:template match="chapter">
      <h2><xsl:value-of select="@name"/></h2>
      <p>
      <xsl:apply-templates/>
      </p>
    </xsl:template>
    
    <xsl:template name="lastword">
      <xsl:param name="in" />
      <xsl:choose>
        <xsl:when test="substring-after($in, ' ')!=''">
          <xsl:call-template name="lastword">
            <xsl:with-param name="in" select="substring-after($in, ' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="$in"/></xsl:otherwise>
      </xsl:choose>
    </xsl:template>
    

  <xsl:template match="liveapi_object">
    <div id="liveapi_section">
      <h3>
        <a>
          <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
          <xsl:value-of select="@name"/>
        </a>
      </h3>
      <p>
      <span class="description">
        <xsl:choose>
          <xsl:when test="description and normalize-space(description) != '' and normalize-space(description) != 'TEXT_HERE'">
            <xsl:apply-templates select="description"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="digest"/> 
          </xsl:otherwise>
        </xsl:choose>
      </span>
      <div id="liveapi_section">
      <xsl:apply-templates select="liveapi_path"/>
      <xsl:apply-templates select="liveapi_children"/>
      <xsl:apply-templates select="liveapi_properties"/>
      <xsl:apply-templates select="liveapi_functions"/>
      </div>
      </p>
    </div>
  </xsl:template>

  <xsl:template match="liveapi_path">
    <xsl:param name="path" />
    <h4>Canonical path</h4>
    <span class="path">
      <xsl:value-of select="@path"/>
    </span>
  </xsl:template>

  <xsl:template name="liveapi_typelink">
    <xsl:param name="type" />
    <xsl:variable name="trim">
      <xsl:call-template name="lastword">
        <xsl:with-param name="in" select="$type"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="substring-before($type, $trim)" />
    <a>
      <xsl:attribute name="href"><xsl:value-of select="concat('#', $trim)"/></xsl:attribute>
      <xsl:value-of select="$trim"/>
    </a>
  </xsl:template>

  <xsl:template match="liveapi_children">
    <h4>Children</h4>
    <xsl:choose>
      <xsl:when test="count(liveapi_child) > 0">
        <table>
          <tr>
            <th>Name</th>
            <th>Type</th>
            <th width="14%">Access</th>
            <th width="50%">Description</th>
          </tr>
          <xsl:for-each select="liveapi_child">
            <tr>
              <td><xsl:value-of select="@name"/></td>
              <td>
                <xsl:call-template name="liveapi_typelink">
                <xsl:with-param name="type" select="@type"/>
                </xsl:call-template>
              </td>
              <td>
                <xsl:choose>
                  <xsl:when test="@gso='gso'">get, set, observe</xsl:when>
                  <xsl:when test="@gso='gs-'">get, set</xsl:when>
                  <xsl:when test="@gso='g-o'">get, observe</xsl:when>
                  <xsl:when test="@gso='-so'">set, observe</xsl:when>
                  <xsl:when test="@gso='g--'">get</xsl:when>
                  <xsl:when test="@gso='-s-'">set</xsl:when>
                  <xsl:when test="@gso='--o'">observe</xsl:when>
                  <xsl:otherwise>none</xsl:otherwise>
                </xsl:choose>
              </td>
              <td class="description">
                <xsl:choose>
                  <xsl:when test="description and normalize-space(description) != '' and normalize-space(description) != 'TEXT_HERE'">
                    <xsl:apply-templates select="description"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:apply-templates select="digest"/> 
                  </xsl:otherwise>
                </xsl:choose>
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </xsl:when>
      <xsl:otherwise>
        <div class="none">
          None
        </div>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="liveapi_properties">
    <h4>Properties</h4>
    <xsl:choose>
      <xsl:when test="count(liveapi_property) > 0">
        <table>
          <tr>
            <th>Name</th>
            <th>Type</th>
            <th width="14%">Access</th>
            <th width="50%">Description</th>
          </tr>
          <xsl:for-each select="liveapi_property">
            <tr>
              <td><xsl:value-of select="@name"/></td>
              <td>
                <xsl:call-template name="liveapi_typelink">
                  <xsl:with-param name="type" select="@type"/>
                </xsl:call-template>
              </td>
              <td>
                <xsl:choose>
                  <xsl:when test="@gso='gso'">get, set, observe</xsl:when>
                  <xsl:when test="@gso='gs-'">get, set</xsl:when>
                  <xsl:when test="@gso='g-o'">get, observe</xsl:when>
                  <xsl:when test="@gso='-so'">set, observe</xsl:when>
                  <xsl:when test="@gso='g--'">get</xsl:when>
                  <xsl:when test="@gso='-s-'">set</xsl:when>
                  <xsl:when test="@gso='--o'">observe</xsl:when>
                  <xsl:otherwise>none</xsl:otherwise>
                </xsl:choose>
              </td>
              <td class="description">
                <xsl:choose>
                  <xsl:when test="description and normalize-space(description) != '' and normalize-space(description) != 'TEXT_HERE'">
                    <xsl:apply-templates select="description"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:apply-templates select="digest"/> 
                  </xsl:otherwise>
                </xsl:choose>
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </xsl:when>
      <xsl:otherwise>
        <div class="none">
          None
        </div>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="liveapi_functions">
      <h4>Functions</h4>
      <table>
        <tr>
          <th>Name</th>
          <th width="50%">Description</th>
        </tr>
      <xsl:for-each select="liveapi_function">
        <tr>
          <td><xsl:value-of select="@name"/></td>
          <td class="description">
            <xsl:choose>
              <xsl:when test="description and normalize-space(description) != '' and normalize-space(description) != 'TEXT_HERE'">
                <xsl:apply-templates select="description"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:apply-templates select="digest"/> 
              </xsl:otherwise>
            </xsl:choose>
          </td>
        </tr>
      </xsl:for-each>
    </table>
  </xsl:template>

</xsl:stylesheet>
