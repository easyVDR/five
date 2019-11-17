/*
 *  $Id: parsing.c,v 1.3 2005/01/30 12:52:52 beejay Exp $
 */

#include <vdr/plugin.h>

#include "parsing.h"
#include "stringfunct.h"

#define ATTRIB_OPT_STRING(_attr,_target) \
        if (attrs.find(_attr) != attrs.end()) { \
                _target = attrs[_attr]; \
        }
#define ATTRIB_MAN_STRING(_attr,_target) \
        ATTRIB_OPT_STRING(_attr,_target) \
        else { \
                esyslog("ERROR : weather: Mandatory attribute %s missing in tag %s", \
                                _attr, name.c_str()); \
                return false; \
        }

#define ROUND_INT(d) ( (int) ((d) + ((d) > 0 ? 0.5 : -0.5)) )        
        
        
static std::vector<std::string> context;
bool locked,IsDay,IsWind;
int dpart=0;
int count=0;
std::string param[255][255][2];
std::string ipart,day,date,town,unit1,unit2;

bool xStartElem(const std::string &name, std::map<std::string, std::string> &attrs) {

  if (name=="part"){
    ATTRIB_MAN_STRING("p",ipart); 

    if (ipart=="d") {
      dpart=0;
      }
    else{
      dpart=1;
    }
  }
  
  if ( name=="us" || name=="ut" || name=="sunr" || name=="suns" || name=="icon" || name=="dnam" || name=="hi" || name=="feelhi" || name=="feellow" || name=="low" || name=="ppcp" || name=="hmid" || name=="t" || name=="s" || name=="maxuv" || name=="thunderprob" || name=="rainamount" || name=="snowamount" || name=="precipamount" ) {
    locked=false;
  }

  if (name=="wind"){
    locked=false;
    IsWind=true;
  }

  if(name=="day"){
    if (++count < 255) {
      // dayname
      ATTRIB_MAN_STRING("t",param[count][8][0]);
      param[count][8][1]=param[count][8][0].c_str();

      // date
      ATTRIB_MAN_STRING("dt",param[count][9][0]);
      param[count][9][1]=param[count][9][0].c_str();
      IsDay=true;
    }
  }

  context.push_back(name);
  return true;
}


bool xCharData(const std::string &text) {
  if(context.size()>0 ) {

    // wetter
    if (context[context.size()-1]=="t" && IsWind==false  && IsDay==true){
      param[count][6][dpart]=text.c_str();
    }

    // winddir
    if (context[context.size()-1]=="t" && IsWind==true  && IsDay==true){
       param[count][10][dpart]=text.c_str();
    }

    // windspeed
    if (context[context.size()-1]=="s" && IsWind==true  && IsDay==true){
      param[count][11][dpart]=text.c_str();
    }
  
    // hi
    if (context[context.size()-1]=="hi" && IsDay==true){
      param[count][2][0]=text.c_str();
      param[count][2][1]=text.c_str();
    }

    // low
    if (context[context.size()-1]=="low" && IsDay==true){
      param[count][3][0]=text.c_str();
      param[count][3][1]=text.c_str();
    }

    // feelhi
    if (context[context.size()-1]=="feelhi" && IsDay==true){
      param[count][4][0]=text.c_str();
      param[count][4][1]=text.c_str();
    }
    
    // feellow
    if (context[context.size()-1]=="feellow" && IsDay==true){
      param[count][5][0]=text.c_str();
      param[count][5][1]=text.c_str();
    }

    // humidity    
    if (context[context.size() -1]=="hmid" && IsDay==true){
      param[count][12][dpart]=text.c_str();
    }

    // raindown
    if (context[context.size() -1]=="ppcp" && IsDay==true){
      param[count][13][dpart]=text.c_str();
    }

    // maxuv
    if (context[context.size() -1]=="maxuv" && IsDay==true){
      param[count][14][dpart]=text.c_str();
    }

    // tstormprob
    if (context[context.size() -1]=="thunderstorm" && IsDay==true){
      param[count][15][dpart]=text.c_str();
    }

    // rainamount
    if (context[context.size() -1]=="rainamount" && IsDay==true){
      param[count][16][dpart]=text.c_str();
    }

    // snowamount
    if (context[context.size() -1]=="snowamount" && IsDay==true){
      param[count][17][dpart]=text.c_str();
    }

    // precipamount
    if (context[context.size() -1]=="precip" && IsDay==true){
      param[count][18][dpart]=text.c_str();
    }

  } // if(context.size()>0 ) {

  if (locked==false){
    if(context.size()>0 ) {
      if (context[context.size()-1]=="dnam"){
        town=text.c_str();
      }

     if (context[context.size()-1]=="ut"){
       unit1=text.c_str();
     }

     if (context[context.size()-1]=="us"){
       unit2=text.c_str();
     }

     if (context[context.size()-1]=="sunr"){
       param[count][0][0]= text.c_str();
       param[count][0][1]= text.c_str();		
     }

     if (context[context.size() -1]=="suns"){
       param[count][1][0]=text.c_str();
       param[count][1][1]=text.c_str();
     }

     if (context[context.size()-1]=="icon"){
       param[count][7][dpart]=text.c_str();
     }
   }
	}

  return true;
}

