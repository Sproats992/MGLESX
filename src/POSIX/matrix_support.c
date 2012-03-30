/*
 
 Copyright © 2012 Robert Sproats. All Rights Reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY ROBERT SPROATS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "matrix_support.h"
#include "matrix_toolkit.h"
#include <math.h>
#include <string.h>

//derived from OpenGL ES glFrustrum. See: http://www.khronos.org/opengles/sdk/1.1/docs/man/
void matsup_LoadFrustum(float *m, float top, float bottom, float left, float right, float nearVal, float farVal) {
	
	float A,B,C,D,E,F;
	
	A = (right+left)/(right-left);
	B = (top+bottom)/(top-bottom);
	C = -((farVal+nearVal)/(farVal-nearVal));
	D = -((2.0f*farVal*nearVal)/(farVal-nearVal));
	E = (2.0f*nearVal)/(right-left);
	F = (2.0f*nearVal)/(top-bottom);
	
	m[0] = E; //E
	m[1] = 0.0f; //0
	m[2] = 0.0f; //0
	m[3] = 0.0f; //0
	
	m[4] = 0.0f; //0
	m[5] = F; //F
	m[6] = 0.0f; //0
	m[7] = 0.0f; //0
	
	m[8] = A; //A
	m[9] = B; //B
	m[10] = C; //C
	m[11] = -1.0f; //-1	
	
	m[12] = 0.0f; //0
	m[13] = 0.0f; //0
	m[14] = D; //D
	m[15] = 0.0f; //0
	
}

//loads identity matrix into target.
void matsup_LoadIdentity(float* m)
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	
	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;
	
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;	
	
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

// s is a 3D vector
void matsup_LoadScale(float* s, float* m)
{
	m[0] = s[0];
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	
	m[4] = 0.0f;
	m[5] = s[1];
	m[6] = 0.0f;
	m[7] = 0.0f;
	
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = s[2];
	m[11] = 0.0f;	
	
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

//load scaling into target matrix.
void matsup_LoadScaleXYZ(float *m, float x_scale, float y_scale, float z_scale) {
	m[0] = x_scale;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	
	m[4] = 0.0f;
	m[5] = y_scale;
	m[6] = 0.0f;
	m[7] = 0.0f;
	
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = z_scale;
	m[11] = 0.0f;	
	
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
	
}

void matsup_SetScaleXYZ(float *m, float x_scale, float y_scale, float z_scale) {
	m[0] *= x_scale;
	m[1] *= x_scale;
	m[2] *= x_scale;
	m[3] *= x_scale;
	
	m[4] *= y_scale;
	m[5] *= y_scale;
	m[6] *= y_scale;
	m[7] *= y_scale;
	
	m[8] *= z_scale;
	m[9] *= z_scale;
	m[10] *= z_scale;
	m[11] *= z_scale;	
}

//derived from OpenGL ES glRotation. See here: http://www.khronos.org/opengles/documentation/opengles1_0/html/glRotate.html
void matsup_LoadRotations(float degrees, float* m, float x_angle, float y_angle, float z_angle) {

	const float degrees2radians = 0.0174532925;
	float radians = degrees * degrees2radians;

    float modulus_squared = calculateVectorModulusSquared(x_angle, y_angle, z_angle);
    float difference = modulus_squared - 1.0f;
    if (difference<0.0f) difference = -difference;
    
    //if the vector modulus doesn't add up to 1.0f the vector has to be normalized.
    if (difference>0.0001f) {
        
        vector the_vector;
        the_vector.i = x_angle;
        the_vector.j = y_angle;
        the_vector.k = z_angle;
    
        slowNormaliseVector(&the_vector); //slow function, but what can you do?
    
        x_angle = the_vector.i;
        y_angle = the_vector.j;
        z_angle = the_vector.k;
    }
    
	//c = cos (angle) , s = sin (angle) 
	float c = cosf(radians);
	float s = sinf(radians);
		
	m[15] = 1.0f;
	m[14] = 0.0f;
	m[13] = 0.0f;
	m[12] = 0.0f;
	
	m[11] = 0.0f;
	m[10] = ((z_angle*z_angle)*(1-c))+c;            
	m[9] = ((y_angle*z_angle)*(1-c))-(x_angle*s);   
	m[8] = ((x_angle*z_angle)*(1-c))+(y_angle*s);  
	
	m[7] = 0.0f;
	m[6] = ((y_angle*z_angle)*(1-c))+(x_angle*s);   
	m[5] = ((y_angle*y_angle)*(1-c))+c;           
	m[4] = ((x_angle*y_angle)*(1-c))-(z_angle*s);	
	
	m[3] = 0.0f;
	m[2] = ((x_angle*z_angle)*(1-c))-(y_angle*s); 
	m[1] = ((x_angle*y_angle)*(1-c))+(z_angle*s); 
	m[0] = ((x_angle*x_angle)*(1-c))+c;           
	
}

void matsup_SetRotations(float degrees, float* m, float x_angle, float y_angle, float z_angle) {
	
	const float degrees2radians = 0.0174532925;
	float radians = degrees * degrees2radians;
		
	//c = cos (angle) , s = sin (angle) 
	float c = cosf(radians);
	float s = sinf(radians);
	
	//m[15] = 1.0f;
	//m[14] = 0.0f;
	//m[13] = 0.0f;
	//m[12] = 0.0f;
	
	//m[11] = 0.0f;
	m[10] *= ((z_angle*z_angle)*(1-c))+c;            //z2 (1 - c) + c
	m[9] *= ((y_angle*z_angle)*(1-c))+(x_angle*s);   //yz (1 - c) + xs
	m[8] *= ((x_angle*z_angle)*(1-c))-(y_angle*s);   //xz (1 - c) - ys
	
	//m[7] = 0.0f;
	m[6] *= ((y_angle*z_angle)*(1-c))-(x_angle*s);  //yz (1 - c) - xs
	m[5] *= ((y_angle*y_angle)*(1-c))+c;            //y2 (1 - c) + c
	m[4] *= ((x_angle*y_angle)*(1-c))+(z_angle*s);	//xy (1 - c) + zs
	
	//m[3] = 0.0f;
	m[2] *= ((x_angle*z_angle)*(1-c))+(y_angle*s); //xz (1 - c) + ys
	m[1] *= ((x_angle*y_angle)*(1-c))-(z_angle*s); //xy (1 - c) - zs
	m[0] *= ((x_angle*x_angle)*(1-c))+c;           //x2 (1 - c) + c
	
}


//test routine to calculate final vertex data.
int vertex_loop;
void matsup_CalculateVertices(float *source_matrix, float *input_vertex_list, float *output_vertex_list, int number_of_vertices) {
	
	for (vertex_loop=0; vertex_loop<(number_of_vertices*3); vertex_loop+=3) {
	  matsup_MultiplyMat4by1f(source_matrix, &input_vertex_list[vertex_loop], &output_vertex_list[vertex_loop]);
	}
	
}

void matsup_LoadXRotation(float radians, float* m)
{
	float cosrad = cosf(radians);
	float sinrad = sinf(radians);
	
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	
	m[4] = 0.0f;
	m[5] = cosrad;
	m[6] = sinrad;
	m[7] = 0.0f;
	
	m[8] = 0.0f;
	m[9] = -sinrad;
	m[10] = cosrad;
	m[11] = 0.0f;	
	
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void matsup_LoadYRotation(float radians, float* mout)
{
	float cosrad = cosf(radians);
	float sinrad = sinf(radians);
	
	mout[0] = cosrad;
	mout[1] = 0.0f;
	mout[2] = -sinrad;
	mout[3] = 0.0f;
	
	mout[4] = 0.0f;
	mout[5] = 1.0f;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = sinrad;
	mout[9] = 0.0f;
	mout[10] = cosrad;
	mout[11] = 0.0f;	
	
	mout[12] = 0.0f;
	mout[13] = 0.0f;
	mout[14] = 0.0f;
	mout[15] = 1.0f;
}

void matsup_LoadZRotation(float radians, float* mout)
{
	float cosrad = cosf(radians);
	float sinrad = sinf(radians);
	
	mout[0] = cosrad;
	mout[1] = sinrad;
	mout[2] = 0.0f;
	mout[3] = 0.0f;
	
	mout[4] = -sinrad;
	mout[5] = cosrad;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = 0.0f;
	mout[9] = 0.0f;
	mout[10] = 1.0f;
	mout[11] = 0.0f;	
	
	mout[12] = 0.0f;
	mout[13] = 0.0f;
	mout[14] = 0.0f;
	mout[15] = 1.0f;
}

void matsup_SetTranslations(float *mout, float x, float y, float z) {
	
	mout[12] += x;
	mout[13] += y;
	mout[14] += z;

}

void matsup_LoadTranslations(float *mout, float x, float y, float z) {
	
	mout[0] = 1.0f;
	mout[1] = 0.0f;
	mout[2] = 0.0f;
	mout[3] = 0.0f;
	
	mout[4] = 0.0f;
	mout[5] = 1.0f;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = 0.0f;
	mout[9] = 0.0f;
	mout[10] = 1.0f;
	mout[11] = 0.0f;	
	
	mout[12] = x;
	mout[13] = y;
	mout[14] = z;
	mout[15] = 1.0f;
}

// v is a 3D vector
void matsup_LoadTranslation(float* v, float* mout)
{
	mout[0] = 1.0f;
	mout[1] = 0.0f;
	mout[2] = 0.0f;
	mout[3] = 0.0f;
	
	mout[4] = 0.0f;
	mout[5] = 1.0f;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = 0.0f;
	mout[9] = 0.0f;
	mout[10] = 1.0f;
	mout[11] = 0.0f;	
	
	mout[12] = v[0];
	mout[13] = v[1];
	mout[14] = v[2];
	mout[15] = 1.0f;
}

void matsup_LoadPerspective(float fov_radians, float aspect, float zNear, float zFar, float* mout)
{
	float f = 1.0f / tanf(fov_radians/2.0f);
	
	mout[0] = f / aspect;
	mout[1] = 0.0f;
	mout[2] = 0.0f;
	mout[3] = 0.0f;
	
	mout[4] = 0.0f;
	mout[5] = f;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = 0.0f;
	mout[9] = 0.0f;
	mout[10] = (zFar+zNear) / (zNear-zFar);
	mout[11] = -1.0f;
	
	mout[12] = 0.0f;
	mout[13] = 0.0f;
	mout[14] = 2 * zFar * zNear /  (zNear-zFar);
	mout[15] = 0.0f;
}

void matsup_LoadOrtho(float left, float right, float bottom, float top, float near, float far, float* mout)
{
	float r_l = right - left;
	float t_b = top - bottom;
	float f_n = far - near;
	float tx = - (right + left) / (right - left);
	float ty = - (top + bottom) / (top - bottom);
	float tz = - (far + near) / (far - near);
	
	mout[0] = 2.0f / r_l;
	mout[1] = 0.0f;
	mout[2] = 0.0f;
	mout[3] = 0.0f;
	
	mout[4] = 0.0f;
	mout[5] = 2.0f / t_b;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = 0.0f;
	mout[9] = 0.0f;
	mout[10] = -2.0f / f_n;
	mout[11] = 0.0f;
	
	mout[12] = tx;
	mout[13] = ty;
	mout[14] = tz;
	mout[15] = 1.0f;
}

void matsup_CopyMatrix(float *input_matrix, float *output_matrix) {

   // memcpy(input_matrix, output_matrix, 16*4); //16 floats of 4 bytes each.
    
    
	output_matrix[0] = input_matrix[0];
	output_matrix[1] = input_matrix[1];
	output_matrix[2] = input_matrix[2];
	output_matrix[3] = input_matrix[3];

	output_matrix[4] = input_matrix[4];
	output_matrix[5] = input_matrix[5];
	output_matrix[6] = input_matrix[6];
	output_matrix[7] = input_matrix[7];

	output_matrix[8] = input_matrix[8];
	output_matrix[9] = input_matrix[9];
	output_matrix[10] = input_matrix[10];
	output_matrix[11] = input_matrix[11];

	output_matrix[12] = input_matrix[12];
	output_matrix[13] = input_matrix[13];
	output_matrix[14] = input_matrix[14];
	output_matrix[15] = input_matrix[15];
     

}

void matsup_MultiplyMat4f(const float* a, const float* b, float* mout)
{
#ifdef USE_VFP
	Matrix4Mul(a, b, mout);
#else
	mout[0]  = a[0] * b[0]  + a[4] * b[1]  + a[8] * b[2]   + a[12] * b[3];
	mout[1]  = a[1] * b[0]  + a[5] * b[1]  + a[9] * b[2]   + a[13] * b[3];
	mout[2]  = a[2] * b[0]  + a[6] * b[1]  + a[10] * b[2]  + a[14] * b[3];
	mout[3]  = a[3] * b[0]  + a[7] * b[1]  + a[11] * b[2]  + a[15] * b[3];
	
	mout[4]  = a[0] * b[4]  + a[4] * b[5]  + a[8] * b[6]   + a[12] * b[7];
	mout[5]  = a[1] * b[4]  + a[5] * b[5]  + a[9] * b[6]   + a[13] * b[7];
	mout[6]  = a[2] * b[4]  + a[6] * b[5]  + a[10] * b[6]  + a[14] * b[7];
	mout[7]  = a[3] * b[4]  + a[7] * b[5]  + a[11] * b[6]  + a[15] * b[7];
	
	mout[8]  = a[0] * b[8]  + a[4] * b[9]  + a[8] * b[10]  + a[12] * b[11];
	mout[9]  = a[1] * b[8]  + a[5] * b[9]  + a[9] * b[10]  + a[13] * b[11];
	mout[10] = a[2] * b[8]  + a[6] * b[9]  + a[10] * b[10] + a[14] * b[11];
	mout[11] = a[3] * b[8]  + a[7] * b[9]  + a[11] * b[10] + a[15] * b[11];
	
	mout[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14]  + a[12] * b[15];
	mout[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14]  + a[13] * b[15];
	mout[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
	mout[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
#endif
}

//a = 4x4 matrix, b = 1x3
void matsup_MultiplyMat4by1f(const float* a, const float* b, float* mout)
{
	mout[0]  = (a[0] * b[0])  + (a[4] * b[1])  + (a[8] * b[2])   + (a[12]); 
	mout[1]  = (a[1] * b[0])  + (a[5] * b[1])  + (a[9] * b[2])   + (a[13]); 
	mout[2]  = (a[2] * b[0])  + (a[6] * b[1])  + (a[10] * b[2])  + (a[14]); 
	mout[3]  = (a[3] * b[0])  + (a[7] * b[1])  + (a[11] * b[2])  + (a[15]); 
}

//a = 4x4 matrix, b = 1x2, b[3] = 0.0
void matsup_MultiplyMat4by2f(const float* a, const float* b, float* mout)
{
	mout[0]  = (a[0] * b[0])  + (a[4] * b[1]) + (a[12]); 
	mout[1]  = (a[1] * b[0])  + (a[5] * b[1]) + (a[13]); 
	mout[2]  = (a[2] * b[0])  + (a[6] * b[1]) + (a[14]); 
	mout[3]  = (a[3] * b[0])  + (a[7] * b[1]) + (a[15]); 
}

