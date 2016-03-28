/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Header describing the function stubs for calling OpenGL
*               utility API functions.

****************************************************************************/

#ifndef __GLU_STUB_H
#define __GLU_STUB_H

/*--------------------- Macros and type definitions -----------------------*/

/* Structure containing all public OpenGL functions supported by the MGL */

#pragma pack(1)
typedef struct GA_gluFuncs {
    long    dwSize;
    void    (APIENTRYP gluBeginCurve)(GLUnurbs *nobj);
    void    (APIENTRYP gluBeginPolygon)(GLUtesselator *tess);
    void    (APIENTRYP gluBeginSurface)(GLUnurbs *nobj);
    void    (APIENTRYP gluBeginTrim)(GLUnurbs *nobj);
    int     (APIENTRYP gluBuild1DMipmaps)(GLenum target,GLint components,GLint width,GLenum format,GLenum type,const void *data);
    int     (APIENTRYP gluBuild2DMipmaps)(GLenum target,GLint components,GLint width,GLint height,GLenum format,GLenum type,const void *data);
    void    (APIENTRYP gluCylinder)(GLUquadric *qobj,GLdouble baseRadius,GLdouble topRadius,GLdouble height,GLint slices,GLint stacks);
    void    (APIENTRYP gluDeleteNurbsRenderer)(GLUnurbs *nobj);
    void    (APIENTRYP gluDeleteQuadric)(GLUquadric *state);
    void    (APIENTRYP gluDeleteTess)(GLUtesselator *tess);
    void    (APIENTRYP gluDisk)(GLUquadric *qobj,GLdouble innerRadius,GLdouble outerRadius,GLint slices,GLint loops);
    void    (APIENTRYP gluEndCurve)(GLUnurbs *nobj);
    void    (APIENTRYP gluEndPolygon)(GLUtesselator *tess);
    void    (APIENTRYP gluEndSurface)(GLUnurbs *nobj);
    void    (APIENTRYP gluEndTrim)(GLUnurbs *nobj);
    const GLubyte*  (APIENTRYP gluErrorString)(GLenum errCode);
    void    (APIENTRYP gluGetNurbsProperty)(GLUnurbs *nobj,GLenum property,GLfloat *value);
    const GLubyte*  (APIENTRYP gluGetString)(GLenum name);
    void    (APIENTRYP gluGetTessProperty)(GLUtesselator *tess,GLenum which,GLdouble *value);
    void    (APIENTRYP gluLoadSamplingMatrices)(GLUnurbs *nobj,const GLfloat modelMatrix[16],const GLfloat projMatrix[16],const GLint viewport[4]);
    void    (APIENTRYP gluLookAt)(GLdouble eyex,GLdouble eyey,GLdouble eyez,GLdouble centerx,GLdouble centery,GLdouble centerz,GLdouble upx,GLdouble upy,GLdouble upz);
    GLUnurbs* (APIENTRYP gluNewNurbsRenderer)(void);
    GLUquadric* (APIENTRYP gluNewQuadric)(void);
    GLUtesselator* (APIENTRYP gluNewTess)(void );
    void    (APIENTRYP gluNextContour)(GLUtesselator *tess,GLenum type);
    void    (APIENTRYP gluNurbsCallback)(GLUnurbs *nobj,GLenum which,void (CALLBACKP fn)());
    void    (APIENTRYP gluNurbsCurve)(GLUnurbs *nobj,GLint nknots,GLfloat *knot,GLint stride,GLfloat *ctlarray,GLint order,GLenum type);
    void    (APIENTRYP gluNurbsProperty)(GLUnurbs *nobj,GLenum property,GLfloat value);
    void    (APIENTRYP gluNurbsSurface)(GLUnurbs *nobj,GLint sknot_count,float *sknot,GLint tknot_count,GLfloat *tknot,GLint s_stride,GLint t_stride,GLfloat *ctlarray,GLint sorder,GLint torder,GLenum type);
    void    (APIENTRYP gluOrtho2D)(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top);
    void    (APIENTRYP gluPartialDisk)(GLUquadric *qobj,GLdouble innerRadius,GLdouble outerRadius,GLint slices,GLint loops,GLdouble startAngle,GLdouble sweepAngle);
    void    (APIENTRYP gluPerspective)(GLdouble fovy,GLdouble aspect,GLdouble zNear,GLdouble zFar);
    void    (APIENTRYP gluPickMatrix)(GLdouble x,GLdouble y,GLdouble width,GLdouble height,GLint *viewport);
    int     (APIENTRYP gluProject)(GLdouble objx,GLdouble objy,GLdouble objz,const GLdouble modelMatrix[16],const GLdouble projMatrix[16],const GLint viewport[4],GLdouble *winx,GLdouble *winy,GLdouble *winz);
    void    (APIENTRYP gluPwlCurve)(GLUnurbs *nobj,GLint count,GLfloat *array,GLint stride,GLenum type);
    void    (APIENTRYP gluQuadricCallback)(GLUquadric *qobj,GLenum which,void (CALLBACKP fn)());
    void    (APIENTRYP gluQuadricDrawStyle)(GLUquadric *quadObject,GLenum drawStyle);
    void    (APIENTRYP gluQuadricNormals)(GLUquadric *quadObject,GLenum normals);
    void    (APIENTRYP gluQuadricOrientation)(GLUquadric *quadObject,GLenum orientation);
    void    (APIENTRYP gluQuadricTexture)(GLUquadric *quadObject,GLboolean textureCoords);
    int     (APIENTRYP gluScaleImage)(GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut);
    void    (APIENTRYP gluSphere)(GLUquadric *qobj,GLdouble radius,GLint slices,GLint stacks);
    void    (APIENTRYP gluTessBeginContour)(GLUtesselator *tess);
    void    (APIENTRYP gluTessBeginPolygon)(GLUtesselator *tess,GLvoid* data);
    void    (APIENTRYP gluTessCallback)(GLUtesselator *tess,GLenum which,void (CALLBACKP fn)());
    void    (APIENTRYP gluTessEndContour)(GLUtesselator *tess);
    void    (APIENTRYP gluTessEndPolygon)(GLUtesselator *tess);
    void    (APIENTRYP gluTessNormal)(GLUtesselator *tess,GLdouble x,GLdouble y,GLdouble z);
    void    (APIENTRYP gluTessProperty)(GLUtesselator *tess,GLenum which,GLdouble value);
    void    (APIENTRYP gluTessVertex)(GLUtesselator *tess,GLdouble *location,GLvoid* data);
    int     (APIENTRYP gluUnProject)(GLdouble winx,GLdouble winy,GLdouble winz,const GLdouble modelMatrix[16],const GLdouble projMatrix[16],const GLint viewport[4],GLdouble *objx,GLdouble *objy,GLdouble *objz);
    } GA_gluFuncs;
