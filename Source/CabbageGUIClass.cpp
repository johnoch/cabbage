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
								midiCtrl(-99)
{
//Default values are assigned to all attributres 
//before parsing begins
	items.clear();
	items.add(T(""));
	key.clear();

	//changing case to lower to make syntax non case-sensitive
	if(compStr.indexOfIgnoreCase("hslider ")!=-1){
          top = 10;
          left = 10;
          width = 250;
          height = 20;
          channel = "hslider";
          kind = "horizontal";
          min = 0;
          max = 100;	
		  caption = "";
          name = "hslider";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("vslider ")!=-1){
          top = 10;
          left = 10;
          width = 50;
          height = 180;
          channel = "vslider";
          kind = "vertical";
          min = 0;
          max = 100;	
		  caption = "";
          name = "vslider";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("rslider ")!=-1){
          top = 10;
          left = 10;
          width = 120;
          height = 80;
          channel = "rslider";
          kind = "rotary";
          min = 0;
          max = 100;	
		  caption = "";
          name = "rslider";
		  type = name;
          name.append(String(ID), 1024);
	}

    else if(compStr.indexOfIgnoreCase("button ")!=-1){
          top = 10;
          left = 10;
          width = 70;
          height = 22;
          channel = "buttonchan";
          items.add(T("off"));
          items.add(T("on"));
          name = "button";
		  max = 1;
		  min = 0;
		  caption = "";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("checkbox ")!=-1){
          top = 10;
          left = 10;
          width = 100;
          height = 22;
          channel = "checkchan";
          caption = "";
          name = "checkbox";
		  caption = "";
		  items.add(T(""));
		  min = 0;
		  max = 1;
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("combobox ")!=-1){
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
    else if(compStr.indexOfIgnoreCase("label ")!=-1){
          top = 10;
          left = 10;
          width = 100;
          height = 16;
          channel = "label";
          beveltype = "none";
          colour = "LIGHT GREY";
          fontcolour = "BLACK";
          text = "hello";
          name = "label";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("image ")!=-1){
          top = 10;
          left = 10;
          width = 160;
          height = 120;
          channel = "image";
          beveltype = "none";
          colour = "";
          fontcolour = "";
          text = "";
		  line = 2;
          name = "image";
		  type = name;
		  shape="rounded";
		  outline = "black";
		  colour = "white";
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("groupbox ")!=-1){
          top = 10;
          left = 10;
          width = 80;
          height = 22;
          colour = "LIGHT GREY";
          name = "groupbox";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("keyboard ")!=-1){
          top = 10;
          left = 10;
          width = 400;
          height = 100;
          colour = "LIGHT GREY";
          name = "keyboard";
		  type = name;
          name.append(String(ID), 1024);
	}
    else if(compStr.indexOfIgnoreCase("form ")!=-1){
          top = 10;
          left = 10;
          width = 280;
          height = 122;
          name = "form";
		  type = "form";
	}

    else if(compStr.indexOfIgnoreCase("hostbpm ")!=-1){
          name = "hostbpm";
		  type = "hostbpm";
	}

    else if(compStr.indexOfIgnoreCase("hostppqpos ")!=-1){
          name = "hostppqpos";
		  type = "hostppqpos";
	}

    else if(compStr.indexOfIgnoreCase("hostplaying ")!=-1){
          name = "hostplaying";
		  type = "hostplaying";
	}

    else if(compStr.indexOfIgnoreCase("hostrecording ")!=-1){
          name = "hostrecording";
		  type = "hostrecording";
	}

    else if(compStr.indexOfIgnoreCase("hosttime ")!=-1){
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
	identArray.add("plant(");
    identArray.add("channel(");
    identArray.add("name(");
    identArray.add("caption(");
    identArray.add(",colour(");
	identArray.add(" colour(");
    identArray.add("topitem(");
    identArray.add("menuitem(");
    identArray.add("stdout(");
    identArray.add("exit(");
    identArray.add("cssetup(");
    identArray.add("kind(");
    identArray.add("fontcolour(");
    identArray.add("beveltype(");
    identArray.add("items(");
	identArray.add("text(");
    identArray.add("runcsound(");
	identArray.add("tabs(");
	identArray.add("fill(");
	identArray.add("file(");
	identArray.add("outline(");
	identArray.add("shape("); 
	identArray.add("textcolour("); 
	//add a few dummy identifiers so we can catch bogus one in the Cabbage code
	identArray.add("scale(");
	identArray.add("chan(");
	identArray.add("key(");
	identArray.add("");
	identArray.add("");
	identArray.add("");

	//retrieve paramters consisting of strings
	for(int indx=0;indx<identArray.size();indx++)
	{		
		int identPos = str.indexOf(identArray.getReference(indx));
          if(identPos!=-1){
			String newString = str.substring(identPos+identArray.getReference(indx).length());
			String tstr = newString.substring(0, newString.indexOf(0, ")"));
			if(tstr.length()==0) return 0;

			StringArray strTokens;
			strTokens.addTokens(tstr.removeCharacters(")\""), ",", "\"");
				
			if(identArray.getReference(indx).equalsIgnoreCase("name(")) name = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("plant(")) plant = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("caption(")) caption = strTokens[0].trim();
            else if(identArray.getReference(indx).equalsIgnoreCase("channel(")||
				identArray.getReference(indx).equalsIgnoreCase("chan(")) channel = strTokens[0].trim();
            else if(identArray.getReference(indx).equalsIgnoreCase(" colour(")||
				identArray.getReference(indx).equalsIgnoreCase(",colour(")) colour = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("kind(")) kind = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("file(")) file = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("fill(")) fill = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("shape(")) shape = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("outline(")) outline = strTokens[0].trim();
			else if(identArray.getReference(indx).equalsIgnoreCase("textcolour(")) textcolour = strTokens[0].trim();

            else if(identArray.getReference(indx).equalsIgnoreCase("key(")){
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

            else if(identArray.getReference(indx).equalsIgnoreCase("items(")||
					identArray.getReference(indx).equalsIgnoreCase("text(")){
              items.clear();//clear any unwanted items
			  for(int i= 0;i<(int)strTokens.size();i++){
				String test = strTokens[i]; 
				items.add(strTokens[i]);	
				max = i;
              }
			}
				
			//numberic paramters
			else if(identArray.getReference(indx).equalsIgnoreCase("size(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to size(): usage pos(width, height\")");
				}
				else{
					width = strTokens[0].trim().getFloatValue();  
					height = strTokens[1].trim().getFloatValue();  
				}
            }
			else if(identArray.getReference(indx).equalsIgnoreCase("scale(")){
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
			else if(identArray.getReference(indx).equalsIgnoreCase("bounds(")){
				if(strTokens.size()<4){
					debugMessage = T("WARNING: Not enough paramters passed to bounds(): usage pos(top, left width, height\")");
				}
				else{
              left = strTokens[0].trim().getFloatValue();  
              top = strTokens[1].trim().getFloatValue();  
              width = strTokens[2].trim().getFloatValue();  
              height = strTokens[3].trim().getFloatValue();  
				}
            }
            else if(identArray.getReference(indx).equalsIgnoreCase("pos(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to pos(): usage pos(top, left\")");
				}
				else{
              left = strTokens[0].trim().getFloatValue(); 
              top = strTokens[1].trim().getFloatValue(); 
			  //if(strTokens.size()>2)
			  //reltoplant = strTokens[2].trim();
				}
            }
			else if(identArray.getReference(indx).equalsIgnoreCase("range(")){
				if(strTokens.size()<3){
					debugMessage =T("WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")");
				}
				else{
				min = strTokens[0].trim().getFloatValue();  
				max = strTokens[1].trim().getFloatValue();  
				value = strTokens[2].trim().getFloatValue();  
				}
			}
			else if(identArray.getReference(indx).equalsIgnoreCase("min(")){
				min = strTokens[0].trim().getFloatValue();  
			}
			else if(identArray.getReference(indx).equalsIgnoreCase("midiCtrl(")){
				if(strTokens.size()<2){
					debugMessage =T("WARNING: Not enough paramters passed to midiCtrl(): usage midiCtrl(midiChan, midiCtrl\")");
				}
				else{
				midiChan = strTokens[0].trim().getFloatValue();  
				midiCtrl = strTokens[1].trim().getFloatValue();  
			}
			}
            else if(identArray.getReference(indx).equalsIgnoreCase("max(")) max = strTokens[0].trim().getFloatValue();  
			else if(identArray.getReference(indx).equalsIgnoreCase(",line(")||
					identArray.getReference(indx).equalsIgnoreCase(" line(")) line = strTokens[0].trim().getFloatValue();  
            else if(identArray.getReference(indx).equalsIgnoreCase("value(")) value = strTokens[0].trim().getFloatValue();  
			else if(identArray.getReference(indx).equalsIgnoreCase("tabpage(")) tabpage = strTokens[0].trim().getFloatValue();  
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
