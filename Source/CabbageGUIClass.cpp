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
								debugMessage(""), 
								plant(""), 
								reltoplant(""), 
								scaleX(1), 
								scaleY(1), 
								midiChan(-99), 
								midiCtrl(-99),
								boundsText(""),
								posText(""),
								sizeText(""),
								preset(""),
								trackerFill(),
								masterSnap(0),
								width(0),
								height(0),
								left(0),
								top(0), 
								xyAutoIndex(0),
								fileType(0),
								workingDir(""),
								alpha(1),
								sliderRange(1)
{
//Default values are assigned to all attributres 
//before parsing begins
	items.clear();
	items.add("");
	key.clear();
	min=0;
	max=1;
	sliderIncr = 0.01;
	sliderSkew = 1;
	value = 0;
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
		  trackerFill = Colours::lime;
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
		  trackerFill = Colours::lime;
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
		  trackerFill = Colours::lime;
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

    else if(strTokens[0].indexOfIgnoreCase("infobutton")!=-1){
          top = 10;
          left = 10;
          width = 600;
          height = 440;
          channel = "";
          name = "infobutton";
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
          items.add("");
          items.add("");
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
          type = "checkbox";
		  caption = "";
          name = "checkbox";
		  caption = "";
		  items.add("");
		  min = 0;
		  max = 1;
		  shape = "square";
          name.append(String(ID), 1024);
		  //default colour for toggles
		  colour = Colours::lime;
	}
    else if(strTokens[0].indexOfIgnoreCase("combobox")!=-1){
		  items.clear();
          top = 10;
          left = 10;
          width = 80;
          height = 22;
          channel = "combochan";
          caption = "";
		  //add one item by befault, will be overwritten by users values
          items.add("Item 1");
		  items.add("Item 2");
		  items.add("Item 3");
		  items.add("Item 4");
		  items.add("Item 5");
          name = "combobox";		  type = name;
          name.append(String(ID), 1024);
		  min=1;
		  value = 1;
		 
	} 
    else if(strTokens[0].indexOfIgnoreCase("label")!=-1){
          top = 10;
          left = 10;
          width = 100;
          height = 16;
          channel = "label";
          beveltype = "none";
		  colour = CabbageUtils::getComponentFontColour();
          fontcolour = CabbageUtils::getComponentFontColour();
          text = "hello";
          name = "label";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("snapshot")!=-1){
		  items.clear();
		  top = 10;
          left = 10;
          width = 200;
          height = 20;
          channel = "snapshot";
		  colour = CabbageUtils::getComponentFontColour();
          fontcolour = Colours::white;
          text = "Snap";
          name = "snapshot";
		  type = name;
          name.append(String(ID), 1024);
		  items.add("One");
		  items.add("Two");
		  items.add("Three");
		  items.add("Four");
		  min=1;
		  value=1;
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
		  plantButton = 0;
	}
    else if(strTokens[0].indexOfIgnoreCase("groupbox")!=-1){
          top = 10;
          left = 10;
          width = 80;
          height = 22;
          colour = CabbageUtils::getComponentSkin();
		  //fontcolour = CabbageUtils::getComponentFontColour();
          name = "groupbox";
		  type = name;
          name.append(String(ID), 1024);
		  line = 1;
		  plantButton = 0;
	}
    else if(strTokens[0].indexOfIgnoreCase("line")!=-1){
          top = 10;
          left = 10;
          width = 180;
          height = 2;
          name = "line";
		  type = name;
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
          colour = Colours::lime;
          name = "table";
		  type = name;
		  value = 1;
		  tableNum = 1;
          name.append(String(ID), 1024);
	}
    else if(strTokens[0].indexOfIgnoreCase("pvsview")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 200;
          colour = Colours::white;
          name = "pvsview";
		  type = name;
		  value = 1;
		  tableNum = 1;
          name.append(String(ID), 1024);
		  overlapSize = 256;
		  fftSize = 1024;
		  frameSize = 1024;
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
		  decimalPlaces = 2;
		  valueY = 0;
		  type = "xypad";
          name.append(String(ID), 1024);
		  xyChannel = "";
		  colour = Colours::lime;
		  fontcolour = Colours::cornflowerblue;
	}
    else if(strTokens[0].indexOfIgnoreCase("keyboard")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 100;
          name = "keyboard";
		  type = name;
          name.append(String(ID), 1024);
		  value=60;
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
	if(str.indexOf(0, ";")!=-1)
		str = str.substring(0, str.indexOf(0, ";"));

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
	identArray.add("populate(");
	identArray.add("range(");
	identArray.add("rangex(");
	identArray.add("rangey(");
	identArray.add("plant(");
	identArray.add("alpha(");
    identArray.add("channel(");
	identArray.add(" chan(");
    identArray.add("channels(");
	identArray.add(" chans(");
    identArray.add("name(");
    identArray.add("textbox(");
	identArray.add("instrs(");
    identArray.add("caption(");
    identArray.add(",colour(");
	identArray.add(" colour(");
    identArray.add(",colours(");
	identArray.add(" colours(");
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
	identArray.add("tablenum(");
	identArray.add("tablenumbers(");
	identArray.add("tablenums(");
	identArray.add("fill(");
	identArray.add("file(");
	identArray.add("outline(");
	identArray.add("master(");
	identArray.add("shape("); 
	identArray.add("textcolour("); 
	identArray.add("scale(");
	identArray.add("chan(");
	identArray.add("key(");
	identArray.add("steps(");
	identArray.add("pluginid(");
	identArray.add("rctrls(");
	identArray.add("tracker(");
	identArray.add("preset(");
	identArray.add("popup(");
	identArray.add("fftsize(");
	identArray.add("overlap(");
	identArray.add("framesize(");
	identArray.add("pvschan(");
	identArray.add("author(");
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
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" chan(")||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" chans(") ||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" channels("))
				{
					channel = strTokens[0].trim();
					channels.add(channel);
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
					else{ 
						if(strTokens.size()>1)
						for(int i=1;i<strTokens.size();i++)
						channels.add(strTokens[i].trim());
					
					}
			}
			//don't be so LAZY! Look at all the repeated code!!
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" colour(")||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(",colour(")||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(" colours(")||
				identArray.getReference(indx).toLowerCase().equalsIgnoreCase(", colours(")){
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
									
					for(int i=0;i<strTokens.size();i++)
						colours.add(Colours::findColourForName(strTokens[i].trim(), Colours::white).toString());									
									
			}


            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("fontcolour(")){
					if(strTokens.size()<2)
						fontcolour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						fontcolour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						fontcolour = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
			}

            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tracker(")){

					if(strTokens.size()<2){
						if(strTokens[0].trim().length()>2)
						trackerFill = Colours::findColourForName(strTokens[0].trim(), Colours::white);
						else 
						trackerFill = Colour::fromRGBA (255, 255, 255, 0);
					}
					else if(strTokens.size()==4)
						trackerFill = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						trackerFill = Colour::fromRGB (strTokens[0].getIntValue(),
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
					debugMessage ="WARNING: Too many parameters passed to key(): usage key(\"key1\", \"key2\"\")";

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

			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("preset(")){
				preset = strTokens[0].trim();
			}

			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("populate(")){
				fileType = strTokens[0].trim();
				if(strTokens.size()>1) 
					workingDir = strTokens[1].trim();
			}

			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("author(")){
				author = strTokens[0].trim();
				//CabbageUtils::showMessage(author);
			}
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			//numeric paramters
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("size(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to size(): usage pos(width, height\")";
				}
				else{
					sizeText = identArray.getReference(indx)+tstr+")";
					width = strTokens[0].trim().getFloatValue();  
					height = strTokens[1].trim().getFloatValue();  
				}
            }
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("scale(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to scale(): usage scale(width, height\")";
				}
				else{
              scaleX = strTokens[0].trim().getFloatValue();  
			  scaleX = scaleX/width;
              scaleY = strTokens[1].trim().getFloatValue();  
			  scaleY = scaleY/height;
				}
            }
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("bounds(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to bounds(): usage pos(top, left width, height\")";
				}
				else{
				  left = strTokens[0].trim().getFloatValue();  
				  top = strTokens[1].trim().getFloatValue();  
				  width = strTokens[2].trim().getFloatValue();  
				  if(strTokens.size()==3)
					  height = width;
				  else
				  height = strTokens[3].trim().getFloatValue(); 
				  boundsText = identArray.getReference(indx)+tstr+")";
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
					debugMessage ="WARNING: Not enough paramters passed to pos(): usage pos(top, left\")";
				}
				else{
				  left = strTokens[0].trim().getFloatValue(); 
				  top = strTokens[1].trim().getFloatValue();
				  posText = identArray.getReference(indx)+tstr+")";
				}
            }
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("range(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value, incr\")";
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
					debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
				}
				else{
				minX = strTokens[0].trim().getFloatValue();  
				maxX = strTokens[1].trim().getFloatValue();  
				valueX = strTokens[2].trim().getFloatValue(); 
				if(strTokens.size()==4)
				decimalPlaces = strTokens[3].trim().getFloatValue();

				xypadRangeX = maxX-minX;
				}
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("rangey(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
				}
				else{
				minY = strTokens[0].trim().getFloatValue();  
				maxY = strTokens[1].trim().getFloatValue();  
				valueY = strTokens[2].trim().getFloatValue();
				if(strTokens.size()==4)
				decimalPlaces = strTokens[3].trim().getFloatValue();				
				
				xypadRangeY = maxY-minY;
				}
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("min(")){
				min = strTokens[0].trim().getFloatValue();  
			}
			else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("midiCtrl(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to midiCtrl(): usage midiCtrl(midiChan, midiCtrl\")";
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
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("master(")){
				masterSnap = strTokens[0].trim().getFloatValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("textbox(")){
				textBox = strTokens[0].trim().getFloatValue();  
			}
			
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("alpha(")){
				alpha = strTokens[0].trim().getFloatValue();  
			}			
			
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tablenum(")||
			(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tablenumber("))||
			identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tablenumbers(")||
			identArray.getReference(indx).toLowerCase().equalsIgnoreCase("tablenumbs(")){
				tableNum = strTokens[0].trim().getFloatValue();  
				tableNumbers.add(tableNum);
				if(strTokens.size()>1)
					for(int i=1;i<strTokens.size();i++)
						tableNumbers.add(strTokens[i].trim().getFloatValue());
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("popup(")){
				plantButton = strTokens[0].trim().getIntValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("fftsize(")){
				fftSize = strTokens[0].trim().getIntValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("framesize(")){
				frameSize = strTokens[0].trim().getIntValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("overlap(")){
				overlapSize = strTokens[0].trim().getIntValue();  
			}
            else if(identArray.getReference(indx).toLowerCase().equalsIgnoreCase("pvschan(")){
				pvsChannel = strTokens[0].trim().getIntValue();  
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
		if(prop.equalsIgnoreCase("width"))
			return width;
		if(prop.equalsIgnoreCase("alpha"))
			return alpha;
		else if(prop.equalsIgnoreCase("height"))
			return height;
		else if(prop.equalsIgnoreCase("textbox"))
			return textBox;
		else if(prop.equalsIgnoreCase("top"))
			return top;
		else if(prop.equalsIgnoreCase("paramIndex"))
			return paramIndex;
		else if(prop.equalsIgnoreCase("channels"))
			return channels.size();
		else if(prop.equalsIgnoreCase("left"))
			return left;
		else if(prop.equalsIgnoreCase("min"))
			return min;
		else if(prop.equalsIgnoreCase("midichan"))
			return midiChan;
		else if(prop.equalsIgnoreCase("midictrl"))
			return midiCtrl;
		else if(prop.equalsIgnoreCase("max"))
			return max;
		else if(prop.equalsIgnoreCase("sliderRange"))
			return sliderRange;
		else if(prop.equalsIgnoreCase("xypadRangeY"))
			return xypadRangeY;
		else if(prop.equalsIgnoreCase("xypadRangeX"))
			return xypadRangeX;
		else if(prop.equalsIgnoreCase("maxX"))
			return maxX;
		else if(prop.equalsIgnoreCase("maxY"))
			return maxY;
		else if(prop.equalsIgnoreCase("minX"))
			return minX;
		else if(prop.equalsIgnoreCase("minY"))
			return minY;
		else if(prop.equalsIgnoreCase("valueX"))
			return valueX;
		else if(prop.equalsIgnoreCase("valueY"))
			return valueY;
		else if(prop.equalsIgnoreCase("tabpage"))
			return tabpage;
		else if(prop.equalsIgnoreCase("noOfMenus"))
			return noOfMenus;
		else if(prop.equalsIgnoreCase("onoff"))
			return onoff;
		else if(prop.equalsIgnoreCase("value"))
			return value;
		else if(prop.equalsIgnoreCase("comboRange"))
			return comboRange;
		else if(prop.equalsIgnoreCase("line"))
			return line;
		else if(prop.equalsIgnoreCase("scaleX"))
			return scaleX;
		else if(prop.equalsIgnoreCase("scaleY"))
			return scaleY;
		else if(prop.equalsIgnoreCase("maxitems"))
			return maxItems;
		else if(prop.equalsIgnoreCase("linkTo"))
			return linkTo;
		else if(prop.equalsIgnoreCase("tableNum"))
			return tableNum;
		else if(prop.equalsIgnoreCase("sliderIncr"))
			return sliderIncr;
		else if(prop.equalsIgnoreCase("sliderSkew"))
			return sliderSkew;
		else if(prop.equalsIgnoreCase("noSteps"))
			return noSteps;
		else if(prop.equalsIgnoreCase("rCtrls"))
			return rCtrls;
		else if(prop.equalsIgnoreCase("tableNum"))
			return tableNum;
		else if(prop.equalsIgnoreCase("masterSnap"))
			return masterSnap;
		else if(prop.equalsIgnoreCase("lineIsVertical"))
			return lineIsVertical;
		else if(prop.equalsIgnoreCase("button"))
			return plantButton;
		else if(prop.equalsIgnoreCase("noPatterns"))
			return items.size();
		else if(prop.equalsIgnoreCase("fftSize"))
			return fftSize; 
		else if(prop.equalsIgnoreCase("overlapSize"))
			return overlapSize; 
		else if(prop.equalsIgnoreCase("winSize"))
			return frameSize; 
		else if(prop.equalsIgnoreCase("decimalPlaces"))
			return decimalPlaces; 
		else if(prop.equalsIgnoreCase("xyAutoIndex"))
			return xyAutoIndex;
		else if(prop.equalsIgnoreCase("pvsChannel"))
			return pvsChannel; 
		else return -9999;
}

double CabbageGUIClass::getNumProp(String prop, int index)
{
		if(prop.equalsIgnoreCase("configArray"))
			return vuConfig[index];
		else 
			return 0;
}

//set numerical attributes
void CabbageGUIClass::setNumProp(String prop, float val)
{
		if(prop.equalsIgnoreCase("width"))
			 width = val;
		else if(prop.equalsIgnoreCase("height"))
			 height = val;
		else if(prop.equalsIgnoreCase("top"))
			 top = val;
		else if(prop.equalsIgnoreCase("left"))
			 left = val;
		else if(prop.equalsIgnoreCase("min"))
			 min = val;
		else if(prop.equalsIgnoreCase("midichan"))
			 midiChan = val;
		else if(prop.equalsIgnoreCase("midictrl"))
			 midiCtrl = val;
		else if(prop.equalsIgnoreCase("max"))
			 max = val;
		else if(prop.equalsIgnoreCase("paramIndex"))
			 paramIndex = val;
		else if(prop.equalsIgnoreCase("tabpage"))
			 tabpage = val;
		else if(prop.equalsIgnoreCase("noOfMenus"))
			 noOfMenus = val;
		else if(prop.equalsIgnoreCase("onoff"))
			 onoff = val;
		else if(prop.equalsIgnoreCase("value"))
			 value = val;
		else if(prop.equalsIgnoreCase("line"))
			 line = val;
		else if(prop.equalsIgnoreCase("scaleX"))
			 scaleX = val;
		else if(prop.equalsIgnoreCase("scaleY"))
			 scaleY = val;
		else if(prop.equalsIgnoreCase("linkTo"))
			 linkTo = val;
		else if(prop.equalsIgnoreCase("sliderRange"))
			 sliderRange = val;
		else if(prop.equalsIgnoreCase("sliderIncr"))
			 sliderIncr = val;
		else if(prop.equalsIgnoreCase("xypadRangeY"))
			 xypadRangeY = val;
		else if(prop.equalsIgnoreCase("xypadRangeX"))
			 xypadRangeX = val;
		else if(prop.equalsIgnoreCase("maxX"))
			 maxX =val;
		else if(prop.equalsIgnoreCase("maxY"))
			 maxY = val;
		else if(prop.equalsIgnoreCase("minX"))
			 minX = val;
		else if(prop.equalsIgnoreCase("minY"))
			 minY = val;
		else if(prop.equalsIgnoreCase("valueX"))
			 valueX = val;
		else if(prop.equalsIgnoreCase("valueY"))
			 valueY = val;
		else if(prop.equalsIgnoreCase("button"))
			 plantButton = val;
		else if(prop.equalsIgnoreCase("winSize"))
			 fftSize = val;
		else if(prop.equalsIgnoreCase("frameSize"))
			 frameSize = val;
		else if(prop.equalsIgnoreCase("overlapSize"))
			 overlapSize = val;
		else if(prop.equalsIgnoreCase("pvsChannel"))
			 pvsChannel = val;
		else if(prop.equalsIgnoreCase("xyAutoIndex"))
			 xyAutoIndex = val;


}


float CabbageGUIClass::getTableChannelValues(int index)
{
if(index<tableChannelValues.size())
	return tableChannelValues.getReference(index);
else
	return 0.f;	
}

void CabbageGUIClass::addTableChannelValues()
{
tableChannelValues.add(0.f);
}

void CabbageGUIClass::setTableChannelValues(int index, float val)
{
if(index<tableChannelValues.size())
tableChannelValues.set(index, val);		
}


String CabbageGUIClass::getPropsString()
{
	return type << String(" bounds(") << String(left) << String(", ") << String(top) << String(", ") << String(width)
				<< String(", ") << String(height) << String("), channel(\"") << channel << String("), xyChannel(\"") << xyChannel << String("\"), value(")
				<< String(value) << String("), items(\"") << items[0].trim() << String("\", \"") << items[1].trim() << String("\")")
				<< String("), range(\"") << String(min) << String(", ") << String(max) << String(", ") << String(value) << String("\")");
}

String CabbageGUIClass::getStringProp(String prop, int index)
{
		if(prop.equalsIgnoreCase("channel"))
			return channels[index].trim();
		else if(prop.equalsIgnoreCase("snapshotData"))
			return snapshotData[index].trim();
		else	
		return "";
}

Rectangle<int> CabbageGUIClass::getComponentBounds()
{
Rectangle<int> rect;
rect.setBounds(left, top, width, height);
return rect;
}

String CabbageGUIClass::getStringProp(String prop)
{
		if(prop.equalsIgnoreCase("channel"))
			return channel.trim();
		else if(prop.equalsIgnoreCase("xyChannel"))
			return xyChannel.trim();
		else if(prop.equalsIgnoreCase("xChannel"))
			return xChannel.trim();
		else if(prop.equalsIgnoreCase("yChannel"))
			return yChannel.trim();
		else if(prop.equalsIgnoreCase("name"))
			return name.trim();
		else if(prop.equalsIgnoreCase("initBoundsText"))
			return boundsText.trim();
		else if(prop.equalsIgnoreCase("newBoundsText")){
			String bounds;
			bounds << "bounds(" << String(left) << ", " << String(top) << ", " << String(width) << ", " << String(height) << ")";
			return bounds;
		}
		else if(prop.equalsIgnoreCase("pos"))
			return posText.trim();
		else if(prop.equalsIgnoreCase("fileType"))
			return fileType.trim();
		else if(prop.equalsIgnoreCase("workingDir"))
			return workingDir.trim();
		else if(prop.equalsIgnoreCase("size"))
			return sizeText.trim();
		else if(prop.equalsIgnoreCase("text"))
			return text.trim();
		else if(prop.equalsIgnoreCase("type"))
			return type.trim();
		else if(prop.equalsIgnoreCase("shape"))
			return shape.trim();
		else if(prop.equalsIgnoreCase("beveltype"))
			return beveltype.trim();
		else if(prop.equalsIgnoreCase("caption"))
			return caption.trim();
		else if(prop.equalsIgnoreCase("kind"))
			return kind.trim();
		else if(prop.equalsIgnoreCase("topitem"))
			return topitem.trim();
		else if(prop.equalsIgnoreCase("file"))
			return file.trim();
		else if(prop.equalsIgnoreCase("cssetup"))
			return cssetup.trim();
		else if(prop.equalsIgnoreCase("csstdout"))
			return csstdout.trim();
		else if(prop.equalsIgnoreCase("exit"))
			return exit.trim();
		else if(prop.equalsIgnoreCase("plant"))
			return plant.trim();
		else if(prop.equalsIgnoreCase("plantText")){
			String plantText;
			plantText << "plant(\"" << plant.trim() << "\")";
			return plantText;
		}
		else if(prop.equalsIgnoreCase("reltoplant"))
			return reltoplant.trim();
		else if(prop.equalsIgnoreCase("debugmessage"))
			return debugMessage;
		else if(prop.equalsIgnoreCase("preset"))
			return preset.trim();
		else if(prop.equalsIgnoreCase("pluginID"))
			return pluginID;
		else if(prop.equalsIgnoreCase("author"))
			return author;
		else if(prop.equalsIgnoreCase("snapshotData")){
			String data;
			data << "------------------------ Instrument ID: " << preset << "\n";
			data << snapshotData.joinIntoString("\n") << "------------------------ End of Instrument ID: " << preset << "\n";
			return data;
		}
		else return "";
}

void CabbageGUIClass::setStringProp(String prop, int index, String value)
{		
			if(prop.equalsIgnoreCase("channel")){
				channels.set(index, value);
			}
			else if(prop.equalsIgnoreCase("snapshotData")){
				snapshotData.set(index, value);
			}
}

void CabbageGUIClass::setStringProp(String prop, String val)
{
		if(prop.equalsIgnoreCase("channel"))
			channel = val;
		else if(prop.equalsIgnoreCase("xyChannel"))
			xyChannel = val;
		else if(prop.equalsIgnoreCase("yChannel"))
			yChannel = val;
		else if(prop.equalsIgnoreCase("xChannel"))
			xChannel = val;
		else if(prop.equalsIgnoreCase("name"))
			name = val;
		else if(prop.equalsIgnoreCase("text"))
			text = val;
		else if(prop.equalsIgnoreCase("type"))
			type = val;
		else if(prop.equalsIgnoreCase("shape"))
			shape = val;
		else if(prop.equalsIgnoreCase("beveltype"))
			beveltype = val;
		else if(prop.equalsIgnoreCase("caption"))
			caption = val;
		else if(prop.equalsIgnoreCase("kind"))
			kind = val;
		else if(prop.equalsIgnoreCase("topitem"))
			topitem = val;
		else if(prop.equalsIgnoreCase("file"))
			file = val;
		else if(prop.equalsIgnoreCase("cssetup"))
			cssetup = val;
		else if(prop.equalsIgnoreCase("csstdout"))
			csstdout = val;
		else if(prop.equalsIgnoreCase("exit"))
			exit = val;
		else if(prop.equalsIgnoreCase("plant"))
			plant = val;
		else if(prop.equalsIgnoreCase("preset"))
			preset = val;
		else if(prop.equalsIgnoreCase("snapshotData"))
			snapshotData.addLines(val);
		else if(prop.equalsIgnoreCase("reltoplant"))
			reltoplant = val;
}

String CabbageGUIClass::getColourProp(String prop)
{
		if(prop.equalsIgnoreCase("textcolour"))
			return textcolour.toString();
		else if(prop.equalsIgnoreCase("fontcolour"))
			return fontcolour.toString();
		else if(prop.equalsIgnoreCase("colour"))
			return colour.toString();
		else if(prop.equalsIgnoreCase("fill"))
			return fill.toString();
		else if(prop.equalsIgnoreCase("outline"))
			return outline.toString();
		else if(prop.equalsIgnoreCase("tracker"))
			return trackerFill.toString();
		else return "";

}
