/*
 
Copyright © 2012 Robert Sproats. All Rights Reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ROBERT SPROATS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MGLESX_h
#define MGLESX_h

/*check if the compiler is of C++*/
#ifdef __cplusplus
extern "C" {
#endif

typedef float MGLESXMatrix4x4[16];

static const int MGLESX_MATRIX_MODE_PROJECTION = 1;
static const int MGLESX_MATRIX_MODE_MODEL_VIEW = 2;
static const int MGLESX_MATRIX_MODE_TEXTURE = 3; //not implemented yet.

    
void MGLESX_pushMatrix();
void MGLESX_popMatrix();

void MGLESX_loadIdentity(); //also initialise function.
void MGLESX_translatef(float x_translation, float y_translation, float z_translation);
void MGLESX_rotatef(float angle, float x_rotation, float y_rotation, float z_rotation);
void MGLESX_scalef(float x_scale, float y_scale, float z_scale);
void MGLESX_set_matrix_mode(int matrix_mode_to_set);
void MGLESX_raise_error(int error_code);

void MGLESX_frustumf(float left,  float right,  float bottom,  float top,  float near,  float far);
void MGLESX_orthof(float left,  float right,  float bottom,  float top,  float near,  float far);
void MGLESX_loadMatrixf(float * m);
    
int MGLESX_getError();

//test functions.
MGLESXMatrix4x4* MGLESX_getMatrixPointerAtStackPosition(int stack_position);
MGLESXMatrix4x4* MGLESX_getCurrentMatrixPointer();
int MGLESX_getCurrentMatrixMode();

    
//error codes
static const int MGLESX_ERROR_CODE_OVERFLOW_MODELVIEW_MATRIX_STACK = 1;
static const int MGLESX_ERROR_CODE_OVERFLOW_PROJECTION_MATRIX_STACK = 2;
static const int MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE = 3;
static const int MGLESX_ERROR_CODE_UNDERFLOW_MODELVIEW_MATRIX_STACK = 4;
static const int MGLESX_ERROR_CODE_UNDERFLOW_PROJECTION_MATRIX_STACK = 5;

/*check if the compiler is of C++ */
#ifdef __cplusplus
}
#endif

#endif
