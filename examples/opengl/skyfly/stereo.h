/*
 * Stereoscopic 3D support for OpenGL applications
 * using SciTech Software MGL 4.x or GLUT support library.
 * http://www.scitechsoft.com
 *
 * by Dave Milici
 * http://www.wenet.net/~ironic/stereo3d.html
 *
 *
 * Need to use updated MGL libraries with stereo driver support,
 * including MGL_GL_STEREO definition.
 *
 * Requires Stereo-ready graphics accelerator with OpenGL ICD driver:
 * --3DLabs Permedia2
 *   (Diamond FireGL 1000Pro, Hercules Dynamite3D/GL)
 * --Rendition Verite 2200
 *   (Hercules Thriller3D, Genoa Raptor3D)
 * --3DFX VoodooRush with Alliance display controller
 *   (Intergraph Intense3D VoodooRush, Hercules Stingray3D)
 * --other stereo-ready graphics accelerator supporting GL_STEREO
 *   quad-buffered display visuals;
 *
 * Or OpenGL driver supporting GL_STEREO quad-buffered page-flipping
 * operation via software emulation:
 * --SciTech Software GLDirect
 *
 * Stereo3d viewing requires Liquid Crystal Shutter (LCS) Glasses:
 * --StereoGraphics SimulEyes, CrystalEyes
 * --H3D
 * --NuVision 3D-Spex
 * --VRex VR-Surfer
 * --other LCS glasses designed for field-sequential stereo formats
 *   generated from graphics display controller, and synchronized by
 *   left/right stereo sync signal.
 */


/* Define STEREO compiler switch for modified MGL OpenGL demos here,
 * so we don't need to fix up any makefiles. */
#define STEREO


/*
 * Uses 2-step parallal-axes stereoscopic perspective generation algorithm:
 * (1) Render left/right perspectives offset from default, dxViewpoint;
 * (2) Compensate with right/left image shift, dxImageShift;
 * Note image shift may be accomplished by using 1 of 2 possible methods,
 * either asymetric viewing frustums, or clipped viewport windows.
 *
 * Algorithm summarized at
 * http://www.wenet.net/~ironic/algorithm.html
 */


/* Note use of GLfloats here instead of GLdoubles, since that seems to be
 * default types used in all of these OpenGL demo apps. */

#ifdef __cplusplus
extern "C" {
#endif

/* stereo3d display control vars */
extern GLboolean bStereoRequested;	/* user request for stereo display ? */
extern GLboolean bStereoEnabled;	/* stereo display mode activated ? */

/* stereo3d parallax control vars */
extern GLfloat fDxViewpoint;    /* left/right viewpoint separation */
extern GLfloat fDxImageShift;	/* left/right image shift (frustum method) */
extern GLint iDxPixelShift;     /* left/right image shift (viewport method) */

/* stereo3d support vars for building asymetric frustum with glFrustum() */
/* (preferred method for accomplishing image shift compensation) */
extern GLfloat fFrustumWidth2;
extern GLfloat fFrustumHeight2;
extern GLfloat fFrustumZnear;
extern GLfloat fFrustumZfar;

/* stereo3d projection matrixes for loading at run-time */
extern GLfloat mProjectionLeft[16];
extern GLfloat mProjectionRight[16];

/* stereo3d support vars for clipping viewport with glViewport() */
/* (alternate method for accomplishing image shift compensation) */
extern GLint iViewportX;
extern GLint iViewportY;
extern GLint iViewportWidth;
extern GLint iViewportHeight;

/* stereo3d matrix calculation routine for rebuilding application's
 * projection matrixes with left/right perspective shifts. */
void calcStereoMatrix(void);

#ifdef __cplusplus
};
#endif
