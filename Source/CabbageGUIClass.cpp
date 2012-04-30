/*
  Copyright (C) 2007 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
   
  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
  
*/
  
#include "CabbageGUIClass.h"

CabbageGUIClass::CabbageGUIClass(String compStr, int ID):
								debugMessage(T("")), 
								plant(T("")), 
								reltoplant(T("")), 
								scaleX(1), 
								scaleY(1), 
								midiChan(-99), 
								midiCtrl(-99),
								boundsText(""),
								posText(""),
								sizeText("")
{
//Default values are assigned to all attributres 
//before parsing begins
	items.clear();
	items.add(T(""));
	key.clear();
	min=0;
	max=1;
	sliderIncr = 0.01;
	sliderSkew = 1;
	trackerFill = 1;

	StringArray strTokens;
	strTokens.addTokens(compStr, " ", "\"");

	//changing case to lower to make syntax non case-sensitive
	if(strTokens[0].indexOfIgnoreCase("hslider")!=-1){
          top = 10;
          left = 10;
          width = 150;
          height = 50;		  
          channel = "hslider";
          kind = "horizontal";
          min = 0;
          max = 100;	
		  caption = "";
          name = "hslider";
		  type = name;
          name.append(String(ID), 1024);
		  textBox = 0;
		  colour = Colours::white;
	}
    else if(strTokens[0].indexOfIgnoreCase("vslider")!=-1){
          top = 10;
          left = 10;
          width = 50;
          height = 150;
          channel = "vslider";
          kind = "vertical";
          min = 0;
          max = 100;	
		  caption = "";
          name = "vslider";
		  type = name;
          name.append(String(ID), 1024);
		  textBox = 0;
		  colour = Colours::white;
	}
    else if(strTokens[0].indexOfIgnoreCase("rslider")!=-1){
          top = 10;
          left = 10;
          width = 60;
          height = 60;
          channel = "rslider";
          kind = "rotary";
          min = 0;
          max = 100;	
		  caption = "";
          name = "rslider";
		  type = name;
          name.append(String(ID), 1024);
		  textBox = 0;
		  colour = Colours::white;
	}

    else if(strTokens[0].indexOfIgnoreCase("source")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 240;
          channel = "";
          name = "source";
		  caption = "";
		  type = name;
          //name.append(String(ID), 1024);
	}

    else if(strTokens[0].indexOfIgnoreCase("patmatrix")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 240;
          channel = "";
          name = "patmatrix";
		  caption = "Pattern Matrix";
		  type = name;
		  noSteps = 8;
		  noPatterns=1;
		  rCtrls=0;
          //name.append(String(ID), 1024);
	}

    else if(strTokens[0].indexOfIgnoreCase("button")!=-1){
          top = 10;
          left = 10;
          width = 80;
          height = 40;
          channel = "buttonchan";
          items.add(T(""));
          items.add(T(""));
          name = "button";
		  max = 1;
		  min = 0;
		  caption = "";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("checkbox")!=-1){
          top = 10;
          left = 10;
          width = 100;
          height = 22;
          channel = "checkchan";
          caption = "";
          name = "checkbox";
		  caption = "";
		  items.add(T(""));
		  sliderRange = 1;
		  min = 0;
		  max = 1;
		  type = name;
		  shape = "circle";
          name.append(String(ID), 1024);
		  //default colour for toggles
		  colour = Colours::lime;
	}
    else if(strTokens[0].indexOfIgnoreCase("combobox")!=-1){
		
          top = 10;
          left = 10;
          width = 80;
          height = 22;
          channel = "combochan";
          caption = "";
		  //add one item by befault, will be overwritten by users values
          items.add(T("item1"));
          name = "combobox";
		  type = name;
          name.append(String(ID), 1024);
		  min=1;
		 
	} 
    else if(strTokens[0].indexOfIgnoreCase("label")!=-1){
          top = 10;
          left = 10;
          width = 100;
          height = 16;
          channel = "label";
          beveltype = "none";
		  colour = CabbageUtils::getComponentFontColour();
          fontcolour = Colours::white;
          text = "hello";
          name = "label";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("image")!=-1){
          top = 10;
          left = 10;
          width = 160;
          height = 120;
          channel = "image";
          beveltype = "none";
          colour = Colours::white; 
          fontcolour = Colours::white;
          text = "";
		  line = 2;
          name = "image";
		  type = name;
		  shape="rounded";
		  outline = Colours::black;
		  colour = Colours::white;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("groupbox")!=-1){
          top = 10;
          left = 10;
          width = 80;
          height = 22;
          colour = Colours::white;
          name = "groupbox";
		  type = name;
          name.append(String(ID), 1024);
		  line = 1;
	}
    else if(strTokens[0].indexOfIgnoreCase("vline")!=-1){
          top = 10;
          left = 10;
          width = 180;
          height = 2;
          name = "vline";
		  type = name;
		  lineIsVertical = 1;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("hline")!=-1){
          top = 10;
          left = 10;
          width = 2;
          height = 180;
          name = "hline";
		  type = name;
		  lineIsVertical = 0;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("csoundoutput")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 200;
          colour = Colours::white;
          name = "csoundoutput";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("vumeter")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 200;
		  vuConfig.clear();
		  colour = Colours::white;
          name = "vumeter";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("table")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 200;
          colour = Colours::white;
          name = "table";
		  type = name;
		  value = 1;
		  tableNum = 1;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("xypad")!=-1){
          top = 10;
          left = 10;
          width = 200;
          height = 200;
          xChannel = "xypadchanX";
		  yChannel = "xypadchanY";
		  channel = "";
          caption = "";
          name = "xypad";
		  caption = "";
		  minX = 0;
		  maxX = 200;
		  minY = 0;
		  maxY = 200;
		  valueX = 0;
		  valueY = 0;
		  type = "xypad";
          name.append(String(ID), 1024);
		  xyChannel = "";
	}
    else if(strTokens[0].indexOfIgnoreCase("keyboard")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 100;
          name = "keyboard";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("form")!=-1){
          top = 10;
          left = 10;
          width = 600;
          height = 300;
          name = "form";
		  type = "form";
		  text = "";
		  caption = "";
	}

    else if(strTokens[0].indexOfIgnoreCase("hostbpm")!=-1){
          name = "hostbpm";
		  type = "hostbpm";
	}

    else if(strTokens[0].indexOfIgnoreCase("hostppqpos")!=-1){
          name = "hostppqpos";
		  type = "hostppqpos";
	}

    else if(strTokens[0].indexOfIgnoreCase("hostplaying")!=-1){
          name = "hostplaying";
		  type = "hostplaying";
	}

    else if(strTokens[0].indexOfIgnoreCase("hostrecording")!=-1){
          name = "hostrecording";
		  type = "hostrecording";
	}

    else if(strTokens[0].indexOfIgnoreCase("hosttime")!=-1){
          name = "hosttime";
		  type = "hosttime";
	}

	tabpage = 0;
	value = 0;
//parse the text now that all default values ahve been assigned
parse(compStr);
}

