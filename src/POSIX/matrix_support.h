/*
 
 Copyright © 2012 Robert Sproats. All Rights Reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY ROBERT SPROATS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef matrix_support_h
#define matrix_support_h

/*check if the compiler is of C++*/
#ifdef __cplusplus
extern "C" {
#endif
    
void matsup_LoadIdentity(float* m);
void matsup_LoadScale(float* s, float* m);
void matsup_LoadXRotation(float radians, float* m);
void matsup_LoadYRotation(float radians, float* mout);
void matsup_LoadZRotation(float radians, float* mout);
void matsup_LoadTranslation(float* v, float* mout);
void matsup_LoadTranslations(float *m, float x, float y, float z);
void matsup_SetTranslations(float *m, float x, float y, float z);
void matsup_LoadPerspective(float fov_radians, float aspect, float zNear, float zFar, float* mout);
void matsup_LoadFrustum(float *m, float top, float bottom, float left, float right, float nearVal, float farVal);
void matsup_LoadOrtho(float left, float right, float bottom, float top, float near, float far, float* mout);
void matsup_MultiplyMat4f(const float* a, const float* b, float* mout);
void matsup_MultiplyMat4by1f(const float* a, const float* b, float* mout);
void matsup_MultiplyMat4by2f(const float* a, const float* b, float* mout);
void matsup_LoadRotations(float degrees, float* m, float x_angle, float y_angle, float z_angle);
void matsup_SetRotations(float degrees, float* m, float x_angle, float y_angle, float z_angle);
void matsup_LoadScaleXYZ(float *m, float x_scale, float y_scale, float z_scale);
void matsup_SetScaleXYZ(float *m, float x_scale, float y_scale, float z_scale);
void matsup_CalculateVertices(float *source_matrix, float *input_vertex_list, float *output_vertex_list, int number_of_vertices);
void matsup_CopyMatrix(float *input_matrix, float *output_matrix);

/*check if the compiler is of C++ */
#ifdef __cplusplus
}
#endif

#endif
