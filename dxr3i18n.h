#ifndef _DXR3_I18N_H_
#define _DXR3_I18N_H_

#include <vdr/i18n.h>

extern const char *i18n_name;
extern const tI18nPhrase Phrases[];

#undef tr
#define tr(s)  I18nTranslate(s, i18n_name)

#endif /*_DXR3_I18N_H_*/
