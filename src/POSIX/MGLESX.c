/*
 
 Copyright © 2012 Robert Sproats. All Rights Reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY ROBERT SPROATS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MGLESX.h"
#include "matrix_support.h"
#include <math.h>

static const int MGLESX_MODELVIEW_MATRIX_STACK_MAX_DEPTH = 32;
static const int MGLESX_PROJECTION_MATRIX_STACK_MAX_DEPTH = 4;

MGLESXMatrix4x4 the_modelview_matrix_stack[MGLESX_MODELVIEW_MATRIX_STACK_MAX_DEPTH];
MGLESXMatrix4x4 the_projection_matrix_stack[MGLESX_PROJECTION_MATRIX_STACK_MAX_DEPTH];

int MGLESX_current_matrix_mode;
int MGLESX_current_projection_matrix_stack_pointer = 0;
int MGLESX_current_modelview_matrix_stack_pointer = 0;
int MGLESX_current_error_code = 0;

void MGLESX_matrix3x3InvertAndTranspose(const float * input_3x3_matrix, float * output_3x3_matrix) {
    
    double determinant = +input_3x3_matrix[0]*(input_3x3_matrix[4]*input_3x3_matrix[8]-input_3x3_matrix[7]*input_3x3_matrix[5])
    -input_3x3_matrix[1]*(input_3x3_matrix[3]*input_3x3_matrix[8]-input_3x3_matrix[5]*input_3x3_matrix[6])
    +input_3x3_matrix[2]*(input_3x3_matrix[3]*input_3x3_matrix[7]-input_3x3_matrix[4]*input_3x3_matrix[6]);
    
    if (determinant!=0.0f) {
        double invdet = 1/determinant;
        output_3x3_matrix[0] =  (input_3x3_matrix[4]*input_3x3_matrix[8]-input_3x3_matrix[7]*input_3x3_matrix[5])*invdet;
        output_3x3_matrix[3] = -(input_3x3_matrix[1]*input_3x3_matrix[8]-input_3x3_matrix[2]*input_3x3_matrix[7])*invdet;
        output_3x3_matrix[6] =  (input_3x3_matrix[1]*input_3x3_matrix[5]-input_3x3_matrix[2]*input_3x3_matrix[4])*invdet;
        output_3x3_matrix[1] = -(input_3x3_matrix[3]*input_3x3_matrix[8]-input_3x3_matrix[5]*input_3x3_matrix[6])*invdet;
        output_3x3_matrix[4] =  (input_3x3_matrix[0]*input_3x3_matrix[8]-input_3x3_matrix[2]*input_3x3_matrix[6])*invdet;
        output_3x3_matrix[7] = -(input_3x3_matrix[0]*input_3x3_matrix[5]-input_3x3_matrix[3]*input_3x3_matrix[2])*invdet;
        output_3x3_matrix[2] =  (input_3x3_matrix[3]*input_3x3_matrix[7]-input_3x3_matrix[6]*input_3x3_matrix[4])*invdet;
        output_3x3_matrix[5] = -(input_3x3_matrix[0]*input_3x3_matrix[7]-input_3x3_matrix[6]*input_3x3_matrix[1])*invdet;
        output_3x3_matrix[8] =  (input_3x3_matrix[0]*input_3x3_matrix[4]-input_3x3_matrix[3]*input_3x3_matrix[1])*invdet;
    } else {
        MGLESX_raise_error(MGLESX_ERROR_CODE_MATRIX_DETERMINANT_IS_ZERO);
    }
    
}

void MGLESX_get3x3MatrixFrom4x4Matrix(const float * input_4x4_matrix, float * output_3x3_matrix) {
    
    output_3x3_matrix[0] = input_4x4_matrix[0];
    output_3x3_matrix[1] = input_4x4_matrix[1];
    output_3x3_matrix[2] = input_4x4_matrix[2];

    output_3x3_matrix[3] = input_4x4_matrix[4];
    output_3x3_matrix[4] = input_4x4_matrix[5];
    output_3x3_matrix[5] = input_4x4_matrix[6];

    output_3x3_matrix[6] = input_4x4_matrix[8];
    output_3x3_matrix[7] = input_4x4_matrix[9];
    output_3x3_matrix[8] = input_4x4_matrix[10];
    
}


//derived from gluLookAt. See: http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml
void MGLESX_lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
    
    //construct vectors.
    MGLESXVector3f upVector = {upX, upY, upZ};
    
    MGLESXVector3f fVector;
    MGLESXVector3f sVector;
    MGLESXVector3f uVector;
    
    fVector[0] = centerX - eyeX;
    fVector[1] = centerY - eyeY;
    fVector[2] = centerZ - eyeZ;
    
    //make UP and F unit length.
    matsup_NormalizeVector(&upVector[0]);
    matsup_NormalizeVector(&fVector[0]);
    
    //s = f X UP'
    matstup_VectorCrossProduct3f(&fVector[0], &upVector[0], &sVector[0]); 
    
    //u = s X f
    matstup_VectorCrossProduct3f(&sVector[0], &fVector[0], &uVector[0]); 
    
    MGLESXMatrix4x4 M;
    
    M[0] = sVector[0];
    M[1] = sVector[1];
    M[2] = sVector[2];
    M[3] = 0;
    
    M[4] = uVector[0];
    M[5] = uVector[1];
    M[6] = uVector[2];
    M[7] = 0;
    
    M[8] = -fVector[0];
    M[9] = -fVector[1];
    M[10] = -fVector[2];
    M[11] = 0;
    
    M[12] = 0;
    M[13] = 0;
    M[14] = 0;
    M[15] = 1;
    
    //Note:
    //and gluLookAt is equivalent to 
    //
    //glMultMatrixf(M);
    //glTranslated(-eyex, -eyey, -eyez);
    
    MGLESXMatrix4x4 copy_matrix;
    
    //note this operation only works on the projection matrix rather than the currently selected.
    matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], &copy_matrix[0]);
    matsup_MultiplyMat4f(&copy_matrix[0], &M[0],
                         &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);

    MGLESX_translatef(-eyeX, -eyeY, -eyeZ);
    
    //and we're done. Phew.
    
    
}

//See: http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml
//Note that working matrix elements 11 and 14 are transposed here. Possibly the opengl.org docs are in error, their stated solution doesn't seem to work.
void MGLESX_createPerspective(float fovy_degrees, float aspect_ratio, float z_near, float z_far) {
    
    
    const float degrees2radians = 0.0174532925;
	float fovy_radians = fovy_degrees * degrees2radians; //convert the field of view y to radians.
    float f = 1.0f/(tan(fovy_radians/2.0f)); //using 1/tan(x) for cotangent.

    /* derived from gluPerspective posted on Jeff LaMarche's blog - maybe should use this instead?
    double xmin, xmax, ymin, ymax;
    ymax = z_near * tan(fovy_radians/2.0f);
    ymin = -ymax;
    xmin = ymin * aspect_ratio;
    xmax = ymax * aspect_ratio;
    
    MGLESX_frustumf(xmin, xmax, ymin, ymax, z_near, z_far);
    */
    
    float working_matrix[16];
    float copy_matrix[16];
    
    working_matrix[0] = f/aspect_ratio;
    working_matrix[1] = 0.0f;
    working_matrix[2] = 0.0f;
    working_matrix[3] = 0.0f;
    
    working_matrix[4] = 0.0f;
    working_matrix[5] = f;
    working_matrix[6] = 0.0f;
    working_matrix[7] = 0.0f;
    
    working_matrix[8] = 0.0f;
    working_matrix[9] = 0.0f;
    working_matrix[10] = (z_far+z_near)/(z_near-z_far);
    working_matrix[14] = (2.0f*z_far*z_near)/(z_near-z_far);
    
    working_matrix[12] = 0.0f;
    working_matrix[13] = 0.0f;
    working_matrix[11] = -1.0f;
    working_matrix[15] = 0.0f;
    
    matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], &copy_matrix[0]);
    matsup_MultiplyMat4f(&copy_matrix[0], &working_matrix[0],
                         &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
     
    
}