CabbageGUIClass::~CabbageGUIClass()
{

}
//===========================================================================================
int CabbageGUIClass::parse(String str)
{
	//remove any text after a semicolon
	if(str.indexOf(0, T(";"))!=-1)
		str = str.substring(0, str.indexOf(0, T(";")));

	try{

	StringArray identArray;
	identArray.clear();
	identArray.add("size(");
    identArray.add("pos(");
    identArray.add("min(");
    identArray.add("max(");
    identArray.add("value(");
	identArray.add("tabpage(");
	identArray.add("midictrl(");
	//check line values with spaces, otherwise
	//it gets confused with the outline identifier
	identArray.add(" line(");
	identArray.add(",line(");
	identArray.add("bounds(");
	identArray.add("range(");
	identArray.add("rangex(");
	identArray.add("rangey(");
	identArray.add("plant(");
    identArray.add("channel(");
    identArray.add("name(");
    identArray.add("textbox(");
	identArray.add("instrs(");
    identArray.add("caption(");
    identArray.add(",colour(");
	identArray.add(" colour(");
    identArray.add("topitem(");
    identArray.add("menuitem(");
    identArray.add("stdout(");
    identArray.add("exit(");
    identArray.add("cssetup(");
    identArray.add("kind(");
	identArray.add("config(");
    identArray.add("fontcolour(");
    identArray.add("beveltype(");
    identArray.add("items(");
	identArray.add("text(");
    identArray.add("runcsound(");
	identArray.add("tabs(");
	identArray.add("tablenumber(");
	identArray.add("fill(");
	identArray.add("file(");
	identArray.add("outline(");
	identArray.add("shape("); 
	identArray.add("textcolour("); 
	identArray.add("scale(");
	identArray.add("chan(");
	identArray.add("key(");
	identArray.add("steps(");
	identArray.add("pluginid(");
	identArray.add("rctrls(");
	identArray.add("tracker(");
	//add a few dummy identifiers so we can catch bogus one in the Cabbage code
	identArray.add("");
	identArray.add("");
	identArray.add("");

	//retrieve paramters consisting of strings
	for(int indx=0;indx<identArray.size();indx++)
	{		
		//check to see if identifier is part of input string..turn to lowercase first..
		int identPos = str.toLowerCase().indexOf(identArray.getReference(indx));
          if(identPos!=-1){
			String newString = str.substring(identPos+identArray.getReference(indx).length());
			String tstr = newString.substring(0, newString.indexOf(0, ")"));
			if(tstr.length()==0) return 0;
			StringArray strTokens;
			strTokens.addTokens(tstr.removeCharacters(")\""), ",", "\"");

			if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("name(")) name = strTokens[0].trim();
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("plant(")) plant = strTokens[0].trim();
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("caption(")) caption = strTokens[0].trim();
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("channel(")||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase("chan(")){
					channel = strTokens[0].trim();
					if(str.containsIgnoreCase("xypad")){
					xChannel = strTokens[0].trim();
					yChannel = strTokens[1].trim();
					}
					else if(str.containsIgnoreCase("vumeter")){
						channels.clear();
						for(int u=0;u<strTokens.size();u++){
							channels.add(strTokens[u].trim());
						}
					}
					else channels.add(strTokens[0].trim());
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" colour(")||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(",colour(")){
					if(strTokens.size()<2)
						colour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						colour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						colour = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("kind(")) kind = strTokens[0].trim();
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("file(")) file = strTokens[0].trim();
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("fill(")){
					if(strTokens.size()<2)
						fill = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						fill = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						fill = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());

			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("shape(")) shape = strTokens[0].trim();
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("outline(")){
					if(strTokens.size()<2)
						outline = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						outline = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						outline = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("textcolour(")){
					if(strTokens.size()<2)
						textcolour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						textcolour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						textcolour = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("key(")){
				key.clear();
				if(strTokens.size()==1){
					key.add(strTokens[0].trim());
				}
				else if(strTokens.size()==2){
					key.add(strTokens[0].trim());
					key.add(strTokens[1].trim());
				}
				else
					debugMessage =T("WARNING: Too many parameters passed to key(): usage key(\"key1\", \"key2\"\")");

			}

			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("pluginid(")){
				pluginID = strTokens[0].trim();
			}

            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("items(")||
					identArray.getReference(indx).toLowerCase().equalsIgnoreCase("text(")){
              items.clear();//clear any unwanted items
			  text = strTokens[0];
			  for(int i= 0;i<(int)strTokens.size();i++){
				String test = strTokens[i]; 
				items.add(strTokens[i]);	
				if((int)strTokens.size()<2) {
					items.add(strTokens[i]);
					i++;
				}

				//messing with my checkbox!
				comboRange = i;
              }
			}

			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("instrs(")){
              items.clear();//clear any unwanted items
			  for(int i= 0;i<(int)strTokens.size();i++){
				String test = strTokens[i]; 
				items.add(strTokens[i]);	
				}
			}

			//numeric paramters

			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("size(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to size(): usage pos(width, height\")");
				}
				else{
					sizeText = identArray.getReference(indx)+tstr+T(")");
					width = strTokens[0].trim().getFloatValue();  
					height = strTokens[1].trim().getFloatValue();  
				}
            }
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("scale(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to scale(): usage scale(width, height\")");
				}
				else{
              scaleX = strTokens[0].trim().getFloatValue();  
			  scaleX = scaleX/width;
			  Logger::writeToLog(T("ScaleX:")+String(scaleX));
              scaleY = strTokens[1].trim().getFloatValue();  
			  scaleY = scaleY/height;
			  Logger::writeToLog(T("ScaleY:")+String(scaleY));
				}
            }
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("bounds(")){
				if(strTokens.size()<3){
					debugMessage = T("WARNING: Not enough paramters passed to bounds(): usage pos(top, left width, height\")");
				}
				else{
				  left = strTokens[0].trim().getFloatValue();  
				  top = strTokens[1].trim().getFloatValue();  
				  width = strTokens[2].trim().getFloatValue();  
				  if(strTokens.size()==3)
					  height = width;
				  else
				  height = strTokens[3].trim().getFloatValue(); 
				  boundsText = identArray.getReference(indx)+tstr+T(")");
				}
            }
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("config(")){
				vuConfig.clear();
				 for(int i= 0;i<(int)strTokens.size();i++){
					 vuConfig.add(strTokens[i].trim().getFloatValue());
				 }
				}

            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("pos(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to pos(): usage pos(top, left\")");
				}
				else{
				  left = strTokens[0].trim().getFloatValue(); 
				  top = strTokens[1].trim().getFloatValue();
				  posText = identArray.getReference(indx)+tstr+T(")");
				}
            }
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("range(")){
				if(strTokens.size()<3){
					debugMessage =T("WARNING: Not enough paramters passed to range(): usage range(minx, max, value, incr\")");
				}
				else{
				min = strTokens[0].trim().getDoubleValue();// getFloatValue();  
				max = strTokens[1].trim().getDoubleValue();//.getFloatValue();  
				if(strTokens.size()>2)
				value = strTokens[2].trim().getDoubleValue();//.getFloatValue(); 
				else value = 0;

				if(strTokens.size()>3)
				sliderSkew = strTokens[3].trim().getDoubleValue();//.getFloatValue(); 
				if(strTokens.size()>4)
				sliderIncr = strTokens[4].trim().getDoubleValue();//.getFloatValue(); 

				sliderRange = max-min;
				}
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("rangex(")){
				if(strTokens.size()<3){
					debugMessage =T("WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")");
				}
				else{
				minX = strTokens[0].trim().getFloatValue();  
				maxX = strTokens[1].trim().getFloatValue();  
				valueX = strTokens[2].trim().getFloatValue(); 
				xypadRangeX = maxX-minX;
				}
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("rangey(")){
				if(strTokens.size()<3){
					debugMessage =T("WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")");
				}
				else{
				minY = strTokens[0].trim().getFloatValue();  
				maxY = strTokens[1].trim().getFloatValue();  
				valueY = strTokens[2].trim().getFloatValue();
				xypadRangeY = maxY-minY;
				}
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("min(")){
				min = strTokens[0].trim().getFloatValue();  
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("midiCtrl(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to midiCtrl(): usage midiCtrl(midiChan, midiCtrl\")");
				}
				else{
				midiChan = strTokens[0].trim().getFloatValue();  
				midiCtrl = strTokens[1].trim().getFloatValue();  
			}
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("max(")){
				max = strTokens[0].trim().getFloatValue();  
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("steps(")){
				noSteps = strTokens[0].trim().getFloatValue();  
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tracker(")){
				trackerFill = strTokens[0].trim().getFloatValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("textbox(")){
				textBox = strTokens[0].trim().getFloatValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tablenumber(")){
				tableNum = strTokens[0].trim().getFloatValue();  
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase(",line(")||
					identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" line(")) line = strTokens[0].trim().getFloatValue();  
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("value(")) value = strTokens[0].trim().getFloatValue();  
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tabpage(")) tabpage = strTokens[0].trim().getFloatValue();  
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("rctrls(")){
				if(strTokens[0].trim().getFloatValue()>5)
				rCtrls = 5;
				else
				rCtrls = strTokens[0].trim().getFloatValue();  
			}
			else {
			}
			strTokens.clear();
          }
          else{
 
          }
	}
	return 1;//must add error checking to this...
	}

