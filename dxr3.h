#ifndef _DXR3_H_
#define _DXR3_H_

// color setting bar
static const char *SettingBar[] =
{
	"[................................]",
	"[|...............................]",
	"[||..............................]",
	"[|||.............................]",
	"[||||............................]",
	"[|||||...........................]",
	"[||||||..........................]",
	"[|||||||.........................]",
	"[||||||||........................]",
	"[|||||||||.......................]",
	"[||||||||||......................]",
	"[|||||||||||.....................]",
	"[||||||||||||....................]",
	"[|||||||||||||...................]",
	"[||||||||||||||..................]",
	"[|||||||||||||||.................]",
	"[||||||||||||||||................]",
	"[|||||||||||||||||...............]",
	"[||||||||||||||||||..............]",
	"[|||||||||||||||||||.............]",
	"[||||||||||||||||||||............]",
	"[|||||||||||||||||||||...........]",
	"[||||||||||||||||||||||..........]",
	"[|||||||||||||||||||||||.........]",
	"[||||||||||||||||||||||||........]",
	"[|||||||||||||||||||||||||.......]",
	"[||||||||||||||||||||||||||......]",
	"[|||||||||||||||||||||||||||.....]",
	"[||||||||||||||||||||||||||||....]",
	"[|||||||||||||||||||||||||||||...]",
	"[||||||||||||||||||||||||||||||..]",
	"[|||||||||||||||||||||||||||||||.]"
	"[||||||||||||||||||||||||||||||||]"	// 32 x |
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
	const char *menuVideoModes[3];
	const char *menuDebugModes[2];
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
enum eDxr3ColorItem 
{
	DXR3_BRIGHTNESS,
	DXR3_CONTRAST,
	DXR3_SATURATION
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
// used to change color settings
class cDxr3OsdColorItem : public cMenuEditItem 
{
public:
	cDxr3OsdColorItem(const char* text, eDxr3ColorItem item);
	virtual eOSState ProcessKey(eKeys Key);

protected:
	eDxr3ColorItem	m_item;
	int				m_value;
	int				m_min;
	int				m_max;

	virtual void Set();
};

/*
// ==================================
// used to change color settings
class cDxr3OsdColorItem : public cOsdItem//cMenuEditItem
{
public:    
    cDxr3OsdColorItem(const char* text, eDxr3ColorItem item);

	cDxr3OsdColorItem::~cDxr3OsdColorItem()
	{
		free(m_name);
		free(m_caption);
	}

	eOSState ProcessKey(eKeys Key);

protected:
	eDxr3ColorItem	m_item;
	int				m_value;
	int				m_min;
	int				m_max;
	char*			m_caption;
	char*			m_name;

	void Set();
	void SetValue(const char *Value);
};
*/

// ==================================
// main screen
class cDxr3OsdMenu : public cOsdMenu 
{
public:
    cDxr3OsdMenu();

	int b,c,s;
};

#endif /*_DXR3_H_*/