bool xEndElem(const std::string &name) {

  if (context[context.size()-1]=="sunr" || context[context.size()-1]=="hi" || context[context.size()-1]=="hifeel" || context[context.size()-1]=="low" || context[context.size()-1]=="lowfeel"|| context[context.size()-1]=="us" || context[context.size()-1]=="ut" || context[context.size()-1]=="dnam" || context[context.size()-1]=="suns" || context[context.size()-1]=="icon" || context[context.size()-1]=="ppcp" || context[context.size()-1]=="hmid" || context[context.size()-1]=="t" || context[context.size()-1]=="s") {
    locked= true;
  }

	if (context[context.size()-1]=="day"){
    IsDay= false;
  }

	if (context[context.size()-1]=="wind"){
		locked= true;
		IsWind=false;
	}

	if (context.size()>0){
		context.pop_back();   
	}

	return true;
}


void cxmlParse::xmlParse(int daycount, std::string DataDir, std::string data_file, int inDPart) {
		
  std::string xml_file;
  context.clear();
  count = 0;
  xml_file = DataDir;
  xml_file = xml_file + data_file;

#ifdef DEBUG_WEATHER_SYSTEM
  printf("DEBUG: weather: DataDir  : '%s'\n",DataDir.c_str());
  printf("DEBUG: weather: data_file: '%s'\n",data_file.c_str());
  printf("DEBUG: weather: xml_file : '%s'\n",xml_file.c_str());
  printf("DEBUG: weather: Now parse XML\n");
#endif

  XML xml(xml_file);
  xml.nodeStartCB(xStartElem);
  xml.nodeEndCB(xEndElem);
  xml.cdataCB(xCharData);
  if (xml.parse() != 0) {
    esyslog("ERROR : weather: Can not parse %s\n",data_file.c_str());
    }
  else{
  
    cxmlParse::ort               =town.c_str();
    // MEDIUM , units
    cxmlParse::celsius           =unit1.c_str();
    cxmlParse::speed             =unit2.c_str();
//    cxmlParse::kilometerspeed    =unit3.c_str();
//    cxmlParse::kilometerspeed    =unit4.c_str();
//    cxmlParse::kilometerspeed    =unit5.c_str();
    cxmlParse::sunrise       =param[daycount][0][inDPart].c_str();
    cxmlParse::sunset        =param[daycount][1][inDPart].c_str();
    cxmlParse::hi            =param[daycount][2][inDPart].c_str();
    cxmlParse::low           =param[daycount][3][inDPart].c_str();
    cxmlParse::feelhi        =param[daycount][4][inDPart].c_str();
    cxmlParse::feellow       =param[daycount][5][inDPart].c_str();
    // weather
    replaceAll(param[daycount][6][inDPart],"a couple of",tr("a couple of"));
    replaceAll(param[daycount][6][inDPart],"A couple of",tr("A couple of"));
    replaceAll(param[daycount][6][inDPart],"Very",tr("Very"));
    replaceAll(param[daycount][6][inDPart],"very",tr("very"));
    replaceAll(param[daycount][6][inDPart],"with",tr("with"));
    replaceAll(param[daycount][6][inDPart],"sunshine",tr("sunshine"));
    replaceAll(param[daycount][6][inDPart],"Sunshine",tr("Sunshine"));
    replaceAll(param[daycount][6][inDPart],"clouds",tr("clouds"));
    replaceAll(param[daycount][6][inDPart],"Clouds",tr("Clouds"));
    replaceAll(param[daycount][6][inDPart],"cloudy",tr("cloudy"));
    replaceAll(param[daycount][6][inDPart],"Cloudy",tr("Cloudy"));
    replaceAll(param[daycount][6][inDPart],"and",tr("and"));
    replaceAll(param[daycount][6][inDPart],"Partly",tr("Partly"));
    replaceAll(param[daycount][6][inDPart],"partly",tr("partly"));
    replaceAll(param[daycount][6][inDPart],"sunny",tr("sunny"));
    replaceAll(param[daycount][6][inDPart],"Sunny",tr("Sunny"));
    replaceAll(param[daycount][6][inDPart],"not as",tr("not as"));
    replaceAll(param[daycount][6][inDPart],"Not as",tr("Not as"));
    replaceAll(param[daycount][6][inDPart],"not",tr("not"));
    replaceAll(param[daycount][6][inDPart],"Not",tr("Not"));
    replaceAll(param[daycount][6][inDPart],"as",tr("as"));
    replaceAll(param[daycount][6][inDPart],"Warm",tr("Warm"));
    replaceAll(param[daycount][6][inDPart],"warm",tr("warm"));
    replaceAll(param[daycount][6][inDPart],"pleasant",tr("pleasant"));
    replaceAll(param[daycount][6][inDPart],"Pleasant",tr("Pleasant"));
    replaceAll(param[daycount][6][inDPart],"Mostly",tr("Mostly"));
    replaceAll(param[daycount][6][inDPart],"mostly",tr("mostly"));
    replaceAll(param[daycount][6][inDPart],"Warmer",tr("Warmer"));
    replaceAll(param[daycount][6][inDPart],"warmer",tr("warmer"));
    replaceAll(param[daycount][6][inDPart],"time",tr("time"));
    replaceAll(param[daycount][6][inDPart],"Time",tr("Time"));
    replaceAll(param[daycount][6][inDPart],"times",tr("times"));
    replaceAll(param[daycount][6][inDPart],"Times",tr("Times"));
    replaceAll(param[daycount][6][inDPart],"of",tr("of"));
    replaceAll(param[daycount][6][inDPart],"sun",tr("sun"));
    replaceAll(param[daycount][6][inDPart],"Sun",tr("Sun"));
    replaceAll(param[daycount][6][inDPart],"and",tr("and"));
    replaceAll(param[daycount][6][inDPart],"mild",tr("mild"));
    replaceAll(param[daycount][6][inDPart],"Mild",tr("Mild"));
    replaceAll(param[daycount][6][inDPart],"increasing",tr("increasing"));
    replaceAll(param[daycount][6][inDPart],"increalsing",tr("increalsing"));
    replaceAll(param[daycount][6][inDPart],"pachy",tr("pachy"));
    replaceAll(param[daycount][6][inDPart],"pachty",tr("pachty"));
    replaceAll(param[daycount][6][inDPart],"Cooler",tr("Cooler"));
    replaceAll(param[daycount][6][inDPart],"cooler",tr("cooler"));
    replaceAll(param[daycount][6][inDPart],"rain",tr("rain"));
    replaceAll(param[daycount][6][inDPart],"Rain",tr("Rain"));
    replaceAll(param[daycount][6][inDPart],"snow",tr("snow"));
    replaceAll(param[daycount][6][inDPart],"Snow",tr("Snow"));
//    replaceAll(param[daycount][6][inDPart],"",tr(""));
    replaceAll(param[daycount][6][inDPart],"cloudiness",tr("cloudiness"));
    replaceAll(param[daycount][6][inDPart],"Cloudiness",tr("Cloudiness"));
    replaceAll(param[daycount][6][inDPart],"some",tr("some"));
    replaceAll(param[daycount][6][inDPart],"Some",tr("Some"));
    replaceAll(param[daycount][6][inDPart],"shower",tr("shower"));
    replaceAll(param[daycount][6][inDPart],"Shower",tr("Shower"));
    
//    replaceAll(param[daycount][6][inDPart],"",tr(""));
//    replaceAll(param[daycount][6][inDPart],"",tr(""));
//    replaceAll(param[daycount][6][inDPart],"",tr(""));
//    replaceAll(param[daycount][6][inDPart],"",tr(""));
//    replaceAll(param[daycount][6][inDPart],"",tr(""));
    cxmlParse::wetter        =param[daycount][6][inDPart].c_str();

    // weathericon
    cxmlParse::icon          =param[daycount][7][inDPart].c_str();


    // dayname
    replaceAll(param[daycount][8][inDPart],"Sunday",tr("Sunday"));
    replaceAll(param[daycount][8][inDPart],"Monday",tr("Monday"));
    replaceAll(param[daycount][8][inDPart],"Tuesday",tr("Tuesday"));
    replaceAll(param[daycount][8][inDPart],"Wednesday",tr("Wednesday"));
    replaceAll(param[daycount][8][inDPart],"Thursady",tr("Thursday"));
    replaceAll(param[daycount][8][inDPart],"Friday",tr("Friday"));
    replaceAll(param[daycount][8][inDPart],"Saturday",tr("Saturday"));
		cxmlParse::dayname       =param[daycount][8][inDPart].c_str();

    // date
		cxmlParse::date          =param[daycount][9][inDPart].c_str();

    // winddir
    replaceAll(param[daycount][10][inDPart],"E",tr("E"));
    replaceAll(param[daycount][10][inDPart],"W",tr("W"));
    replaceAll(param[daycount][10][inDPart],"S",tr("S"));
    replaceAll(param[daycount][10][inDPart],"N",tr("N"));
    cxmlParse::winddir       =param[daycount][10][inDPart].c_str();

    // windspeed in km/h
    int km;
    km = atoi(param[daycount][11][inDPart].c_str());
    km = ROUND_INT(km * 3.6);
     cxmlParse::windspeed     =itoa(km);
//    cxmlParse::windspeed     =param[daycount][11][inDPart].c_str();


    cxmlParse::humidity      =param[daycount][12][inDPart].c_str();
    cxmlParse::raindown      =param[daycount][13][inDPart].c_str();
    cxmlParse::maxuv         =param[daycount][14][inDPart].c_str();
    cxmlParse::tstormprob    =param[daycount][15][inDPart].c_str();
    cxmlParse::rainamount    =param[daycount][16][inDPart].c_str();
    cxmlParse::snowamount    =param[daycount][17][inDPart].c_str();
    cxmlParse::precipamount  =param[daycount][18][inDPart].c_str();
    
#ifdef DEBUG_WEATHER
    showDebug();
#endif    
    
  }
}