catch(...){

	return 0;
	

}
}
//=========================================================================
//retrieve numerical attributes
double CabbageGUIClass::getNumProp(String prop)
{
		if(prop.equalsIgnoreCase(T("width")))
			return width;
		else if(prop.equalsIgnoreCase(T("height")))
			return height;
		else if(prop.equalsIgnoreCase(T("textbox")))
			return textBox;
		else if(prop.equalsIgnoreCase(T("top")))
			return top;
		else if(prop.equalsIgnoreCase(T("channels")))
			return channels.size();
		else if(prop.equalsIgnoreCase(T("left")))
			return left;
		else if(prop.equalsIgnoreCase(T("min")))
			return min;
		else if(prop.equalsIgnoreCase(T("midichan")))
			return midiChan;
		else if(prop.equalsIgnoreCase(T("midictrl")))
			return midiCtrl;
		else if(prop.equalsIgnoreCase(T("max")))
			return max;
		else if(prop.equalsIgnoreCase(T("sliderRange")))
			return sliderRange;
		else if(prop.equalsIgnoreCase(T("xypadRangeY")))
			return xypadRangeY;
		else if(prop.equalsIgnoreCase(T("xypadRangeX")))
			return xypadRangeX;
		else if(prop.equalsIgnoreCase(T("maxX")))
			return maxX;
		else if(prop.equalsIgnoreCase(T("maxY")))
			return maxY;
		else if(prop.equalsIgnoreCase(T("minX")))
			return minX;
		else if(prop.equalsIgnoreCase(T("minY")))
			return minY;
		else if(prop.equalsIgnoreCase(T("valueX")))
			return valueX;
		else if(prop.equalsIgnoreCase(T("valueY")))
			return valueY;
		else if(prop.equalsIgnoreCase(T("tabpage")))
			return tabpage;
		else if(prop.equalsIgnoreCase(T("noOfMenus")))
			return noOfMenus;
		else if(prop.equalsIgnoreCase(T("onoff")))
			return onoff;
		else if(prop.equalsIgnoreCase(T("value")))
			return value;
		else if(prop.equalsIgnoreCase(T("comboRange")))
			return comboRange;
		else if(prop.equalsIgnoreCase(T("line")))
			return line;
		else if(prop.equalsIgnoreCase(T("scaleX")))
			return scaleX;
		else if(prop.equalsIgnoreCase(T("scaleY")))
			return scaleY;
		else if(prop.equalsIgnoreCase(T("maxitems")))
			return maxItems;
		else if(prop.equalsIgnoreCase(T("linkTo")))
			return linkTo;
		else if(prop.equalsIgnoreCase(T("tableNum")))
			return tableNum;
		else if(prop.equalsIgnoreCase(T("sliderIncr")))
			return sliderIncr;
		else if(prop.equalsIgnoreCase(T("sliderSkew")))
			return sliderSkew;
		else if(prop.equalsIgnoreCase(T("noSteps")))
			return noSteps;
		else if(prop.equalsIgnoreCase(T("rCtrls")))
			return rCtrls;
		else if(prop.equalsIgnoreCase(T("lineIsVertical")))
			return lineIsVertical;
		else if(prop.equalsIgnoreCase(T("noPatterns")))
			return items.size(); 
		else if(prop.equalsIgnoreCase(T("tracker")))
			return trackerFill; 
		else return -9999;
}

