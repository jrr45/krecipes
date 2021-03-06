<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html"/>
  <xsl:param name="photoDir">krecipes_recipe_view_photos</xsl:param>
  <xsl:param name="cssStyle">style.css</xsl:param>
  <xsl:template match="/">
    <html>
      <head>
        <title>
          <xsl:choose>
            <xsl:when test="count(krecipes-description/title) &gt; 1">Krecipes</xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="krecipes-description/title"/>
            </xsl:otherwise>
          </xsl:choose>
        </title>
        <link rel="stylesheet" type="text/css" href="{$cssStyle}"/>
      </head>
      <body>
        <xsl:for-each select="krecipes/krecipes-recipe">
          <table width="100%" class="background">
            <tr><td valign="top">
                <xsl:variable name="photoSrc">
                  <xsl:choose>
                    <xsl:when test="count(krecipes-description/pictures/pic) &gt; 0"><xsl:value-of select="@id"/>.png</xsl:when>
                    <xsl:otherwise>default_photo.png</xsl:otherwise>
                  </xsl:choose>
                </xsl:variable>
                <xsl:if test="$photoSrc != 'default_photo.png'">
                  <img src="{$photoDir}/{$photoSrc}" class="photo"/>
                </xsl:if>
                <div class="title">
                  <xsl:value-of select="krecipes-description/title"/>
                </div>
                <xsl:if test="count(krecipes-ratings/rating/criterion/criteria/stars) &gt; 0">
                  <p class="overall-rating"><xsl:element name="img"><xsl:attribute name="src"><xsl:value-of select="$photoDir"/>/<xsl:value-of select="round(2*sum(krecipes-ratings/rating/criterion/criteria/stars) div count(krecipes-ratings/rating/criterion/criteria/stars)) div 2"/>-stars.png</xsl:attribute>
       <xsl:attribute name="height">12</xsl:attribute></xsl:element>
        (<xsl:value-of select="count(krecipes-ratings/rating)"/><xsl:text> </xsl:text><xsl:value-of select="$I18N_REVIEWS"/>)</p>
                </xsl:if>
                <xsl:if test="count(krecipes-description/author) &gt; 0
                              or count(krecipes-description/category/cat) &gt; 0
                              or krecipes-description/yield/amount != 0
                              or krecipes-description/preparation-time/text() != '00:00'">
                <div class="basic-info">
                <xsl:if test="count(krecipes-description/author) &gt; 0">
                  <span class="authors">
                    <span class="header"><xsl:value-of select="$I18N_AUTHORS"/>: </span>
                    <xsl:for-each select="krecipes-description/author">
                      <span class="author">
                        <xsl:value-of select="text()"/>
                      </span>
                      <xsl:if test="last() != position()">, </xsl:if>
                    </xsl:for-each>
                  </span>
                </xsl:if>
                <xsl:if test="count(krecipes-description/category/cat) &gt; 0">
                  <span class="categories">
                    <span class="header"><xsl:value-of select="$I18N_CATEGORIES"/>: </span>
                    <xsl:for-each select="krecipes-description/category/cat">
                      <span class="category">
                        <xsl:value-of select="text()"/>
                      </span>
                      <xsl:if test="last() != position()">, </xsl:if>
                    </xsl:for-each>
                  </span>
                </xsl:if>
                <xsl:if test="krecipes-description/yield and krecipes-description/yield/amount != 0">
                  <span class="yield">
                    <span class="header"><xsl:value-of select="$I18N_YIELD"/>: </span>
                    <span class="amount">
                      <xsl:choose>
                        <xsl:when test="krecipes-description/yield/amount/min">
                          <xsl:value-of select="krecipes-description/yield/amount/min"/>
                            -
                          <xsl:value-of select="krecipes-description/yield/amount/max"/>
                        </xsl:when>
                        <xsl:otherwise>
                          <xsl:value-of select="krecipes-description/yield/amount"/>
                        </xsl:otherwise>
                      </xsl:choose>
                      <xsl:text> </xsl:text>
                    </span>
                    <xsl:if test="krecipes-description/yield/type">
                      <span class="type">
                        <xsl:value-of select="krecipes-description/yield/type"/>
                      </span>
                    </xsl:if>
                  </span>
                </xsl:if>
                <xsl:if test="krecipes-description/preparation-time and krecipes-description/preparation-time/text() != '00:00'">
                  <span class="prep-time">
                    <span class="header"><xsl:value-of select="$I18N_PREP"/>: </span>
                    <xsl:value-of select="krecipes-description/preparation-time"/>
                  </span>
                </xsl:if>
                </div>
                </xsl:if>
            <xsl:if test="count(krecipes-ingredients/*) &gt; 0">
                  <div class="ingredients">
                    <h1><xsl:value-of select="$I18N_INGREDIENTS"/></h1>
                    <table width="100%">
                      <xsl:for-each select="krecipes-ingredients">
                            <xsl:call-template name="ingredients"/>
                      </xsl:for-each>

                      <xsl:for-each select="krecipes-ingredients/ingredient-group">
                                <tr><td colspan="2"><div class="ingredient-group">
                                  <xsl:value-of select="@name"/>
                                </div></td></tr>
                              <xsl:call-template name="ingredients"/>
                      </xsl:for-each>
                    </table>
                  </div>
            </xsl:if>
            <xsl:if test="krecipes-instructions">
                <div class="instructions">
                  <h1><xsl:value-of select="$I18N_INSTRUCTIONS"/></h1>
                  <p>
                    <xsl:call-template name="br-replace">
                      <xsl:with-param name="word" select="krecipes-instructions"/>
                    </xsl:call-template>
                  </p>
                </div>
            </xsl:if>
            <xsl:if test="count(krecipes-properties/property[not(@hidden='true')]) > 0">
                  <div class="properties">
                    <h1><xsl:value-of select="$I18N_PROPERTIES"/></h1>
                    <xsl:call-template name="properties"/>
                  </div>
            </xsl:if>
            <xsl:if test="count(krecipes-ratings/rating) &gt; 0">
                <div class="ratings">
                  <h1><xsl:value-of select="$I18N_RATINGS"/></h1>
                  <xsl:for-each select="krecipes-ratings/rating">
                    <span class="rater">
                      <xsl:value-of select="rater"/>
                    </span>
                    <xsl:if test="count(criterion/criteria) &gt; 0">
                      <table>
                        <xsl:for-each select="criterion/criteria">
                          <tr>
                            <td>
                              <span class="criteria-name">
                                <xsl:value-of select="name"/>
                              </span>
                            </td>
                            <td>
                              <xsl:element name="img">
                                <xsl:attribute name="src"><xsl:value-of select="$photoDir"/>/<xsl:value-of select="stars"/>-stars.png</xsl:attribute><xsl:attribute name="height">12</xsl:attribute>
                              </xsl:element>
                            </td>
                          </tr>
                        </xsl:for-each>
                      </table>
                    </xsl:if>
                    <div class="comments">
                      <xsl:value-of select="comment"/>
                    </div>
                    <xsl:if test="position() != last()">
                      <br /><br />
                    </xsl:if>
                  </xsl:for-each>
                </div>
            </xsl:if>
            </td></tr>
          </table>
        </xsl:for-each>
      </body>
    </html>
  </xsl:template>
  <xsl:template name="ingredient">
    <xsl:param name="ingSub"/>
    <xsl:param name="ing"/>
        <xsl:if test="$ingSub"><br /> <xsl:value-of select="$I18N_OR"/><xsl:text> </xsl:text> </xsl:if>
        <xsl:if test="$ing/amount/max != 0 or $ing/amount != 0">
          <xsl:choose>
            <xsl:when test="$ing/amount/min"><xsl:value-of select="$ing/amount/min"/>-<xsl:value-of select="$ing/amount/max"/><xsl:text> </xsl:text></xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$ing/amount"/><xsl:text> </xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
        <xsl:value-of select="$ing/unit"/><xsl:text> </xsl:text>
        <xsl:value-of select="$ing/name"/>
        <xsl:for-each select="$ing/prep">
          <xsl:if test="position() = 1">; </xsl:if>
          <xsl:value-of select="text()"/>
          <xsl:if test="position() != last()">, </xsl:if>
        </xsl:for-each>
    <xsl:if test="count($ing/substitutes/ingredient) &gt; 0">
      <xsl:for-each select="$ing/substitutes/ingredient">
        <xsl:call-template name="ingredient">
          <xsl:with-param name="ingSub"><xsl:value-of select="$I18N_OR"/><xsl:text> </xsl:text> </xsl:with-param>
          <xsl:with-param name="ing" select="."/>
        </xsl:call-template>
      </xsl:for-each>
    </xsl:if>
  </xsl:template>
  <xsl:template name="br-replace">
    <xsl:param name="word">
      <xsl:value-of select="."/>
    </xsl:param>
    <xsl:param name="recursing"/>
    <!-- </xsl:text> on next line on purpose to get newline -->
    <xsl:variable name="cr">
      <xsl:text>