void cxmlParse::showDebug(void) {
  printf("\n=====================\n");
  printf("Weather Debug output:\n");
  printf("=====================\n");

  printf("Unit temperature   (ut)         : %s\n", celsius.c_str());
  printf("Unit windspeed     (us)         : %s\n", speed.c_str());

  printf("Location           (daynam)     : %s\n", ort.c_str());
  printf("Date               (dt)         : %s\n", date.c_str());
  printf("Dayname            (day -> t)   : %s\n", dayname.c_str());
  printf("Weather            (part -> t)  : %s\n", wetter.c_str());
  printf("Icon               (icon)       : %s\n", icon.c_str());

  printf("High temp.         (hi)         : %s\n", hi.c_str());
  printf("Low temp.          (low)        : %s\n", low.c_str());
  printf("High temp.         (feelhi)     : %s\n", feelhi.c_str());
  printf("Low temp.          (feellow)    : %s\n", feellow.c_str());
  
  
  printf("Sunrise            (sunr)       : %s\n", sunrise.c_str());
  printf("Sunset             (suns)       : %s\n", sunset.c_str());
  printf("Direction of wind  (wind -> t)  : %s\n", winddir.c_str());
  printf("Windspeed          (s)          : %s\n", windspeed.c_str());
  printf("Humidity           (hmid)       : %s\n", humidity.c_str());
  printf("Raindown           (ppcp)       : %s\n", raindown.c_str());

  
  printf("Max UV Index       (maxuv)        : %s\n", maxuv.c_str());
  printf("Thunderstorm       (thunderstorm) : %s\n", tstormprob.c_str());
  printf("Rain in mm         (rainamount)   : %s\n", rainamount.c_str());
  printf("Snow in mm         (snowamount)   : %s\n", snowamount.c_str());
  printf("Precipation in mm  (precip)       : %s\n", precipamount.c_str());
  
  printf("\n");
}
	