double CabbageGUIClass::getNumProp(String prop, int index)
{
		if(prop.equalsIgnoreCase(T("configArray")))
			return vuConfig[index];
		else 
			return 0;
}

//set numerical attributes
void CabbageGUIClass::setNumProp(String prop, float val)
{
		if(prop.equalsIgnoreCase(T("width")))
			 width = val;
		else if(prop.equalsIgnoreCase(T("height")))
			 height = val;
		else if(prop.equalsIgnoreCase(T("top")))
			 top = val;
		else if(prop.equalsIgnoreCase(T("left")))
			 left = val;
		else if(prop.equalsIgnoreCase(T("min")))
			 min = val;
		else if(prop.equalsIgnoreCase(T("midichan")))
			 midiChan = val;
		else if(prop.equalsIgnoreCase(T("midictrl")))
			 midiCtrl = val;
		else if(prop.equalsIgnoreCase(T("max")))
			 max = val;
		else if(prop.equalsIgnoreCase(T("tabpage")))
			 tabpage = val;
		else if(prop.equalsIgnoreCase(T("noOfMenus")))
			 noOfMenus = val;
		else if(prop.equalsIgnoreCase(T("onoff")))
			 onoff = val;
		else if(prop.equalsIgnoreCase(T("value")))
			 value = val;
		else if(prop.equalsIgnoreCase(T("line")))
			 line = val;
		else if(prop.equalsIgnoreCase(T("scaleX")))
			 scaleX = val;
		else if(prop.equalsIgnoreCase(T("scaleY")))
			 scaleY = val;
		else if(prop.equalsIgnoreCase(T("linkTo")))
			 linkTo = val;
		else if(prop.equalsIgnoreCase(T("sliderRange")))
			 sliderRange = val;
		else if(prop.equalsIgnoreCase(T("sliderIncr")))
			 sliderIncr = val;
		else if(prop.equalsIgnoreCase(T("xypadRangeY")))
			 xypadRangeY = val;
		else if(prop.equalsIgnoreCase(T("xypadRangeX")))
			 xypadRangeX = val;
		else if(prop.equalsIgnoreCase(T("maxX")))
			 maxX =val;
		else if(prop.equalsIgnoreCase(T("maxY")))
			 maxY = val;
		else if(prop.equalsIgnoreCase(T("minX")))
			 minX = val;
		else if(prop.equalsIgnoreCase(T("minY")))
			 minY = val;
		else if(prop.equalsIgnoreCase(T("valueX")))
			 valueX = val;
		else if(prop.equalsIgnoreCase(T("valueY")))
			 valueY = val;
}

