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

static const int MGLESX_MODELVIEW_MATRIX_STACK_MAX_DEPTH = 32;
static const int MGLESX_PROJECTION_MATRIX_STACK_MAX_DEPTH = 4;

MGLESXMatrix4x4 the_modelview_matrix_stack[MGLESX_MODELVIEW_MATRIX_STACK_MAX_DEPTH];
MGLESXMatrix4x4 the_projection_matrix_stack[MGLESX_PROJECTION_MATRIX_STACK_MAX_DEPTH];

int MGLESX_current_matrix_mode;
int MGLESX_current_projection_matrix_stack_pointer = 0;
int MGLESX_current_modelview_matrix_stack_pointer = 0;
int MGLESX_current_error_code = 0;

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


int MGLESX_getCurrentMatrixMode() {
    return MGLESX_current_matrix_mode;
}