//return the final matrix used to calculate vertex positions. It is current modelview matrix multiplied by the current projection matrix.
void MGLESX_getModelProjectionMatrix(float * m) {
    matsup_MultiplyMat4f(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0],
                         &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0],
                         m);
        
}

void MGLESX_pushMatrix() {
    
    MGLESXMatrix4x4* input_matrix;
    MGLESXMatrix4x4* output_matrix;
    
    //check limits, update stack pointer and assign input/output matrices.
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            if (MGLESX_current_projection_matrix_stack_pointer >= (MGLESX_PROJECTION_MATRIX_STACK_MAX_DEPTH-1)) {
                MGLESX_raise_error(MGLESX_ERROR_CODE_OVERFLOW_PROJECTION_MATRIX_STACK);
                return;
            } else {
                //assign in/out matrices to be copied and update stack pointer.
                input_matrix = &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer++];
                output_matrix = &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer];
            }
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            if (MGLESX_current_modelview_matrix_stack_pointer >= (MGLESX_MODELVIEW_MATRIX_STACK_MAX_DEPTH-1)) {
                MGLESX_raise_error(MGLESX_ERROR_CODE_OVERFLOW_MODELVIEW_MATRIX_STACK);
                return;
            } else {
                //assign in/out matrices to be copied and update stack pointer.
                input_matrix = &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer++];
                output_matrix = &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer];
            }
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }
    
    //mirror the current matrix into the new position.
    matsup_CopyMatrix(input_matrix[0], output_matrix[0]);
    
}