String CabbageGUIClass::getPropsString()
{
	return type << T(" bounds(") << String(left) << T(", ") << String(top) << T(", ") << String(width)
				<< T(", ") << String(height) << T("), channel(\"") << channel << T("), xyChannel(\"") << xyChannel << T("\"), value(")
				<< String(value) << T("), items(\"") << items[0].trim() << T("\", \"") << items[1].trim() << T("\")")
				<< T("), range(\"") << String(min) << T(", ") << String(max) << T(", ") << String(value) << T("\")");
}

String CabbageGUIClass::getStringProp(String prop, int index)
{
		if(prop.equalsIgnoreCase(T("channel")))
			return channels[index].trim();
		else 
			return "";
}
String CabbageGUIClass::getStringProp(String prop)
{
		if(prop.equalsIgnoreCase(T("channel")))
			return channel.trim();
		else if(prop.equalsIgnoreCase(T("xyChannel")))
			return xyChannel.trim();
		else if(prop.equalsIgnoreCase(T("xChannel")))
			return xChannel.trim();
		else if(prop.equalsIgnoreCase(T("yChannel")))
			return yChannel.trim();
		else if(prop.equalsIgnoreCase(T("name")))
			return name.trim();
		else if(prop.equalsIgnoreCase(T("bounds")))
			return boundsText.trim();
		else if(prop.equalsIgnoreCase(T("pos")))
			return posText.trim();
		else if(prop.equalsIgnoreCase(T("size")))
			return sizeText.trim();
		else if(prop.equalsIgnoreCase(T("text")))
			return text.trim();
		else if(prop.equalsIgnoreCase(T("type")))
			return type.trim();
		else if(prop.equalsIgnoreCase(T("shape")))
			return shape.trim();
		else if(prop.equalsIgnoreCase(T("beveltype")))
			return beveltype.trim();
		else if(prop.equalsIgnoreCase(T("caption")))
			return caption.trim();
		else if(prop.equalsIgnoreCase(T("kind")))
			return kind.trim();
		else if(prop.equalsIgnoreCase(T("topitem")))
			return topitem.trim();
		else if(prop.equalsIgnoreCase(T("file")))
			return file.trim();
		else if(prop.equalsIgnoreCase(T("cssetup")))
			return cssetup.trim();
		else if(prop.equalsIgnoreCase(T("csstdout")))
			return csstdout.trim();
		else if(prop.equalsIgnoreCase(T("exit")))
			return exit.trim();
		else if(prop.equalsIgnoreCase(T("plant")))
			return plant.trim();
		else if(prop.equalsIgnoreCase(T("reltoplant")))
			return reltoplant.trim();
		else if(prop.equalsIgnoreCase(T("debugmessage")))
			return debugMessage;
		else if(prop.equalsIgnoreCase(T("pluginID")))
			return pluginID;
		else return "";
}

