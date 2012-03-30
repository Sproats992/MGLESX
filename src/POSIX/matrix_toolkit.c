/*
 
 Copyright © 2012 Robert Sproats. All Rights Reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY ROBERT SPROATS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "matrix_toolkit.h"
#include <math.h>

void crossProduct(vector* a, vector *b, vector* cross_product_vector) {
	
	//(a2.b3 - a3.b2)i
	cross_product_vector->i = (a->j*b->k) - (a->k*b->j);
	
	//(a3.b1 - a1.b3)j
	cross_product_vector->j = (a->k*b->i) - (a->i*b->k);
	
	//(a1.b2 - a2.b1)k
	cross_product_vector->k = (a->i*b->j) - (a->j*b->i);
	
}

float calculateVectorModulusSquared(float i, float j, float k) {
    return ((i*i)+(j*j)+(k*k)); //note we return the squared value as square root functions are too expensive and we only need to test for 1.0.
}

/* This function is slow (contains sqrt) and should be used only when accuracy is required. */
void slowNormaliseVector(vector* vector_to_normalise) {

    double modulus = sqrt(calculateVectorModulusSquared(vector_to_normalise->i, vector_to_normalise->j, vector_to_normalise->k));
    
    double scaling_factor = 1.0f/modulus;
    
    vector_to_normalise->i *= scaling_factor;
    vector_to_normalise->j *= scaling_factor;
    vector_to_normalise->k *= scaling_factor;
    
}

float inverseSquareRoot(float x) {
	
	float xhalf = 0.5f*x;
	
	union
	{
		float x;
		int i;
	} u;
	
	u.x = x;
	u.i = 0x5f3759df - (u.i >> 1);
	x = u.x * (1.5f - xhalf * u.x * u.x);
	
	return x;
}

void normaliseVector(vector* vector_to_normalise) {
	
	double reciprocal_magnitude = inverseSquareRoot((vector_to_normalise->i*vector_to_normalise->i) + (vector_to_normalise->j*vector_to_normalise->j) + (vector_to_normalise->k*vector_to_normalise->k));
	
	vector_to_normalise->i *= reciprocal_magnitude;
	vector_to_normalise->j *= reciprocal_magnitude;
	vector_to_normalise->k *= reciprocal_magnitude;
	
}