void MGLESX_popMatrix() {
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            if (MGLESX_current_projection_matrix_stack_pointer <= 0) {
                MGLESX_current_projection_matrix_stack_pointer = 0;
                MGLESX_raise_error(MGLESX_ERROR_CODE_UNDERFLOW_PROJECTION_MATRIX_STACK);
                return;
            } else {
                MGLESX_current_projection_matrix_stack_pointer--;
            }
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            if (MGLESX_current_modelview_matrix_stack_pointer <= 0) {
                MGLESX_current_modelview_matrix_stack_pointer = 0;
                MGLESX_raise_error(MGLESX_ERROR_CODE_UNDERFLOW_MODELVIEW_MATRIX_STACK);
                return;
            } else {
                MGLESX_current_modelview_matrix_stack_pointer--;
            }
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }
}

void MGLESX_loadIdentity() {
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            MGLESX_current_projection_matrix_stack_pointer = 0;
            matsup_LoadIdentity(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            MGLESX_current_modelview_matrix_stack_pointer = 0;
            matsup_LoadIdentity(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0]);
           break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }    
}

void MGLESX_translatef(float x_translation, float y_translation, float z_translation) {
    
    MGLESXMatrix4x4 loading_matrix;
    MGLESXMatrix4x4 current_working_matrix;

    matsup_LoadTranslations(&loading_matrix[0], x_translation, y_translation, z_translation);

    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):            
            matsup_CopyMatrix(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }    


}

void MGLESX_rotatef(float angle, float x_rotation, float y_rotation, float z_rotation) {
    
    MGLESXMatrix4x4 loading_matrix;
    MGLESXMatrix4x4 current_working_matrix;
    
    matsup_LoadRotations(angle, &loading_matrix[0], x_rotation, y_rotation, z_rotation);
    
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):            
            matsup_CopyMatrix(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0]);
            
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }    
}

void MGLESX_scalef(float x_scale, float y_scale, float z_scale) {
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_SetScaleXYZ(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], x_scale, y_scale, z_scale);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            matsup_SetScaleXYZ(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0], x_scale, y_scale, z_scale);
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }    

}

void MGLESX_set_matrix_mode(int matrix_mode_to_set) {
    //vertify it's a valid mode
    switch(matrix_mode_to_set) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            MGLESX_current_matrix_mode = matrix_mode_to_set;
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            MGLESX_current_matrix_mode = matrix_mode_to_set;
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_current_matrix_mode = -1;
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            break;
    } 
}

int MGLESX_getError() {
    //reading the error code clears it.
    int code_to_return = MGLESX_current_error_code;
    MGLESX_current_error_code = 0;
    return code_to_return;
}

void MGLESX_frustumf(float left,  float right,  float bottom,  float top,  float near,  float far) {
    
    MGLESXMatrix4x4 loading_matrix;
    MGLESXMatrix4x4 current_working_matrix;
    
    MGLESX_loadIdentity();
    
    //check for sane values.
    if (near<=0.0f) return;
    
    matsup_LoadFrustum(&loading_matrix[0], top, bottom, left, right, near, far);
    
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):            
            matsup_CopyMatrix(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    }    

}

void MGLESX_orthof(float left,  float right,  float bottom,  float top,  float near,  float far) {
    
    MGLESXMatrix4x4 loading_matrix;
    MGLESXMatrix4x4 current_working_matrix;
    
    MGLESX_loadIdentity();
    matsup_LoadOrtho(left, right, bottom, top, near, far, &loading_matrix[0]);
    
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):            
            matsup_CopyMatrix(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0], &current_working_matrix[0]);
            matsup_MultiplyMat4f(&current_working_matrix[0], &loading_matrix[0], &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    } 
    
}

void MGLESX_loadMatrixf(float * m) {
    
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_CopyMatrix(m, &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):            
            matsup_CopyMatrix(m, &the_modelview_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0]);
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
            return;
    } 

    
}


void MGLESX_raise_error(int error_code) {
    MGLESX_current_error_code = error_code;
}

//test functions.
MGLESXMatrix4x4* getMatrixPointerAtStackPosition(int stack_position) {
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            return &the_projection_matrix_stack[stack_position];
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            return &the_modelview_matrix_stack[stack_position];
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
    }   
    return 0;
}

MGLESXMatrix4x4* MGLESX_getCurrentMatrixPointer() {
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            return &the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer];
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            return &the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer];
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
    }    
    return 0;    
}

void MGLESX_getCurrentMatrix(MGLESXMatrix4x4* matrix_to_load) {
    switch(MGLESX_current_matrix_mode) {
        case(MGLESX_MATRIX_MODE_PROJECTION):
            matsup_CopyMatrix(&the_projection_matrix_stack[MGLESX_current_projection_matrix_stack_pointer][0], matrix_to_load[0]);
            break;
        case(MGLESX_MATRIX_MODE_MODEL_VIEW):
            matsup_CopyMatrix(&the_modelview_matrix_stack[MGLESX_current_modelview_matrix_stack_pointer][0], matrix_to_load[0]);
            break;
        case(MGLESX_MATRIX_MODE_TEXTURE):
            break;
        default:
            //error! Shouldn't reach here.
            MGLESX_raise_error(MGLESX_ERROR_CODE_UNRECOGNISED_MATRIX_MODE);
    }    
    
}

int MGLESX_getCurrentMatrixMode() {
    return MGLESX_current_matrix_mode;
}