String CabbageGUIClass::getColourProp(String prop)
{
		if(prop.equalsIgnoreCase(T("textcolour")))
			return textcolour.toString();
		else if(prop.equalsIgnoreCase(T("fontcolour")))
			return fontcolour.toString();
		else if(prop.equalsIgnoreCase(T("colour")))
			return colour.toString();
		else if(prop.equalsIgnoreCase(T("fill")))
			return fill.toString();
		else return "";

}

void CabbageGUIClass::setStringProp(String prop, String val)
{
		if(prop.equalsIgnoreCase(T("channel")))
			channel = val;
		else if(prop.equalsIgnoreCase(T("xyChannel")))
			xyChannel = val;
		else if(prop.equalsIgnoreCase(T("yChannel")))
			yChannel = val;
		else if(prop.equalsIgnoreCase(T("xChannel")))
			xChannel = val;
		else if(prop.equalsIgnoreCase(T("name")))
			name = val;
		else if(prop.equalsIgnoreCase(T("text")))
			text = val;
		else if(prop.equalsIgnoreCase(T("type")))
			type = val;
		else if(prop.equalsIgnoreCase(T("shape")))
			shape = val;
		else if(prop.equalsIgnoreCase(T("beveltype")))
			beveltype = val;
		else if(prop.equalsIgnoreCase(T("caption")))
			caption = val;
		else if(prop.equalsIgnoreCase(T("kind")))
			kind = val;
		else if(prop.equalsIgnoreCase(T("topitem")))
			topitem = val;
		else if(prop.equalsIgnoreCase(T("file")))
			file = val;
		else if(prop.equalsIgnoreCase(T("cssetup")))
			cssetup = val;
		else if(prop.equalsIgnoreCase(T("csstdout")))
			csstdout = val;
		else if(prop.equalsIgnoreCase(T("exit")))
			exit = val;
		else if(prop.equalsIgnoreCase(T("plant")))
			plant = val;
		else if(prop.equalsIgnoreCase(T("reltoplant")))
			reltoplant = val;
}