<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:in="http://www.accuweather.com" exclude-result-prefixes="in">
    <xsl:output method="xml" encoding="ISO-8859-1" indent="yes" />

    <!-- 
             This XSLT is used to translate an XML response from the accuweather.com
             XML API.
             
             created for vdr's weatherhd plugin
             by Alexander Weber <xweber@kamelfreunde.de>
    -->

    <xsl:template match="/in:adc_database">
        <weather ver="2.0">
            <xsl:call-template name="head" />
            <xsl:call-template name="loc" />
            <xsl:call-template name="cc" />
            <xsl:call-template name="dayf" />
        </weather>
    </xsl:template>

    <xsl:template name="head">
        <head>
            <locale>en_US</locale>
            <form>MEDIUM</form>
            <ut>
                <xsl:value-of select="in:units/in:temp" />
            </ut>
            <ud>
                <xsl:value-of select="in:units/in:dist" />
            </ud>
            <us>
                <xsl:value-of select="in:units/in:speed" />
            </us>
            <up>
                <xsl:value-of select="in:units/in:pres" />
            </up>
            <ur>
                <xsl:value-of select="in:units/in:prec" />
            </ur>
        </head>
    </xsl:template>

    <xsl:template name="loc">
        <loc id="accuweather">
            <dnam>
                <xsl:value-of select="in:local/in:city" />
            </dnam>
            <tm>
                <xsl:value-of select="in:local/in:time" />
            </tm>
            <lat>
                <xsl:value-of select="in:local/in:lat" />
            </lat>
            <lon>
                <xsl:value-of select="in:local/in:lon" />
            </lon>
            <sunr>
                <xsl:value-of select="in:planets/in:sun/@rise" />
            </sunr>
            <suns>
                <xsl:value-of select="in:planets/in:sun/@set" />
            </suns>
            <zone>
                <xsl:value-of select="in:local/in:gmtdiff" />
            </zone>
        </loc>
    </xsl:template>

    <xsl:template name="cc">
        <cc>
            <lsup />
            <obst>
                <xsl:value-of select="in:local/in:city" />
            </obst>
            <tmp>
                <xsl:value-of select="in:currentconditions/in:temperature" />
            </tmp>
            <flik>
                <xsl:value-of select="in:currentconditions/in:realfeel" />
            </flik>
            <t>
                <xsl:value-of select="in:currentconditions/in:weathertext" />
            </t>
            <icon>
                <xsl:call-template name="iconTransform">
                    <xsl:with-param name="iconnumber" select="in:currentconditions/in:weathericon"
                     />
                </xsl:call-template>
            </icon>
            <bar>
                <r>
                    <xsl:value-of select="in:currentconditions/in:pressure" />
                </r>
                <d>
                    <xsl:value-of select="in:currentconditions/in:pressure/@state" />
                </d>
            </bar>
            <wind>
                <s>
                    <xsl:value-of select="in:currentconditions/in:windspeed" />
                </s>
                <gust>
                    <xsl:value-of select="in:currentconditions/in:windgusts" />
                </gust>
                <d />
                <t>
                    <xsl:value-of select="in:currentconditions/in:winddirection" />
                </t>
            </wind>
            <hmid>
                <xsl:value-of select="in:currentconditions/in:humidity" />
            </hmid>
            <vis>
                <xsl:value-of select="in:currentconditions/in:visibility" />
            </vis>
            <uv>
                <i>
                    <xsl:value-of select="in:currentconditions/in:uvindex/@index" />
                </i>
                <t>
                    <xsl:value-of select="in:currentconditions/in:uvindex" />
                </t>
            </uv>
            <dewp />
            <moon>
                <icon>na</icon>
                <t />
            </moon>
        </cc>
    </xsl:template>

    <xsl:template name="dayf">
        <dayf>
            <lsup>
                <xsl:value-of select="in:forecast/in:day[@number='1']/in:obsdate" />
            </lsup>
            <xsl:for-each select="in:forecast/in:day">
                <xsl:call-template name="day" />
            </xsl:for-each>
        </dayf>
    </xsl:template>

    <xsl:template name="day">
        <day>
            <xsl:attribute name="d">
                <xsl:value-of select="@number - 1" />
            </xsl:attribute>
            <xsl:attribute name="t">
                <xsl:value-of select="in:daycode" />
            </xsl:attribute>
            <xsl:attribute name="dt">
                <!-- converts the date to german style -->
                <xsl:call-template name="convertDate">
                    <xsl:with-param name="date" select="in:obsdate" />
                </xsl:call-template>
                <!-- uncomment this to get the date in us style -->
                <!-- <xsl:value-of select="in:obsdate" /> -->
            </xsl:attribute>
            <hi>
                <xsl:value-of select="in:daytime/in:hightemperature" />
            </hi>
            <low>
                <xsl:value-of select="in:daytime/in:lowtemperature" />
            </low>
            <feelhi>
                <xsl:value-of select="in:daytime/in:realfeelhigh" />
            </feelhi>
            <feellow>
                <xsl:value-of select="in:daytime/in:realfeellow" />
            </feellow>
            <sunr>
                <xsl:value-of select="in:sunrise" />
            </sunr>
            <suns>
                <xsl:value-of select="in:sunset" />
            </suns>
            <part p="d">
                <icon>
                    <xsl:call-template name="iconTransform">
                        <xsl:with-param name="iconnumber" select="in:daytime/in:weathericon" />
                    </xsl:call-template>
                </icon>
                <t>
                    <xsl:value-of select="in:daytime/in:txtlong" />
                </t>
                <wind>
                    <s>
                        <xsl:value-of select="in:daytime/in:windspeed" />
                    </s>
                    <gust>
                        <xsl:value-of select="in:daytime/in:windgust" />
                    </gust>
                    <d />
                    <t>
                        <xsl:value-of select="in:daytime/in:winddirection" />
                    </t>
                </wind>
                <maxuv>
                	<xsl:value-of select="in:daytime/in:maxuv" />
                </maxuv>
                <bt />
                <precip>
                	<xsl:value-of select="in:daytime/in:precipamount" />
                </precip>
                <rainamount>
                	<xsl:value-of select="in:daytime/in:rainamount" />
                </rainamount>
                <snowamount>
                	<xsl:value-of select="in:daytime/in:snowamount" />
                </snowamount>
                <thunderstorm>
                	<xsl:value-of select="in:daytime/in:tstormprob" />
                </thunderstorm>
            </part>
            <part p="n">
                <icon>
                    <xsl:call-template name="iconTransform">
                        <xsl:with-param name="iconnumber" select="in:nighttime/in:weathericon" />
                    </xsl:call-template>
                </icon>
                <t>
                    <xsl:value-of select="in:nighttime/in:txtlong" />
                </t>
                <wind>
                    <s>
                        <xsl:value-of select="in:nighttime/in:windspeed" />
                    </s>
                    <gust>
                        <xsl:value-of select="in:nighttime/in:windgust" />
                    </gust>
                    <d />
                    <t>
                        <xsl:value-of select="in:nighttime/in:winddirection" />
                    </t>
                </wind>
                <bt />
                <precip>
                    <xsl:value-of select="in:nighttime/in:precipamount" />
                </precip>
                <rainamount>
                    <xsl:value-of select="in:nighttime/in:rainamount" />
                </rainamount>
                <snowamount>
                    <xsl:value-of select="in:nighttime/in:snowamount" />
                </snowamount>
                <thunderstorm>
                    <xsl:value-of select="in:nighttime/in:tstormprob" />
                </thunderstorm>
            </part>
        </day>
    </xsl:template>

    <xsl:template name="iconTransform">
        <xsl:param name="iconnumber" select="iconnumber" />
        <xsl:choose>
            <xsl:when test="$iconnumber = '01'">32</xsl:when>
            <xsl:when test="$iconnumber = '02'">34</xsl:when>
            <xsl:when test="$iconnumber = '03'">30</xsl:when>
            <xsl:when test="$iconnumber = '04'">28</xsl:when>
            <xsl:when test="$iconnumber = '05'">28</xsl:when>
            <xsl:when test="$iconnumber = '06'">28</xsl:when>
            <xsl:when test="$iconnumber = '07'">26</xsl:when>
            <xsl:when test="$iconnumber = '08'">26</xsl:when>
            <xsl:when test="$iconnumber = '11'">20</xsl:when>
            <xsl:when test="$iconnumber = '12'">11</xsl:when>
            <xsl:when test="$iconnumber = '13'">39</xsl:when>
            <xsl:when test="$iconnumber = '14'">39</xsl:when>
            <xsl:when test="$iconnumber = '15'">17</xsl:when>
            <xsl:when test="$iconnumber = '16'">38</xsl:when>
            <xsl:when test="$iconnumber = '17'">37</xsl:when>
            <xsl:when test="$iconnumber = '18'">40</xsl:when>
            <xsl:when test="$iconnumber = '19'">15</xsl:when>
            <xsl:when test="$iconnumber = '20'">41</xsl:when>
            <xsl:when test="$iconnumber = '21'">15</xsl:when>
            <xsl:when test="$iconnumber = '22'">18</xsl:when>
            <xsl:when test="$iconnumber = '23'">39</xsl:when>
            <xsl:when test="$iconnumber = '24'">25</xsl:when>
            <xsl:when test="$iconnumber = '25'">18</xsl:when>
            <xsl:when test="$iconnumber = '26'">5</xsl:when>
            <xsl:when test="$iconnumber = '27'">20</xsl:when>
            <xsl:when test="$iconnumber = '29'">7</xsl:when>
            <xsl:when test="$iconnumber = '30'">36</xsl:when>
            <xsl:when test="$iconnumber = '31'">25</xsl:when>
            <xsl:when test="$iconnumber = '32'">23</xsl:when>
            <xsl:when test="$iconnumber = '33'">31</xsl:when>
            <xsl:when test="$iconnumber = '34'">33</xsl:when>
            <xsl:when test="$iconnumber = '35'">29</xsl:when>
            <xsl:when test="$iconnumber = '36'">29</xsl:when>
            <xsl:when test="$iconnumber = '37'">46</xsl:when>
            <xsl:when test="$iconnumber = '38'">27</xsl:when>
            <xsl:when test="$iconnumber = '39'">45</xsl:when>
            <xsl:when test="$iconnumber = '40'">45</xsl:when>
            <xsl:when test="$iconnumber = '41'">47</xsl:when>
            <xsl:when test="$iconnumber = '42'">47</xsl:when>
            <xsl:when test="$iconnumber = '43'">46</xsl:when>
            <xsl:when test="$iconnumber = '44'">45</xsl:when>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="convertDate">
        <xsl:param name="date" select="date" />
        <xsl:variable name="day" select="substring-before(substring-after($date, '/'), '/')" />
        <xsl:variable name="month" select="substring-before($date, '/')" />
        <xsl:variable name="year" select="substring-after(substring-after($date, '/'), '/')" />
        <xsl:value-of select="format-number($day, '00')" />.<xsl:value-of
            select="format-number($month, '00')" />.<xsl:value-of select="$year" />
    </xsl:template>

</xsl:stylesheet>