</xsl:text>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="contains($word,$cr)">
        <xsl:if test="substring-before($word,$cr) or $recursing">
          <xsl:value-of select="substring-before($word,$cr)"/>
          <br/>
        </xsl:if>
        <xsl:call-template name="br-replace">
          <xsl:with-param name="word" select="substring-after($word,$cr)"/>
          <xsl:with-param name="recursing" select="'1'"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$word"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="properties">
    <xsl:variable name="visible-properties" select="krecipes-properties/property[not(@hidden='true')]"/>
    <xsl:variable name="t-size" select="count($visible-properties)"/>
    <xsl:variable name="half" select="ceiling($t-size div 3)"/>
    <table width="100%">
      <xsl:for-each select="$visible-properties[position() &lt;= $half]">
        <xsl:variable name="here" select="position()"/>
        <tr>
          <td width="33%">
            <li><xsl:value-of select="name"/>: <xsl:value-of select="amount"/><xsl:value-of select="units"/></li>
          </td>
          <td width="33%">
            <xsl:choose>
              <xsl:when test="$visible-properties[$here+$half]">
                <li><xsl:value-of select="$visible-properties[$here+$half]/name"/>: <xsl:value-of select="$visible-properties[$here+$half]/amount"/><xsl:value-of select="$visible-properties[$here+$half]/units"/></li>
              </xsl:when>
              <xsl:otherwise/>
            </xsl:choose>
          </td>
          <td width="33%">
            <xsl:choose>
              <xsl:when test="$visible-properties[$here+$half*2]">
                <li><xsl:value-of select="$visible-properties[$here+$half*2]/name"/>: <xsl:value-of select="$visible-properties[$here+$half*2]/amount"/><xsl:value-of select="$visible-properties[$here+$half*2]/units"/></li>
              </xsl:when>
              <xsl:otherwise/>
            </xsl:choose>
          </td>
        </tr>
      </xsl:for-each>
    </table>
  </xsl:template>
  <xsl:template name="ingredients">
    <xsl:variable name="visible-properties" select="ingredient"/>
    <xsl:variable name="t-size" select="count($visible-properties)"/>
    <xsl:variable name="half" select="ceiling($t-size div 2)"/>
      <xsl:for-each select="$visible-properties[position() &lt;= $half]">
        <xsl:variable name="here" select="position()"/>
        <tr>
          <td width="50%">
            <xsl:call-template name="ingredient"><xsl:with-param name="ing" select="."/></xsl:call-template>
          </td>
          <td width="50%">
            <xsl:choose>
              <xsl:when test="$visible-properties[$here+$half]">
                <xsl:call-template name="ingredient"><xsl:with-param name="ing" select="$visible-properties[$here+$half]"/></xsl:call-template>
              </xsl:when>
              <xsl:otherwise/>
            </xsl:choose>
          </td>
        </tr>
      </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>
