#ifndef _DXR3_H_
#define _DXR3_H_

// --- cMenuSetupDxr3 -------------------------------------------------------
const char* menuVideoModes[] = 
{
    "PAL",
	"PAL60",
    "NTSC"
};

// debug modes
const char* menuDebugModes[] = 
{
	"Low",
	"Everything"
};

// ==================================
// setup screen
class cMenuSetupDxr3 : public cMenuSetupPage 
{
public:
	cMenuSetupDxr3();
	
protected:
	virtual void Store();
	
private:
	int newUseDigitalOut;
	int newDxr3Card;
	int newVideoMode;
	int newDebug;
	int newDebugLevel;
};


// ==================================
enum eDxr3OsdItem 
{
    DXR3_RESET_HARDWARE,
    DXR3_FORCE_LETTER_BOX,
    DXR3_DIGITAL_OUT,
    DXR3_ANALOG_OUT,
    DXR3_AC3_OUT
};

// ==================================
// osd item
class cDxr3OsdItem : public cOsdItem 
{
public:    
    cDxr3OsdItem(const char* text, eDxr3OsdItem item) : cOsdItem(text), m_item(item) {}
	
	// process fb input
    eOSState ProcessKey(eKeys Key);

protected:
    eDxr3OsdItem m_item; 
};

// ==================================
// main screen
class cDxr3OsdMenu : public cOsdMenu 
{
public:
    cDxr3OsdMenu(): cOsdMenu("DXR3 Adjustment") 
	{
        Clear();
        SetHasHotkeys();
        Add(new cDxr3OsdItem(hk("Reset DXR3 Hardware"), DXR3_RESET_HARDWARE));        
        Add(new cDxr3OsdItem(hk("Toggle Force LetterBox"), DXR3_FORCE_LETTER_BOX));

		if (cDxr3ConfigData::Instance().GetUseDigitalOut())
		{
			Add(new cDxr3OsdItem(hk("Analog Output"), DXR3_ANALOG_OUT));
		}
		else
		{
			Add(new cDxr3OsdItem(hk("Digital Output"), DXR3_DIGITAL_OUT));
		}
/*		
        if (cDxr3ConfigData::Instance().GetUseDigitalOut()) 
		{
			Add(new cDxr3OsdItem(hk("Analog Output"), DXR3_ANALOG_OUT));

            if (cDxr3ConfigData::Instance().GetAc3OutPut()) 
			{
                Add(new cDxr3OsdItem(hk("AC3 Output Off"), DXR3_AC3_OUT));
            } 
			else 
			{
                if (cDxr3Interface::Instance().IsAc3Present()) 
				{
                    Add(new cDxr3OsdItem(hk("AC3 Output On"), DXR3_AC3_OUT));
                }
            }
        } 
		else 
		{
            Add(new cDxr3OsdItem(hk("Digital Output"), DXR3_DIGITAL_OUT));
        }
		*/
    }
};

#endif /*_DXR3_H_*/
