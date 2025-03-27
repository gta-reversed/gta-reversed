/****************************************************************************
 *
 * File: rwg/rwsdk/plugin/adc/rpenv.h
 *
 * Copyright (C) 2003 Criterion Technologies.
 *
 * Purpose: RenderWare environment plugin.
 *
 ****************************************************************************/
/* RWPUBLIC */

#ifndef _RPENV_H_
#define _RPENV_H_

/**
 * \defgroup rpenv RpEnvironment
 * \ingroup cameras
 *
 * Environment Plug-In for RenderWare.
 */

typedef struct RpEnvironment RpEnvironment;

/**
 * \ingroup rpenv
 * \struct RpEnvironment
 * Environment settings.
 */
struct RpEnvironment
{
    RwRGBA  bgColor;        /**< The background color for this RpEnvironment
                                 setting. */
};

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */


extern RpEnvironment *
RpEnvironmentCreate(void);

extern void 
RpEnvironmentDestroy(RpEnvironment *environment);

extern RwInt32 
RpEnvironmentStreamGetSize(RpEnvironment *environment);

extern RpEnvironment *
RpEnvironmentStreamRead(RwStream *stream);

extern RpEnvironment *
RpEnvironmentStreamWrite(RpEnvironment *environment,
                         RwStream      *stream);

extern void 
RpEnvironmentSetBackground(RpEnvironment *environment,
                           RwRGBA         color);

extern RwRGBA *
RpEnvironmentGetBackground(RpEnvironment *environment);
                                      

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _RPENV_H_ */

/* RWPUBLICEND */