#pragma pack()

#if !defined(NO_GLU_MACROS)
#define gluBeginCurve(nobj)                                                                                     _GA_gluFuncs.gluBeginCurve(nobj)
#define gluBeginPolygon(tess)                                                                                   _GA_gluFuncs.gluBeginPolygon(tess)
#define gluBeginSurface(nobj)                                                                                   _GA_gluFuncs.gluBeginSurface(nobj)
#define gluBeginTrim(nobj)                                                                                      _GA_gluFuncs.gluBeginTrim(nobj)
#define gluBuild1DMipmaps(target,components,width,format,type,data)                                             _GA_gluFuncs.gluBuild1DMipmaps(target,components,width,format,type,data)
#define gluBuild2DMipmaps(target,components,width,height,format,type,data)                                      _GA_gluFuncs.gluBuild2DMipmaps(target,components,width,height,format,type,data)
#define gluCylinder(qobj,baseRadius,topRadius,height,slices,stacks)                                             _GA_gluFuncs.gluCylinder(qobj,baseRadius,topRadius,height,slices,stacks)
#define gluDeleteNurbsRenderer(nobj)                                                                            _GA_gluFuncs.gluDeleteNurbsRenderer(nobj)
#define gluDeleteQuadric(state)                                                                                 _GA_gluFuncs.gluDeleteQuadric(state)
#define gluDeleteTess(tess)                                                                                     _GA_gluFuncs.gluDeleteTess(tess)
#define gluDisk(qobj,innerRadius,outerRadius,slices,loops)                                                      _GA_gluFuncs.gluDisk(qobj,innerRadius,outerRadius,slices,loops)
#define gluEndCurve(nobj)                                                                                       _GA_gluFuncs.gluEndCurve(nobj)
#define gluEndPolygon(tess)                                                                                     _GA_gluFuncs.gluEndPolygon(tess)
#define gluEndSurface(nobj)                                                                                     _GA_gluFuncs.gluEndSurface(nobj)
#define gluEndTrim(nobj)                                                                                        _GA_gluFuncs.gluEndTrim(nobj)
#define gluErrorString(errCode)                                                                                 _GA_gluFuncs.gluErrorString(errCode)
#define gluGetNurbsProperty(nobj,property,value)                                                                _GA_gluFuncs.gluGetNurbsProperty(nobj,property,value)
#define gluGetString(name)                                                                                      _GA_gluFuncs.gluGetString(name)
#define gluGetTessProperty(tess,which,value)                                                                    _GA_gluFuncs.gluGetTessProperty(tess,which,value)
#define gluLoadSamplingMatrices(nobj,modelMatrix,projMatrix,viewport)                                           _GA_gluFuncs.gluLoadSamplingMatrices(nobj,modelMatrix,projMatrix,viewport)
#define gluLookAt(eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz)                                           _GA_gluFuncs.gluLookAt(eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz)
#define gluNewNurbsRenderer()                                                                                   _GA_gluFuncs.gluNewNurbsRenderer()
#define gluNewQuadric()                                                                                         _GA_gluFuncs.gluNewQuadric()
#define gluNewTess()                                                                                            _GA_gluFuncs.gluNewTess()
#define gluNextContour(tess,type)                                                                               _GA_gluFuncs.gluNextContour(tess,type)
#define gluNurbsCallback(nobj,which,fn)                                                                         _GA_gluFuncs.gluNurbsCallback(nobj,which,fn)
#define gluNurbsCurve(nobj,nknots,knot,stride,ctlarray,order,type)                                              _GA_gluFuncs.gluNurbsCurve(nobj,nknots,knot,stride,ctlarray,order,type)
#define gluNurbsProperty(nobj,property,value)                                                                   _GA_gluFuncs.gluNurbsProperty(nobj,property,value)
#define gluNurbsSurface(nobj,sknot_count,sknot,tknot_count,tknot,s_stride,t_stride,ctlarray,sorder,torder,type) _GA_gluFuncs.gluNurbsSurface(nobj,sknot_count,sknot,tknot_count,tknot,s_stride,t_stride,ctlarray,sorder,torder,type)
#define gluOrtho2D(left,right,bottom,top)                                                                       _GA_gluFuncs.gluOrtho2D(left,right,bottom,top)
#define gluPartialDisk(qobj,innerRadius,outerRadius,slices,loops,startAngle,sweepAngle)                         _GA_gluFuncs.gluPartialDisk(qobj,innerRadius,outerRadius,slices,loops,startAngle,sweepAngle)
#define gluPerspective(fovy,aspect,zNear,zFar)                                                                  _GA_gluFuncs.gluPerspective(fovy,aspect,zNear,zFar)
#define gluPickMatrix(x,y,width,height,viewport)                                                                _GA_gluFuncs.gluPickMatrix(x,y,width,height,viewport)
#define gluProject(objx,objy,objz,modelMatrix,projMatrix,viewport,winx,winy,winz)                               _GA_gluFuncs.gluProject(objx,objy,objz,modelMatrix,projMatrix,viewport,winx,winy,winz)
#define gluPwlCurve(nobj,count,array,stride,type)                                                               _GA_gluFuncs.gluPwlCurve(nobj,count,array,stride,type)
#define gluQuadricCallback(qobj,which,fn)                                                                       _GA_gluFuncs.gluQuadricCallback(qobj,which,fn)
#define gluQuadricDrawStyle(quadObject,drawStyle)                                                               _GA_gluFuncs.gluQuadricDrawStyle(quadObject,drawStyle)
#define gluQuadricNormals(quadObject,normals)                                                                   _GA_gluFuncs.gluQuadricNormals(quadObject,normals)
#define gluQuadricOrientation(quadObject,orientation)                                                           _GA_gluFuncs.gluQuadricOrientation(quadObject,orientation)
#define gluQuadricTexture(quadObject,textureCoords)                                                             _GA_gluFuncs.gluQuadricTexture(quadObject,textureCoords)
#define gluScaleImage(format,widthin,heightin,typein,datain,widthout,heightout,typeout,dataout)                 _GA_gluFuncs.gluScaleImage(format,widthin,heightin,typein,datain,widthout,heightout,typeout,dataout)
#define gluSphere(qobj,radius,slices,stacks)                                                                    _GA_gluFuncs.gluSphere(qobj,radius,slices,stacks)
#define gluTessBeginContour(tess)                                                                               _GA_gluFuncs.gluTessBeginContour(tess)
#define gluTessBeginPolygon(tess,polygon_data)                                                                  _GA_gluFuncs.gluTessBeginPolygon(tess,polygon_data)
#define gluTessCallback(tess,which,fn)                                                                          _GA_gluFuncs.gluTessCallback(tess,which,fn)
#define gluTessEndContour(tess)                                                                                 _GA_gluFuncs.gluTessEndContour(tess)
#define gluTessEndPolygon(tess)                                                                                 _GA_gluFuncs.gluTessEndPolygon(tess)
#define gluTessNormal(tess,x,y,z)                                                                               _GA_gluFuncs.gluTessNormal(tess,x,y,z)
#define gluTessProperty(tess,which,value)                                                                       _GA_gluFuncs.gluTessProperty(tess,which,value)
#define gluTessVertex(tess,coords,data)                                                                         _GA_gluFuncs.gluTessVertex(tess,coords,data)
#define gluUnProject(winx,winy,winz,modelMatrix,projMatrix,viewport,objx,objy,objz)                             _GA_gluFuncs.gluUnProject(winx,winy,winz,modelMatrix,projMatrix,viewport,objx,objy,objz)
#endif

/*---------------------------- Global variables ---------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern GA_gluFuncs _VARAPI _GA_gluFuncs;

#ifdef __cplusplus
}
#endif

#endif /* __GLU_STUB_H */

